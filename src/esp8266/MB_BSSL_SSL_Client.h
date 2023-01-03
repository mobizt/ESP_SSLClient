/**
 *
 * The TCP Client Class that supports external network interfaces for ESP8266 and Raspberry Pi v1.0.1
 *
 * This work is based on Arduino-Pico https://github.com/earlephilhower/arduino-pico by Earle F. Philhower.
 *
 * Created January 3, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
    WiFiClientBearSSL- SSL client/server for esp8266 using BearSSL libraries
    - Mostly compatible with Arduino WiFi shield library and standard
    WiFiClient/ServerSecure (except for certificate handling).

    Copyright (c) 2018 Earle F. Philhower, III

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include "BSSL_SSLClient_FS.h"
#if defined(USE_BSSL_SSL_ENGINE)

#include <vector>
#include <bearssl/bearssl.h>
#include "MB_BSSL_Helper.h"
#include "CertStoreBearSSL.h"

#include <memory>
#include <Client.h>
#include <Stream.h>

#if defined(ESP8266)
#include "PolledTimeout.h"
#if defined(ESP8266_CORE_SDK_V3_X_X)
#include <umm_malloc/umm_heap_select.h>
#endif
#endif

namespace BearSSL
{

    class MB_BSSL_SSL_Client_Ctx
    {
    public:
        MB_BSSL_SSL_Client_Ctx();
        MB_BSSL_SSL_Client_Ctx(const MB_BSSL_SSL_Client_Ctx &rhs) = delete;
        ~MB_BSSL_SSL_Client_Ctx();

        MB_BSSL_SSL_Client_Ctx &operator=(const MB_BSSL_SSL_Client_Ctx &) = delete;

        void setClient(Client *client);
        void setTimeout(unsigned long timeout);

        int connect(IPAddress ip, uint16_t port);
        int connect(const String &host, uint16_t port);
        int connect(const char *name, uint16_t port);

        uint8_t connected();
        size_t write(const uint8_t *buf, size_t size);
        size_t write_P(PGM_P buf, size_t size);
        size_t write(Stream &stream); // Note this is not virtual
        int read(uint8_t *buf, size_t size);
        int read(char *buf, size_t size)
        {
            return read((uint8_t *)buf, size);
        }
        int available();
        int read();
        int peek();
        size_t peekBytes(uint8_t *buffer, size_t length);
        void flush();
        void stop();

        int availableForWrite();

        // Allow sessions to be saved/restored automatically to a memory area
        void setSession(BearSSL_Session *session)
        {
            _session = session;
        }

        // Don't validate the chain, just accept whatever is given.  VERY INSECURE!
        void setInsecure()
        {
            _clearAuthenticationSettings();
            _use_insecure = true;
        }
        // Assume a given public key, don't validate or use cert info at all
        void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN)
        {
            _clearAuthenticationSettings();
            _knownkey = pk;
            _knownkey_usages = usages;
        }
        // Only check SHA1 fingerprint of certificate
        bool setFingerprint(const uint8_t fingerprint[20])
        {
            _clearAuthenticationSettings();
            _use_fingerprint = true;
            memcpy_P(_fingerprint, fingerprint, 20);
            return true;
        }
        bool setFingerprint(const char *fpStr);
        // Accept any certificate that's self-signed
        void allowSelfSignedCerts()
        {
            _clearAuthenticationSettings();
            _use_self_signed = true;
        }
        // Install certificates of trusted CAs or specific site
        void setTrustAnchors(const X509List *ta)
        {
            _clearAuthenticationSettings();
            _ta = ta;
        }
        // In cases when NTP is not used, app must set a time manually to check cert validity
        void setX509Time(time_t now)
        {
            _now = now;
        }
        // Install a client certificate for this connection, in case the server requires it (i.e. MQTT)
        void setClientRSACert(const X509List *cert, const PrivateKey *sk);
        void setClientECCert(const X509List *cert, const PrivateKey *sk,
                             unsigned allowed_usages, unsigned cert_issuer_key_type);

        // Sets the requested buffer size for transmit and receive
        void setBufferSizes(int recv, int xmit);

        // Returns whether MFLN negotiation for the above buffer sizes succeeded (after connection)
        int getMFLNStatus()
        {
            return connected() && br_ssl_engine_get_mfln_negotiated(_eng);
        }

        // Return an error code and possibly a text string in a passed-in buffer with last SSL failure
        int getLastSSLError(char *dest = nullptr, size_t len = 0);

        // Attach a preconfigured certificate store
        void setCertStore(CertStoreBase *certStore)
        {
            _certStore = certStore;
        }

        // Select specific ciphers (i.e. optimize for speed over security)
        // These may be in PROGMEM or RAM, either will run properly
        bool setCiphers(const uint16_t *cipherAry, int cipherCount);
        bool setCiphers(const std::vector<uint16_t> &list);
        bool setCiphersLessSecure(); // Only use the limited set of RSA ciphers without EC

        // Limit the TLS versions BearSSL will connect with.  Default is
        // BR_TLS10...BR_TLS12
        bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12);

#if defined(ESP8266)
        // peek buffer API is present
        bool hasPeekBufferAPI() const
        {
            return true;
        }

        // return number of byte accessible by peekBuffer()
        size_t peekAvailable()
        {
            return MB_BSSL_SSL_Client_Ctx::available();
        }

        // return a pointer to available data buffer (size = peekAvailable())
        // semantic forbids any kind of read() before calling peekConsume()
        const char *peekBuffer();

        // consume bytes after use (see peekBuffer)
        void peekConsume(size_t consume);
#endif
        // ESP32 compatibility
        void setCACert(const char *rootCA)
        {
            if (_esp32_ta)
            {
                delete _esp32_ta;
            }
            _esp32_ta = new X509List(rootCA);
        }
        void setCertificate(const char *client_ca)
        {
            if (_esp32_chain)
            {
                delete _esp32_chain;
            }
            _esp32_chain = new X509List(client_ca);
        }
        void setPrivateKey(const char *private_key)
        {
            if (_esp32_sk)
            {
                delete _esp32_sk;
            }
            _esp32_sk = new PrivateKey(private_key);
        }
        bool loadCACert(Stream &stream, size_t size)
        {
            bool ret = false;
            auto buff = new char[size];
            if (size == stream.readBytes(buff, size))
            {
                setCACert(buff);
                ret = true;
            }
            delete[] buff;
            return ret;
        }
        bool loadCertificate(Stream &stream, size_t size)
        {
            bool ret = false;
            auto buff = new char[size];
            if (size == stream.readBytes(buff, size))
            {
                setCertificate(buff);
                ret = true;
            }
            delete[] buff;
            return ret;
        }
        bool loadPrivateKey(Stream &stream, size_t size)
        {
            bool ret = false;
            auto buff = new char[size];
            if (size == stream.readBytes(buff, size))
            {
                setPrivateKey(buff);
                ret = true;
            }
            delete[] buff;
            return ret;
        }
        int connect(IPAddress ip, uint16_t port, int32_t timeout)
        {
            auto save = _timeout;
            _timeout = timeout * 1000; // timeout is in secs, _timeout in milliseconds
            auto ret = connect(ip, port);
            _timeout = save;
            return ret;
        }
        int connect(const char *host, uint16_t port, int32_t timeout)
        {
            auto save = _timeout;
            _timeout = timeout * 1000; // timeout is in secs, _timeout in milliseconds
            auto ret = connect(host, port);
            _timeout = save;
            return ret;
        }

        int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key)
        {
            setSecure(rootCABuff, cli_cert, cli_key);
            return connect(ip, port);
        }

        int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key)
        {
            setSecure(rootCABuff, cli_cert, cli_key);
            return connect(host, port);
        }

    protected:
        bool _connectSSL(); // Do initial SSL handshake

    private:
        friend class MB_BSSL_SSL_Client;
        void _clear();
        void _clearAuthenticationSettings();
        // Only one of the following two should ever be != nullptr!
        std::shared_ptr<br_ssl_client_context> _sc;
        std::shared_ptr<br_ssl_server_context> _sc_svr;
        inline bool ctx_present()
        {
            return (_sc != nullptr) || (_sc_svr != nullptr);
        }
        br_ssl_engine_context *_eng; // &_sc->eng, to allow for client or server contexts
        std::shared_ptr<br_x509_minimal_context> _x509_minimal;
        std::shared_ptr<struct br_x509_insecure_context> _x509_insecure;
        std::shared_ptr<br_x509_knownkey_context> _x509_knownkey;
        std::shared_ptr<unsigned char> _iobuf_in;
        std::shared_ptr<unsigned char> _iobuf_out;
        time_t _now;
        const X509List *_ta;
        CertStoreBase *_certStore;
        int _iobuf_in_size;
        int _iobuf_out_size;
        bool _handshake_done;
        bool _oom_err;

        // Optional storage space pointer for session parameters
        // Will be used on connect and updated on close
        BearSSL_Session *_session;

        bool _use_insecure;
        bool _use_fingerprint;
        uint8_t _fingerprint[20];
        bool _use_self_signed;
        const PublicKey *_knownkey;
        unsigned _knownkey_usages;

        // Custom cipher list pointer or nullptr if default
        std::shared_ptr<uint16_t> _cipher_list;
        uint8_t _cipher_cnt;

        // TLS ciphers allowed
        uint32_t _tls_min;
        uint32_t _tls_max;

        unsigned char *_recvapp_buf;
        size_t _recvapp_len;

        bool _clientConnected(); // Is the underlying socket alive?
        std::shared_ptr<unsigned char> _alloc_iobuf(size_t sz);
        void _freeSSL();
        int _run_until(unsigned target, bool blocking = true);
        size_t _write(const uint8_t *buf, size_t size, bool pmem);
        bool _wait_for_handshake(); // Sets and return the _handshake_done after connecting

        // Optional client certificate
        const X509List *_chain;
        const PrivateKey *_sk;
        unsigned _allowed_usages;
        unsigned _cert_issuer_key_type;

        // RSA keyed server
        bool _connectSSLServerRSA(const X509List *chain, const PrivateKey *sk,
                                  BearSSL_ServerSessions *cache, const X509List *client_CA_ta);
        // EC keyed server
        bool _connectSSLServerEC(const X509List *chain, unsigned cert_issuer_key_type, const PrivateKey *sk,
                                 BearSSL_ServerSessions *cache, const X509List *client_CA_ta);

        // X.509 validators differ from server to client
        bool _installClientX509Validator();                             // Set up X509 validator for a client conn.
        bool _installServerX509Validator(const X509List *client_CA_ta); // Setup X509 client cert validation, if supplied

        uint8_t *_streamLoad(Stream &stream, size_t size);

        void setSecure(const char *rootCABuff, const char *cli_cert, const char *cli_key)
        {
            if (_esp32_ta)
            {
                delete _esp32_ta;
                _esp32_ta = nullptr;
            }
            if (_esp32_chain)
            {
                delete _esp32_chain;
                _esp32_chain = nullptr;
            }
            if (_esp32_sk)
            {
                delete _esp32_sk;
                _esp32_sk = nullptr;
            }
            if (rootCABuff)
            {
                setCertificate(rootCABuff);
            }
            if (cli_cert && cli_key)
            {
                setCertificate(cli_cert);
                setPrivateKey(cli_key);
            }
        }

        // ESP32 compatibility
        X509List *_esp32_ta = nullptr;
        X509List *_esp32_chain = nullptr;
        PrivateKey *_esp32_sk = nullptr;

        String host;
        Client *base_client;
        bool secured = false;
        unsigned long _timeout = 15000;

    }; // class MB_BSSL_SSL_Client_Ctx

    class MB_BSSL_SSL_Client : public Client
    {
    public:
        MB_BSSL_SSL_Client() : _ctx(new MB_BSSL_SSL_Client_Ctx())
        {
        }
        MB_BSSL_SSL_Client(const MB_BSSL_SSL_Client &rhs) : _ctx(rhs._ctx)
        {
        }
        ~MB_BSSL_SSL_Client()
        {
            _ctx = nullptr;
        }

        MB_BSSL_SSL_Client &operator=(const MB_BSSL_SSL_Client &) = default;

        void setClient(Client *client)
        {
            _ctx->setClient(client);
            this->basic_client = client;
        }

        void setTimeout(unsigned long timeout)
        {
            this->_timeout = timeout;
            _ctx->setTimeout(timeout);
        }

        bool connectSSL()
        {
            return _ctx->_connectSSL();
        }

        int connect(IPAddress ip, uint16_t port) override
        {
            return _ctx->connect(ip, port);
        }
        int connect(const String &host, uint16_t port)
        {
            return _ctx->connect(host, port);
        }
        int connect(const char *name, uint16_t port) override
        {
            return _ctx->connect(name, port);
        }

        uint8_t connected() override
        {
            return _ctx->connected();
        }
        size_t write(const uint8_t *buf, size_t size) override
        {
            return _ctx->write(buf, size);
        }
        size_t write_P(PGM_P buf, size_t size) { return _ctx->write_P(buf, size); }
        size_t write(const char *buf)
        {
            return write((const uint8_t *)buf, strlen(buf));
        }
        size_t write(Stream &stream)
        { /* Note this is not virtual */
            return _ctx->write(stream);
        }
        int read(uint8_t *buf, size_t size) override
        {
            return _ctx->read(buf, size);
        }
        int available() override
        {
            return _ctx->available();
        }
        int availableForWrite() override
        {
            return _ctx->availableForWrite();
        }
        int read() override
        {
            return _ctx->read();
        }
        int peek() override
        {
            return _ctx->peek();
        }
        size_t peekBytes(uint8_t *buffer, size_t length)
        {
            return _ctx->peekBytes(buffer, length);
        }
        void flush()
        {
            _ctx->flush();
        }
        void stop()
        {
            _ctx->stop();
        }

        // Allow sessions to be saved/restored automatically to a memory area
        void setSession(BearSSL_Session *session)
        {
            _ctx->setSession(session);
        }

        // Don't validate the chain, just accept whatever is given.  VERY INSECURE!
        void setInsecure()
        {
            _ctx->setInsecure();
        }

        // Assume a given public key, don't validate or use cert info at all
        void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN)
        {
            _ctx->setKnownKey(pk, usages);
        }
        // Only check SHA1 fingerprint of certificate
        bool setFingerprint(const uint8_t fingerprint[20])
        {
            return _ctx->setFingerprint(fingerprint);
        }
        bool setFingerprint(const char *fpStr)
        {
            return _ctx->setFingerprint(fpStr);
        }
        // Accept any certificate that's self-signed
        void allowSelfSignedCerts()
        {
            _ctx->allowSelfSignedCerts();
        }

        // Install certificates of trusted CAs or specific site
        void setTrustAnchors(const X509List *ta)
        {
            _ctx->setTrustAnchors(ta);
        }
        // In cases when NTP is not used, app must set a time manually to check cert validity
        void setX509Time(time_t now)
        {
            _ctx->setX509Time(now);
        }
        // Install a client certificate for this connection, in case the server requires it (i.e. MQTT)
        void setClientRSACert(const X509List *cert, const PrivateKey *sk)
        {
            _ctx->setClientRSACert(cert, sk);
        }
        void setClientECCert(const X509List *cert, const PrivateKey *sk,
                             unsigned allowed_usages, unsigned cert_issuer_key_type)
        {
            _ctx->setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type);
        }

        // Sets the requested buffer size for transmit and receive
        void setBufferSizes(int recv, int xmit)
        {
            _ctx->setBufferSizes(recv, xmit);
        }

        // Returns whether MFLN negotiation for the above buffer sizes succeeded (after connection)
        int getMFLNStatus()
        {
            return _ctx->getMFLNStatus();
        }

        // Return an error code and possibly a text string in a passed-in buffer with last SSL failure
        int getLastSSLError(char *dest = nullptr, size_t len = 0)
        {
            return _ctx->getLastSSLError(dest, len);
        }

        // Attach a preconfigured certificate store
        void setCertStore(CertStoreBase *certStore)
        {
            _ctx->setCertStore(certStore);
        }

        // Select specific ciphers (i.e. optimize for speed over security)
        // These may be in PROGMEM or RAM, either will run properly
        bool setCiphers(const uint16_t *cipherAry, int cipherCount)
        {
            return _ctx->setCiphers(cipherAry, cipherCount);
        }
        bool setCiphers(const std::vector<uint16_t> list)
        {
            return _ctx->setCiphers(list);
        }
        bool setCiphersLessSecure()
        {
            return _ctx->setCiphersLessSecure(); // Only use the limited set of RSA ciphers without EC
        }

        // Limit the TLS versions BearSSL will connect with.  Default is
        // BR_TLS10...BR_TLS12. Allowed values are: BR_TLS10, BR_TLS11, BR_TLS12
        bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12)
        {
            return _ctx->setSSLVersion(min, max);
        };

        // Check for Maximum Fragment Length support for given len before connection (possibly insecure)
        bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len);
        bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len);
        bool probeMaxFragmentLength(const String &host, uint16_t port, uint16_t len);
#if defined(ESP8266)
        // peek buffer API is present
        bool hasPeekBufferAPI() const { return true; }

        // return number of byte accessible by peekBuffer()
        size_t peekAvailable() { return _ctx->available(); }

        // return a pointer to available data buffer (size = peekAvailable())
        // semantic forbids any kind of read() before calling peekConsume()
        const char *peekBuffer() { return _ctx->peekBuffer(); }

        // consume bytes after use (see peekBuffer)
        void peekConsume(size_t consume) { return _ctx->peekConsume(consume); }
#endif
        // ESP32 compatibility
        void setCACert(const char *rootCA)
        {
            return _ctx->setCACert(rootCA);
        }
        void setCertificate(const char *client_ca)
        {
            return _ctx->setCertificate(client_ca);
        }
        void setPrivateKey(const char *private_key)
        {
            return _ctx->setPrivateKey(private_key);
        }
        bool loadCACert(Stream &stream, size_t size)
        {
            return _ctx->loadCACert(stream, size);
        }
        bool loadCertificate(Stream &stream, size_t size)
        {
            return _ctx->loadCertificate(stream, size);
        }
        bool loadPrivateKey(Stream &stream, size_t size)
        {
            return _ctx->loadPrivateKey(stream, size);
        }

        int connect(IPAddress ip, uint16_t port, int32_t timeout)
        {
            return _ctx->connect(ip, port, timeout);
        }
        int connect(const char *host, uint16_t port, int32_t timeout)
        {
            return _ctx->connect(host, port, timeout);
        }
        int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key)
        {
            return _ctx->connect(ip, port, rootCABuff, cli_cert, cli_key);
        }
        int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key)
        {
            return _ctx->connect(host, port, rootCABuff, cli_cert, cli_key);
        }

    private:
        std::shared_ptr<MB_BSSL_SSL_Client_Ctx> _ctx;
        bool secured = false;
        Client *basic_client = nullptr;

        // Checks for support of Maximum Frame Length Negotiation at the given
        // blocksize.  Note that, per spec, only 512, 1024, 2048, and 4096 are
        // supported.  Many servers today do not support this negotiation.

        // TODO - Allow for fragmentation...but not very critical as the ServerHello
        //      we use comes to < 80 bytes which has no reason to ever be fragmented.
        // TODO - Check the type of returned extensions and that the MFL is the exact
        //      same one we sent.  Not critical as only horribly broken servers would
        //      return changed or add their own extensions.
        bool probeMaxFragmentLength(Client *probe, uint16_t len);

    }; // class MB_BSSL_SSL_Client

}; // namespace BearSSL

#endif