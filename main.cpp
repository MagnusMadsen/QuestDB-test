#include "deps/c-questdb-client/include/questdb/ingress/line_sender.hpp"
#include <iostream>

int main() {
    try {
        // Create the sender using the connection string
        auto sender = questdb::ingress::line_sender::from_conf("http::addr=localhost:9000;");

        // Get the protocol version for the sender
        auto version = sender.protocol_version();

        // Initialize the buffer with protocol version and buffer size parameters
        size_t initial_buffer_size = 16384;    // 16 KB, adjust as needed
        size_t max_name_length = 64;            // Max name length for symbols and columns
        questdb::ingress::line_sender_buffer buffer(version, initial_buffer_size, max_name_length);

        // Populate the buffer with a sample trade row
        buffer.table("trades")
              .symbol("symbol", "ETH-USD")
              .symbol("side", "sell")
              .column("price", 2615.54)
              .column("amount", 0.00044)
              .at(questdb::ingress::timestamp_nanos::now());
 
        // Send the data
        sender.flush(buffer);

        // Close the sender connection
        sender.close();

        std::cout << "Data sent successfully." << std::endl;
        return 0;
    } catch (const questdb::ingress::line_sender_error& err) {
        std::cerr << "Error sending data to QuestDB: " << err.what() << std::endl;
        return 1;
    }
}
