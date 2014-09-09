#include "ADThreadPool.h"
ADThreadPool::ADThreadPool(const int threads)
    :
      _threads(threads),
      _stop_action_done(false),
      _is_stop(false)
{
}

ADThreadPool::~ADThreadPool()
{
    //stop();
}

void ADThreadPool::addTask(const Task& task, const int priority)
{
    TaskPriority tp;
    tp._task = task;
    tp._priority = priority;

    { // acquire lock
        std::unique_lock<std::mutex> lock(queue_mutex);

        // add the task
        tasks.push(tp);
    } // release lock

    // wake up one thread
    condition.notify_one();
}

void ADThreadPool::start()
{
    if(workers.size() < _threads)
    {
        for(size_t i = 0;i<_threads;++i)
        {
            workers.push_back(std::thread([this](){
                worker();
            }));
        }
    }
}

void ADThreadPool::join()
{
    for(size_t i = 0;i<workers.size();++i)
        workers[i].join();
}

void ADThreadPool::stop()
{
    bool stop_actions = false;
    { // acquire lock
        std::unique_lock<std::mutex> lock(queue_mutex);
        if(!_is_stop)
        {
            stop_actions = true;
            _is_stop = true;
        }

    }

    if(stop_actions)
    {
        condition.notify_all();

        // join them
        for(size_t i = 0;i<workers.size();++i)
            workers[i].join();
    }
}
void ADThreadPool::setOnStopAction(const Task& task)
{
    _on_stop = task;
}

void ADThreadPool::worker()
{
    TaskPriority task;
    while(true)
    {
        {   // acquire lock
            std::unique_lock<std::mutex>
                    lock(queue_mutex);

            // look for a work item
            while(!_is_stop && tasks.empty())
            { // if there are none wait for notification
                return;
            }


            // get the task from the queue
            task = tasks.top();
            tasks.pop();


        }   // release lock

        //cocos2d::CCLog("Task priority: %d", task._priority);
        // execute the task
        if(task._task)
        {
            task._task();
        }

    }
}
