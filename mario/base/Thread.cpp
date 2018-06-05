#include "mario/base/Thread.h"
#include "mario/base/CurrentThread.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

using namespace mario;


void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

AtomicInt32 Thread::_numCreated;

Thread::Thread(const ThreadFunc& func, const std::string& name)
    : _started(false),
      _joined(false),
      _pthreadId(0),
      _tid(0),
      // TODO. Learn Move and &&
      _func(func),
      _name(name),
      _latch(1) {

    setDefaultName();
}

Thread::~Thread() {
    if (_started && !_joined) {
        pthread_detach(_pthreadId);
    }
}

void Thread::setDefaultName() {
    int num = _numCreated.incrementAndGet();
    if (_name.empty()) {
        _name = "Thread" + std::to_string(num);
    }
}

void Thread::start() {
    _started = true;

    ThreadData* data = new ThreadData(_func, _name, &_tid, &_latch);
    if (pthread_create(&_pthreadId, NULL, &startThread, data)) {
        _started = false;
        delete data;
        // TODO. LOG FATAL.
    } else {
        _latch.wait();
    }
}

int Thread::join() {
    _joined = true;
    return pthread_join(_pthreadId, NULL);
}

ThreadData::ThreadData(const ThreadFunc& func, const std::string& name,
                       pid_t* tid, CountDownLatch* latch)
    : _func(func),
     _name(name),
     _tid(tid),
     _latch(latch) { 
}

void ThreadData::runInThread() {
    *_tid = CurrentThread::tid(); 
    _tid = NULL;
    _latch->countDown();
    _latch = NULL;
    // TODO. more about prctl.
    try {
        _func();
    } catch (const std::exception& ex) {
        fprintf(stderr, "exception caught in Thread %s\n", _name.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    } catch (...) {
        fprintf(stderr, "unknown exception caught int Thread %s\n", _name.c_str());
        throw;
    }
}
