#include <cppcoro/task.hpp>
#include <cppcoro/io_service.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <chrono>

cppcoro::task<> sleep_some(cppcoro::io_service& service)
{
  using namespace std::chrono_literals;
  co_await service.schedule_after(1s);
}

cppcoro::task<> do_thing(cppcoro::io_service& service)
{
  co_await sleep_some(service);
  co_await sleep_some(service);
}

int main()
{
  cppcoro::io_service service;
  std::thread ioThread{ [&] { service.process_events(); } };
  cppcoro::sync_wait(do_thing(service));
  service.stop();
  ioThread.join();
  return 0;
}