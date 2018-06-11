// TODO. 用 std::atomic 来替代这个抽象
//
#ifndef MARIO_BASE_ATOMIC_H
#define MARIO_BASE_ATOMIC_H

#include <stdint.h>

namespace mario {

template<typename T>
class AtomicIntegerT {

public:
    AtomicIntegerT()
        : _value(0)
    {}

    T get() {
        return __atomic_load_n(&_value, __ATOMIC_SEQ_CST);
    }

    T getAndAdd(T x) {
        return __atomic_fetch_add(&_value, x, __ATOMIC_SEQ_CST);
    }

    T addAndGet(T x) {
        return getAndAdd(x) + x;
    }

    T incrementAndGet() {
        return addAndGet(1);
    }

    T decrementAndGet() {
        return addAndGet(-1);
    }

    void add(T x) {
        getAndAdd(x);
    }

    void increment() {
        incrementAndGet();
    }

    void decrement() {
        decrementAndGet();
    }

    T getAndSet(T newValue) {
        return __atomic_exchange_n(&_value, newValue, __ATOMIC_SEQ_CST);
    }

private:
    // TODO. Learn more about volatile.
    volatile T _value;
};

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

}

#endif
