#include "../src/logger.hpp"
#include "../src/config.hpp"

using namespace mysylar;
int main() {
    // 1. Add config
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)8080);
    // 2. change config 
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)880);
    ConfigManager::GetInstance().SearchConfig("system.port")->SetValueString("120");

    // 3. load config
    auto node = YAML::LoadFile("/home/xac/mysylar/bin/config.yaml");
    ConfigManager::ConfigFromYaml(node);
}
