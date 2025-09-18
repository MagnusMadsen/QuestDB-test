#include <questdb/ingress/line_sender.hpp>
#include <iostream>
#include <chrono>

int main()
{
    try
    {
        // Create a sender using HTTP protocol
        auto sender = questdb::ingress::line_sender::from_conf(
            "http::addr=localhost:9000;username=admin;password=quest;retry_timeout=20000;");

        // Get the current time as a timestamp
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

        // Add rows to the buffer of the sender with the same timestamp
        questdb::ingress::line_sender_buffer buffer;
        buffer
            .table("trades")
            .symbol("symbol", "ETH-USD")
            .symbol("side", "sell")
            .column("price", 2615.54)
            .column("amount", 0.00044)
            .at(questdb::ingress::timestamp_nanos(nanos));

        buffer
            .table("trades")
            .symbol("symbol", "BTC-USD")
            .symbol("side", "sell")
            .column("price", 39269.98)
            .column("amount", 0.001)
            .at(questdb::ingress::timestamp_nanos(nanos));

        // Transactionality check
        if (!buffer.transactional()) {
            std::cerr << "Buffer is not transactional" << std::endl;
            sender.close();
            return 1;
        }

        // Flush and clear the buffer, sending the data to QuestDB
        sender.flush(buffer);

        // Close the connection after all rows ingested
        sender.close();
        return 0;
    }
    catch (const questdb::ingress::line_sender_error& err)
    {
        std::cerr << "Error running example: " << err.what() << std::endl;
        return 1;
    }
}
