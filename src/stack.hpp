#include <cstdint>
#include <cstdio>
#include <type_traits>
#include <utility>
#include "vector.hpp"

namespace algo {
/// The `Stack` class is a container adapter that gives the programmer the
/// functionality of a `stack` - specifically, a `LIFO` (last-in, first-out)
/// data structure.
template <typename T, typename Container = Vector<T>>
class Stack {
public:
    using ContainerType = Container;
    using ValueType = typename Container::ValueType;
    using SizeType = typename Container::SizeType;
    using Reference = typename Container::Reference;
    using ConstReference = typename Container::ConstReference;

    /// Default constructor. Value-initializes the container.
    template <bool C = false,
              std::enable_if_t<C || std::is_default_constructible_v<Container>,
                               int> = 0>
    Stack() : Stack(Container()) {}

    /// Copy-constructs the underlying container `c_` with the contents of
    /// `cont`.
    explicit Stack(const Container& cont) : c_(cont) {}

    /// Move-constructs the underlying container with `std::move(cont)`.
    explicit Stack(Container&& cont) : c_(std::move(cont)) {}

    /// Constructs the underlying container using `alloc` as allocator, as if by
    /// `c_(alloc)`.
    template <typename Alloc>
    explicit Stack(const Alloc& alloc) : c_(alloc) {}

    /// Constructs the underlying container with the contents of `cont` and
    /// using `alloc` as allocator, as if by `c_(cont, alloc)`.
    template <typename Alloc>
    Stack(const Container& cont, const Alloc& alloc) : c_(cont, alloc) {}

    /// Constructs the underlying container with the contents of `cont` using
    /// move semantics while utilizing `alloc` as allocator, as if by
    /// `c_(std::move(cont), alloc)`.
    template <typename Alloc>
    Stack(Container&& cont, const Alloc& alloc) : c_(std::move(cont), alloc) {}

    /// Constructs the underlying container with the contents of `rhs.c_` and
    /// using `alloc` as allocator, as if by `c_(rhs.c_, alloc)`.
    template <typename Alloc>
    Stack(const Stack& rhs, const Alloc& alloc);

    /// Constructs the adaptor with the contents of `rhs` using move semantics
    /// while utilizing `alloc` as allocator, as if by
    /// `c_(std::move(rhs.c_),alloc)`.
    template <typename Alloc>
    Stack(Stack&& rhs, const Alloc& alloc) : c_(std::move(rhs.c_), alloc) {}

    ~Stack() noexcept = default;

    ////////////////////////////////////////////////////////////////////////////
    // Element access
    ////////////////////////////////////////////////////////////////////////////

    /// Returns reference to the top element in the stack. This is the most
    /// recently pushed element. This element will be removed on a call to
    /// `pop()`. Effectively calls c.back().
    Reference top() {
        return c_.back();
    }

    /// const version of `top()`.
    ConstReference top() const {
        return c_.back();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Capacity
    ////////////////////////////////////////////////////////////////////////////

    /// Checks if the underlying container has no elements, i.e. whether
    /// `c.empty()`.
    [[nodiscard]] bool empty() const {
        return c_.empty();
    }

    /// Returns the number of elements in the underlying container, that is,
    /// `c.size()`.
    SizeType size() const {
        return c_.size();
    }

    ////////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////////

    /// Pushes the given element value to the top of the stack. Effectively
    /// calls `c.push_back(value)`.
    void push(const ValueType& value) {
        c_.push_back(value);
    }

    /// Pushes the given element value to the top of the stack. Effectively
    /// calls `c.push_back(std::move(value))`.
    void push(ValueType&& value) {
        c_.push_back(std::move(value));
    }

    /// Pushes a new element on top of the stack. The element is constructed
    /// in-place, i.e. no copy or move operations are performed. The constructor
    /// of the element is called with exactly the same arguments as supplied to
    /// the function. Effectively calls
    /// `c.emplace_back(std::forward<Args>(args)...);`.
    template <typename... Args>
    auto emplace(Args&&... args) {
        return c_.emplace_back(std::forward<Args>(args)...);
    }

    /// Removes the top element.
    void pop() {
        c_.pop_back();
    }

    /// Exchanges the contents of the container adaptor with those of other.
    /// Effectively calls `using std::swap; swap(c, other.c);`.
    void swap(Stack& rhs) noexcept(std::is_nothrow_swappable_v<Container>) {
        using std::swap;
        swap(c_, rhs.c_);
    }

private:
    Container c_;
};
}
