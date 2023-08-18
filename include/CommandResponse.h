#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "definitions.h"

using json = nlohmann::json;

struct BMC_SDK CommandResponse {
    std::string transaction_id;
    std::string status;
    bool is_promise;
    json data;

    CommandResponse()
        : transaction_id(""), status(""), is_promise(false), data(json::object()) {}

    CommandResponse(const std::string& id, const std::string& st, bool pr, const json& dt)
        : transaction_id(id), status(st), is_promise(pr), data(dt) {}
};

BMC_SDK inline void to_json(json& j, const CommandResponse& p) {
    j = json{{"transaction_id", p.transaction_id}, {"status", p.status}, {"is_promise", p.is_promise}, {"data", p.data}};
}

BMC_SDK inline void from_json(const json& j, CommandResponse& p) {
    if (j.at("transaction_id").is_string()) {
        j.at("transaction_id").get_to(p.transaction_id);
    } else if (j.at("transaction_id").is_number()) {
        p.transaction_id = std::to_string(j.at("transaction_id").get<int64_t>());
    } else {
        throw std::runtime_error("transaction_id is neither a string nor a number");
    }

    j.at("status").get_to(p.status);
    j.at("is_promise").get_to(p.is_promise);
    j.at("data").get_to(p.data);
}



