#include "config.hpp"

namespace mysylar {


void ConfigManager::LoadFromNode(
    const std::string& prefix, 
    const YAML::Node& node,
    std::list<std::pair<std::string, YAML::Node> >& map_node) {
    std::string node_name;
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); it++) {
            node_name = prefix.empty() ? it->first.Scalar() : 
                prefix + "." + it->first.Scalar();
            LoadFromNode(node_name, it->second, map_node);
        }
    } else {
        map_node.emplace_back(std::make_pair(prefix, node));
        // LRINFO << prefix << " : " << node;
    }
}
void ConfigManager::ConfigFromYaml(const YAML::Node& root_node) {
    std::list<std::pair<std::string, YAML::Node> > map_node;
    ConfigManager::LoadFromNode("", root_node, map_node);
    LRINFO << map_node.size();
    for (auto& node : map_node) {
        if (node.second.IsScalar()) {
            LRINFO << node.first;
        }
    }
}
}