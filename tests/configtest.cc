#include "../src/logger.hpp"
#include "../src/config.hpp"

using namespace mysylar;
int main() {
    // 1. Add config
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)8080);
    ConfigManager::GetInstance().SetConfig("system.value", "system value", std::vector<int>{1,2});
    // 2. change config 
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)880); // not recommand
    ConfigManager::GetInstance().SearchConfigBase("system.port")->SetValueString("120"); // recommand

    // 3. load config
    auto node = YAML::LoadFile("/home/xac/mysylar/bin/config.yml");
    ConfigManager::ConfigFromYaml(node);

    //auto config = ConfigManager::GetInstance().SearchConfig("system.port");
    LRDEBUG << ConfigManager::GetInstance().SearchConfigBase("system.logger")->GetTypeName();
    LRDEBUG << ConfigManager::GetInstance().SearchConfigBase("system.value")->GetTypeName();

}
