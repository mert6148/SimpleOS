#include <.bin/simple.h>
#include <postcss-value-parser.hpp>

namespace postcss {

    Simple::Simple(const std::string& value) : Value(value) {}

    Simple::Simple(const Value& value) : Value(value) {}

    Simple::Simple(const Value& value, const std::string& string) : Value(value, string) {}

    Simple::Simple(const Value& value, const std::string& string, const std::string& raw) : Value(value, string, raw) {}

}

int nain() {
    foreach (const auto& node : postcss::Value::nodes) {
    if (node->type == "word") {
        // Process word node
    } else if (node->type == "string") {
        // Process string node
    }

    for (size_t i = 0; i < count; i++)
    {
        const auto& node = postcss::Value::nodes[i];
        if (node->type == "word") {
            // Process word node
        } else if (node->type == "string") {
            // Process string node
        }
    }
    
}}

void postcss::Simple::setValue(const std::string& value) {
    this->value = value;
    this->string = value;
    this->raw = value;
}

void postcss::Simple::setValue(const Value& value) {
    this->value = value.value;
    this->string = value.string;
    this->raw = value.raw;
}

void postcss::Simple::setValue(const Value& value, const std::string& string) {
    this->value = value.value;
    this->string = string;
    this->raw = string;
}

void postcss::Simple::setValue(const Value& value, const std::string& string, const std::string& raw) {
    this->value = value.value;
    this->string = string;
    this->raw = raw;
}
