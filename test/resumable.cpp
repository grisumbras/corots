#include <corots/coroutine_handle.hpp>
#include <corots/resumable_object.hpp>

#include <boost/core/lightweight_test.hpp>


void minimal_resumable() {
  auto counter = 0;
  resumable_auto(r, [&]{ ++counter; }());
  BOOST_TEST_EQ(counter, 0);
  BOOST_TEST(!r.ready());

  r.resume({});
  BOOST_TEST_EQ(counter, 1);
  BOOST_TEST(r.ready());
}


void suspending_resumable() {
  auto counter = 0;
  resumable_auto(r, [&]{
    ++counter;
    break_resumable();
    ++counter;
  }());
  BOOST_TEST_EQ(counter, 0);
  BOOST_TEST(!r.ready());

  r.resume({});
  BOOST_TEST_EQ(counter, 1);
  BOOST_TEST(!r.ready());

  r.resume({});
  BOOST_TEST_EQ(counter, 2);
  BOOST_TEST(r.ready());
}


void destroying_unfinished_resumable() {
  auto counter = 0;
  {
    resumable_auto(r, [&]{
      ++counter;
      break_resumable();
      ++counter;
    }());
    BOOST_TEST_EQ(counter, 0);
    BOOST_TEST(!r.ready());

    r.resume({});
    BOOST_TEST_EQ(counter, 1);
    BOOST_TEST(!r.ready());
  }
  BOOST_TEST_EQ(counter, 1);
}


// void void_suspend() {
//   struct awaiter_type {
//     int& counter;
//
//     auto await_suspend(corots::coroutine_handle<>) { ++counter; }
//   };
//
//   auto counter = 0;
//   resumable_auto(r, [&] {
//     auto awaiter = awaiter_type{counter};
//     break_resumable
//       ( corots::detail::await_void_tag()
//       , corots::coroutine_handle<>()
//       , awaiter
//       );
//     ++counter;
//   }());
//   BOOST_TEST(!r.ready());
//   BOOST_TEST_EQ(counter, 0);
//
//   r.resume({});
//   BOOST_TEST(!r.ready());
//   BOOST_TEST_EQ(counter, 1);
//
//   r.resume({});
//   BOOST_TEST(r.ready());
//   BOOST_TEST_EQ(counter, 2);
// }
//
//
// void bool_suspend() {
//   struct awaiter_type {
//     int& counter;
//     bool resume_immediately;
//
//     auto await_suspend(corots::coroutine_handle<>) {
//       ++counter;
//       return !resume_immediately;
//     }
//   };
//
//   {
//     auto counter = 0;
//     resumable_auto(r, [&] {
//       auto awaiter = awaiter_type{counter, false};
//       break_resumable
//         ( corots::detail::await_bool_tag()
//         , corots::coroutine_handle<>()
//         , awaiter
//         );
//       ++counter;
//     }());
//     BOOST_TEST(!r.ready());
//     BOOST_TEST_EQ(counter, 0);
//
//     r.resume({});
//     BOOST_TEST(!r.ready());
//     BOOST_TEST_EQ(counter, 1);
//
//     r.resume({});
//     BOOST_TEST(r.ready());
//     BOOST_TEST_EQ(counter, 2);
//   }
//
//   {
//     auto counter = 0;
//     resumable_auto(r, [&] {
//       auto awaiter = awaiter_type{counter, true};
//       break_resumable
//         ( corots::detail::await_bool_tag()
//         , corots::coroutine_handle<>()
//         , awaiter
//         );
//       ++counter;
//     }());
//     BOOST_TEST(!r.ready());
//     BOOST_TEST_EQ(counter, 0);
//
//     r.resume({});
//     BOOST_TEST(!r.ready());
//     BOOST_TEST_EQ(counter, 1);
//
//     r.resume({});
//     BOOST_TEST(r.ready());
//     BOOST_TEST_EQ(counter, 2);
//   }
// }


int main() {
  minimal_resumable();
  suspending_resumable();
  destroying_unfinished_resumable();
  // void_suspend();
  // bool_suspend();
  return boost::report_errors();
}
