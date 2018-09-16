#ifndef COROTS_YIELD_HPP
#define COROTS_YIELD_HPP


#include <corots/await.hpp>


namespace corots {


template <class Promise, class Value>
void yield(Promise& promise, Value&& value) {
  await(promise, promise.yield_value(static_cast<Value&&>(value)));
}


} // namespace corots


#endif // COROTS_YIELD_HPP
