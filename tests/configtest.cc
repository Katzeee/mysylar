#include "../src/logger.hpp"
#include "../src/config.hpp"

using namespace mysylar;
int main() {
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)8080);
    ConfigManager::GetInstance().SetConfig("system.port", "system port", (int)880);
    ConfigManager::GetInstance().SearchConfig("system.port")->SetValueString("120");
    auto node = YAML::LoadFile("/home/xac/mysylar/bin/config.yaml");
    LRINFO << node.Style() << " " << node.Tag() << " " << node.Type();
    ConfigManager::ConfigFromYaml(node);
}
