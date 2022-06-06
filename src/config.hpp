#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "logger.hpp"

class ConfigVaribleBase {
public:
    typedef std::shared_ptr<ConfigVaribleBase> SharedPtr;
    ConfigVaribleBase(const std::string& name, const std::string& description)
                        : name_(name), description_(description) {}
    virtual ~ConfigVaribleBase() {}
private:
    std::string name_;
    std::string description_;

};

template<class T>
class ConfigVarible {
public:
    typedef std::shared_ptr<ConfigVarible> SharedPtr;
    ConfigVarible(const std::string& name,
                  const std::string& description,
                  const T& value) :
                  ConfigVaribleBase(name, description),
                  value_(value) {}
    void SetValue(const T& value) { value_ = value; }
    T GetValue() { return value_; }
    std::string GetValueString() { return NULL;}
private:
    T value_; // the value of the config varible
};

class ConfigManager {
public:
    typedef std::shared_ptr<ConfigManager> SharedPtr;
    template<class T>
    void SetConfig(const std::string& name, 
                   const std::string& description,
                   const T& value) {
        auto config_base = configs_.find(name);
        if (config_base == configs_.end()) {
            typename ConfigVarible<T>::SharedPtr config(
                name, description, value);
            configs_.insert(std::make_pair(name, config));
            LRINFO << "name: " << name << " doesn't exist, set the value to " 
                << config->GetValueString();  
        } else {
            auto config = std::dynamic_pointer_cast<ConfigVarible<T> >(
                    config_base->second);
            config->SetValue(value);
            LRINFO << "name: " << name << " exist, change the value to " 
                << config->GetValueString();  
        }
    }
    template<class T>
    ConfigVaribleBase::SharedPtr SearchConfig(const std::string& name) {
        auto config = configs_.find(name);
        if (config == configs_.end()) {
            return nullptr;
        } else {
            return config->second;
        }
    }
private:
    std::map<std::string, ConfigVaribleBase::SharedPtr> configs_;
};

