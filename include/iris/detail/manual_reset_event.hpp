#pragma once

#include <iris/config.hpp>

#include <condition_variable>
#include <mutex>

namespace iris::detail {

class manual_reset_event {
public:
    void set()
    {
        {
            std::unique_lock lock(mutex_);
            set_ = true;
        }

        cv_.notify_all();
    }

    void unset()
    {
        std::unique_lock lock(mutex_);
        set_ = false;
    }

    void wait()
    {
        std::unique_lock lock(mutex_);
        while (!set_) {
            cv_.wait(lock);
        }
    }

private:
    bool set_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}
