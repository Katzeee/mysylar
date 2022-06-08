#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include "logger.hpp"
#include "singleton.hpp"

namespace mysylar{

class ConfigVariableBase {
public:
    typedef std::shared_ptr<ConfigVariableBase> SharedPtr;
    virtual ~ConfigVariableBase() {}
    const std::string GetName() const { return name_; }
    const std::string GetDescription() const { return description_; }
    virtual const std::string GetValueString() const = 0;
    virtual void SetValueString(const std::string& value_string) = 0;
protected:
    ConfigVariableBase(const std::string& name, const std::string& description)
                        : name_(name), description_(description) {}
    std::string name_;
    std::string description_;

};

template<class F, class T>
class StdYamlCast {
public:
    T operator()(const F& from) {
        return boost::lexical_cast<T>(from);
    }
};


template<
    class T, 
    class ToValue = StdYamlCast<std::string, T>, 
    class ToString = StdYamlCast<T, std::string> >
class ConfigVariable : public ConfigVariableBase, 
                       public std::enable_shared_from_this<ConfigVariable<T> > {
friend class ConfigManager;
public:
    /**
     * @brief Construct a new Config Variable object, it will be add into config manager automaticly
     * @param name variable name like "system.port"
     * @param description variable description
     * @param value variable value
     **/
    ConfigVariable(
        const std::string& name,
        const std::string& description,
        const T& value); 
    typedef std::shared_ptr<ConfigVariable> SharedPtr;
    /**
     * @brief Once set the new value, inform the manager
     * @param value config value
     **/
    void SetValue(const T& value);
    /**
     * @brief Set the Value from string
     * @param value_string value in string
     **/
    void SetValueString(const std::string& value_string) override {
        try {
            auto value = ToValue()(value_string);
            SetValue(value);
        } catch (std::exception& e) {
            LRERROR << "" << e.what();
        }
    }
    /**
     * @brief Get the Value 
     * @return const T 
     **/
    const T GetValue() const { return value_; }
    /**
     * @brief Get the Value as String 
     * @return const std::string 
     **/
    const std::string GetValueString() const override { 
        try {
            return ToString()(value_); 
        } catch (std::exception& e) {
            LRERROR << "" << e.what();
            return "";
        }
    }
private:
    T value_; // the value of the config varible
};

class ConfigManager : public Singleton<ConfigManager> {
template<class T, class ToValue, class ToString>
friend class ConfigVariable;
public:
    /**
     * @brief Set the Config object
     * @tparam T 
     * @param name config varible name
     * @param description config varible description
     * @param value config varible value
     **/
    template<class T>
    void SetConfig(const std::string& name, 
                   const std::string& description,
                   const T& value) {
        std::make_shared<ConfigVariable<T> >(name, description, value);
    }
    /**
     * @brief search the config object by name
     * @tparam T 
     * @param name config varible name
     * @return ConfigVariable<T>::SharedPtr 
     **/
    ConfigVariableBase::SharedPtr SearchConfig(const std::string& name) {
        auto config = configs_.find(name);
        if (config == configs_.end()) {
            return nullptr;
        } else {
            return config->second;
        }
    }
    /**
     * @brief Load a yaml map node, store in map_node
     * @param prefix 
     * @param node 
     * @param map_node 
     **/
    static void LoadFromNode(
        const std::string& prefix, 
        const YAML::Node& node,
        std::list<std::pair<std::string, YAML::Node> >& map_node); 
    static void ConfigFromYaml(const YAML::Node& root_node);
private:
    std::unordered_map<std::string, ConfigVariableBase::SharedPtr> configs_;
    template<class T>
    void AddConfig(ConfigVariable<T> config) {
        auto config_ptr = std::make_shared<ConfigVariable<T> >(config);
        auto config_base_ptr = std::dynamic_pointer_cast<ConfigVariableBase>(config_ptr);
        AddConfig(config_base_ptr);
    }
    void AddConfig(ConfigVariableBase::SharedPtr config_base) {
        auto name = config_base->GetName();
        auto config_exist = configs_.find(name);
        if (config_exist == configs_.end()) { // config doesn't exist
            configs_.insert(std::make_pair(name, config_base));
            LRINFO << "name: " << name << " doesn't exist, set the value to " 
                << config_base->GetValueString();  
        } else { // config exists
            configs_[name] = config_base;
            LRINFO << "name: " << name << " exist, change the value to " 
                << config_base->GetValueString();  
        }
        configs_.insert(std::make_pair(name, config_base));
    }
};

template<class T, class ToValue, class ToString>
ConfigVariable<T, ToValue, ToString>::ConfigVariable(
    const std::string& name,
    const std::string& description,
    const T& value) :
    ConfigVariableBase(name, description),
    value_(value) {
    ConfigManager::GetInstance().AddConfig(*this);
}

template<class T, class ToValue, class ToString>
void ConfigVariable<T, ToValue, ToString>::SetValue(const T& value) { 
    value_ = value; 
    ConfigManager::GetInstance().AddConfig(*this);
}


} //end namespace mysylar