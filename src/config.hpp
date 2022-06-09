#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <list>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include "logger.hpp"
#include "singleton.hpp"
#include "utils.hpp"

namespace mysylar{

class ConfigVariableBase {
public:
    typedef std::shared_ptr<ConfigVariableBase> SharedPtr;
    virtual ~ConfigVariableBase() {}
    const std::string GetName() const { return name_; }
    const std::string GetDescription() const { return description_; }
    virtual const std::string GetValueString() const = 0;
    virtual void SetValueString(const std::string& value_string) = 0;
    virtual const std::string GetTypeName() const = 0;
protected:
    ConfigVariableBase(const std::string& name, const std::string& description)
                        : name_(name), description_(description) {}
    std::string name_;
    std::string description_;

};

/**
 * @brief type cast form type F to type T, only support fundamental types
 * @tparam F from type
 * @tparam T to type
 **/
template<class F, class T>
class StdYamlCast {
public:
    T operator()(const F& from) {
        return boost::lexical_cast<T>(from);
    }
};

/**
 * @brief type cast from string to vector<T>
 * @tparam T 
 **/
template<class T>
class StdYamlCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const std::string& from) {
        std::vector<T> to_vector;
        YAML::Node node = YAML::Load(from);
        std::stringstream context_ss;
        for (size_t it = 0; it < node.size(); it++) {
            context_ss.str("");
            context_ss << node[it];
            to_vector.emplace_back(StdYamlCast<std::string, T>()(context_ss.str()));
        }
        return to_vector;
    }
};

template<class T>
class StdYamlCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node.push_back(StdYamlCast<T, std::string>()(it));
        }
        std::stringstream to;
        to << node;
        return to.str();
    }
};

template<class T>
class StdYamlCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& from) {
        std::list<T> to_list;
        YAML::Node node = YAML::Load(from);
        std::stringstream context_ss;
        for (size_t it = 0; it < node.size(); it++) {
            context_ss.str("");
            context_ss << node[it];
            to_list.emplace_back(StdYamlCast<std::string, T>()(context_ss.str()));
        }
        return to_list;
    }
};

template<class T>
class StdYamlCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node.push_back(StdYamlCast<T, std::string>()(it));
        }
        std::stringstream to;
        to << node;
        return to.str();
    }
};

template<class T>
class StdYamlCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& from) {
        std::set<T> to_set;
        YAML::Node node = YAML::Load(from);
        std::stringstream context_ss;
        for (size_t it = 0; it < node.size(); it++) {
            context_ss.str("");
            context_ss << node[it];
            to_set.insert(StdYamlCast<std::string, T>()(context_ss.str()));
        }
        return to_set;
    }
};

template<class T>
class StdYamlCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node.push_back(StdYamlCast<T, std::string>()(it));
        }
        std::stringstream to;
        to << node;
        return to.str();
    }
};

template<class T>
class StdYamlCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& from) {
        std::map<std::string, T> to_map;
        YAML::Node node = YAML::Load(from);
        std::stringstream second_ss; // to restore the str in `it->second` which can't be easily get by call Scalar method.
        for (auto it = node.begin(); it != node.end(); it++) {
            second_ss.str("");
            second_ss << it->second;
            to_map.insert(std::make_pair(it->first.Scalar(), StdYamlCast<std::string, T>()(second_ss.str())));
        }
        return to_map;
    }
};

template<class T>
class StdYamlCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node[it.first] = YAML::Load(StdYamlCast<T, std::string>()(it.second));
        }
        std::stringstream to;
        to << node;
        return to.str();
    }
};

template<class T>
class StdYamlCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& from) {
        std::set<T> to_set;
        YAML::Node node = YAML::Load(from);
        std::stringstream context_ss;
        for (size_t it = 0; it < node.size(); it++) {
            context_ss.str("");
            context_ss << node[it];
            to_set.insert(StdYamlCast<std::string, T>()(context_ss.str()));
        }
        return to_set;
    }
};

template<class T>
class StdYamlCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node.push_back(StdYamlCast<T, std::string>()(it));
        }
        std::stringstream to;
        to << node;
        return to.str();
    }
};

template<class T>
class StdYamlCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& from) {
        std::unordered_map<std::string, T> to_map;
        YAML::Node node = YAML::Load(from);
        std::stringstream second_ss; // to restore the str in `it->second` which can't be easily get by call Scalar method.
        for (auto it = node.begin(); it != node.end(); it++) {
            second_ss.str("");
            second_ss << it->second;
            to_map.insert(std::make_pair(it->first.Scalar(), StdYamlCast<std::string, T>()(second_ss.str())));
        }
        return to_map;
    }
};

template<class T>
class StdYamlCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& from) {
        YAML::Node node;
        for (const auto& it : from) {
            node[it.first] = YAML::Load(StdYamlCast<T, std::string>()(it.second));
        }
        std::stringstream to;
        to << node;
        return to.str();
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
    typedef std::shared_ptr<ConfigVariable> SharedPtr;
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
    const std::string GetTypeName() const {
        return TypeToName<T>();
    }
private:
    T value_; // the value of the config varible
};

class ConfigManager : public Singleton<ConfigManager> {
template<class T, class ToValue, class ToString>
friend class ConfigVariable;
friend class Singleton<ConfigManager>;
public:
    /**
     * @brief Set the Config object
     * @tparam T 
     * @param name config varible name
     * @param description config varible description
     * @param value config varible value
     **/
    template<class T>
    typename ConfigVariable<T>::SharedPtr SetConfig(const std::string& name, 
                   const std::string& description,
                   const T& value) {
        auto config_base = SearchConfigBase(name);
        if (!config_base) { // doesn't exist
            LRINFO << "\'" << name << "\' doesn't exist, set the value to " 
                << ConfigVariable<T>(name, description, value).GetValueString();  
        } else { // already exist
            auto real_type = config_base->GetTypeName();
            auto get_type = TypeToName<T>();
            if (real_type != get_type) { // type doesn't match
                LRERROR << "Set Config Type doesn't match! Real Type is " 
                    << real_type << ", but get type " << get_type;
                return nullptr;
            }
            config_base->SetValueString(StdYamlCast<T, std::string>()(value));
        }
        return std::dynamic_pointer_cast<ConfigVariable<T> >(SearchConfigBase(name));
    }
    /**
     * @brief search the config object by name
     * @tparam T 
     * @param name config varible name
     * @return ConfigVariable<T>::SharedPtr 
     **/
    ConfigVariableBase::SharedPtr SearchConfigBase(const std::string& name) {
        auto config = configs_.find(name);
        return (config == configs_.end()) ? nullptr : config->second;
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
    ConfigManager() {}
    template<class T>
    void AddConfig(const ConfigVariable<T>& config) {
        auto config_ptr = std::make_shared<ConfigVariable<T> >(config);
        auto config_base_ptr = std::dynamic_pointer_cast<ConfigVariableBase>(config_ptr);
        AddConfig(config_base_ptr);
    }
    void AddConfig(ConfigVariableBase::SharedPtr config_base) {
        auto name = config_base->GetName();
        auto config_exist = configs_.find(name);
        if (config_exist == configs_.end()) { // config doesn't exist
            configs_.insert(std::make_pair(name, config_base));
        } else { // config exists
            configs_[name] = config_base;
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
    auto old_value_string = GetValueString();
    value_ = value; 
    LRINFO << "\'" << name_ << "\' exist, change the value from " << 
        old_value_string << " to " << GetValueString();
    ConfigManager::GetInstance().AddConfig(*this);
}


} //end namespace mysylar