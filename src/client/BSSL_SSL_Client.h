/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef BSSL_SSL_CLIENT_H
#define BSSL_SSL_CLIENT_H

#include <Arduino.h>
#include "BSSL_CertStore.h"
#include "BSSL_CertStore.cpp"

#if defined(USE_LIB_SSL_ENGINE) || defined(USE_EMBED_SSL_ENGINE)


#if defined(USE_LIB_SSL_ENGINE)

using namespace bssl;

#elif defined(USE_EMBED_SSL_ENGINE)

#include "BearSSLHelpers.h"
#include "BSSL_Helper.h"
#include "CertStoreBearSSL.h"

using namespace BearSSL;

#if defined(ESP8266)

#include "PolledTimeout.h"
#if defined(ESP8266_CORE_SDK_V3_X_X)
#include <umm_malloc/umm_heap_select.h>
#endif

#if __has_include(<core_esp8266_version.h>)
#include <core_esp8266_version.h>
#endif

#endif

#endif

class BSSL_SSL_Client : public Client
{
public:
    explicit BSSL_SSL_Client(Client *client = nullptr);

    ~BSSL_SSL_Client();

    void setClient(Client *client, bool ssl = false);

    void setDebugLevel(int level);

    int connect(IPAddress ip, uint16_t port) override;

    int connect(const char *host, uint16_t port) override;

    uint8_t connected() override;

    void validate(const char *host, uint16_t port);

    void validate(IPAddress ip, uint16_t port);

    int available() override;

    int read() override;

    int read(uint8_t *buf, size_t size) override;

    size_t write(const uint8_t *buf, size_t size) override;

    size_t write(uint8_t b) override;

    size_t write_P(PGM_P buf, size_t size);

    size_t write(Stream &stream);

    int peek() override;

    size_t peekBytes(uint8_t *buffer, size_t length);

    void setInsecure();

    void enableSSL(bool enable);

    int connectSSL(IPAddress ip, uint16_t port);

    int connectSSL(const char *host, uint16_t port);

    void stop() override;

    void setTimeout(unsigned int timeoutMs);

    void setHandshakeTimeout(unsigned int timeoutMs);

    void setSessionTimeout(uint32_t seconds);

    void flush() override;

    void setBufferSizes(int recv, int xmit);

    operator bool() override { return connected() > 0; }

    int availableForWrite() override;

    void setSession(BearSSL_Session *session);

    void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN);

    bool setFingerprint(const uint8_t fingerprint[20]);

    bool setFingerprint(const char *fpStr);

    void allowSelfSignedCerts();

    void setTrustAnchors(const X509List *ta);

    void setX509Time(time_t now);

    void setClientRSACert(const X509List *chain, const PrivateKey *sk);

    void setClientECCert(const X509List *chain, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type);

    int getMFLNStatus();

    int getLastSSLError(char *dest, size_t len);
#if defined(ENABLE_FS)
    void setCertStore(CertStoreBase *certStore);
#endif
    bool setCiphers(const uint16_t *cipherAry, int cipherCount);

    bool setCiphers(const std::vector<uint16_t> &list);

    bool setCiphersLessSecure();

    bool setSSLVersion(uint32_t min, uint32_t max);

    bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len);

    bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len);

    bool probeMaxFragmentLength(const String &host, uint16_t port, uint16_t len);

    size_t peekAvailable() EMBED_SSL_ENGINE_BASE_OVERRIDE;

    const char *peekBuffer() EMBED_SSL_ENGINE_BASE_OVERRIDE;

    void peekConsume(size_t consume) EMBED_SSL_ENGINE_BASE_OVERRIDE;

    void setCACert(const char *rootCA);

    void setCertificate(const char *client_ca);

    void setPrivateKey(const char *private_key);

    bool loadCACert(Stream &stream, size_t size);

    bool loadCertificate(Stream &stream, size_t size);

    bool loadPrivateKey(Stream &stream, size_t size);

    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);

    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key);

    BSSL_SSL_Client &operator=(const BSSL_SSL_Client &other);

    bool operator==(const bool value) { return bool() == value; }

    bool operator!=(const bool value) { return bool() != value; }

    bool operator==(const BSSL_SSL_Client &);

    bool operator!=(const BSSL_SSL_Client &rhs) { return !this->operator==(rhs); };

    unsigned int getTimeout() const;

    void setSecure(const char *rootCABuff, const char *cli_cert, const char *cli_key);

private:
    // Checks for support of Maximum Frame Length Negotiation at the given
    // blocksize.  Note that, per spec, only 512, 1024, 2048, and 4096 are
    // supported.  Many servers today do not support this negotiation.

    // TODO - Allow for fragmentation...but not very critical as the ServerHello
    //      we use comes to < 80 bytes which has no reason to ever be fragmented.
    // TODO - Check the type of returned extensions and that the MFL is the exact
    //      same one we sent.  Not critical as only horribly broken servers would
    //      return changed or add their own extensions.
    bool mProbeMaxFragmentLength(Client *probe, uint16_t len);

    bool mProbeMaxFragmentLength(const char *name, IPAddress ip, uint16_t port, uint16_t len);

    int mIsClientInitialized(bool notify);

    int mConnectBasicClient(const char *host, IPAddress ip, uint16_t port);
    // Returns whether or not the engine is connected, without polling the client over SPI or other (as opposed to connected())
    bool mSoftConnected(const char *func_name);

    int mConnectSSL(const char *host = nullptr);

    bool mConnectionValidate(const char *host, IPAddress ip, uint16_t port);

    bool mCheckSessionTimeout();

    int mRunUntil(const unsigned target, unsigned long timeout = 0);

    unsigned mUpdateEngine();

    void mPrintClientError(const int ssl_error, int level, const char *func_name);

    void mPrintSSLError(const unsigned br_error_code, int level, const char *func_name);

    void mPrintSSLState(const unsigned br_state, int level, const char *func_name);

    bool mIsSecurePort(uint16_t port);

    void mBSSLX509InsecureInit(bssl::br_x509_insecure_context *ctx, int _use_fingerprint, const uint8_t _fingerprint[20], int _allow_self_signed);

    void mClearAuthenticationSettings();

    void mClear();

    bool mInstallClientX509Validator();

    void mFreeSSL();

    uint8_t *mStreamLoad(Stream &stream, size_t size);

    void *mallocImpl(size_t len, bool clear = true);

    void freeImpl(void *ptr);

    size_t getReservedLen(size_t len);

    // store whether to enable debug logging
    int _debug_level = 0;

    bool _is_connected = false;

    //  store the index of where we are writing in the buffer
    //  so we can send our records all at once to prevent
    //  weird timing issues
    size_t _write_idx = 0;

    // store the last BearSSL state so we can print changes to the console
    unsigned int _bssl_last_state = 0;

    bool _secure = false;

    Client *_basic_client = nullptr;

    std::shared_ptr<br_ssl_client_context> _sc;
    br_ssl_engine_context *_eng = nullptr; // &_sc->eng, to allow for client or server contexts
    std::shared_ptr<br_x509_minimal_context> _x509_minimal;
    std::shared_ptr<struct bssl::br_x509_insecure_context> _x509_insecure;
    std::shared_ptr<br_x509_knownkey_context> _x509_knownkey;

    unsigned char *_iobuf_in = nullptr;
    unsigned char *_iobuf_out = nullptr;
    int _iobuf_in_size = 512;
    int _iobuf_out_size = 512;

    time_t _now = 0;
    const X509List *_ta = nullptr;
#if defined(ENABLE_FS)
    CertStoreBase *_certStore = 0;
#endif
    // Optional client certificate
    const X509List *_chain = nullptr;
    const PrivateKey *_sk = nullptr;
    unsigned int _allowed_usages = 0;
    unsigned int _cert_issuer_key_type = 0;

    // Optional storage space pointer for session parameters
    // Will be used on connect and updated on close
    BearSSL_Session *_session = nullptr;

    bool _use_insecure = false;
    bool _use_fingerprint = false;
    uint8_t _fingerprint[20];
    bool _use_self_signed = false;
    const PublicKey *_knownkey;
    unsigned int _knownkey_usages = 0;

    // Custom cipher list pointer or nullptr if default
    uint16_t *_cipher_list = nullptr;
    uint8_t _cipher_cnt = 0;

    // TLS ciphers allowed
    uint32_t _tls_min = BR_TLS10;
    uint32_t _tls_max = BR_TLS12;

    X509List *_esp32_ta = nullptr;
    X509List *_esp32_chain = nullptr;
    PrivateKey *_esp32_sk = nullptr;

    bool _handshake_done = false;
    bool _oom_err = false;
    unsigned char *_recvapp_buf = nullptr;
    size_t _recvapp_len;
    // Renameing from _timeout which also defined in parent's Stream class.
    unsigned long _timeout_ms = 15000;
    unsigned long _handshake_timeout = 60000;
    unsigned long _tcp_session_timeout = 0;
    unsigned long _session_ts = 0;
    bool _isSSLEnabled = false;
    String _host;
    uint16_t _port = 0;
    IPAddress _ip;
    bool _connect_with_ip = false;
};

#endif

#endif /** BSSL_SSL_Client_H */
