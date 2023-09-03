#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

int main()
{
    nlohmann::json root;

    root["id"]   = 1001;
    root["data"] = "hello";

    const std::string request = root.dump();

    std::cout << "Request is " << request << std::endl;

    const nlohmann::json root2 = nlohmann::json::parse(request);

    std::cout << "ID is " << root2["id"] << " msg is " << root2["data"] << std::endl;

    return 0;
}