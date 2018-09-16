#ifndef COROTS_TRIVIAL_AWAITABLES_HPP
#define COROTS_TRIVIAL_AWAITABLES_HPP


#include <corots/coroutine_handle.hpp>


namespace corots {


struct suspend_never {
  auto constexpr await_ready() const noexcept -> bool { return true; }
  void constexpr await_suspend(coroutine_handle<>) const noexcept {}
  void constexpr await_resume() const noexcept {}
};


struct suspend_always {
  auto constexpr await_ready() const noexcept -> bool { return false; }
  void constexpr await_suspend(coroutine_handle<>) const noexcept {}
  void constexpr await_resume() const noexcept {}
};

} // namespace corots


#endif // COROTS_TRIVIAL_AWAITABLES_HPP
