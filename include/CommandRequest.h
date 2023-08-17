#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "definitions.h"

using json = nlohmann::json;

struct MCT_API CommandRequest {
    std::string transaction_id;
    std::string command;
    json params;

    CommandRequest()
        : transaction_id("0"), command(""), params(json::object()) {}

    CommandRequest(const std::string& id, const std::string& cmd)
        : transaction_id(id), command(cmd), params(json::object()) {}

    CommandRequest(const std::string& id, const std::string& cmd, const json& prms)
        : transaction_id(id), command(cmd), params(prms) {}
};

MCT_API inline void to_json(json& j, const CommandRequest& p) {
    j = json{{"transaction_id", p.transaction_id}, {"command", p.command}, {"params", p.params}};
}

MCT_API inline void from_json(const json& j, CommandRequest& p) {
    j.at("transaction_id").get_to(p.transaction_id);
    j.at("command").get_to(p.command);
    j.at("params").get_to(p.params);
}
