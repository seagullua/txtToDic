#ifndef ADADThreadPool_H
#define ADADThreadPool_H
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

class ADThreadPool
{
public:
    typedef std::function<void ()> Task;
    struct TaskPriority
    {
        Task _task;
        int _priority;
        const bool operator<(const TaskPriority& p) const
        {
            return _priority < p._priority;
        }
    };
    typedef std::priority_queue<TaskPriority> Queue;

    ADThreadPool(const int threads);
    ~ADThreadPool();

    void addTask(const Task& task, const int priority=0);
    void start();
    void setOnStopAction(const Task& task);
    void stop();
    void join();
private:
    void worker();
    int _threads;
    Task _on_stop;
    bool _stop_action_done;
    std::vector< std::thread > workers;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    Queue tasks;
    bool _is_stop;
};

#endif // ADADThreadPool_H
