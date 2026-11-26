#pragma once

namespace chess::domain {

class Offset {
public:
    constexpr Offset(int dFile, int dRank) : dFile_(dFile), dRank_(dRank) {}

    constexpr int dFile() const { return dFile_; }
    constexpr int dRank() const { return dRank_; }

    friend constexpr bool operator==(Offset a, Offset b) {
        return a.dFile_ == b.dFile_ && a.dRank_ == b.dRank_;
    }
    friend constexpr bool operator!=(Offset a, Offset b) { return !(a == b); }

private:
    int dFile_;
    int dRank_;
};

}
