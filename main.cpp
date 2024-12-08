#include <iostream>
#include <string>
#include <unordered_set>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// Credentials
#define CLIENT_ID "your_client_id"
#define CLIENT_SECRET "your_client_secret"
std::unordered_set<std::string> my_orders;

// Function to print header
void printHeader()
{
    std::cout << "\n====================================\n";
    std::cout << "        Deribit Trading Menu         \n";
    std::cout << "====================================\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Cancel Order\n";
    std::cout << "3. Modify Order\n";
    std::cout << "4. Get Order Book\n";
    std::cout << "5. View Current Positions\n";
    std::cout << "6. Exit\n";
}

// Function to print footer
void printFooter()
{
    std::cout << "====================================\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore(); // Ignore leftover newline
    std::cin.get();    // Wait for user to press Enter
}

// Callback function to write the response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

// Function to authenticate and get the access token
std::string authenticate(const std::string &client_id, const std::string &client_secret)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Construct the URL for the GET authentication request
    std::string url = "https://test.deribit.com/api/v2/public/auth?client_id=" + client_id +
                      "&client_secret=" + client_secret +
                      "&grant_type=client_credentials";

    curl = curl_easy_init();

    // failed: SSL peer certificate or SSH remote key was not OK
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Program Files/curl/cacert.pem");

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return "";
        }

        // Parse the JSON response
        try
        {
            auto jsonResponse = json::parse(readBuffer);
            if (jsonResponse.contains("result") && jsonResponse["result"].contains("access_token"))
            {
                std::cout << "** Authentication Done !!! Token Generated successfully **\n";
                std::string access_token = jsonResponse["result"]["access_token"];
                curl_easy_cleanup(curl);
                return access_token;
            }
            else
            {
                std::cerr << "Error: Access token not found in response." << std::endl;
            }
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return "";
}

// Function to place an order
void placeOrder(const std::string &access_token, int amount, bool modify = false)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Define the order parameters
    std::string instrument_name = "BTC-15NOV24-100000-C"; // Update with your instrument
    std::string type = "market";

    // Construct the GET URL with query parameters for the order
    std::string url = "https://test.deribit.com/api/v2/private/buy";
    url += "?instrument_name=" + instrument_name;
    url += "&amount=" + std::to_string(amount);
    url += "&type=" + type;

    // Initialize CURL
    curl = curl_easy_init();

    // failed: SSL peer certificate or SSH remote key was not OK
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Program Files/curl/cacert.pem");

    if (curl)
    {
        // Set the URL with parameters
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the Authorization header with Bearer token
        struct curl_slist *headers = NULL;
        std::string authHeader = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        // Error handling
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Print the response
            if (modify)
            {
                json jsonResponse = json::parse(readBuffer);
                my_orders.insert(jsonResponse["result"]["order"]["order_id"]);
                return;
            }
            std::cout << "*** Order placed successfully ***\n";
            std::cout << "Order Response: " << readBuffer << std::endl;
            json jsonResponse = json::parse(readBuffer);
            my_orders.insert(jsonResponse["result"]["order"]["order_id"]);
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Function to cancel an order
void cancelOrder(const std::string &access_token, const std::string &order_id, bool modify = false)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize CURL
    curl = curl_easy_init();

    // failed: SSL peer certificate or SSH remote key was not OK
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Program Files/curl/cacert.pem");

    if (curl)
    {
        // Construct the URL with the order_id parameter
        std::string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;

        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Authorization header with Bearer token
        std::string authHeader = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Print the response
            if (modify)
            {
                my_orders.erase(order_id);
                return;
            }
            std::cout << "*** Order cancelled successfully ***\n";
            std::cout << "Cancel Order Response: " << readBuffer << std::endl;
            my_orders.erase(order_id);
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

// Function to modify an order
void modifyOrder(const std::string &access_token)
{
    std::cout << "Enter order id to modify\n";
    int i = 1;
    std::string order_id;
    for (auto order : my_orders)
    {
        std::cout << i << ". " << order << "\n";
        i++;
    }
    std::cout << "Order Id: ";
    std::cin >> order_id;
    std::cout << "Enter new Amount: ";
    int amount;
    std::cin >> amount;
    cancelOrder(access_token, order_id, true);
    placeOrder(access_token, amount, true);

    std::cout << "*** Order modified successfully ***\n";
}

// Function to parse and display the order book response
void parseOrderBookResponse(const std::string &response)
{
    try
    {
        // Parse the JSON response
        json jsonResponse = json::parse(response);

        // Check if the result field is present
        if (jsonResponse.contains("result"))
        {
            json result = jsonResponse["result"];

            // Display general instrument info
            std::cout << "Instrument Name: " << result["instrument_name"] << "\n";
            std::cout << "State: " << result["state"] << "\n";
            std::cout << "Underlying Price: " << result["underlying_price"] << "\n";
            std::cout << "Index Price: " << result["index_price"] << "\n\n";

            // Best bid and ask
            std::cout << "Best Bid Price: " << result["best_bid_price"] << "\n";
            std::cout << "Best Ask Price: " << result["best_ask_price"] << "\n";
            std::cout << "Best Bid Amount: " << result["best_bid_amount"] << "\n";
            std::cout << "Best Ask Amount: " << result["best_ask_amount"] << "\n\n";

            // Open interest and market details
            std::cout << "Open Interest: " << result["open_interest"] << "\n";
            std::cout << "Mark Price: " << result["mark_price"] << "\n\n";

            // Display Bids
            std::cout << "Bids:\n";
            for (const auto &bid : result["bids"])
            {
                std::cout << " - Price: " << bid[0] << ", Amount: " << bid[1] << "\n";
            }

            // Display Asks
            std::cout << "\nAsks:\n";
            for (const auto &ask : result["asks"])
            {
                std::cout << " - Price: " << ask[0] << ", Amount: " << ask[1] << "\n";
            }

            // Display Greeks if available
            if (result.contains("greeks"))
            {
                std::cout << "\nGreeks:\n";
                json greeks = result["greeks"];
                std::cout << " - Delta: " << greeks["delta"] << "\n";
                std::cout << " - Gamma: " << greeks["gamma"] << "\n";
                std::cout << " - Vega: " << greeks["vega"] << "\n";
                std::cout << " - Theta: " << greeks["theta"] << "\n";
                std::cout << " - Rho: " << greeks["rho"] << "\n";
            }
        }
        else
        {
            std::cerr << "Error: 'result' field not found in response.\n";
        }
    }
    catch (json::exception &e)
    {
        std::cerr << "JSON Parsing error: " << e.what() << "\n";
    }
}

// Method to fetch order book
void getOrderBook(const std::string &instrument_name)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    // failed: SSL peer certificate or SSH remote key was not OK
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Program Files/curl/cacert.pem");

    if (curl)
    {
        // Construct URL with instrument name
        std::string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument_name;

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Execute the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Print the response
            parseOrderBookResponse(readBuffer);
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
}

// Function to parse and display the current position response
void parsePositionsResponse(const std::string &response)
{
    try
    {
        // Parse the JSON response
        json parsedResponse = json::parse(response);

        // Check if the result is an array
        if (parsedResponse["result"].is_array())
        {
            for (const auto &position : parsedResponse["result"])
            {
                // Extract important fields
                std::string instrumentName = position["instrument_name"];
                double size = position["size"];

                // Explicitly convert to double if the field exists
                double averagePriceUsd = position.contains("average_price_usd")
                                             ? position["average_price_usd"].get<double>()
                                             : 0.0;
                double floatingProfitLoss = position["floating_profit_loss"];
                double realizedProfitLoss = position["realized_profit_loss"];
                std::string direction = position["direction"];

                // Display the important fields
                std::cout << "Instrument Name: " << instrumentName << std::endl;
                std::cout << "Size: " << size << std::endl;
                std::cout << "Average Price (USD): " << averagePriceUsd << std::endl;
                std::cout << "Floating Profit/Loss: " << floatingProfitLoss << std::endl;
                std::cout << "Realized Profit/Loss: " << realizedProfitLoss << std::endl;
                std::cout << "Direction: " << direction << std::endl;
                std::cout << "----------------------------------" << std::endl;
            }
        }
        else
        {
            std::cerr << "Expected an array in the result." << std::endl;
        }
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    catch (const json::out_of_range &e)
    {
        std::cerr << "Access error: " << e.what() << std::endl;
    }
}

// Function to get current positions for a specific instrument
void viewCurrentPositions(const std::string &access_token, const std::string &instrument_name)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    // Initialize CURL
    curl = curl_easy_init();

    // failed: SSL peer certificate or SSH remote key was not OK
    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:/Program Files/curl/cacert.pem");

    if (curl)
    {
        // Set the URL for getting positions
        std::string url = "https://test.deribit.com/api/v2/private/get_positions?instrument_name=" + instrument_name;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Authorization header with Bearer token
        std::string authHeader = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            // Print the response
            parsePositionsResponse(readBuffer);
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    // return readBuffer;  // Return the response for parsing
}

int main()
{
    // Set your client ID and secret here
    const std::string client_id = CLIENT_ID;
    const std::string client_secret = CLIENT_SECRET;

    // Authenticate and get the access token
    std::string access_token = authenticate(client_id, client_secret);

    int choice;
    while (true)
    {
        printHeader(); // Print header with options

        std::cout << "Enter your choice (1-6): ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
        {
            std::cout << "Enter Amount: ";
            int amount = 10;
            std::cin >> amount;
            placeOrder(access_token, amount);
            break;
        }
        case 2:
        {
            std::cout << "Enter order id to cancel\n";
            int i = 1;
            std::string order_id;
            for (auto order : my_orders)
            {
                std::cout << i << ". " << order << "\n";
                i++;
            }
            std::cout << "Order Id: ";
            std::cin >> order_id;
            cancelOrder(access_token, order_id);
            break;
        }
        case 3:
        {
            modifyOrder(access_token);
            break;
        }
        case 4:
        {
            std::cout << "The order book, along with other market values\n";
            getOrderBook("BTC-15NOV24-100000-C");
            break;
        }
        case 5:
        {
            viewCurrentPositions(access_token, "BTC-15NOV24-100000-C");
            break;
        }
        case 6:
        {
            std::cout << "\nExiting the program. Thank you!\n";
            return 0;
        }
        default:
        {
            std::cout << "\nInvalid choice. Please enter a number between 1 and 6.\n";
            break;
        }
        }

        printFooter(); // Print footer after each operation
    }

    return 0;
}
