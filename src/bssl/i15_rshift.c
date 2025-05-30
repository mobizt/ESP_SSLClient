/*
 * Copyright (c) 2017 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "bssl_config.h"
#if defined(USE_LIB_SSL_ENGINE)

#include "inner.h"

/* see inner.h */
void
br_i15_rshift(uint16_t *x, int count)
{
	size_t u, len;
	unsigned r;

	len = (x[0] + 15) >> 4;
	if (len == 0) {
		return;
	}
	r = x[1] >> count;
	for (u = 2; u <= len; u ++) {
		unsigned w;

		w = x[u];
		x[u - 1] = ((w << (15 - count)) | r) & 0x7FFF;
		r = w >> count;
	}
	x[len] = r;
}

#endif