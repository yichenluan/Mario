#ifndef MARIO_BASE_THREAD_H
#define MARIO_BASE_THREAD_H

#include "mario/base/CountDownLatch.h"
#include "mario/base/Atomic.h"

#include <pthread.h>
#include <functional>
#include <string>


namespace mario {

class Thread {

public:
    // TODO. Learn more about function && bind
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc&, const std::string& name = std::string());

    ~Thread();

    void start();
    int join();

    bool started() const {
        return _started;
    }

    pid_t tid() const {
        return _tid;
    }

    const std::string& name() const {
        return _name;
    }

    static int numCreated() {
        return _numCreated.get();
    }

private:

    void setDefaultName();

    bool            _started;
    bool            _joined;
    pthread_t       _pthreadId;
    pid_t           _tid;
    ThreadFunc      _func;
    std::string     _name;
    CountDownLatch  _latch;

    static AtomicInt32 _numCreated;
};

class ThreadData {

public:
    typedef Thread::ThreadFunc ThreadFunc;

    ThreadData(const ThreadFunc& func, const std::string& name,
               pid_t* tid, CountDownLatch* latch);

    void runInThread();
private:
    ThreadFunc _func;
    std::string _name;
    pid_t* _tid;
    CountDownLatch*  _latch;
};

}

#endif
