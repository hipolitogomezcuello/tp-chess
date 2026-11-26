#pragma once

#include "domain/Position.hpp"

namespace chess::domain {

class BoardDimensions {
public:
    static constexpr int kStandardSize = 8;

    explicit BoardDimensions(int files = kStandardSize, int ranks = kStandardSize);

    int files() const { return files_; }
    int ranks() const { return ranks_; }

    bool contains(Position position) const;

    friend bool operator==(const BoardDimensions& a, const BoardDimensions& b) {
        return a.files_ == b.files_ && a.ranks_ == b.ranks_;
    }
    friend bool operator!=(const BoardDimensions& a, const BoardDimensions& b) { return !(a == b); }

private:
    int files_;
    int ranks_;
};

}
