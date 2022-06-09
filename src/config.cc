#include "config.hpp"

namespace mysylar {


void ConfigManager::LoadFromNode(
    const std::string& prefix, 
    const YAML::Node& node,
    std::list<std::pair<std::string, YAML::Node> >& map_node) {
    std::string node_name;
    map_node.emplace_back(std::make_pair(prefix, node));
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            node_name = prefix.empty() ? it->first.Scalar() : 
                prefix + "." + it->first.Scalar();
            LoadFromNode(node_name, it->second, map_node);
        }
    }
}
void ConfigManager::ConfigFromYaml(const YAML::Node& root_node) {
    std::list<std::pair<std::string, YAML::Node> > map_node;
    ConfigManager::LoadFromNode("", root_node, map_node);
    for (auto& node : map_node) {
        LRDEBUG << node.first << ' ' << node.second.Type() << ' ' << node.second;
        if (node.first.empty()) {
            continue;
        }
        std::stringstream ss;
        ss << node.second;
        auto config_base = ConfigManager::GetInstance().SearchConfigBase(node.first);
        if (config_base)
            config_base->SetValueFromString(ss.str());
    }
}
} // end mysylar