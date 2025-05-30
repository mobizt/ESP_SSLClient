/*
 * Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
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

/* see bearssl_block.h */
void
br_aes_small_cbcdec_init(br_aes_small_cbcdec_keys *ctx,
	const void *key, size_t len)
{
	ctx->vtable = &br_aes_small_cbcdec_vtable;
	ctx->num_rounds = br_aes_keysched(ctx->skey, key, len);
}

/* see bearssl_block.h */
void
br_aes_small_cbcdec_run(const br_aes_small_cbcdec_keys *ctx,
	void *iv, void *data, size_t len)
{
	unsigned char *buf, *ivbuf;

	ivbuf = iv;
	buf = data;
	while (len > 0) {
		unsigned char tmp[16];
		int i;

		memcpy(tmp, buf, 16);
		br_aes_small_decrypt(ctx->num_rounds, ctx->skey, buf);
		for (i = 0; i < 16; i ++) {
			buf[i] ^= ivbuf[i];
		}
		memcpy(ivbuf, tmp, 16);
		buf += 16;
		len -= 16;
	}
}

/* see bearssl_block.h */
const br_block_cbcdec_class br_aes_small_cbcdec_vtable = {
	sizeof(br_aes_small_cbcdec_keys),
	16,
	4,
	(void (*)(const br_block_cbcdec_class **, const void *, size_t))
		&br_aes_small_cbcdec_init,
	(void (*)(const br_block_cbcdec_class *const *, void *, void *, size_t))
		&br_aes_small_cbcdec_run
};

#endif