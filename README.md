# Deribit Trading API Client

## Overview

This is a C++ command-line application for interacting with the Deribit cryptocurrency derivatives trading platform. The application provides a menu-driven interface to perform various trading operations such as placing orders, canceling orders, modifying orders, checking order books, and viewing current positions.

## Features

- Authentication with Deribit API
- Place market orders
- Cancel existing orders
- Modify active orders
- Retrieve order book information
- View current trading positions
- Support for options trading

## Prerequisites

- C++ Compiler (C++11 or later)
- libcurl library
- nlohmann/json library
- OpenSSL (for HTTPS support)

## Dependencies

- [libcurl](https://curl.se/libcurl/): For making HTTP requests
- [nlohmann/json](https://github.com/nlohmann/json): For JSON parsing
- [OpenSSL](https://www.openssl.org/): For secure connections

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/amanraj8848/crypto_trading_api.git
   cd crypto_trading_api
   ```

2. Install dependencies:

   - On Ubuntu/Debian:
     ```bash
     sudo apt-get update
     sudo apt-get install libcurl4-openssl-dev libssl-dev
     ```
   - On macOS (using Homebrew):
     ```bash
     brew install curl openssl
     ```
   - On Windows:
     - Use vcpkg or download precompiled libraries for libcurl and OpenSSL

3. Compile the project:
   ```bash
   g++ -std=c++11 main.cpp -lcurl -I/path/to/json/include -o deribit_trader
   ```

## Configuration

1. Replace the placeholder credentials in the source code:

   ```cpp
   #define CLIENT_ID "your_client_id"
   #define CLIENT_SECRET "your_client_secret"
   ```

2. Ensure the SSL certificate path is correct:
   ```cpp
   curl_easy_setopt(curl, CURLOPT_CAINFO, "path/to/cacert.pem");
   ```

## Usage

Run the compiled executable:

```bash
./deribit_trader
```

### Menu Options

1. Place Order
2. Cancel Order
3. Modify Order
4. Get Order Book
5. View Current Positions
6. Exit

## Disclaimer

**WARNING**: Cryptocurrency trading involves significant financial risk. This application is for educational purposes only. Always use caution and never trade with funds you cannot afford to lose.

## Note on Security

- Never share your API credentials
- Use environment variables or secure credential management
- Implement additional security measures in a production environment

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

## Contact

Project Link: [https://github.com/amanraj8848/crypto_trading_api](https://github.com/amanraj8848/crypto_trading_api)

## Acknowledgments

- [libcurl](https://curl.se/libcurl/)
- [nlohmann/json](https://github.com/nlohmann/json)
