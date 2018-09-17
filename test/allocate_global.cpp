#include "trivial_promise.hpp"

#include <corots/coroutine_frame.hpp>

#include <boost/core/lightweight_test.hpp>


auto counter = 0;

auto operator new(std::size_t size) -> void* {
  ++counter;
  return std::malloc(size);
}


void operator delete(void* ptr) {
  ++counter;
  std::free(ptr);
}


void operator delete(void* ptr, std::size_t) {
  ::operator delete(ptr);
}


void allocate_default() {
  counter = 0;
  struct my_result { using promise_type = test::trivial_promise<my_result>; };
  auto frame = corots::detail::allocate_coroutine_frame<my_result>([](auto&){});
  BOOST_TEST_NE(frame, nullptr);
  BOOST_TEST_EQ(counter, 1);
  corots::detail::deallocate_coroutine_frame(frame);
  BOOST_TEST_EQ(counter, 2);
}


int main() {
  allocate_default();
  return boost::report_errors();
}
