#ifndef WPOOL_H
#define WPOOL_H

namespace lib {

template<typename ResultT, typename WorkerFunc>
class WorkersPool
{
public:
    WorkersPool(Result result, WorkerFunc& worker) : m_result(result), m_worker(worker){};

    void PushData(T data);


private:
    ResultT m_result;
    WorkerFunc m_worker;

    mutable std::mutex m_mutex;
    std::conditinal_variable cv;
    std::atomic<bool> done{false};
};
}  // namespace lib

#endif WPOOL_H
