#include <corots/coroutine.hpp>
#include <corots/trivial_awaitables.hpp>
#include <corots/yield.hpp>

#include <iostream>


template <class Value> class generator;


template <class Value>
class generator {
public:
  class promise_type;
  class iterator;

  generator() noexcept = default;

  auto begin() -> iterator {
    if (handle_) { return {}; }
    auto result = iterator(handle_.get());
    return ++result;
  }

  auto end() -> iterator { return iterator(); }

private:
  friend class promise_type;

  using handle_type = corots::coroutine_handle<promise_type>;

  struct destroy_handle {
    using pointer = handle_type;

    void operator()(pointer handle) const noexcept { handle.destroy(); }
  };

  explicit generator(handle_type handle) noexcept : handle_(handle) {}

  std::unique_ptr<handle_type, destroy_handle> handle_;
};


template <class Value>
class generator<Value>::iterator {
private:
  using handle_type = corots::coroutine_handle<generator::promise_type>;

public:
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::size_t;
  using value_type = std::remove_reference_t<Value>;
  using reference = value_type&;
  using pointer = value_type*;

  iterator() noexcept = default;
  explicit iterator(handle_type handle) noexcept : handle_(handle) {}

  auto operator++(int) -> iterator& = delete;
  auto operator++() -> iterator& {
    handle_.resume();
    if (handle_.done()) {
      std::exchange(handle_, {}).promise().rethrow_if_exception();
    }

    return *this;
  }

  auto operator*() const noexcept -> reference {
    return handle_.promise().value();
  }

  auto operator->() const noexcept -> pointer {
    return std::addressof(**this);
  }

  friend auto operator==(iterator const& l, iterator const& r) noexcept -> bool
  {
    return l.handle_ == r.handle_;
  }

  friend auto operator!= (iterator const& l , iterator const& r) noexcept
    -> bool
  { return !(l == r); }

private:
  handle_type handle_;
};


template <class Value>
class generator<Value>::promise_type {
public:
  auto get_return_object() {
    auto const handle = generator::handle_type::from_promise(*this);
    return generator(handle);
  }

  auto constexpr initial_suspend() const { return corots::suspend_always(); }
  auto constexpr final_suspend() const { return corots::suspend_always(); }

  template <class T>
  auto yield_value(T&& value) noexcept {
    value_ = std::addressof(value);
    return corots::suspend_always();
  }

  void return_void() { value_ = nullptr; }

  void unhandled_exception() {
    value_ = nullptr;
    exception_ = std::current_exception();
  }

  void rethrow_if_exception() {
    if (exception_) { std::rethrow_exception(exception_); }
  }

  auto value() const noexcept
    -> std::conditional_t<std::is_reference_v<Value>, Value, Value&>
  { return *value_; }

  template <class U>
  auto await_transform(U&&) -> corots::suspend_never = delete;

private:
  using value_type = std::remove_reference_t<Value>;

  value_type* value_ = nullptr;
  std::exception_ptr exception_;
};


auto constexpr fibonacci = corots::coroutine<generator<int>>([](auto& promise) {
  auto current = 0;
  auto previous = 0;
  for (;;) {
    corots::yield(promise, current);
    current += std::exchange(previous, current);
  }
});


int main() {
  for (auto n: fibonacci()) {
    std::cout << n << ' ';
    if (n > 1'000'000) { break; }
  }
}
