/* Optimized strncpy implementation for POWER8.
   Copyright (C) 2015-2017 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

#undef EALIGN
#define EALIGN(name, alignt, words)				\
  .section ".text";						\
  ENTRY_2(__strncpy_power8)					\
  .align ALIGNARG(alignt);					\
  EALIGN_W_##words;						\
  BODY_LABEL(__strncpy_power8):					\
  cfi_startproc;						\
  LOCALENTRY(__strncpy_power8)

#undef END
#define END(name)						\
  cfi_endproc;							\
  TRACEBACK(__strncpy_power8)					\
  END_2(__strncpy_power8)

#undef libc_hidden_builtin_def
#define libc_hidden_builtin_def(name)

/* memset is used to pad the end of the string.  */
#define MEMSET __memset_power8

#include <sysdeps/powerpc/powerpc64/power8/strncpy.S>
