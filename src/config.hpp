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
public:
    typedef std::shared_ptr<ConfigVariable> SharedPtr;
    ConfigVariable(const std::string& name,
                  const std::string& description,
                  const T& value) :
                  ConfigVariableBase(name, description),
                  value_(value) {}
    void SetValue(const T& value); 
    void SetValue(const std::string& value_string) {
        try {
            auto value = ToValue()(value_string);
            SetValue(value);
        } catch (std::exception& e) {
            LRERROR << "" << e.what();
        }
    }
    const T GetValue() const { return value_; }
    /**
     * @brief Get the Value as String 
     * @return const std::string 
     **/
    const std::string GetValueString() const { 
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
public:
    //typedef std::shared_ptr<ConfigManager> SharedPtr;
    template<class T>
    void SetConfig(const typename ConfigVariable<T>::SharedPtr config) {}
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
        auto config_base = configs_.find(name);
        if (config_base == configs_.end()) {
            typename ConfigVariable<T>::SharedPtr config(new ConfigVariable<T>(
                name, description, value));
            configs_.insert(std::make_pair(name, config));
            LRINFO << "name: " << name << " doesn't exist, set the value to " 
                << config->GetValueString();  
        } else {
            auto config = std::dynamic_pointer_cast<ConfigVariable<T> >(
                    config_base->second);
            config->SetValue(value);
            LRINFO << "name: " << name << " exist, change the value to " 
                << config->GetValueString();  
        }
    }
    /**
     * @brief search the config object by name
     * @tparam T 
     * @param name config varible name
     * @return ConfigVariable<T>::SharedPtr 
     **/
    template<class T>
    typename ConfigVariable<T>::SharedPtr SearchConfig(const std::string& name) {
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
};

template<
    class T, 
    class ToValue, 
    class ToString>
void ConfigVariable<T, ToValue, ToString>::SetValue(const T& value) { 
    value_ = value; 
    ConfigManager::GetInstance().SetConfig(name_, description_, value_);
}

}