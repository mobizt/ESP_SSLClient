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
br_ssl_server_init_minv2g(br_ssl_server_context *cc,
	const br_x509_certificate *chain, size_t chain_len,
	const br_ec_private_key *sk)
{
	static const uint16_t suites[] = {
		BR_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
	};

	/*
	 * Reset server context and set supported versions to TLS-1.2 (only).
	 */
	br_ssl_server_zero(cc);
	br_ssl_engine_set_versions(&cc->eng, BR_TLS12, BR_TLS12);

	/*
	 * Set suites.
	 */
	br_ssl_engine_set_suites(&cc->eng, suites,
		(sizeof suites) / (sizeof suites[0]));

	/*
	 * Set the "server policy": handler for the certificate chain
	 * and private key operations.
	 */
	br_ssl_server_set_single_ec(cc, chain, chain_len, sk,
		BR_KEYTYPE_KEYX, BR_KEYTYPE_EC, &br_ec_all_m15, 0);

	/*
	 * Set supported hash functions.
	 */
	br_ssl_engine_set_hash(&cc->eng, br_sha256_ID, &br_sha256_vtable);

	/*
	 * Set the PRF implementations.
	 */
	br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);

	/*
	 * Symmetric encryption.
	 */
	br_ssl_engine_set_default_aes_gcm(&cc->eng);
}

#endif