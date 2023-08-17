#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "definitions.h"

using json = nlohmann::json;

struct MCT_API CommandResponse {
    std::string transaction_id;
    std::string status;
    bool is_promise;
    json data;

    CommandResponse()
        : transaction_id(""), status(""), is_promise(false), data(json::object()) {}

    CommandResponse(const std::string& id, const std::string& st, bool pr, const json& dt)
        : transaction_id(id), status(st), is_promise(pr), data(dt) {}
};

MCT_API inline void to_json(json& j, const CommandResponse& p) {
    j = json{{"transaction_id", p.transaction_id}, {"status", p.status}, {"is_promise", p.is_promise}, {"data", p.data}};
}

MCT_API inline void from_json(const json& j, CommandResponse& p) {
    j.at("transaction_id").get_to(p.transaction_id);
    j.at("status").get_to(p.status);
    j.at("is_promise").get_to(p.is_promise);
    j.at("data").get_to(p.data);
}


