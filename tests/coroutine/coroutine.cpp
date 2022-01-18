#include <coroutine>
#include <chrono>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <future>
#include <assert.h>

template<typename T, size_t Size>
class spsc_queue {
public:
    spsc_queue() : head_(0), tail_(0) {}

    bool push(const T & value)
    {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next_head = next(head);
        if (next_head == tail_.load(std::memory_order_acquire))
            return false;
        ring_[head] = value;
        head_.store(next_head, std::memory_order_release);
        return true;
    }
    bool pop(T & value)
    {
        size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire))
            return false;
        value = ring_[tail];
        tail_.store(next(tail), std::memory_order_release);
        return true;
    }
private:
    size_t next(size_t current)
    {
        return (current + 1) % Size;
    }
    T ring_[Size];
    std::atomic<size_t> head_, tail_;
};

struct message
{
    int request_id = 0;
    bool is_request = true;
    bool requires_reply = true;
    int error_code = 0;
    std::string command;
    std::string payload;
};
class resumable {
public:
    struct promise_type;
    using coro_handle = std::coroutine_handle<promise_type>;
    resumable(coro_handle handle) : handle_(handle) { assert(handle); }
    resumable(resumable&) = delete;
    resumable(resumable&& other)
    {
        std::swap(handle_, other.handle_);
    }
    bool resume() {
        if (not handle_.done())
            handle_.resume();
        return not handle_.done();
    }
    ~resumable() 
    { 
        if(handle_)
        {
            handle_.destroy(); 
        }
    }
    message& return_val();

    struct awaiter 
    {
        bool await_ready() { return false; }
        message await_resume();
        void await_suspend(std::coroutine_handle<> h) {
            h.resume();
            //return false;
        }
        std::coroutine_handle<promise_type> coro;
    };
    awaiter operator co_await() 
    { 
        return awaiter{handle_}; 
    }
    
private:
    coro_handle handle_;
};

struct resumable::promise_type {
    message msg;
    using coro_handle = std::coroutine_handle<promise_type>;
    coro_handle get_return_object() {
        return coro_handle::from_promise(*this);
    }
    auto initial_suspend() { return std::suspend_always(); }
    auto final_suspend() noexcept { return std::suspend_always(); }
    //void return_void() {}
    void return_value(message& msg_) 
    {
        msg = msg_;
    }
    void unhandled_exception() {
        std::terminate();
    }
};

message& resumable::return_val(){
  return handle_.promise().msg;
}

message resumable::awaiter::await_resume() 
{
    return std::move(coro.promise().msg); 
}

///////////////////////////////////////////////////////////

class function_dispatcher {
public:
    struct promise_type;
    using coro_handle = std::coroutine_handle<promise_type>;
    function_dispatcher(coro_handle handle) : handle_(handle) { assert(handle); }
    function_dispatcher(function_dispatcher&) = delete;
    function_dispatcher(function_dispatcher&& other)
    {
        std::swap(handle_, other.handle_);
    }
    bool resume() {
        if (not handle_.done())
            handle_.resume();
        return not handle_.done();
    }
    ~function_dispatcher() 
    { 
        if(handle_)
        {
            handle_.destroy(); 
        }
    }

    struct awaiter 
    {
        bool await_ready() { return false; }
        message await_resume();
        auto await_suspend(std::coroutine_handle<> h) {
            return false;
        }
        std::coroutine_handle<promise_type> coro;
    };
    
private:
    coro_handle handle_;
};

struct function_dispatcher::promise_type {
    using coro_handle = std::coroutine_handle<promise_type>;
    coro_handle get_return_object() {
        return coro_handle::from_promise(*this);
    }
    auto initial_suspend() { return std::suspend_always(); }
    auto final_suspend() noexcept { return std::suspend_always(); }
    void return_void() {}
    void unhandled_exception() {
        std::terminate();
    }
};

///////////////////////////////////////////////////////////

class executor;
message echo_world(message& m, executor& exec);
resumable pong(message& m, executor& exec);

struct awaitable_message;

class executor
{
    using resumable_function = resumable(message& m, executor& exec);
    using callback_lookup = std::unordered_map<int, std::function<void(message&)>>;
    using function_dispatcher_lookup = std::unordered_map<int, function_dispatcher>;
    using garbage_collection = std::vector<int>;
    friend awaitable_message;

    callback_lookup callback_map;
    function_dispatcher_lookup resume_map;
    garbage_collection garbage;
    spsc_queue<message, 200>& send_queue;
    spsc_queue<message, 200>& receive_queue;
    bool& finish_request;
    bool executor_started = false;
    bool executor_finished = false;

public:

    executor(spsc_queue<message, 200>& send_q, spsc_queue<message, 200>& receive_q, bool& finish_r) :
        send_queue(send_q),
        receive_queue(receive_q),
        finish_request(finish_r)
    {}

    auto send_message(message& m)
    {
        struct send_awaitable
        {
            executor& exec;
            message& request_msg;
            message response_msg;
            bool await_ready(){return false;}
            bool await_suspend(std::coroutine_handle<> h) 
            {   
                if(exec.is_shutting_down())
                {
                    return false;
                }
                while(!exec.receive_queue.push(request_msg))
                {
                    if(exec.is_shutting_down())
                    {
                        return false;
                    }
                }
                exec.callback_map.emplace(request_msg.request_id, [&](message& m){
                    response_msg = m;
                    h.resume();
                });
                return true;
            }
            message await_resume() 
            {
                return response_msg;
            }

        };
        return send_awaitable{*this, m};
    }
    
    auto post_message(message& m)
    {
        struct post_awaitable
        {
            executor& exec;
            message& request_msg;
            bool await_ready(){return false;}
            bool await_suspend(std::coroutine_handle<> h) 
            {   
                if(exec.is_shutting_down())
                {
                    return false;
                }
                while(!exec.receive_queue.push(request_msg))
                {
                    if(exec.is_shutting_down())
                    {
                        return false;
                    }
                }
                return false;
            }
            void await_resume() 
            {}

        };
        return post_awaitable{*this, m};
    }
    
    bool is_shutting_down(){return finish_request;}
    bool has_started(){return executor_started;}
    bool has_shut_down(){return executor_finished;}
    
    //for non awaitables
    function_dispatcher message_wrapper(message& m, std::function<message(message&, executor&)> fn)
    {
        auto message_id = m.request_id;
        bool requires_reply = m.requires_reply;
        try
        {
            auto ret = fn(m, *this);

            if(requires_reply)
            {
                co_await this->post_message(ret);
            }
        }
        catch(...)
        {
            garbage.push_back(message_id);
            throw;
        }        

        garbage.push_back(message_id);
        co_return;
    }    

    //for co_awaitables
    function_dispatcher message_wrapper(message& m, std::function<resumable (message&, executor&)> fn)
    {
        auto message_id = m.request_id;
        bool requires_reply = m.requires_reply;
        try
        {
            auto ret = co_await fn(m, *this);

            if(requires_reply)
            {
                co_await this->post_message(ret);
            }
        }
        catch(...)
        {
            garbage.push_back(message_id);
            throw;
        }        
        garbage.push_back(message_id);
        co_return;
    }

    std::thread run_thread()
    {
        std::thread t([&](){

            while(!finish_request)
            {
                message m;
                while(send_queue.pop(m))
                {
                    executor_started  = true;
                    if(m.is_request)
                    {
                        std::pair<function_dispatcher_lookup::iterator,bool> it;
                        if(m.command == "hello")
                        {
                            it = resume_map.emplace(m.request_id, message_wrapper(m, echo_world));
                        }
                        else if(m.command == "ping")
                        {
                            it = resume_map.emplace(m.request_id, message_wrapper(m, pong));
                        }
                        else
                        {                        
                            m.error_code = 1;// we have a dodgy request so report is as such
                            m.payload = "command not recognised";
                            m.is_request = false;
                            while(!receive_queue.push(m))
                            {}
                        }
                        
                        it.first->second.resume();
                    }
                    else
                    {
                        auto cb = callback_map.find(m.request_id);
                        if(cb == callback_map.end())
                        {
                            m.error_code = 1;// we have a dodgy request so report is as such
                            m.payload = "callback not recognised";
                        }                    
                        else
                        {
                            cb->second(m);
                            callback_map.erase(cb);
                        }
                    }   
                    for(int i : garbage)
                    {
                        resume_map.erase(i);
                    }
                    garbage.clear();
                }
                executor_started  = true;
            } 
            executor_finished = true;
        });
        return t;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////

message echo_world(message& m, executor& exec)
{
    message msg = m;
    msg.payload += " world";
    msg.is_request = false;
    return msg;
}

resumable pong(message& m, executor& exec)
{
    message query = m;
    query.command = "pingy";
    query.is_request = false;
    query.payload = "pingy";
    auto ret = co_await exec.send_message(query);
    co_return ret;
}

int main()
{
    bool finish_request = false;
    spsc_queue<message, 200> send_queue;
    spsc_queue<message, 200> receive_queue;

    executor ex(send_queue, receive_queue,finish_request);

    auto t = ex.run_thread();

    int count = 0;
    {
        message m{0, true, true, 0, "hello", "hello"};
        while(!send_queue.push(m))
        {}
        count++;
    }
    {
        message m{0, true, true, 0, "ping", "hello"};
        while(!send_queue.push(m))
        {}
        count++;
    }

    {

        while(!ex.has_shut_down())
        {
            message m;
            while(receive_queue.pop(m))
            {
//                finish_request = true;
                std::cout << m.payload << "\n";
                count--;
            }
        }
            message m;
            while(receive_queue.pop(m))
            {
                std::cout << m.payload << "\n";
                count--;
            }
    }

    t.join();
            message m;
            while(receive_queue.pop(m))
            {
                std::cout << m.payload << "\n";
                count--;
            }

    return 0;
}