#ifndef COROTS_RESUMABLE_OBJECT_HPP
#define COROTS_RESUMABLE_OBJECT_HPP


#include <corots/on_suspend.hpp>

#include <boost/coroutine2/coroutine.hpp>



namespace corots::detail {


using coroutine_type = ::boost::coroutines2::coroutine<on_suspend>;
using coroutine_sink = coroutine_type::push_type;
using coroutine_source = coroutine_type::pull_type;


inline auto current_resumable() -> coroutine_source*& {
  static thread_local coroutine_source* source = nullptr;
  return source;
}


template <class Result>
class resumable_object {
public:
  using result_type = Result;

  resumable_object() = default;

  resumable_object(resumable_object const&) = delete;
  auto operator=(resumable_object const&) -> resumable_object& = delete;

  template <class Body>
  resumable_object(Body body);

  auto ready() const -> bool { return ready_; }

  void resume(on_suspend cbk);

private:
  template <class Body>
  auto make_sink(Body body);

  coroutine_source* source_ = nullptr;
  bool ready_ = false;
  std::exception_ptr exception_;
  coroutine_sink sink_;
};


template <class Result>
template <class Body>
resumable_object<Result>::resumable_object(Body body)
  : sink_(make_sink(std::move(body)))
{ sink_(on_suspend()); }


template <class Result>
void resumable_object<Result>::resume(on_suspend handler) {
  auto const prev = current_resumable();
  try {
    current_resumable() = source_;
    sink_(handler);

    current_resumable() = prev;
    if (exception_) { std::rethrow_exception(exception_); }
  } catch (...) {
    current_resumable() = prev;
    throw;
  }
}


template <class Result>
template <class Body>
auto resumable_object<Result>::make_sink(Body body) {
  return [this, body = std::move(body)](auto& source) {
    source_ = &source;
    (*source_)();
    try {
      body();
      ready_ = true;
    } catch(boost::context::detail::forced_unwind const&) {
      throw;
    } catch(boost::coroutines2::detail::forced_unwind const&) {
      throw;
    } catch (...) {
      exception_ = std::current_exception();
      ready_ = true;
    }
  };
}


template <class... Args>
void break_current_resumable(Args&& ... args) {
  assert(detail::current_resumable());
  auto& resumable = *detail::current_resumable();
  resumable.get()(static_cast<Args&&>(args)...);
  resumable();
}


} // namespace corots::detail


#define resumable_auto(name, ...)                                             \
  ::corots::detail::resumable_object<void> name{                              \
    [&]{ __VA_ARGS__; }}                                                      \

#define resumable_auto_member(name, ...)                                      \
  ::corots::detail::resumable_object<void> name{                              \
    [this]{ __VA_ARGS__; }}                                                   \


#define break_resumable(...)                                                  \
  do { ::corots::detail::break_current_resumable(__VA_ARGS__); } while (0)    \


#endif // COROTS_RESUMABLE_OBJECT_HPP
