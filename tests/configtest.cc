#include "../src/logger.hpp"
#include "../src/config.hpp"

using namespace mysylar;

void AddConfigs() {
    // 1. Add config
    auto int_config = ConfigManager::GetInstance().SetConfig("test.int", "system int value", (int)8080);
    auto float_config = ConfigManager::GetInstance().SetConfig("test.float", "system float value", (float)100);
    auto vec_int_config = ConfigManager::GetInstance().SetConfig("test.vector", "system vec int value", std::vector<int>{1,2});
    auto list_int_config = ConfigManager::GetInstance().SetConfig("test.list", "system list int value", std::list<int>{1,2});
    auto set_int_config = ConfigManager::GetInstance().SetConfig("test.set", "system set int value", std::set<int>{30,20,20,30,40,40});
    auto map_string_string_config = ConfigManager::GetInstance().SetConfig("test.map", "system map string string value", std::map<std::string, std::string>{{"i","1"}, {"j","2"}});
    auto unordered_set_int_config = ConfigManager::GetInstance().SetConfig("test.unordered_set", "system unordered set int value", std::set<int>{30,20,20,30,40,40});
    auto unordered_map_string_int_config = ConfigManager::GetInstance().SetConfig("test.unordered_map", "system map string string value", std::map<std::string, int>{{"i",3}, {"j",4}});
    auto grade = ConfigManager::GetInstance().SetConfig("grade", "", std::map<std::string, std::vector<std::string> >());
    auto grade_map = ConfigManager::GetInstance().SetConfig("grade_map", "", std::map<std::string, std::map<std::string, std::string> >());
    // 2. change config 
    int_config->SetValue(80);
    LRINFO << ConfigManager::GetInstance().SetConfig("test.float", "system float value", (float)0.5)->GetTypeName();
    auto xx = ConfigManager::GetInstance().SetConfig("test.float", "system float value", std::string("1.0"));

    // 3. load config
    auto node = YAML::LoadFile("/home/xac/mysylar/bin/config.yml");
    ConfigManager::ConfigFromYaml(node);

    // 4. Inspect config type
#define XX(name) LRINFO << #name << " type: " << name->GetTypeName();
    XX(int_config)
    XX(float_config)
    XX(vec_int_config)
    XX(list_int_config)
    XX(set_int_config)
    XX(map_string_string_config)
    XX(unordered_set_int_config)
    XX(unordered_map_string_int_config)
    XX(grade)
#undef XX

}

struct Person {
    std::string name;
    int age;
    bool operator==(const struct Person& person) const {
        return name == person.name && age == person.age;
    }
};

namespace mysylar {
template<>
class StdYamlCast<std::string, Person> {
public:
    Person operator()(const std::string& from) {
        YAML::Node node = YAML::Load(from);
        Person person;
        person.name = node["name"].as<std::string>();
        person.age = node["age"].as<int>();
        return person;
    }
};

template<>
class StdYamlCast<Person, std::string> {
public:
    std::string operator()(const Person& from) {
        YAML::Node node;
        node["name"] = from.name;
        node["age"] = from.age;
        std::stringstream to;
        to << node;
        return to.str();
    }
};
}


void LoadPersonConfig() {
    auto person_config = ConfigManager::GetInstance().SetConfig("person", "", std::vector<Person>());
    auto node = YAML::LoadFile("/home/xac/mysylar/bin/config.yml");
    ConfigManager::ConfigFromYaml(node);
}

int main() {
    LoggerManager::GetInstance().GetLogger("root")->SetLevel(LogLevel::Level::INFO);
    AddConfigs();
    //LoadPersonConfig();
}
