#ifndef COROTS_TYPE_COROUTINE_FRAME_BASE_HPP
#define COROTS_TYPE_COROUTINE_FRAME_BASE_HPP


namespace corots::detail {


class type_erased_coroutine_frame_base {
public:
  virtual auto done() -> bool = 0;
  virtual void resume() = 0;
  virtual void destroy() = 0;

protected:
  ~type_erased_coroutine_frame_base() = default;
};


template <class Promise>
class type_erased_coroutine_frame : public type_erased_coroutine_frame_base {
public:
  virtual auto promise() -> Promise& = 0;

protected:
  ~type_erased_coroutine_frame() = default;
};


template <class Promise>
struct coroutine_frame_base {
  coroutine_frame_base(type_erased_coroutine_frame<Promise>* self)
    : self_(self)
  {}

  auto promise() -> Promise& { return promise_; }
  auto promise() const -> Promise const& { return promise_; }

  Promise promise_;
  type_erased_coroutine_frame<Promise>* self_;
};




} // namespace corots::detail


#endif // COROTS_TYPE_COROUTINE_FRAME_BASE_HPP
