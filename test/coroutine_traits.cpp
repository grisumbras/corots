#include <corots/detection.hpp>
#include <corots/coroutine_traits.hpp>

#include <type_traits>


struct c1 { using promise_type = void; };

static_assert(std::is_void_v<corots::coroutine_traits<c1>::promise_type>);


struct c2 { using promise_type = int; };

static_assert(std::is_same_v<int, corots::coroutine_traits<c2>::promise_type>);

static_assert
  (std::is_same_v<int, corots::coroutine_traits<c2, int, char>::promise_type>);


struct c3 {};

template <class T>
using get_nested_type = typename T::type;

static_assert
  (!corots::is_detected_v<get_nested_type, corots::coroutine_traits<c3>>);


struct c4 {};

namespace corots {
template <class... Args>
struct coroutine_traits<c4, Args...> { using promise_type = double; };

template <>
struct coroutine_traits<c4, int>  { using promise_type = float; };
} // namespace corots

static_assert
  (std::is_same_v<double, corots::coroutine_traits<c4>::promise_type>);

static_assert
  (std::is_same_v<double, corots::coroutine_traits<c4, char>::promise_type>);

static_assert
  (std::is_same_v
    <double, corots::coroutine_traits<c4, char, char>::promise_type>
  );

static_assert
  (std::is_same_v<float, corots::coroutine_traits<c4, int>::promise_type>);


int main() {}
