#!/usr/bin/python
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023, 2024 Emily "TTG" Banerjee <prs.ttg+aga@pm.me>

# We're following the same vertex spec defined in `agacore.c'
# > struct aga_vertex {
# >	 float col[4];
# >	 float uv[2];
# >	 float norm[3];
# >	 float pos[3];
# > };

from numpy import float32, array
import pyassimp
from pyassimp.postprocess import *
from sys import argv
from struct import pack

MAGIC = 0xA6A3D700

VERTSZ = 3 + 4 + 2 + 3

print('warn: `aga-vertgen\' is deprecated in favour of `agabuild.sgml\'')

if len(argv) != 3:
	print('usage: ' + argv[0] + ' <input> <output>')
	exit(1)

data = list()
vertices = list()
index_off = 0

min_exts = [ 0.0, 0.0, 0.0 ]
max_exts = [ 0.0, 0.0, 0.0 ]

function = aiProcess_PreTransformVertices | aiProcess_Triangulate
with pyassimp.load(argv[1], None, function) as scene:
	for mesh in scene.meshes:
		for i in range(len(mesh.vertices)):
			z = float32(0.0)
			zz = array([z, z], dtype=float32)
			zzz = array([z, z, z], dtype=float32)
			zzzz = array([z, z, z, z], dtype=float32)

			o = float32(1.0)
			oo = array([o, o], dtype=float32)
			ooo = array([o, o, o], dtype=float32)
			oooo = array([o, o, o, o], dtype=float32)

			vertices.extend(oooo) # No color

			if len(mesh.texturecoords):
				a = mesh.texturecoords[0][i]
				vertices.append(a[0])
				vertices.append(-a[1])
			else:
				vertices.extend(zz)

			if len(mesh.normals):
				a = mesh.normals[i]
				vertices.append(a[0])
				vertices.append(a[1])
				vertices.append(a[2])
			else:
				vertices.extend(zzz)

			if len(mesh.vertices):
				a = mesh.vertices[i]
				vertices.append(a[0])
				vertices.append(a[1])
				vertices.append(a[2])

				min_exts[0] = min(min_exts[0], a[0])
				min_exts[1] = min(min_exts[1], a[1])
				min_exts[2] = min(min_exts[2], a[2])

				max_exts[0] = max(max_exts[0], a[0])
				max_exts[1] = max(max_exts[1], a[1])
				max_exts[2] = max(max_exts[2], a[2])
			else:
				vertices.extend(zzz)

		for face in mesh.faces:
			for vertex in face:
				st = (index_off + vertex) * VERTSZ
				end = (index_off + vertex + 1) * VERTSZ
				data.extend(vertices[st:end])
		index_off = len(vertices) // VERTSZ

data_s = array(data, dtype=float32)

with open(argv[2], 'wb+') as f:
	f.write(data_s.tobytes())
	f.write(pack('ffffffI',
		min_exts[0], min_exts[1], min_exts[2],
		max_exts[0], max_exts[1], max_exts[2],
		MAGIC))
