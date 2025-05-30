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

/* see bearssl_ssl.h */
void
br_ssl_client_zero(br_ssl_client_context *cc)
{
	/*
	 * For really standard C, we should explicitly set to NULL all
	 * pointers, and 0 all other fields. However, on all our target
	 * architectures, a direct memset() will work, be faster, and
	 * use a lot less code.
	 */
	memset(cc, 0, sizeof *cc);
}

/* see bearssl_ssl.h */
int
br_ssl_client_reset(br_ssl_client_context *cc,
	const char *server_name, int resume_session)
{
	size_t n;

	br_ssl_engine_set_buffer(&cc->eng, NULL, 0, 0);
	cc->eng.version_out = cc->eng.version_min;
	if (!resume_session) {
		br_ssl_client_forget_session(cc);
	}
	if (!br_ssl_engine_init_rand(&cc->eng)) {
		return 0;
	}

	/*
	 * We always set back the "reneg" flag to 0 because we use it
	 * to distinguish between first handshake and renegotiation.
	 * Note that "renegotiation" and "session resumption" are two
	 * different things.
	 */
	cc->eng.reneg = 0;

	if (server_name == NULL) {
		cc->eng.server_name[0] = 0;
	} else {
		n = strlen(server_name) + 1;
		if (n > sizeof cc->eng.server_name) {
			br_ssl_engine_fail(&cc->eng, BR_ERR_BAD_PARAM);
			return 0;
		}
		memcpy(cc->eng.server_name, server_name, n);
	}

	br_ssl_engine_hs_reset(&cc->eng,
		br_ssl_hs_client_init_main, br_ssl_hs_client_run);
	return br_ssl_engine_last_error(&cc->eng) == BR_ERR_OK;
}

#endif