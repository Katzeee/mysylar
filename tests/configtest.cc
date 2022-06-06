#include "../src/logger.hpp"
#include "../src/config.hpp"

int main() {
    ConfigManager::SharedPtr config_manager(new ConfigManager());
    config_manager->SetConfig("system.port", "system port", (int)8080);
}
