/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef BSSL_TCP_CLIENT_H
#define BSSL_TCP_CLIENT_H

#if defined(BSSL_BUILD_INTERNAL_CORE) || defined(BSSL_BUILD_PLATFORM_CORE)

class BSSL_TCPClient : public Client
{

protected:
    bool _use_insecure;

public:
    // The default class constructor
    BSSL_TCPClient()
    {
        setClient(nullptr);
        _ssl_client.setTimeout(120000);
        _use_insecure = false;
    }

    // The class deconstructor
    ~BSSL_TCPClient() { setClient(nullptr); }

    /**
     * Set the client.
     */
    void setClient(Client *client, bool enableSSL = true)
    {
        _basic_client = client;
        _ssl_client.setClient(_basic_client, enableSSL);
    }

    /**
     * Set debug level.
     */
    void setDebugLevel(int level) { _ssl_client.setDebugLevel(level); }

    /**
     * Connect to server (IP only).
     */
    int connect(IPAddress ip, uint16_t port) override { return connect(ip, port, 0); }

    /**
     * Connect to server (IP with timeout).
     */
    int connect(IPAddress ip, uint16_t port, int32_t timeout)
    {

#if defined(SSLCLIENT_INSECURE_ONLY)
        // Auto-set insecure mode when client is compiled for minimal security.
        setInsecure();
#endif

        _port = port;

        if (timeout > 0)
        {
            _timeout_ms = timeout;
            if (_basic_client)
                _basic_client->setTimeout(_timeout_ms);
            _ssl_client.setTimeout(_timeout_ms);
        }

        return _ssl_client.connect(ip, port);
    }

    /**
     * Connect to server (Host only).
     */
    int connect(const char *host, uint16_t port) override { return connect(host, port, 0); }

    /**
     * Connect to server (Host with timeout).
     */
    int connect(const char *host, uint16_t port, int32_t timeout)
    {

#if defined(SSLCLIENT_INSECURE_ONLY)
        // Auto-set insecure mode when client is compiled for minimal security.
        setInsecure();
#endif

        // Host storage uses C-string array, safe for AVR.
        strcpy(_host, host);
        _port = port;

        if (timeout > 0)
        {
            _timeout_ms = timeout;
            if (_basic_client)
                _basic_client->setTimeout(_timeout_ms);
            _ssl_client.setTimeout(_timeout_ms);
        }

        return _ssl_client.connect(host, port);
    }

    /**
     * Get TCP connection status.
     */
    uint8_t connected() override { return _ssl_client.connected(); }

    /**
     * Validate the last Client connection with these host and port.
     */
    void validate(const char *host, uint16_t port) { _ssl_client.validate(host, port); }

    /**
     * Validate the last Client connection with these IP and port.
     */
    void validate(IPAddress ip, uint16_t port) { _ssl_client.validate(ip, port); }

    /**
     * @brief Checks if the current connection is encrypted (SSL/TLS).
     * @return bool True if SSL/TLS is enabled and active, false otherwise (plain TCP).
     */
    bool isSecure() const
    {
        return _ssl_client.isSecure();
    }

    /**
     * Get available data size to read.
     */
    int available() override { return _ssl_client.available(); }

    /**
     * The TCP data read function (single byte).
     */
    int read() override
    {
        uint8_t data = 0;
        int res = read(&data, 1);
        return res < 0 ? res : data;
    }

    /**
     * The TCP data read function (buffer).
     */
    int read(uint8_t *buf, size_t size) override
    {
        return _ssl_client.read(buf, size);
    }

    /**
     * The TCP data print function (integer).
     */
    int print(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d", data);
        // Delegating to write(const char *buf) which delegates to write(const uint8_t *buf, size_t size)
        int ret = write(buf);
        return ret;
    }

    /**
     * The TCP data print function (C-string).
     */
    int print(const char *data)
    {
        return write(data);
    }

    /**
     * The TCP data print with new line function (C-string).
     */
    int println(const char *data)
    {
        // Uses the implemented write(const char *buf) method below
        int sz = write(data);
        if (sz < 0)
            return sz;
        // Uses the implemented write(const char *buf) method below
        sz += write("\r\n");
        return sz;
    }

#if !defined(__AVR__)
    /**
     * The TCP data print function (String&).
     */
    int print(const String &data) { return print(data.c_str()); }

    /**
     * The TCP data print with new line function (String&).
     */
    int println(const String &data) { return println(data.c_str()); }
#endif

    /**
     * The TCP data print with new line function (integer).
     */
    int println(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d\r\n", data);
        // Delegating to write(const char *buf)
        int ret = write(buf);
        return ret;
    }

    /**
     * The TCP data write function (buffer).
     */
    size_t write(const uint8_t *buf, size_t size) override
    {
        if (!_ssl_client.connected())
            return 0;
        return _ssl_client.write(buf, size);
    }

    /**
     * The TCP data write function (byte).
     */
    size_t write(uint8_t data) override { return write(&data, 1); }

    /**
     * The TCP data write function (PGM data).
     */
    size_t write_P(PGM_P buf, size_t size) { return _ssl_client.write_P(buf, size); }

    /**
     * The TCP data write function (C-string, essential base function).
     */
    size_t write(const char *buf) { return write(reinterpret_cast<const uint8_t *>(buf), strlen(buf)); }

    /**
     * The TCP data write function (Stream).
     */
    size_t write(Stream &stream) { return _ssl_client.write(stream); }

    /**
     * Read one byte from Stream with time out (peek).
     */
    int peek() override { return _ssl_client.peek(); }

    /**
     * Disable certificate verification and ignore the authentication.
     */
    void setInsecure()
    {
        _ssl_client.setInsecure();
        _use_insecure = true;
    }

    /**
     * Enable/disable the SSL layer transport.
     */
    void enableSSL(bool enable) { _ssl_client.enableSSL(enable); }

    /**
     * Upgrade the current connection by setting up the SSL and perform the SSL handshake.
     */
    bool connectSSL()
    {
        // Uses the host stored in _host[64]
        if (!_ssl_client.connectSSL(_host, _port))
        {
            stop();
            return 0;
        }
        return 1;
    }

    /**
     * Upgrade the current connection (C-string host compatibility wrapper).
     */
    bool connectSSL(const char *host, uint16_t port)
    {
        // Note: host and port are ignored, as current connection uses stored _host/_port
        (void)host;
        (void)port;
        return connectSSL();
    }

    /**
     * Stop the TCP connection and release resources.
     */
    void stop() override { _ssl_client.stop(); }

    /**
     * Set the TCP connection timeout in seconds.
     */
    int setTimeout(uint32_t seconds)
    {
        _timeout_ms = seconds * 1000;
        _ssl_client.setTimeout(_timeout_ms);
        return 1;
    }

    /**
     * Get the TCP connection timeout in seconds.
     */
    int getTimeout() { return _ssl_client.getTimeout() / 1000; }

    /**
     * Set the SSL handshake timeout in seconds.
     */
    void setHandshakeTimeout(unsigned long handshake_timeout)
    {
        _handshake_timeout = handshake_timeout * 1000;
        _ssl_client.setHandshakeTimeout(_handshake_timeout);
    }

    /**
     * Set the TCP session timeout in seconds.
     */
    void setSessionTimeout(uint32_t seconds)
    {
        if (seconds > 0 && seconds < BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC)
            seconds = BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC;
        _tcp_session_timeout = seconds;
        _ssl_client.setSessionTimeout(seconds);
    }

    /**
     * Wait for all receive buffer data read.
     */
    void flush() override
    {
        if (!_basic_client)
            return;

        while (available() > 0)
            read();
    }

    /**
     * Sets the requested buffer size for transmit and receive.
     */
    void setBufferSizes(int recv, int xmit) { _ssl_client.setBufferSizes(recv, xmit); }

    operator bool() override { return connected(); }

    int availableForWrite() override
    {
        return _ssl_client.availableForWrite();
    }

    void setSession(BearSSL_Session *session) { _ssl_client.setSession(session); };

#if !defined(SSLCLIENT_INSECURE_ONLY)
    void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN) { _ssl_client.setKnownKey(pk, usages); }

    /**
     * Verify certificate's SHA256 fingerprint.
     */
    bool setFingerprint(const uint8_t fingerprint[20]) { return _ssl_client.setFingerprint(fingerprint); }

    bool setFingerprint(const char *fpStr) { return _ssl_client.setFingerprint(fpStr); }

    void allowSelfSignedCerts() { _ssl_client.allowSelfSignedCerts(); }

    void setClientRSACert(const X509List *cert, const PrivateKey *sk) { _ssl_client.setClientRSACert(cert, sk); }

    void setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type) { _ssl_client.setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type); }

#if defined(ENABLE_FS)
    void setCertStore(CertStoreBase *certStore) { _ssl_client.setCertStore(certStore); }
#endif
    bool setCiphers(const uint16_t *cipherAry, int cipherCount) { return _ssl_client.setCiphers(cipherAry, cipherCount); }
#if !defined(__AVR__)
    bool setCiphers(const std::vector<uint16_t> &list) { return _ssl_client.setCiphers(list); }
#endif

void setTrustAnchors(const X509List *ta) { _ssl_client.setTrustAnchors(ta); }

#endif

    
    void setX509Time(uint32_t now) { _ssl_client.setX509Time(now); }

    int getMFLNStatus() { return _ssl_client.getMFLNStatus(); };

    int getLastSSLError(char *dest = NULL, size_t len = 0) { return _ssl_client.getLastSSLError(dest, len); }

    bool setCiphersLessSecure() { return _ssl_client.setCiphersLessSecure(); }

    bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12) { return _ssl_client.setSSLVersion(min, max); }

    bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(ip, port, len); }

    bool probeMaxFragmentLength(const char *host, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(host, port, len); }

    bool hasPeekBufferAPI() const EMBED_SSL_ENGINE_BASE_OVERRIDE { return true; }

    size_t peekAvailable() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekAvailable(); }

    const char *peekBuffer() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekBuffer(); }

    void peekConsume(size_t consume) EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekConsume(consume); }

#if !defined(SSLCLIENT_INSECURE_ONLY)
    /**
     * Set the Root CA or CA certificate.
     */
    void setCACert(const char *rootCA) { _ssl_client.setCACert(rootCA); }

    void setCertificate(const char *client_ca) { return _ssl_client.setCertificate(client_ca); }

    void setPrivateKey(const char *private_key) { return _ssl_client.setPrivateKey(private_key); }

    /**
     * Read and set CA cert from file (Stream).
     */
    bool loadCACert(Stream &stream, size_t size)
    {
        return _ssl_client.loadCACert(stream, size);
    }

    bool loadCertificate(Stream &stream, size_t size) { return _ssl_client.loadCertificate(stream, size); }

    bool loadPrivateKey(Stream &stream, size_t size) { return _ssl_client.loadPrivateKey(stream, size); }
#endif

    void clearAuthenticationSettings() { _ssl_client.clearAuthenticationSettings(); }

    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(ip, port, rootCABuff, cli_cert, cli_key); }

    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(host, port, rootCABuff, cli_cert, cli_key); }

    BSSL_TCPClient &operator=(const BSSL_TCPClient &other)
    {
        stop();
        setClient(other._basic_client);
        _use_insecure = other._use_insecure;
        _timeout_ms = other._timeout_ms;
        _handshake_timeout = other._handshake_timeout;
        _tcp_session_timeout = other._tcp_session_timeout;
        _ssl_client.setTimeout(_timeout_ms);
        _ssl_client.setHandshakeTimeout(_handshake_timeout);
        _ssl_client.setSessionTimeout(_tcp_session_timeout);
        if (_use_insecure)
            _ssl_client.setInsecure();
        return *this;
    }

    bool operator==(const bool value) { return bool() == value; }

    bool operator!=(const bool value) { return bool() != value; }

    bool operator==(const BSSL_TCPClient &rhs) { return _basic_client == rhs._basic_client && _port == rhs._port && strcmp(_host, rhs._host) == 0; }

    bool operator!=(const BSSL_TCPClient &rhs) { return !this->operator==(rhs); };

private:
    char _host[64];
    uint16_t _port;
    BSSL_SSLClient _ssl_client;
    Client *_basic_client = nullptr;
    // Renameing from _timeout which also defined in parent's Stream class.
    unsigned long _timeout_ms = 15000;
    unsigned long _handshake_timeout = 60000;
    unsigned long _tcp_session_timeout = 0;
};

#endif

#endif