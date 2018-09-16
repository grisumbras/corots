#ifndef COROTS_RESUMABLE_OBJECT_HPP
#define COROTS_RESUMABLE_OBJECT_HPP


namespace corots::detail {


template <class> struct resumable_object {
  resumable_object() = default;
  resumable_object(resumable_object const&) = delete;
  auto operator=(resumable_object const&) -> resumable_object& = delete;

  auto ready() const -> bool { return true; }

  template <class OnSuspend>
  void resume(OnSuspend&&) {}
};


} // namespace corots::detail


#define resumable_auto(name, ...) ::corots::detail::resumable_object<void> name                                                                      \


#define break_resumable(...)


#endif // COROTS_RESUMABLE_OBJECT_HPP
