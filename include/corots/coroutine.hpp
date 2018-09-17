#ifndef COROTS_COROUTINE_HPP
#define COROTS_COROUTINE_HPP


#include <corots/coroutine_frame.hpp>

#include <memory>


namespace corots {
namespace detail {


template <class T>
struct coroutine_return_object {
  auto get() -> T&& { return static_cast<T&&>(value); }

  T value;
};

template <>
struct coroutine_return_object<void> {
  void get() {}
};

template <class Promise>
auto get_return_object(Promise& promise)
  -> coroutine_return_object<decltype(promise.get_return_object())>
{
  if constexpr (std::is_void_v<decltype(promise.get_return_object())>) {
    promise.get_return_object();
    return {};
  } else {
    return {promise.get_return_object()};
  }
}


template <class R, class Factory, class... Args>
auto make_coroutine(Factory& factory, Args&& ... args) -> R {
  auto frame = allocate_coroutine_frame<R>(std::move(factory.body_));
  if (!frame) {
    // TODO: here we are supposed to produce an alternative result
    // via Promise::get_return_object_on_allocation_failure()
    // if that function exists
  }

  auto result = [&]{
    try {
      return get_return_object(frame->promise());
    } catch (...) {
      deallocate_coroutine_frame(frame);
      throw;
    }
  }();
  // at this point, the object stored in result variable is supposed to manage
  // the coroutine frame

  frame->resume();
  return result.get();
  // note, no manual destruction of coroutine frame, the result is supposed to
  // handle that
}


template <class R, class Body>
class coroutine_factory {
public:
  constexpr coroutine_factory(Body body) : body_(std::move(body)) {}

  template <class... Args>
  auto operator()(Args&& ... args) & -> R {
    return make_coroutine<R>(*this, static_cast<Args&&>(args)...);
  }

  template <class... Args>
  auto operator()(Args&& ... args) const& -> R {
    return make_coroutine<R>(*this, static_cast<Args&&>(args)...);
  }


private:
  template <class Returned, class Factory, class... As>
  friend auto make_coroutine(Factory& factory, As&& ... args) -> Returned;

  Body body_;
};


} // namespace detail


template <class R, class Body>
auto constexpr coroutine(Body body) -> detail::coroutine_factory<R, Body> {
  return std::move(body);
}


} // namespace corots


#endif // COROTS_COROUTINE_HPP
