/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>
 */

#ifndef AGA_WIN32_WINDOW_H
#define AGA_WIN32_WINDOW_H

#include <asys/system.h>
#include <asys/main.h>
#include <asys/memory.h>
#include <asys/log.h>

/* NOTE: Consumer Windows only started supporting OpenGL in 1997-ish. */

/* TODO: Separate WGL from period-accurate win32. */

#define AGA_CLASS_NAME ("AftGangAglay")

#define AGA_KEY_MAX (0xFF)

static void aga_set_window_long(void* hwnd, asys_native_long_t value) {
#ifdef ASYS_WIN64
	SetWindowLongPtr(hwnd, 0, value);
#else
	SetWindowLong(hwnd, 0, value);
#endif
}

static asys_native_long_t aga_get_window_long(void* hwnd) {
#ifdef ASYS_WIN64
	return GetWindowLongPtr(hwnd, 0);
#else
	return GetWindowLong(hwnd, 0);
#endif
}

static const PIXELFORMATDESCRIPTOR pixel_format = {
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		/*
		 * TODO: Broader palletization support -- user-specified palletes etc.
		 * 		 See:
		 * 		 https://learn.microsoft.com/en-us/windows/win32
		 * 		 		/opengl/color-index-mode-and-windows-palette-management
		 */
#ifdef AGA_PALLETIZE
		PFD_TYPE_COLORINDEX,
#else
		PFD_TYPE_RGBA,
#endif
		/* TODO: Configurable bitdepth. */
		24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0,
		PFD_MAIN_PLANE,
		0, 0, 0, 0
};

#define AGA_WINPROC_PACK_MAGIC (0x547123AA)

struct aga_winproc_pack {
	struct aga_window_device* env;
	struct aga_window* win;
	struct aga_keymap* keymap;
	struct aga_pointer* pointer;
	struct aga_buttons* buttons;
	asys_bool_t* die;
	asys_uint_t magic;
};

static void aga_setbuttondown(struct aga_buttons* b, enum aga_button t) {
	enum aga_button_state* v = &b->states[t];
	*v = ((*v == AGA_BUTTON_UP) ? AGA_BUTTON_CLICK : AGA_BUTTON_DOWN);
}

/*
 * TODO: Attach winpack to all windows so poll can be window-independent?
 * 		 Current setup does not handle multiwindow well -- especially teardown.
 */
static LRESULT FAR CALLBACK aga_winproc(
		HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {

	struct aga_winproc_pack* pack;
	asys_bool_t down = ASYS_TRUE;

	if(msg == WM_NCCREATE) return TRUE;

	pack = (void*) aga_get_window_long(hwnd);

	/* TODO: Does this need to be here? */
	if(!pack || pack->magic != AGA_WINPROC_PACK_MAGIC) {
		return DefWindowProc(hwnd, msg, w_param, l_param);
	}

	switch(msg) {
		default: return DefWindowProc(hwnd, msg, w_param, l_param);

		case WM_KEYUP: {
			down = ASYS_FALSE;
			ASYS_FALLTHROUGH;
		}
		/* FALLTHROUGH */
		case WM_KEYDOWN: {
			pack->keymap->states[w_param] = down;
			return 0;
		}

		case WM_MOUSEMOVE: {
			struct aga_window_device* env = pack->env;
			struct aga_window* capture =  env->capture;

			int x = GET_X_LPARAM(l_param);
			int y = GET_Y_LPARAM(l_param);

			int y_adj = env->caption_height + (env->border_sizeable_y * 2);

			if(!capture) {
				update_pointer: {
					y += y_adj;

					pack->pointer->dx = x - pack->pointer->x;
					pack->pointer->dy = y - pack->pointer->y;

					pack->pointer->x = x;
					pack->pointer->y = y;

					return 0;
				}
			}

			/* Handle captured pointer. */
			{
				RECT r;

				asys_bool_t centred;
				int mid_x, mid_y;
				int adj_mid_x, adj_mid_y;

				GetWindowRect(hwnd, &r);

				mid_x = pack->win->width / 2;
				adj_mid_x = mid_x - env->border_sizeable_x;

				mid_y = pack->win->height / 2;
				adj_mid_y =
						mid_y - env->border_sizeable_y -
								env->caption_height;

				centred = (x == adj_mid_x && y == adj_mid_y);

				if(capture->hwnd == hwnd && !centred) {
					pack->pointer->x = adj_mid_x;
					pack->pointer->y = adj_mid_y + y_adj;

					SetCursorPos(mid_x + r.left, mid_y + r.top);
				}

				if(!centred) goto update_pointer;
			}

			return 0;
		}

		/*
		 * TODO: Clean this up and remove the press vs. down distinction at
		 * 		 Engine level.
		 */
		case WM_LBUTTONDOWN: {
			aga_setbuttondown(pack->buttons, AGA_BUTTON_LEFT);
			return 0;
		}
		case WM_LBUTTONUP: {
			pack->buttons->states[AGA_BUTTON_LEFT] = AGA_BUTTON_UP;
			return 0;
		}

		case WM_RBUTTONDOWN: {
			aga_setbuttondown(pack->buttons, AGA_BUTTON_RIGHT);
			return 0;
		}
		case WM_RBUTTONUP: {
			pack->buttons->states[AGA_BUTTON_RIGHT] = AGA_BUTTON_UP;
			return 0;
		}

		case WM_MBUTTONDOWN: {
			aga_setbuttondown(pack->buttons, AGA_BUTTON_MIDDLE);
			return 0;
		}
		case WM_MBUTTONUP: {
			pack->buttons->states[AGA_BUTTON_MIDDLE] = AGA_BUTTON_UP;
			return 0;
		}

		case WM_CLOSE: {
			*pack->die = ASYS_TRUE;
			return 0;
		}
	}
}

const char* asys_global_win32_class_name = AGA_CLASS_NAME;

enum asys_result asys_win32_register_class(void* out, void* module) {
	WNDCLASS* out_class = out;

	out_class->style = 0;
	out_class->lpfnWndProc = aga_winproc;
	out_class->cbClsExtra = 0;
	out_class->cbWndExtra = sizeof(asys_native_long_t);
	out_class->hInstance = module;
	out_class->hIcon = 0;
	out_class->hCursor = 0;
	out_class->hbrBackground = 0;
	out_class->lpszMenuName = 0;
	out_class->lpszClassName = asys_global_win32_class_name;

	/* TODO: Re-enable icon. */
	/*
	out_class.hIcon = LoadIcon(module, MAKEINTRESOURCE(AGA_ICON_RESOURCE);
	 */

	return ASYS_RESULT_OK;
}

/*
 * Many thanks to code taken from: https://github.com/quakeforge/quakeforge/
 * (see libs/video/targets/).
 */

enum asys_result aga_window_device_new(
		struct aga_window_device* env, const char* display) {

	enum asys_result result;

	if(!env) return ASYS_RESULT_BAD_PARAM;

	(void) display;

	env->capture = 0;
	env->captured = ASYS_FALSE;
	env->visible = ASYS_TRUE;

	env->caption_height = GetSystemMetrics(SM_CYCAPTION);
	env->border_x = GetSystemMetrics(SM_CXBORDER);
	env->border_y = GetSystemMetrics(SM_CYBORDER);
	env->border_sizeable_x = GetSystemMetrics(SM_CXFRAME);
	env->border_sizeable_y = GetSystemMetrics(SM_CYFRAME);

	if(!(env->cursor = LoadCursor(0, IDC_ARROW))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "LoadCursor", result);
		return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_device_delete(struct aga_window_device* env) {
	if(!env) return ASYS_RESULT_BAD_PARAM;

	return ASYS_RESULT_OK;
}

enum asys_result aga_keymap_new(
		struct aga_keymap* keymap, struct aga_window_device* env) {

	if(!keymap) return ASYS_RESULT_BAD_PARAM;
	if(!env) return ASYS_RESULT_BAD_PARAM;

	/* VK_OEM_CLEAR + 1 */
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

static enum asys_result aga_window_set_wgl(
		struct aga_window_device* env, struct aga_window* win) {

	enum asys_result result;

	HGDIOBJ font;
	int format;
	void* dc;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(!(dc = GetDC(win->hwnd))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "GetDC", result);
		return result;
	}

	if(!(format = ChoosePixelFormat(dc, &pixel_format))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ChoosePixelFormat", result);
		return result;
	}

	if(!SetPixelFormat(dc, format, &pixel_format)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "SetPixelFormat", result);
		return result;
	}

	if(!(win->wgl = wglCreateContext(dc))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "wglCreateContext", result);
		return result;
	}

	if(!wglMakeCurrent(dc, win->wgl)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "wglMakeCurrent", result);
		return result;
	}

	/*
	 * TODO: This can be shared via. `wglShareLists' between all window wgl
	 * contexts.
	 */
	if(!(font = GetStockObject(SYSTEM_FONT))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "GetStockObject", result);
		return result;
	}

	if(!SelectObject(dc, font)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "SelectObject", result);
		return result;
	}

	if(!wglUseFontBitmaps(dc, 0, 256, AGA_FONT_LIST_BASE)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "wglUseFontBitmaps", result);
		return result;
	}

	if(!ReleaseDC(win->hwnd, dc)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ReleaseDC", result);
		return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_new(
		asys_size_t width, asys_size_t height, const char* title,
		struct aga_window_device* env, struct aga_window* win,
		asys_bool_t do_wgl, struct asys_main_data* main_data) {

	static const long mask = WS_VISIBLE | WS_OVERLAPPEDWINDOW;

	enum asys_result result;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	/* TODO: Leaky error states. */
	win->hwnd = CreateWindow(
			AGA_CLASS_NAME, title, mask, CW_USEDEFAULT, CW_USEDEFAULT,
			(int) width, (int) height, 0, 0, main_data->module, 0);

	if(!win->hwnd) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "CreateWindow", result);
		return result;
	}

	if(!ShowWindow(win->hwnd, main_data->show)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ShowWindow", result);
		return result;
	}

	if(do_wgl) {
		result = aga_window_set_wgl(env, win);
		if(result) return result;
	}
	else win->wgl = 0;

	win->width = width;
	win->height = height;

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_delete(
		struct aga_window_device* env, struct aga_window* win) {

	enum asys_result result;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(win->wgl && !wglDeleteContext(win->wgl)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "wglDeleteContext", result);
		return result;
	}

	if(DestroyWindow(win->hwnd)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "DestroyWindow", result);
		return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_select(
		struct aga_window_device* env, struct aga_window* win) {

	enum asys_result result;

	void* dc;

	(void) env;

	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(!(dc = GetDC((void*) win->hwnd))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "GetDC", result);
		return result;
	}

	if(!wglMakeCurrent(dc, win->wgl)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "wglMakeCurrent", result);
		return result;
	}

	if(!ReleaseDC(win->hwnd, dc)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ReleaseDC", result);
		return result;
	}

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

static enum asys_result aga_setclipcursor(
		struct aga_window_device* env, struct aga_window* win,
		asys_bool_t clip) {

	enum asys_result result;

	RECT rect;
	POINT begin, end;

	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(!clip) {
		ClipCursor(0);
		return ASYS_RESULT_OK;
	}

	if(!GetClientRect(win->hwnd, &rect)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "GetClientRect", result);
		return result;
	}

	begin.x = rect.left;
	begin.y = rect.top;
	end.x = rect.right;
	end.y = rect.bottom;

	if(!ClientToScreen(win->hwnd, &begin)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ClientToScreen", result);
		return result;
	}

	if(!ClientToScreen(win->hwnd, &end)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ClientToScreen", result);
		return result;
	}

	/* TODO: Handle sizeable vs. fixed windows. */
	rect.left = begin.x + env->border_sizeable_x;
	rect.top = begin.y + env->border_sizeable_y;
	rect.right = end.x - env->border_sizeable_x;
	rect.bottom = end.y - env->border_sizeable_y;

	ClipCursor(&rect);

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_set_cursor(
		struct aga_window_device* env, struct aga_window* win,
		asys_bool_t visible, asys_bool_t captured) {

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	SetCursor(visible ? env->cursor : 0);

	env->capture = captured ? win : 0;

	env->visible = visible;
	env->captured = captured;

	return aga_setclipcursor(env, win, captured);
}

enum asys_result aga_window_swap(
		struct aga_window_device* env, struct aga_window* win) {

	enum asys_result result;

	void* dc;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;

	if(!(dc = GetDC((void*) win->hwnd))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "GetDC", result);
		return result;
	}

	if(!SwapBuffers(dc)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "SwapBuffers", result);
		return result;
	}

	if(!ReleaseDC(win->hwnd, dc)) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "ReleaseDC", result);
		return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_window_device_poll(
		struct aga_window_device* env, struct aga_keymap* keymap,
		struct aga_window* win, struct aga_pointer* pointer, asys_bool_t* die,
		struct aga_buttons* buttons) {

	enum asys_result result;
	MSG msg;
	struct aga_winproc_pack pack;
	asys_size_t i;

	if(!env) return ASYS_RESULT_BAD_PARAM;
	if(!keymap) return ASYS_RESULT_BAD_PARAM;
	if(!win) return ASYS_RESULT_BAD_PARAM;
	if(!pointer) return ASYS_RESULT_BAD_PARAM;
	if(!die) return ASYS_RESULT_BAD_PARAM;
	if(!buttons) return ASYS_RESULT_BAD_PARAM;

	/*
	 * TODO: Permanently attach a pack to a window on creation -- use extra
	 * 		 Storage for a pack directly?
	 */
	pack.die = die;
	pack.keymap = keymap;
	pack.pointer = pointer;
	pack.buttons = buttons;
	pack.magic = AGA_WINPROC_PACK_MAGIC;
	pack.win = win;
	pack.env = env;

	for(i = 0; i < ASYS_LENGTH(buttons->states); ++i) {
		if(buttons->states[i] == AGA_BUTTON_CLICK) {
			buttons->states[i] = AGA_BUTTON_DOWN;
		}
	}

	if(env->captured) {
		result = aga_setclipcursor(env, win, GetActiveWindow() == win->hwnd);
		if(result) return result;
	}

	SetCursor(env->visible ? env->cursor : 0);

	aga_set_window_long(win->hwnd, (asys_native_long_t) &pack);

	while(PeekMessage(&msg, win->hwnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_dialog(
		const char* message, const char* title, asys_bool_t* out_response,
		asys_bool_t is_error) {

	enum asys_result result;

	DWORD icon = is_error ? MB_ICONERROR : MB_ICONINFORMATION;
	DWORD flags = MB_YESNO | MB_TASKMODAL | icon;

	int response;

	if(!message) return ASYS_RESULT_BAD_PARAM;
	if(!title) return ASYS_RESULT_BAD_PARAM;
	if(!out_response) return ASYS_RESULT_BAD_PARAM;

	/* TODO: Should we parent dialogs? */
	if(!(response = MessageBox(0, message, title, flags))) {
		result = ASYS_RESULT_ERROR;
		asys_log_result(__FILE__, "MessageBox", result);
		return result;
	}

	*out_response = (response == IDYES);

	return ASYS_RESULT_OK;
}

static enum asys_result aga_windiagerr(DWORD err) {
	switch(err) {
		default: return ASYS_RESULT_ERROR;
		case CDERR_FINDRESFAILURE: return ASYS_RESULT_ERROR;
		case CDERR_INITIALIZATION: return ASYS_RESULT_OOM;
		case CDERR_LOCKRESFAILURE: return ASYS_RESULT_BAD_OP;
		case CDERR_LOADRESFAILURE: return ASYS_RESULT_ERROR;
		case CDERR_LOADSTRFAILURE: return ASYS_RESULT_ERROR;
		case CDERR_MEMALLOCFAILURE: return ASYS_RESULT_OOM;
		case CDERR_MEMLOCKFAILURE: return ASYS_RESULT_BAD_OP;
		case CDERR_NOHINSTANCE: return ASYS_RESULT_ERROR;
		case CDERR_NOHOOK: return ASYS_RESULT_ERROR;
		case CDERR_NOTEMPLATE: return ASYS_RESULT_ERROR;
		case CDERR_STRUCTSIZE: return ASYS_RESULT_BAD_PARAM;
		case FNERR_BUFFERTOOSMALL: return ASYS_RESULT_ERROR;
		case FNERR_INVALIDFILENAME: return ASYS_RESULT_BAD_PARAM;
		case FNERR_SUBCLASSFAILURE: return ASYS_RESULT_OOM;
	}
}

enum asys_result aga_dialog_file(char** out_file) {
	enum asys_result result;

	OPENFILENAME openfile = { 0 };

	if(!out_file) return ASYS_RESULT_BAD_PARAM;

	/* Not ideal but seems to be correct for this particular invoc. pattern. */
	*out_file = asys_memory_allocate_zero(MAX_PATH, sizeof(char));
	if(!*out_file) return ASYS_RESULT_OOM;

	openfile.lStructSize = sizeof(OPENFILENAME);
	/* TODO: Should we parent dialogs? */
	/* openfile.hwndOwner */
	openfile.lpstrFilter = "All Files\0*.*\0\0";
	openfile.nFilterIndex = 1;
	openfile.lpstrFile = *out_file;
	openfile.nMaxFile = MAX_PATH;
	openfile.lpstrInitialDir = ".";
	openfile.Flags = OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&openfile)) {
		asys_memory_free(*out_file);

		result = aga_windiagerr(CommDlgExtendedError());
		asys_log_result(__FILE__, "GetOpenFileName", result);
		return result;
	}

	return ASYS_RESULT_OK;
}

enum asys_result aga_shell_open(const char* uri) {
	enum asys_result result;

	int flags = SW_SHOWNORMAL;

	if(!uri) return ASYS_RESULT_BAD_PARAM;

	if((INT_PTR) ShellExecute(0, 0, uri, 0, 0, flags) > 32) {
		return ASYS_RESULT_OK;
	}

	result = ASYS_RESULT_ERROR;
	asys_log_result(__FILE__, "ShellExecute", result);
	return result;
}

#endif
