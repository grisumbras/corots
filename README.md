# corots

> An attempt to implement Coroutines TS (N4760) as a library on top of
> resumable expressions (P0114R0) with some modifications.

## Usage

First, note, that it doesn't actually work, because both resumable object and
`break resumable` (the core functionality of resumable expressions) are
currently stubs. I am planning on integrating Chris Kohlhoff's prototype
(that uses stackfull coroutines from Boost) into this project.

Other than that, there had to be some changes in usage with regards to TS,
because it relies on some code transformations, and a library-only solution
cannot do that.


Where with TS you'd write

```c++
generator<int> my_generator() {
    co_yield 1;
}

int main() {
    for (auto n: my_generator()) {
        std::cout << n;
    }
}
```

with this library you'd write
```c++
#include <corots/coroutine.hpp>

auto constexpr m_generator = corots::coroutine<generator<int>>(
    [](auto& promise){
        corots::yield(promise, 1);
    }
);

int main() {
    for (auto n: my_generator()) {
        std::cout << n;
    }
}
```
