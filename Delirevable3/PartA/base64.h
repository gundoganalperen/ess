/* base64.h -- Encode binary data using printable characters.
   Copyright (C) 2004, 2005, 2006 Free Software Foundation, Inc.
   Written by Simon Josefsson.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef BASE64_H
# define BASE64_H

/* Get size_t. */
# include <stddef.h>

/* Get bool. */
# include <stdbool.h>

/* This uses that the expression (n+(k-1))/k means the smallest
   integer >= n/k, i.e., the ceiling of n/k.  */
# define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

// in_size : the number bytes to be encoded.
// Returns the recommended memory size to be allocated for the output buffer excluding the null byte
size_t b64e_size(size_t in_size);

// in_size : the number bytes to be decoded.
// Returns the recommended memory size to be allocated for the output buffer
size_t b64d_size(size_t in_size);

extern bool isbase64 (unsigned char ch);

extern void base64_encode (const unsigned char *restrict in, size_t inlen,
                           unsigned char *restrict out, size_t outlen);

extern size_t base64_encode_alloc (const unsigned char *in, size_t inlen, unsigned char **out);

extern bool base64_decode (const unsigned char *restrict in, size_t inlen,
                           unsigned char *restrict out, size_t *outlen);

extern bool base64_decode_alloc (const unsigned char *in, size_t inlen,
                                 unsigned char **out, size_t *outlen);

#endif /* BASE64_H */