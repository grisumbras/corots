#ifndef COROTS_COROUTINE_TRAITS_HPP
#define COROTS_COROUTINE_TRAITS_HPP


#include <corots/detection.hpp>


namespace corots {
namespace detail {


struct empty_coroutine_traits {};


template <class Promise>
struct default_coroutine_traits {
  using promise_type = Promise;
};


template <class T>
using get_default_coroutine_traits
  = default_coroutine_traits<typename T::promise_type>;


} // namespace detail


template <class R, class... Args>
struct coroutine_traits
  : detected_or_t
      < detail::empty_coroutine_traits
      , detail::get_default_coroutine_traits
      , R
      >
{};


} // namespace corots


#endif // COROTS_COROUTINE_TRAITS_HPP
