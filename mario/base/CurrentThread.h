#ifndef MARIO_BASE_CURRENTTHREAD_H
#define MARIO_BASE_CURRENTTHREAD_H

#include <pthread.h>
#include <sys/types.h>

namespace mario {

namespace CurrentThread {

extern __thread pid_t t_cachedTid;

void cacheTid();

inline pid_t tid() {
    // 分支预测优化，gcc 引入。
    if (__builtin_expect(t_cachedTid == 0, 0)) {
        cacheTid();
    } 
    return t_cachedTid;
}

}

}

#endif
