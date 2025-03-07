/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_X_WINDOW_H
#define AGA_X_WINDOW_H

#include <aga/gl.h>
#include <aga/draw.h>

#include <asys/log.h>
#include <asys/memory.h>
#include <asys/main.h>

/* TODO: Parity with Windows not directly including X headers (?). */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

/*
 * NOTE: `0xFF**' represents the highest class of "normal" keycodes which
 * 		 Are supported in `aganio' for X systems. We're sitting in a
 * 		 Transitional period before XKB when keyhandling was a bit all over the
 * 		 Place so this will have to do for now.
 */
#define AGA_KEY_MAX (0xFFFF)

static const char* aga_check_x_last = "xlib";

/* TODO: Check against return value for `0'. */
#define AGA_CHECK_X(function, parameter) \
			(aga_check_x_last = #function, function parameter)

static const int single_buffer_fb[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, GLX_RENDER_TYPE, GLX_RGBA_BIT,

		GLX_RED_SIZE, GLX_DONT_CARE, GLX_GREEN_SIZE, GLX_DONT_CARE,
		GLX_BLUE_SIZE, GLX_DONT_CARE, GLX_DEPTH_SIZE, 1,

		None
};

static const int double_buffer_fb[] = {
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_DOUBLEBUFFER, True,

		GLX_RED_SIZE, GLX_DONT_CARE, GLX_GREEN_SIZE, GLX_DONT_CARE,
		GLX_BLUE_SIZE, GLX_DONT_CARE, GLX_DEPTH_SIZE, 1,

		None
};

static int aga_window_device_error_handler(
		Display* display, XErrorEvent* err) {

	asys_fixed_buffer_t buffer = { 0 };

	XGetErrorText(display, err->error_code, buffer, sizeof(buffer));
	asys_log(__FILE__, "err: %s: %s", aga_check_x_last, buffer);

	return 0;
}

enum asys_result aga_window_device_new(
		struct aga_window_device* env, const char* display) {

	if(!env) return ASYS_RESULT_BAD_PARAM;

	AGA_CHECK_X(XSetErrorHandler, (aga_window_device_error_handler));

	env->capture = 0;
	env->display = AGA_CHECK_X(XOpenDisplay, (display));
	if(!env->display) return ASYS_RESULT_ERROR;

	env->screen = DefaultScreen(env->display);

	env->wm_protocols = AGA_CHECK_X(XInternAtom,
								 (env->display, "WM_PROTOCOLS", True));

	if(env->wm_protocols == None) return ASYS_RESULT_BAD_PARAM;

	env->wm_delete = AGA_CHECK_X(XInternAtom,
							  (env->display, "WM_DELETE_WINDOW", True));

	if(env->wm_delete == None) return ASYS_RESULT_BAD_PARAM;

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_device_delete(struct aga_window_device* env) {
	if(!env) return ASYS_RESULT_BAD_PARAM;

	AGA_CHECK_X(XCloseDisplay, (env->display));

	return ASYS_RESULT_OK;
}

enum asys_result aga_keymap_new(
		struct aga_keymap* keymap, struct aga_window_device* env) {

	if(!keymap) return ASYS_RESULT_BAD_PARAM;
	if(!env) return ASYS_RESULT_BAD_PARAM;

	keymap->states = asys_memory_allocate_zero(
			AGA_KEY_MAX, sizeof(asys_bool_t));

	if(!keymap->states) return ASYS_RESULT_OOM;

	return ASYS_RESULT_OK;
}

enum asys_result aga_keymap_delete(struct aga_keymap* keymap) {
	if(!keymap) return ASYS_RESULT_BAD_PARAM;

	asys_memory_free(keymap->states);

	return ASYS_RESULT_OK;
}

static enum asys_result aga_window_set_glx(
		struct aga_window_device* env, struct aga_window* win) {

	static const char* const names[] = {
			"*bold*iso8859*",
			"*iso8859*",
			"*bold*",
			"*"
	};

	GLXFBConfig* fb;
	int n_fb;
	XVisualInfo* vi;

	Font font;
	int font_count = 0;
	XFontStruct* info;
	unsigned current = 0;

	int res;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	win->double_buffered = ASYS_TRUE;
	fb = AGA_CHECK_X(glXChooseFBConfig,
				  (env->display, env->screen, double_buffer_fb, &n_fb));
	if(!fb) {
		win->double_buffered = ASYS_FALSE;
		fb = AGA_CHECK_X(glXChooseFBConfig,
					  (env->display, env->screen, single_buffer_fb, &n_fb));
		if(!fb) return ASYS_RESULT_ERROR;
	}

	/* TODO: `glXGetVisualFromFBConfig' is too new for us. */
	vi = AGA_CHECK_X(glXGetVisualFromFBConfig, (env->display, *fb));
	if(!vi) return ASYS_RESULT_ERROR;

	win->glx = AGA_CHECK_X(glXCreateContext, (env->display, vi, 0, True));
	if(!win->glx) return ASYS_RESULT_ERROR;

	XFree(vi);

	res = AGA_CHECK_X(glXMakeCurrent,(env->display, win->window, win->glx));
	if(!res) return ASYS_RESULT_ERROR;

	while(ASYS_TRUE) {
		char** fontname;
		if(current >= ASYS_LENGTH(names)) {
			asys_log(__FILE__, "err: no fonts available");
			return ASYS_RESULT_BAD_OP;
		}

		asys_log(__FILE__, "Trying font pattern `%s'...", names[current]);
		fontname = AGA_CHECK_X(XListFonts,
							(env->display, names[current], 1, &font_count));

		if(font_count) {
			if(!fontname) return ASYS_RESULT_ERROR;

			asys_log(__FILE__, "Using X font `%s'", *fontname);

			AGA_CHECK_X(XFreeFontNames, (fontname));

			break;
		}

		AGA_CHECK_X(XFreeFontNames, (fontname));
		current++;
	}

	info = AGA_CHECK_X(XLoadQueryFont, (env->display, names[current]));
	if(!info) return ASYS_RESULT_ERROR;

	font = info->fid;

	/*
	 * NOTE: This function shouldn't produce GL errors, but it can leave behind
	 * 		 An error state sometimes in practice.
	 */
	AGA_CHECK_X(glXUseXFont, (font, 0, 256, AGA_FONT_LIST_BASE));
	(void) aga_error_gl(0, "glXUseXFont");

	AGA_CHECK_X(XUnloadFont, (env->display, font));

	return ASYS_RESULT_OK;
}

/*
 * NOTE: Creating a pixmap from a bitmap seems to die under WSLg X, so for now
 * we just use a sensible default cursor and hope the result isn't too
 * obtrusive.
 */
/*
Pixmap bitmap;
XColor black_col = { 0 };
char empty[8] = { 0 };
bitmap = XCreatePixmapFromBitmapData(
	env->display, win->window, empty, 1, 1, white, black, 1);
AGA_VERIFY(bitmap != None, ASYS_RESULT_OOM);
XFreePixmap(env->display, bitmap);
 */

static const long aga_global_window_mask =
		KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask |
		ButtonReleaseMask;

enum asys_result aga_window_new(
		asys_size_t width, asys_size_t height, const char* title,
		struct aga_window_device* env, struct aga_window* win,
		asys_bool_t do_glx, struct asys_main_data* main_data) {

	enum asys_result result;

	aga_xid_t black, white;
	aga_xid_t root;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	black = BlackPixel(env->display, env->screen);
	white = WhitePixel(env->display, env->screen);
	root = RootWindow(env->display, env->screen);

	win->width = width;
	win->height = height;

	win->window = AGA_CHECK_X(XCreateSimpleWindow,
								(env->display, root, 0, 0, width, height, 8,
									white, black));

	if(win->window == None) return ASYS_RESULT_ERROR;

	AGA_CHECK_X(XSetStandardProperties,
			 (env->display, win->window, title, "",
				None, main_data->argv, main_data->argc, 0));

	AGA_CHECK_X(XSelectInput,
			 (env->display, win->window, aga_global_window_mask));

	{
		Atom* protocols;
		Atom* new_protocols;
		int count;
		int res;

		res = AGA_CHECK_X(XGetWMProtocols,
					   (env->display, win->window, &protocols, &count));

		if(!res) {
			protocols = 0;
			count = 0;
		}

		new_protocols = asys_memory_allocate((count + 1) * sizeof(Atom));
		if(!new_protocols) {
			if(protocols) XFree(protocols);
			return ASYS_RESULT_OOM;
		}

		asys_memory_copy(new_protocols, protocols, count * sizeof(Atom));
		new_protocols[count] = env->wm_delete;
		if(protocols) XFree(protocols);

		res = AGA_CHECK_X(XSetWMProtocols,
					   (env->display, win->window, new_protocols, count + 1));

		if(!res) {
			asys_memory_free(new_protocols);
			return ASYS_RESULT_ERROR;
		}

		asys_memory_free(new_protocols);
	}

	AGA_CHECK_X(XSetInputFocus,
			 (env->display, win->window, RevertToNone, CurrentTime));

	AGA_CHECK_X(XMapRaised, (env->display, win->window));

	win->blank_cursor = AGA_CHECK_X(XCreateFontCursor,
										(env->display, XC_tcross));

	if(win->blank_cursor == None) return ASYS_RESULT_ERROR;

	win->arrow_cursor = AGA_CHECK_X(XCreateFontCursor,
										(env->display, XC_arrow));

	if(win->arrow_cursor == None) return ASYS_RESULT_ERROR;

	if(do_glx) {
		result = aga_window_set_glx(env, win);
		if(result) return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_delete(
		struct aga_window_device* env, struct aga_window* win) {

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(win->glx) AGA_CHECK_X(glXDestroyContext, (env->display, win->glx));

	AGA_CHECK_X(XFreeCursor, (env->display, win->blank_cursor));
	AGA_CHECK_X(XFreeCursor, (env->display, win->arrow_cursor));
	AGA_CHECK_X(XDestroyWindow, (env->display, win->window));

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_select(
		struct aga_window_device* env, struct aga_window* win) {

	int res;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	res = AGA_CHECK_X(glXMakeCurrent,(env->display, win->window, win->glx));
	if(!res) return ASYS_RESULT_ERROR;

	return ASYS_RESULT_OK;
}

enum asys_result aga_keymap_lookup(
		struct aga_keymap* keymap, unsigned sym, asys_bool_t* state) {

	if(!keymap) return ASYS_RESULT_BAD_PARAM;
	if(!state) return ASYS_RESULT_BAD_PARAM;

	if(!keymap->states) return ASYS_RESULT_ERROR;

	if(sym > AGA_KEY_MAX) return ASYS_RESULT_BAD_OP;

	*state = keymap->states[sym];

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_set_cursor(
		struct aga_window_device* env, struct aga_window* win,
		asys_bool_t visible, asys_bool_t captured) {

	aga_xid_t current;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	env->capture = captured ? win : 0;

	current = visible ? win->arrow_cursor : win->blank_cursor;
	AGA_CHECK_X(XDefineCursor, (env->display, win->window, current));

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_swap(
		struct aga_window_device* env, struct aga_window* win) {

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(win->double_buffered) {
		AGA_CHECK_X(glXSwapBuffers, (env->display, win->window));
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_device_poll(
		struct aga_window_device* env, struct aga_keymap* keymap,
		struct aga_window* window, struct aga_pointer* pointer,
		asys_bool_t* die, struct aga_buttons* buttons) {

	XEvent event;
	unsigned i;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!keymap) return ASYS_RESULT_BAD_PARAM;
	if(!window) return ASYS_RESULT_BAD_PARAM;
	if(!pointer) return ASYS_RESULT_BAD_PARAM;
	if(!die) return ASYS_RESULT_BAD_PARAM;
	if(!buttons) return ASYS_RESULT_BAD_PARAM;

	for(i = 0; i < ASYS_LENGTH(buttons->states); ++i) {
		if(buttons->states[i] == AGA_BUTTON_CLICK) {
			buttons->states[i] = AGA_BUTTON_DOWN;
		}
	}

	/* Thanks to https://stackoverflow.com/a/78649018/13771204. */

	/*
	 * TODO: With these `XCheck*WindowEvent' functions we now have an event
	 * 		 Queue split by window -- which means `poll' invocations can be
	 * 		 Restructured elsewhere.
	 */
	while(ASYS_TRUE) {
		int ret = XCheckTypedWindowEvent(
				env->display, window->window, ClientMessage, &event);

		if(!ret) break;

		if(event.xclient.message_type == env->wm_protocols) {
			aga_xid_t atom = event.xclient.data.l[0];
			if(atom == env->wm_delete) {
				*die = ASYS_TRUE;
			}
		}
	}

	while(ASYS_TRUE) {
		asys_bool_t press = ASYS_FALSE;

		int ret = XCheckWindowEvent(
				env->display, window->window, aga_global_window_mask, &event);

		if(!ret) break;

		switch(event.type) {
			default: break;

				/*
				 * NOTE: This assumes `Button1 -> LMB', `Button2 -> RMB' and
				 * 		 `Button3 -> MMB' which (apparently) isn't always true.
				 */
			case ButtonPress: {
				press = ASYS_TRUE;
				ASYS_FALLTHROUGH;
			}
				/* FALLTHROUGH */
			case ButtonRelease: {
				enum aga_button_state state;
				enum aga_button button = event.xbutton.button - 1;

				if(event.xbutton.window != window->window) continue;

				if(press) state = AGA_BUTTON_CLICK;
				else state = AGA_BUTTON_UP;

				buttons->states[button] = state;
				break;
			}

#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
			case KeyPress: {
				press = ASYS_TRUE;
				ASYS_FALLTHROUGH;
			}
				/* FALLTHROUGH */
			case KeyRelease: {
				unsigned keycode = event.xkey.keycode;
				KeySym keysym = XKeycodeToKeysym(env->display, keycode, 0);

				if(event.xkey.window != window->window) continue;

				if(keysym > AGA_KEY_MAX) break; /* Key out of range. */

				keymap->states[keysym] = press;
				break;
			}
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif

			case MotionNotify: {
				struct aga_window* capture = env->capture;

				if(!capture) {
					update_pointer: {
					pointer->dx = event.xmotion.x - pointer->x;
					pointer->dy = event.xmotion.y - pointer->y;

					pointer->x = event.xmotion.x;
					pointer->y = event.xmotion.y;

					break;
				}
				}

				/* Handle captured pointer. */
				{
					aga_xid_t win;
					asys_bool_t centred;
					int x = (int) capture->width / 2;
					int y = (int) capture->height / 2;

					win = capture->window;
					centred = (event.xmotion.x == x && event.xmotion.y == y);

					if(capture->window == event.xmotion.window && !centred) {
						AGA_CHECK_X(XWarpPointer,
								 (env->display, win, win, 0, 0, 0, 0, x, y));
					}

					if(!centred) goto update_pointer;

					break;
				}
			}

			case ClientMessage: {
				if(event.xclient.message_type == env->wm_protocols) {
					aga_xid_t atom = event.xclient.data.l[0];
					if(atom == env->wm_delete) {
						*die = ASYS_TRUE;
					}
				}

				break;
			}
		}
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_dialog(
		const char* message, const char* title, asys_bool_t* response,
		asys_bool_t is_error) {

	(void) message;
	(void) title;
	(void) response;
	(void) is_error;

	return ASYS_RESULT_NOT_IMPLEMENTED;
}

enum asys_result aga_dialog_file(char** result) {
	(void) result;

	return ASYS_RESULT_NOT_IMPLEMENTED;
}

enum asys_result aga_shell_open(const char* uri) {
	(void) uri;

	return ASYS_RESULT_NOT_IMPLEMENTED;
}

#endif
