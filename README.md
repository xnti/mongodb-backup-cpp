# MongoDB Backup in C++
In general, most people want to back up their databases at regular intervals. You can do this in different coding languages, in my opinion the best option was C++.

## Dependencies
There are a few required libraries to compile.

- mongocxx (mongo driver for c++)
- nlohmann/json (json in c++)

## How to compile ? 
You can compile it directly in Microsoft Visual Studio by using the `mongodb-backup-cpp.sln` file.

## How to use it ? 
Examples:
- `.\mongodb-backup-cpp.exe mongodb://127.0.0.1:27017` 
- `.\mongodb-backup-cpp.exe mongodb://username:password@127.0.0.1:27017`

## License
- MIT