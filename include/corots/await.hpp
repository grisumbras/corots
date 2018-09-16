#ifndef COROTS_AWAIT_HPP
#define COROTS_AWAIT_HPP


#include <corots/detection.hpp>
#include <corots/coroutine_handle.hpp>
#include <corots/on_suspend.hpp>
#include <corots/resumable_object.hpp>


namespace corots {
namespace detail {


template <class P, class T>
using get_await_transform
  = decltype(std::declval<P&>().await_transform(std::declval<T&&>()));

template <class P>
using get_member_await = decltype(std::declval<P&&>().await());


template <class P>
using get_adl_await = decltype(await(std::declval<P&&>()));


template <class Promise, class T>
auto get_awaitable(Promise& promise, T&& value) -> decltype(auto) {
  if constexpr (is_detected_v<get_await_transform, Promise, T>) {
    return promise.await_transform(static_cast<T&&>(value));
  } else {
    return static_cast<T&&>(value);
  }
}


template <class Awaitable>
auto get_awaiter(Awaitable&& awaitable) -> decltype(auto) {
  if constexpr (is_detected_v<get_member_await, Awaitable>) {
    return static_cast<Awaitable&&>(awaitable).await();
  } else if constexpr (is_detected_v<get_adl_await, Awaitable>) {
    return await(static_cast<Awaitable&&>(awaitable));
  } else {
    return static_cast<Awaitable&&>(awaitable);
  }
}


} // namespace detail


template <class Promise, class T>
auto await(Promise& promise, T&& value) {
  auto&& awaitable
    = detail::get_awaitable(promise, static_cast<decltype(value)>(value));
  auto&& awaiter
    = detail::get_awaiter(static_cast<decltype(awaitable)>(awaitable));

  if (!awaiter.await_ready()) {
    using promise_t = std::remove_reference_t<Promise>;
    using handle_t = coroutine_handle<promise_t>;
    auto const handle = handle_t::from_promise(promise);

    using await_suspend_result_t = decltype(awaiter.await_suspend(handle));

    if constexpr (std::is_void_v<await_suspend_result_t>) {
      break_resumable(detail::await_void_tag(), handle, awaiter);
    } else if constexpr (std::is_same_v<bool, await_suspend_result_t>) {
      break_resumable(detail::await_bool_tag(), handle, awaiter);
    } else {
      break_resumable(detail::await_handle_tag(), handle, awaiter);
    }
  }

  return awaiter.await_resume();
}


} // namespace corots


#endif // COROTS_AWAIT_HPP
