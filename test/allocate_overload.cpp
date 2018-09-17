#include "trivial_promise.hpp"

#include <corots/coroutine_frame.hpp>

#include <boost/core/lightweight_test.hpp>


void short_overload() {
  static auto counter = 0;

  struct my_result;
  struct promise : test::trivial_promise<my_result> {
    static void* operator new(std::size_t sz) {
      ++counter;
      return ::operator new(sz);
    }
    static void operator delete(void* ptr) {
      ++counter;
      ::operator delete(ptr);
    }
  };
  struct my_result { using promise_type = promise; };

  BOOST_TEST_EQ(counter, 0);
  auto frame = corots::detail::allocate_coroutine_frame<my_result>([](auto&){});
  BOOST_TEST_NE(frame, nullptr);
  BOOST_TEST_EQ(counter, 1);

  corots::detail::deallocate_coroutine_frame(frame);
  BOOST_TEST_EQ(counter, 2);
}


int main() {
  short_overload();
  return boost::report_errors();
}
