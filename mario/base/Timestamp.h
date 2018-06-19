#ifndef MARIO_BASE_TIMESTAMP_H
#define MARIO_BASE_TIMESTAMP_H

#include <stdint.h>
#include <string>

namespace mario {

class Timestamp {
public:

    Timestamp();

    explicit Timestamp(int64_t microSecondsSinceEpoch);

    void swap(Timestamp& that) {
        std::swap(_microSecondsSinceEpoch, that._microSecondsSinceEpoch);
    }

    //std::string toString() const;

    std::string toFormattedString() const;

    bool valid() const {
        return _microSecondsSinceEpoch > 0;
    }

    int64_t microSecondsSinceEpoch() const {
        return _microSecondsSinceEpoch;
    }

    static Timestamp now();

    static Timestamp invalid();

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t _microSecondsSinceEpoch;
};

inline bool operator< (Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator== (Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif
