//
// NES-V2: Config.h
//
// Created by Connor Whalen on 2019-05-19.
//

#ifndef NESV2_CONFIG_H
#define NESV2_CONFIG_H

#include <string>

#include <yaml-cpp/yaml.h>

class Config {
public:
    Config(const std::string &override_filename);
    std::string Get(const std::string &key, const std::string &default_value = "");
private:
    YAML::Node default_config;
    YAML::Node override_config;
};

#endif //NESV2_CONFIG_H
