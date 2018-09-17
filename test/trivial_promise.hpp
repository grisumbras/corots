#ifndef COROTS_TEST_TRIVIAL_PROMISE_HPP
#define COROTS_TEST_TRIVIAL_PROMISE_HPP


#include <corots/trivial_awaitables.hpp>


namespace test {


template <class R>
struct trivial_promise {
  auto initial_suspend() { return corots::suspend_never(); }
  auto final_suspend() { return corots::suspend_never(); }

  auto get_return_object() { return R(); }
  void return_void() {}
  void unhandled_exception() {}
};


} // namespace test


#endif // COROTS_TEST_TRIVIAL_PROMISE_HPP
