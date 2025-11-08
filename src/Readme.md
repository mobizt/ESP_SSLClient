## 📑 API Reference: `ESP_SSLClient`

### ℹ️ Compatibility Note

The following methods mirror the names and behavior of the **`WiFiClientSecure`** class in the **ESP8266 Arduino Core**, which also relies on the BearSSL library. This allows for seamless migration and feature consistency.

***

### I. 🌐 Connection & Status

| Method | Signature | Description |
| :--- | :--- | :--- |
| **`connect`** | `int connect(IPAddress ip, uint16_t port)` | Initiates a connection to the specified **IP address** and **port**. |
| **`connect`** | `int connect(const char *host, uint16_t port)` | Initiates a connection to the specified **host name** and **port**. |
| **`connect`** | `int connect(IPAddress ip, uint16_t port, int32_t timeout)` | Initiates connection to the **IP** and **port** with a maximum **timeout** (in milliseconds). |
| **`connect`** | `int connect(const char *host, uint16_t port, int32_t timeout)` | Initiates connection to the **host** and **port** with a maximum **timeout** (in milliseconds). |
| **`connectSSL`** | `bool connectSSL()` | **Upgrades** an existing plain TCP connection to **TLS/SSL** using the internally stored host/port details. |
| **`connectSSL`** | `bool connectSSL(const char *host, uint16_t port)` | Compatibility wrapper for TLS handshake; uses **stored** host/port, ignoring parameters. |
| **`connected`** | `uint8_t connected() override` | Returns 1 if the underlying network socket and SSL session are active and ready for I/O. |
| **`stop`** | `void stop() override` | Terminates the connection and releases underlying network and SSL resources. |
| **`isSecure`** | `bool isSecure() const` | Returns `true` if the current session is encrypted (SSL/TLS), `false` if it is plain TCP. |
| **`validate`** | `void validate(const char *host, uint16_t port)` | Validates connection details against the current session; closes connection if **host** or **port** differs. |
| **`operator bool`** | `operator bool() override` | Returns the connection status (`true` if connected). |
| **`operator==`** | `bool operator==(const BSSL_TCPClient &rhs)` | Compares two client objects by checking the network client, port, and stored host. |

***

### II. 💾 Data Transfer & Buffers

| Method | Signature | Description |
| :--- | :--- | :--- |
| **`write`** | `size_t write(const uint8_t *buf, size_t size) override` | Writes **binary data** from a **buffer (`buf`)** of a specified **size** to the connection. |
| **`write`** | `size_t write(const char *buf)` | Writes a null-terminated **C-style string (`buf`)** to the connection. |
| **`write_P`** | `size_t write_P(PGM_P buf, size_t size)` | Writes data from a **Flash/PROGMEM buffer (`buf`)** of a specified **size**. |
| **`read`** | `int read() override` | Reads a single byte (0-255) from the buffer, or returns -1. |
| **`available`** | `int available() override` | Gets the total number of bytes currently available to read. |
| **`peek`** | `int peek() override` | Reads the next available byte without consuming it. |
| **`flush`** | `void flush() override` | Reads and discards all remaining data in the receive buffer. |
| **`availableForWrite`** | `int availableForWrite() override` | Gets the number of bytes currently available for writing in the internal transmit buffer. |
| **`peekBuffer`** | `const char *peekBuffer() EMBED_SSL_ENGINE_BASE_OVERRIDE` | Returns a pointer directly to the internal decrypted application data buffer. |
| **`peekConsume`** | `void peekConsume(size_t consume) EMBED_SSL_ENGINE_BASE_OVERRIDE` | Notifies the SSL engine that **`consume`** bytes have been processed from the peek buffer. |
| **`print`** | `int print(const char *data)` | Prints a null-terminated **C-style string (`data`)**. Returns bytes written. |
| **`print`** | `int print(int data)` | Prints an **integer value (`data`)**. Returns bytes written. |
| **`print`** | `int print(const String &data)` | Prints a **String object (`data`)** (non-AVR only). Returns bytes written. |
| **`println`** | `int println(const char *data)` | Prints a **C-style string (`data`)** followed by CR+LF. |
| **`println`** | `int println(int data)` | Prints an **integer value (`data`)** followed by CR+LF. |
| **`println`** | `int println(const String &data)` | Prints a **String object (`data`)** followed by CR+LF (non-AVR only). |

***

### III. ⚙️ Configuration & Validation

| Method | Signature | Description |
| :--- | :--- | :--- |
| **`setClient`** | `void setClient(Client *client, bool enableSSL)` | Assigns the underlying network **client**; **enableSSL** sets the default security state. |
| **`setSession`** | `void setSession(BearSSL_Session *session)` | Provides a memory location for **TLS session parameters** for faster connection resumption. |
| **`setTimeout`** | `int setTimeout(uint32_t seconds)` | Sets the overall connection **timeout** duration in **seconds**. |
| **`setHandshakeTimeout`** | `void setHandshakeTimeout(unsigned long handshake_timeout)` | Sets the maximum allowed duration for the SSL/TLS **handshake** in **seconds**. |
| **`setSessionTimeout`** | `void setSessionTimeout(uint32_t seconds)` | Sets the maximum **idle time** before the TCP session is re-established (minimum 60s, 0 to disable). |
| **`setDebugLevel`** | `void setDebugLevel(int level)` | Sets the **debug verbosity level** (0 for none). |
| **`setBufferSizes`**| `void setBufferSizes(int recv, int xmit)` | Sets the desired **Receive (`recv`)** and **Transmit (`xmit`)** buffer sizes in bytes. |
| **`setInsecure`** | `void setInsecure()` | Disables certificate verification for insecure connection testing. |
| **`clearAuthenticationSettings`**| `void clearAuthenticationSettings()` | Clears all configured authentication settings (trust anchors, fingerprints, client certs). |
| **`getLastSSLError`**| `int getLastSSLError(char *dest = NULL, size_t len = 0)` | Retrieves the last numeric SSL error code and optionally saves its description to **dest** buffer of **len** size. |
| **`setX509Time`** | `void setX509Time(uint32_t now)` | Sets the current **UNIX epoch time** for certificate validity checking. |
| **`setTrustAnchors`** | `void setTrustAnchors(const X509List *ta)` | Sets the list of trusted certificate authorities (**ta**) for chain validation. |
| **`setKnownKey`** | `void setKnownKey(const PublicKey *pk, unsigned usages)` | Sets a known public key for verification, bypassing certificate chain validation. |
| **`setFingerprint`** | `bool setFingerprint(const uint8_t fingerprint[20])` | Verifies the server certificate's SHA256 **fingerprint** (binary). |
| **`setCertificate`** | `void setCertificate(const char *client_ca)` | Sets the client **certificate buffer** (PEM/DER) for mutual authentication. |
| **`loadCertificate`**| `bool loadCertificate(Stream &stream, size_t size)` | Reads and sets the client **certificate** from an **Arduino Stream**. |
| **`probeMaxFragmentLength`**| `bool probeMaxFragmentLength(const char *host, uint16_t port, uint16_t len)` | Probes the server to determine if a specific Maximum Fragment Length (**MFL**) is supported by **hostname**. |