#ifndef COROTS_COROUTINE_FRAME_HPP
#define COROTS_COROUTINE_FRAME_HPP


#include <corots/await.hpp>
#include <corots/coroutine_traits.hpp>
#include <corots/coroutine_frame_base.hpp>
#include <corots/resumable_object.hpp>

#include <tuple>
#include <utility>


namespace corots::detail {


template <class Promise>
using has_return_void = decltype(std::declval<Promise&>().return_void());


template
  < class Promise
  , class Body
  , class Args
  >
auto apply_coroutine(Promise& promise, Body body, Args args)
  -> std::enable_if_t<is_detected_v<has_return_void, Promise>>
{
  std::apply(std::move(body), std::move(args));
  promise.return_void();
}


template
  < class Promise
  , class Body
  , class Args
  >
auto apply_coroutine(Promise& promise, Body body, Args args)
  -> std::enable_if_t<!is_detected_v<has_return_void, Promise>>
{
  promise.return_value(std::apply(std::move(body), std::move(args)));
}


template <class Promise, class Body, class... Args>
class coroutine_frame final
  : coroutine_frame_base<Promise>
  , type_erased_coroutine_frame<Promise>
{
public:
  using promise_type = Promise;

  coroutine_frame(Body body, Args ... args)
    : coroutine_frame_base<promise_type>(this)
    , body_(std::move(body))
    , args_
      (std::forward_as_tuple(this->promise(), static_cast<Args&&>(args)...))
  {}

  auto promise() -> promise_type& override { return this->promise_; }

  auto done() -> bool override { return resumable_.ready(); }

  void resume() override { return resumable_.resume(on_suspend()); }

  void destroy() override { deallocate_coroutine_frame(this); }

private:
  auto run_this() {
    return [&, this] {
      auto&& promise = this->promise();
      ::corots::await(promise, promise.initial_suspend());
      try {
        apply_coroutine(promise, std::move(body_), std::move(args_));
      } catch (...) {
        promise.unhandled_exception();
      }
      ::corots::await(promise, promise.final_suspend());
    };
  }

  Body body_;
  std::tuple<promise_type&, Args...> args_;
  resumable_auto_member(resumable_, run_this()());
};


template <class T>
using has_overloaded_operator_new_short
  = decltype(T::operator new(std::size_t()));


template <class T>
using has_overloaded_operator_delete_short
  = decltype(T::operator delete(static_cast<T*>(nullptr)));


template <class R, class Body, class... Args>
auto allocate_coroutine_frame(Body body, Args&& ... args) {
  using promise_t = typename coroutine_traits<R, Args...>::promise_type;
  using frame_t = coroutine_frame<promise_t, Body, Args...>;

  auto constexpr full_size = sizeof(frame_t);
  auto ptr = static_cast<void*>(nullptr);
  if constexpr
    (is_detected_v<has_overloaded_operator_new_short, promise_t>)
  {
    ptr = promise_t::operator new(full_size);
  } else {
    ptr = ::operator new(full_size);
  }

  new (ptr) frame_t(std::move(body), static_cast<Args&&>(args)...);
  return reinterpret_cast<frame_t*>(ptr);
}


template <class Frame>
void deallocate_coroutine_frame(Frame* ptr) {
  using promise_t = typename Frame::promise_type;

  ptr->~Frame();
  if constexpr
    (is_detected_v<has_overloaded_operator_delete_short, promise_t>)
  {
    promise_t::operator delete(ptr);
  } else {
    ::operator delete(ptr, sizeof(Frame));
  }
}


} // namespace corots::detail


#endif // COROTS_COROUTINE_FRAME_HPP
