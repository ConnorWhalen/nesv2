//
// NES-V2: Config.cpp
//
// Created by Connor Whalen on 2019-05-19.
//

#include "Config.h"

Config::Config(const std::string &override_filename) {
    this->default_config = YAML::LoadFile("default_config.yaml");
    if (!override_filename.empty()) {
        printf("Loading config file %s\n", override_filename.c_str());
        this->override_config = YAML::LoadFile(override_filename);
    } else {
        printf("No config file provided\n");
        this->override_config = YAML::Node();
    }
}

std::string Config::Get(const std::string &key, const std::string &default_value) {
    if (override_config[key]) {
        return override_config[key].as<std::string>();
    } else if (default_config[key]) {
        return default_config[key].as<std::string>();
    }
    printf("Config value %s not found!\nUsing default value: %s\n", key.c_str(), default_value.c_str());
    return default_value;
}

