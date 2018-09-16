#ifndef COROTS_ON_SUSPEND_HPP
#define COROTS_ON_SUSPEND_HPP


namespace corots::detail {


struct await_void_tag {};

struct await_bool_tag {};

struct await_handle_tag {};


struct on_suspend {
  template <class Handle, class Awaiter>
  void operator()(await_void_tag, Handle handle, Awaiter&& awaiter) {
      awaiter.await_suspend(handle);
  }

  template <class Handle, class Awaiter>
  void operator()(await_bool_tag, Handle handle, Awaiter&& awaiter) {
      if (!awaiter.await_suspend(handle)) { handle.resume(); }
  }

  template <class Handle, class Awaiter>
  void operator()(await_handle_tag, Handle handle, Awaiter&& awaiter) {
      auto const other_handle = awaiter.await_suspend(handle);
      other_handle.resume();
  }
};


} // namespace corots::detail


#endif // COROTS_ON_SUSPEND_HPP
