#ifndef COROTS_COROUTINE_HANDLE_HPP
#define COROTS_COROUTINE_HANDLE_HPP


#include <corots/coroutine_frame_base.hpp>

#include <memory>


namespace corots {


template <class Promise = void> struct coroutine_handle;

template <>
struct coroutine_handle<void> {
  constexpr coroutine_handle() noexcept = default;

  constexpr coroutine_handle(nullptr_t) noexcept : coroutine_handle() {}
  auto operator=(nullptr_t) noexcept -> coroutine_handle& {
    return *this = coroutine_handle();
  }

  constexpr void* address() const noexcept { return ptr_; }
  static auto constexpr from_address(void* addr) -> coroutine_handle {
    return coroutine_handle(addr);
  }

  explicit constexpr operator bool() const noexcept { return ptr_; }
  auto done() const -> bool { return frame().done(); }

  void operator()() const { resume(); }
  void resume() const { frame().resume(); }
  void destroy() const { frame().destroy(); }

protected:
  constexpr coroutine_handle(void* ptr) noexcept : ptr_(ptr) {}

  auto frame() const -> detail::type_erased_coroutine_frame_base& {
    return *reinterpret_cast<detail::type_erased_coroutine_frame_base*>(ptr_);
  }

private:
  void* ptr_ = nullptr;
};


template <class Promise>
struct coroutine_handle : coroutine_handle<> {
  using coroutine_handle<>::coroutine_handle;

  static auto from_promise(Promise& promise) -> coroutine_handle {
    using subobject_t = detail::coroutine_frame_base<Promise>;
    auto constexpr diff = offsetof(subobject_t, promise_);
    auto const member_ptr = reinterpret_cast<char*>(std::addressof(promise));
    auto const subobject_ptr
      = reinterpret_cast<subobject_t*>(member_ptr - diff);
    auto const full_object_ptr = subobject_ptr->self_;
    auto const base_ptr
      = static_cast<detail::type_erased_coroutine_frame_base*>
        (full_object_ptr);
    return coroutine_handle(base_ptr);
  }

  auto promise() const -> Promise& { return frame().promise(); }

private:
  auto frame() const -> detail::type_erased_coroutine_frame<Promise>& {
    auto& base_frame = coroutine_handle<>::frame();
    return
      static_cast<detail::type_erased_coroutine_frame<Promise>&>(base_frame);
  }
};


auto constexpr operator==(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return x.address() == y.address();
}

auto constexpr operator!=(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return !(x == y);
}

auto constexpr operator<(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return std::less<>()(x.address(), y.address());
}

auto constexpr operator>(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return y < x;
}

auto constexpr operator<=(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return !(x > y);
}

auto constexpr operator>=(coroutine_handle<> x, coroutine_handle<> y) noexcept
  -> bool
{
  return !(x < y);
}


} // namespace corots


#endif // COROTS_COROUTINE_HANDLE_HPP
