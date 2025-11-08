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
     * @brief Assigns the underlying network client used for transportation.
     * @param client The pointer to the Client interface (e.g., WiFiClient, EthernetClient).
     * @param enableSSL Flag to set the SSL option; true for secure, false for plain TCP.
     * @note The client object must exist for the duration of this wrapper's usage
     * to avoid dangling pointers.
     */
    void setClient(Client *client, bool enableSSL = true)
    {
        _basic_client = client;
        _ssl_client.setClient(_basic_client, enableSSL);
    }

    /**
     * @brief Sets the debug verbosity level for the SSL engine.
     * @param level The debug level (0 for none, 4 for max dump).
     */
    void setDebugLevel(int level) { _ssl_client.setDebugLevel(level); }

    /**
     * @brief Connects to the server using the stored configuration.
     * @param ip The server IP address to connect.
     * @param port The server port.
     * @return 1 for success or 0 for error.
     */
    int connect(IPAddress ip, uint16_t port) override { return connect(ip, port, 0); }

    /**
     * @brief Connects to the server using the specified IP and a timeout.
     * @param ip The server IP address.
     * @param port The server port.
     * @param timeout The connection timeout in milliseconds.
     * @return 1 for success or 0 for error.
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
     * @brief Connects to the server using the stored configuration.
     * @param host The server host name (e.g., "example.com").
     * @param port The server port.
     * @return 1 for success or 0 for error.
     */
    int connect(const char *host, uint16_t port) override { return connect(host, port, 0); }

    /**
     * @brief Connects to the server using the specified host name and a timeout.
     * @param host The server host name.
     * @param port The server port.
     * @param timeout The connection timeout in milliseconds.
     * @return 1 for success or 0 for error.
     */
    int connect(const char *host, uint16_t port, int32_t timeout)
    {

#if defined(SSLCLIENT_INSECURE_ONLY)
        // Auto-set insecure mode when client is compiled for minimal security.
        setInsecure();
#endif
        // Host storage uses C-string array, safe for AVR.
        // The host name is stored internally for session continuation/upgrade.
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
     * @brief Gets the status of the underlying network and SSL/TLS connection.
     * @return 1 (true) if the connection is active and ready for I/O, 0 (false) otherwise.
     */
    uint8_t connected() override { return _ssl_client.connected(); }

    /**
     * @brief Validates the current connection state against the provided host and port.
     * @param host The host name to validate against the internal connection details.
     * @param port The port to validate.
     * @note The connection may be closed if the provided host or port does not match
     * that of the current session.
     */
    void validate(const char *host, uint16_t port) { _ssl_client.validate(host, port); }

    /**
     * @brief Validates the current connection state against the provided IP and port.
     * @param ip The IP address to validate against the internal connection details.
     * @param port The port to validate.
     * @note The connection may be closed if the provided IP or port does not match
     * that of the current session.
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
     * @brief Gets the number of bytes available to read from the SSL/TCP buffer.
     * @return The number of available bytes.
     */
    int available() override { return _ssl_client.available(); }

    /**
     * @brief Reads a single byte from the connection.
     * @return The byte data read (0-255) or -1 for error/no data available.
     */
    int read() override
    {
        uint8_t data = 0;
        int res = read(&data, 1);
        return res < 0 ? res : data;
    }

    /**
     * @brief Reads data into a buffer.
     * @param buf The destination buffer.
     * @param size The maximum number of bytes to read.
     * @return The number of bytes successfully read, or 0 for error.
     */
    int read(uint8_t *buf, size_t size) override
    {
        return _ssl_client.read(buf, size);
    }

    /**
     * @brief Prints integer data over the connection.
     * @param data The integer value to print.
     * @return The size of data successfully printed/written.
     */
    int print(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d", data);
        // Delegates to write(const char *buf)
        int ret = write(buf);
        return ret;
    }

    /**
     * @brief Prints a null-terminated string over the connection.
     * @param data The C-style string to print.
     * @return The size of data successfully printed/written.
     */
    int print(const char *data)
    {
        return write(data);
    }

    /**
     * @brief Prints a null-terminated string followed by CR+LF.
     * @param data The C-style string to print.
     * @return The total size of data and newline characters written.
     */
    int println(const char *data)
    {
        int sz = write(data);
        if (sz < 0)
            return sz;
        sz += write("\r\n");
        return sz;
    }

#if !defined(__AVR__)
    /**
     * @brief Prints a String object over the connection (non-AVR only).
     * @param data The String object to print.
     * @return The size of data successfully printed/written.
     */
    int print(const String &data) { return print(data.c_str()); }

    /**
     * @brief Prints a String object followed by CR+LF (non-AVR only).
     * @param data The String object to print.
     * @return The total size of data and newline characters written.
     */
    int println(const String &data) { return println(data.c_str()); }
#endif

    /**
     * @brief Prints an integer followed by CR+LF.
     * @param data The integer value to print.
     * @return The total size of data and newline characters written.
     */
    int println(int data)
    {
        char buf[64];
        memset(buf, 0, 64);
        sprintf(buf, "%d\r\n", data);
        // Delegates to write(const char *buf)
        int ret = write(buf);
        return ret;
    }

    /**
     * @brief Writes binary data to the connection.
     * @param buf The buffer containing data to write.
     * @param size The number of bytes to write.
     * @return The size of data successfully written, or 0 for error.
     */
    size_t write(const uint8_t *buf, size_t size) override
    {
        if (!_ssl_client.connected())
            return 0;
        return _ssl_client.write(buf, size);
    }

    /**
     * @brief Writes a single byte to the connection.
     * @param data The byte of data to write.
     * @return The size of data successfully written (1), or 0 for error.
     */
    size_t write(uint8_t data) override { return write(&data, 1); }

    /**
     * @brief Writes PROGMEM (Flash) based data to the connection.
     * @param buf The PGM data pointer.
     * @param size The number of bytes to write.
     * @return The size of data successfully written.
     */
    size_t write_P(PGM_P buf, size_t size) { return _ssl_client.write_P(buf, size); }

    /**
     * @brief Writes a null-terminated C-style string to the connection.
     * @param buf The C-style string buffer.
     * @return The size of data successfully written.
     */
    size_t write(const char *buf) { return write(reinterpret_cast<const uint8_t *>(buf), strlen(buf)); }

    /**
     * @brief Writes data available from an input stream to the connection.
     * @param stream The source Stream interface.
     * @return The size of data successfully written.
     */
    size_t write(Stream &stream) { return _ssl_client.write(stream); }

    /**
     * @brief Reads the next available byte without consuming it.
     * @return The byte of data read (0-255) or -1 if no data is available.
     */
    int peek() override { return _ssl_client.peek(); }

    /**
     * @brief Disables certificate verification for insecure connection testing.
     */
    void setInsecure()
    {
        _ssl_client.setInsecure();
        _use_insecure = true;
    }

    /**
     * @brief Enables or disables the secure (SSL/TLS) layer transport.
     * @param enable True to enable SSL/TLS, false for plain TCP.
     */
    void enableSSL(bool enable) { _ssl_client.enableSSL(enable); }

    /**
     * @brief Upgrades an existing plain TCP connection by setting up the SSL layer
     * and performing the TLS handshake using the stored host/port details.
     * @return True if the handshake succeeds, false otherwise.
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
     * @brief Compatibility wrapper for TLS handshake using explicit host/port.
     * @param host The host name (ignored, current stored host is used).
     * @param port The port (ignored, current stored port is used).
     * @return True if the handshake succeeds, false otherwise.
     */
    bool connectSSL(const char *host, uint16_t port)
    {
        // Note: host and port are ignored, as current connection uses stored _host/_port
        (void)host;
        (void)port;
        return connectSSL();
    }

    /**
     * @brief Stops the connection and releases underlying network and SSL resources.
     */
    void stop() override { _ssl_client.stop(); }

    /**
     * @brief Sets the connection timeout duration.
     * @param seconds The timeout duration in seconds.
     * @return 1 for success.
     */
    int setTimeout(uint32_t seconds)
    {
        _timeout_ms = seconds * 1000;
        _ssl_client.setTimeout(_timeout_ms);
        return 1;
    }

    /**
     * @brief Gets the current connection timeout in seconds.
     * @return The timeout duration in seconds.
     */
    int getTimeout() { return _ssl_client.getTimeout() / 1000; }

    /**
     * @brief Sets the maximum allowed duration for the SSL/TLS handshake.
     * @param handshake_timeout The handshake timeout in seconds.
     */
    void setHandshakeTimeout(unsigned long handshake_timeout)
    {
        _handshake_timeout = handshake_timeout * 1000;
        _ssl_client.setHandshakeTimeout(_handshake_timeout);
    }

    /**
     * @brief Sets the maximum idle time before the TCP session is re-established.
     * @param seconds The TCP session timeout in seconds (minimum 60s, 0 to disable).
     */
    void setSessionTimeout(uint32_t seconds)
    {
        if (seconds > 0 && seconds < BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC)
            seconds = BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC;
        _tcp_session_timeout = seconds;
        _ssl_client.setSessionTimeout(seconds);
    }

    /**
     * @brief Reads all remaining data from the buffer until empty.
     */
    void flush() override
    {
        if (!_basic_client)
            return;

        while (available() > 0)
            read();
    }

    /**
     * @brief Sets the requested buffer size for transmit and receive buffers in bytes.
     * @param recv The desired receive (RX) buffer size. Must be at least 512 bytes.
     * Ideal size is 16709 bytes. This size may be safely reduced if MFLN is negotiated
     * via the probeMaxFragmentLength() function.
     * @param xmit The desired transmit (TX) buffer size. Must be at least 512 bytes.
     * Ideal size is 16469 bytes.
     * @note If compiled in Half-Duplex mode, the 'recv' size is used for both directions,
     * and 'xmit' is ignored.
     */
    void setBufferSizes(int recv, int xmit) { _ssl_client.setBufferSizes(recv, xmit); }

    /**
     * @brief Returns the connection status.
     * @return True if connected, false otherwise.
     */
    operator bool() override { return connected(); }

    /**
     * @brief Gets the number of bytes currently available for writing to the connection.
     * @return The write buffer capacity in bytes.
     */
    int availableForWrite() override
    {
        return _ssl_client.availableForWrite();
    }

    /**
     * @brief Provides a memory location to store TLS session parameters for resumption.
     * @param session Pointer to the BearSSL_Session structure.
     */
    void setSession(BearSSL_Session *session) { _ssl_client.setSession(session); };

#if !defined(SSLCLIENT_INSECURE_ONLY)
    /**
     * @brief Sets a known public key for verification, bypassing certificate chain validation.
     * @param pk Pointer to the PublicKey structure.
     * @param usages Allowed key usages (default: BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN).
     */
    void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN) { _ssl_client.setKnownKey(pk, usages); }

    /**
     * @brief Verifies the server certificate's SHA256 fingerprint.
     * @param fingerprint The 20-byte binary SHA256 fingerprint.
     * @return Verification result.
     */
    bool setFingerprint(const uint8_t fingerprint[20]) { return _ssl_client.setFingerprint(fingerprint); }

    /**
     * @brief Verifies the server certificate's SHA256 fingerprint from a hex string.
     * @param fpStr The NULL-terminated hex string of the SHA256 fingerprint.
     * @return Verification result.
     */
    bool setFingerprint(const char *fpStr) { return _ssl_client.setFingerprint(fpStr); }

    /**
     * @brief Allows self-signed certificates from the server.
     */
    void allowSelfSignedCerts() { _ssl_client.allowSelfSignedCerts(); }

    /**
     * @brief Sets the list of trusted certificate authorities (TAs) for chain validation.
     * @param ta Pointer to the X509 trusted anchors list.
     */
    void setTrustAnchors(const X509List *ta) { _ssl_client.setTrustAnchors(ta); }

    /**
     * @brief Sets the client certificate and private key for mutual authentication (RSA).
     * @param cert Pointer to the X509 certificate chain.
     * @param sk Pointer to the PrivateKey structure.
     */
    void setClientRSACert(const X509List *cert, const PrivateKey *sk) { _ssl_client.setClientRSACert(cert, sk); }

    /**
     * @brief Sets the client certificate and private key for mutual authentication (ECC).
     * @param cert Pointer to the X509 certificate chain.
     * @param sk Pointer to the PrivateKey structure.
     * @param allowed_usages Key usages required by the server.
     * @param cert_issuer_key_type Issuer key type expected by the server.
     */
    void setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type) { _ssl_client.setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type); }

#if defined(ENABLE_FS)
    /**
     * @brief Sets the certificate store base for loading trust anchors from the filesystem.
     * @param certStore Pointer to the CertStoreBase implementation.
     */
    void setCertStore(CertStoreBase *certStore) { _ssl_client.setCertStore(certStore); }
#endif
    /**
     * @brief Sets the list of preferred TLS cipher suites.
     * @param cipherAry Pointer to the array of 16-bit cipher suite identifiers.
     * @param cipherCount The number of ciphers in the array.
     * @return True on success.
     */
    bool setCiphers(const uint16_t *cipherAry, int cipherCount) { return _ssl_client.setCiphers(cipherAry, cipherCount); }
#if !defined(__AVR__)
    /**
     * @brief Sets the list of preferred TLS cipher suites using a standard C++ vector (non-AVR only).
     * @param list The vector containing 16-bit cipher suite identifiers.
     * @return True on success.
     */
    bool setCiphers(const std::vector<uint16_t> &list) { return _ssl_client.setCiphers(list); }
#endif

#endif // !SSLCLIENT_INSECURE_ONLY

    /**
     * @brief Sets the current time for certificate validity checking.
     * @param now The current time (UNIX epoch).
     */
    void setX509Time(uint32_t now) { _ssl_client.setX509Time(now); }

    /**
     * @brief Gets the status of Maximum Fragment Length Negotiation (MFLN).
     * @return The MFLN status code or 0 if MFLN was not negotiated.
     */
    int getMFLNStatus() { return _ssl_client.getMFLNStatus(); };

    /**
     * @brief Retrieves the last numeric SSL error code and optionally its description.
     * @param dest Destination buffer for the error string (optional).
     * @param len Size of the destination buffer (optional).
     * @return The last numeric BearSSL error code.
     */
    int getLastSSLError(char *dest = NULL, size_t len = 0) { return _ssl_client.getLastSSLError(dest, len); }

    /**
     * @brief Sets ciphers to a less secure, faster list for performance-critical scenarios.
     * @return True on success.
     */
    bool setCiphersLessSecure() { return _ssl_client.setCiphersLessSecure(); }

    /**
     * @brief Sets the minimum and maximum TLS protocol versions allowed.
     * @param min Minimum TLS version (default BR_TLS10).
     * @param max Maximum TLS version (default BR_TLS12).
     * @return True on success.
     */
    bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12) { return _ssl_client.setSSLVersion(min, max); }

    /**
     * @brief Probes the server to determine if a specific Maximum Fragment Length (MFL) is supported by IP.
     * @param ip Server IP.
     * @param port Server port.
     * @param len Fragment length to test (e.g., 512, 1024, 2048, 4096).
     * @return True if the MFL is supported.
     */
    bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(ip, port, len); }

    /**
     * @brief Probes the server to determine if a specific Maximum Fragment Length (MFL) is supported by hostname.
     * @param host Server hostname.
     * @param port Server port.
     * @param len Fragment length to test.
     * @return True if the MFL is supported.
     */
    bool probeMaxFragmentLength(const char *host, uint16_t port, uint16_t len) { return _ssl_client.probeMaxFragmentLength(host, port, len); }

    /**
     * @brief Checks if the underlying secure layer exposes direct access to its internal buffer.
     * @return True if direct buffer access is available (always true for BearSSL implementations).
     */
    bool hasPeekBufferAPI() const EMBED_SSL_ENGINE_BASE_OVERRIDE { return true; }

    /**
     * @brief Gets the number of decrypted bytes currently buffered and available for zero-copy access.
     * @return The number of bytes available in the internal peek buffer.
     */
    size_t peekAvailable() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekAvailable(); }

    /**
     * @brief Returns a pointer directly to the internal buffer containing decrypted application data.
     * @return A const char* pointer to the start of the internal buffer.
     * @note This allows for zero-copy parsing; data is consumed via peekConsume().
     */
    const char *peekBuffer() EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekBuffer(); }

    /**
     * @brief Notifies the SSL engine that a portion of the internal buffer has been processed (consumed).
     * @param consume The number of bytes to mark as read from the buffer start.
     */
    void peekConsume(size_t consume) EMBED_SSL_ENGINE_BASE_OVERRIDE { return _ssl_client.peekConsume(consume); }

#if !defined(SSLCLIENT_INSECURE_ONLY)
    /**
     * @brief Sets the Root CA or CA certificate for chain validation.
     * @param rootCA The pointer to the PEM-encoded root CA certificate.
     */
    void setCACert(const char *rootCA) { _ssl_client.setCACert(rootCA); }

    /**
     * @brief Sets the client certificate buffer (PEM/DER) for mutual authentication.
     * @param client_ca Pointer to the PEM/DER encoded client certificate.
     */
    void setCertificate(const char *client_ca) { return _ssl_client.setCertificate(client_ca); }

    /**
     * @brief Sets the client private key buffer for mutual authentication.
     * @param private_key Pointer to the PEM/DER encoded client private key.
     */
    void setPrivateKey(const char *private_key) { return _ssl_client.setPrivateKey(private_key); }

    /**
     * @brief Reads and sets the CA certificate from an Arduino Stream (e.g., FS).
     * @param stream The Stream interface to read from.
     * @param size The size of the certificate data.
     * @return True on successful load and set, false otherwise.
     */
    bool loadCACert(Stream &stream, size_t size)
    {
        return _ssl_client.loadCACert(stream, size);
    }

    /**
     * @brief Reads and sets the client certificate from an Arduino Stream.
     * @param stream The Stream interface to read from.
     * @param size The size of the certificate data.
     * @return True on successful load and set, false otherwise.
     */
    bool loadCertificate(Stream &stream, size_t size) { return _ssl_client.loadCertificate(stream, size); }

    /**
     * @brief Reads and sets the client private key from an Arduino Stream.
     * @param stream The Stream interface to read from.
     * @param size The size of the key data.
     * @return True on successful load and set, false otherwise.
     */
    bool loadPrivateKey(Stream &stream, size_t size) { return _ssl_client.loadPrivateKey(stream, size); }
#endif // !SSLCLIENT_INSECURE_ONLY

    /**
     * @brief Clears all authentication settings (trust anchors, fingerprints, client certs).
     */
    void clearAuthenticationSettings() { _ssl_client.clearAuthenticationSettings(); }

    /**
     * @brief Connects using IP and sets certificates from provided buffers (convenience function).
     * @note Authentication buffers must be NULL-terminated.
     */
    int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(ip, port, rootCABuff, cli_cert, cli_key); }

    /**
     * @brief Connects using host and sets certificates from provided buffers (convenience function).
     * @note Authentication buffers must be NULL-terminated.
     */
    int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return _ssl_client.connect(host, port, rootCABuff, cli_cert, cli_key); }

    /**
     * @brief Assignment operator for transferring core connection settings.
     * @param other The source object.
     */
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

    /**
     * @brief Equality operator for boolean comparison (checks if connected).
     * @param value Boolean state (true/false).
     */
    bool operator==(const bool value) { return bool() == value; }

    /**
     * @brief Inequality operator for boolean comparison (checks if disconnected).
     * @param value Boolean state (true/false).
     */
    bool operator!=(const bool value) { return bool() != value; }

    /**
     * @brief Equality operator for comparing two BSSL_TCPClient objects (checks core properties).
     * @param rhs The other client object.
     */
    bool operator==(const BSSL_TCPClient &rhs) { return _basic_client == rhs._basic_client && _port == rhs._port && strcmp(_host, rhs._host) == 0; }

    /**
     * @brief Inequality operator for comparing two BSSL_TCPClient objects.
     * @param rhs The other client object.
     */
    bool operator!=(const BSSL_TCPClient &rhs) { return !this->operator==(rhs); };

private:
    char _host[64];
    uint16_t _port;
    BSSL_SSLClient _ssl_client;
    Client *_basic_client = nullptr;
    // Renaming from _timeout which also defined in parent's Stream class.
    unsigned long _timeout_ms = 15000;
    unsigned long _handshake_timeout = 60000;
    unsigned long _tcp_session_timeout = 0;
};

#endif

#endif