#pragma once

#include <string>
#include <utility>

namespace chess::domain {

class PieceTypeId {
public:
    explicit PieceTypeId(std::string value) : value_(std::move(value)) {}

    const std::string& value() const { return value_; }

    friend bool operator==(const PieceTypeId& a, const PieceTypeId& b) { return a.value_ == b.value_; }
    friend bool operator!=(const PieceTypeId& a, const PieceTypeId& b) { return !(a == b); }
    friend bool operator<(const PieceTypeId& a, const PieceTypeId& b) { return a.value_ < b.value_; }

private:
    std::string value_;
};

}
