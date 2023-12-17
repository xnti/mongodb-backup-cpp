#include <iostream>
#include <fstream>
#include <filesystem>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "nlohmann/json.hpp"
#include <thread>    // Include for std::this_thread::sleep_for
#include <chrono>    // Include for std::chrono::seconds
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please provide a MongoDB connection URI." << std::endl;
        std::cerr << "Example: " << argv[0] << " mongodb://127.0.0.1:27017" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return 1;
    }

    std::string uri = argv[1];
    std::cout << "Connecting to: " << uri << std::endl;

    mongocxx::instance inst{};
    mongocxx::client conn{ mongocxx::uri{uri} };

    try {
        auto admin = conn["admin"];
        auto cursor = admin.run_command(bsoncxx::builder::stream::document{} << "listDatabases" << 1 << bsoncxx::builder::stream::finalize);

        auto backupDir = fs::current_path() / "backups";
        if (!fs::exists(backupDir)) {
            fs::create_directories(backupDir);
        }

        for (auto&& db : cursor.view()["databases"].get_array().value) {
            auto element = db["name"];  // element is a bsoncxx::document::element
            auto utf8_view = element.get_string().value;
            std::string dbName = std::string(utf8_view.data(), utf8_view.size());
            if (dbName == "admin" || dbName == "config" || dbName == "local")
                continue;

            auto database = conn[dbName];
            auto collections = database.list_collections();

            for (auto&& coll : collections) {
                auto element = coll["name"];  // element is a bsoncxx::document::element
                auto utf8_view = element.get_string().value;
                std::string collName = std::string(utf8_view.data(), utf8_view.size());
                auto collection = database[collName];

                mongocxx::options::find opts;
                auto docs = collection.find({}, opts);
                json j;

                for (auto&& doc : docs) {
                    j.push_back(json::parse(bsoncxx::to_json(doc)));
                }

                std::string filename = dbName + "-" + collName + ".json";
                std::ofstream file(backupDir / filename);
                file << std::setw(2) << j;
            }
        }

        std::cout << "Backup completed successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    return 0;
}
