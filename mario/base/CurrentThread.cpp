#include "mario/base/CurrentThread.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace mario {

namespace CurrentThread {

__thread pid_t t_cachedTid = 0;

pid_t gettid() {
    return static_cast<pid_t>(::syscall(SYS_thread_selfid));
    //AUE_GETTID
    //return static_cast<pid_t>(::syscall(SYS_gettid));
}

void cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = gettid();
    }
}

}
}
