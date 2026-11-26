#pragma once

#include "domain/Offset.hpp"

namespace chess::domain {

class Position {
public:
    constexpr Position(int file, int rank) : file_(file), rank_(rank) {}

    constexpr int file() const { return file_; }
    constexpr int rank() const { return rank_; }

    constexpr Position operator+(Offset offset) const {
        return Position(file_ + offset.dFile(), rank_ + offset.dRank());
    }

    friend constexpr bool operator==(Position a, Position b) {
        return a.file_ == b.file_ && a.rank_ == b.rank_;
    }
    friend constexpr bool operator!=(Position a, Position b) { return !(a == b); }

    friend constexpr bool operator<(Position a, Position b) {
        return a.file_ != b.file_ ? a.file_ < b.file_ : a.rank_ < b.rank_;
    }

private:
    int file_;
    int rank_;
};

}
