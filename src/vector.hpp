#include <algorithm>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace algo {
/// A standard-like container which offers fixed time access to individual
/// elements in any order.
///
/// In some terminology a `Vector` can be described as a dynamic C-style array,
/// it offers fast and efficient access to individual elements in any order and
/// saves the user from worring about memory and size allocation. Subscripting
/// (`[]`) access is also provided as with C-style arrays.
///
/// # Example
///
/// ```cpp
/// // Create a Vector containing integers
/// Vector<int> v = {7, 5, 16, 8};
///
/// // Add two more integers to Vector
/// v.push_back(25);
/// v.push_back(13);
///
/// assert(v.size() == 6);
/// assert(v.front() == 7);
/// assert(v.back() == 13);
/// ```
template <typename T, typename Allocator = std::allocator<T>>
class Vector {
    // type checks
    static_assert(std::is_copy_constructible_v<T>, "T must be assignable");
    static_assert(std::is_same_v<std::remove_cv_t<T>, T>,
                  "Vector must have a non-const, non-volatile ValueType");
    static_assert(std::is_same_v<typename Allocator::value_type, T>,
                  "Vector must have the same ValueType as its allocator");

    static constexpr bool relocatable =
        std::is_trivially_copyable_v<T> &&
        std::is_same_v<Allocator, std::allocator<T>>;

public:
    using ValueType = T;
    using Pointer = T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using AllocatorType = Allocator;

public:
    /// Default constructor.
    ///
    /// Constructs an empty container with a default-constructed allocator.
    Vector() noexcept(std::is_nothrow_default_constructible_v<AllocatorType>) =
        default;

    /// Constructs an empty container with the given allocator.
    explicit Vector(const AllocatorType& a) noexcept : allocator_(a) {}

    /// Constructs the container with `count` copies of elements with `value`.
    ///
    /// # Example
    ///
    /// ```cpp
    /// Vector<std::string> words(5, "Mo");
    /// assert(words.size() == 5);
    /// assert(words.front() == "Mo");
    /// assert(words.back() == "Mo")
    /// ```
    explicit Vector(SizeType count, const T& value,
                    const AllocatorType& alloc = AllocatorType())
        : allocator_(alloc) {
        create(count);
        finish_ = std::uninitialized_fill_n(start_, count, value);
    }

    /// Constructs the container with `count` **default-inserted** instances of
    /// `T`. No copies are made.
    explicit Vector(SizeType count,
                    const AllocatorType& alloc = AllocatorType())
        : allocator_(alloc) {
        static_assert(std::is_default_constructible_v<T>,
                      "T cannot be default constructible");
        create(count);
        finish_ = std::uninitialized_default_construct_n(start_, count);
    }

    /// Constructs the container with the contents of the range `[first, last)`.
    ///
    /// # Example
    ///
    /// ```cpp
    /// Vector<int> ints = {1, 2, 3};
    /// Vector<int> ints2(ints.begin(), ints.end());
    /// assert(ints2.back() == 3);
    /// ```
    template <
        typename Iter,
        std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<
                                                   Iter>::iterator_category,
                                               std::input_iterator_tag>,
                         int> = 0>
    Vector(Iter first, Iter last, const AllocatorType& alloc = AllocatorType())
        : allocator_(alloc) {
        create(std::distance(first, last));
        finish_ = std::uninitialized_copy(first, last, start_);
    }

    /// Constructs the container with the contents of `rhs`.
    Vector(const Vector& rhs) : Vector(rhs.begin(), rhs.end()) {}

    /// Constructs the container with the contents of `rhs`.
    Vector(const Vector& rhs, const AllocatorType& alloc)
        : Vector(rhs.begin(), rhs.end(), alloc) {}

    /// Move constructor.
    ///
    /// Leaving the `rhs` empty.
    Vector(Vector&& rhs) noexcept
        : start_(rhs.start_), finish_(rhs.finish_),
          end_of_storage_(rhs.end_of_storage_) {
        rhs.start_ = rhs.finish_ = rhs.end_of_storage_ = nullptr;
    }

    /// Please DONT call it.
    Vector(Vector&& rhs, const AllocatorType& alloc) = delete;

    /// Constructs the container with the contents of the initializer list
    /// `init`.
    ///
    /// # Example
    ///
    /// ```cpp
    /// Vector<std::string> words = {"the", "frogurt", "is", "also", "cursed"};
    /// assert(words.back() == "cursed");
    /// ```
    Vector(std::initializer_list<T> init,
           const AllocatorType& alloc = AllocatorType())
        : Vector(init.begin(), init.end(), alloc) {}

    /// Destructs all elements and free the memory.
    ~Vector() noexcept {
        std::destroy(start_, finish_);
        deallocate(start_, capacity());
    }

    /// Copy assignment operator. Replaces the contents with a copy of the
    /// contents of `rhs`.
    Vector& operator=(const Vector& rhs) {
        Vector tmp(rhs);
        tmp.swap(*this);
        return *this;
    }

    /// Move assignment operator. Replaces the contents with `rhs` using move
    /// semantics.
    Vector& operator=(Vector&& rhs) noexcept {
        Vector tmp(std::move(rhs));
        tmp.swap(*this);
        return *this;
    }

    /// Replaces the contents with those identified by initializer list `ilist`.
    Vector& operator=(std::initializer_list<T> ilist) {
        assign(std::move(ilist));
        return *this;
    }

    /// Replaces the contents with `count` copies of value `value`.
    ///
    /// All iterators, pointers and references to the elements of the container
    /// are invalidated. The past-the-end iterator is also invalidated.
    void assign(SizeType count, const T& value) {
        if (count > capacity()) {
            Vector tmp(count, value, getAllocator());
            tmp.swap(*this);
        } else if (count > size()) {
            std::fill(start_, finish_, value);
            const SizeType added = count - size();
            std::uninitialized_fill_n(finish_, added, value);
            finish_ += added;
        } else {
            auto iter = std::fill_n(start_, count, value);
            erase(iter, finish_);
        }
    }

    /// Replaces the contents with copies of those in the range `[first, last)`.
    /// The behavisor is undefined if either argument is an iterator into *this.
    ///
    /// All iterators, pointers and references to the elements of the container
    /// are invalidated. The past-the-end iterator is also invalidated.
    template <
        typename Iter,
        std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<
                                                   Iter>::iterator_category,
                                               std::input_iterator_tag>,
                         int> = 0>
    void assign(Iter first, Iter last) {
        if constexpr (std::is_convertible_v<typename std::iterator_traits<
                                                Iter>::iterator_category,
                                            std::forward_iterator_tag>) {
            const SizeType count = std::distance(first, last);
            if (count > capacity()) {
                Vector tmp(first, last);
                tmp.swap(*this);
            } else if (count > size()) {
                auto p = std::next(first, size());
                const SizeType added = count - size();
                std::copy(first, p, start_);
                std::uninitialized_copy(p, last, finish_);
                finish_ += added;
            } else {
                auto iter = std::copy(first, last, start_);
                erase(iter, finish_);
            }
        } else {
            auto cur = start_;
            while (first != last && cur != finish_) {
                *cur++ = *first++;
            }
            if (first == last) {
                erase(cur, finish_);
            } else {
                insert(end(), first, last);
            }
        }
    }

    /// Replaces the contents with the elements from the initializer list
    /// `ilist`.
    ///
    /// All iterators, pointers and references to the elements of the container
    /// are invalidated. The past-the-end iterator is also invalidated.
    void assign(std::initializer_list<T> ilist) {
        if (ilist.size() > capacity()) {
            Vector tmp(ilist.begin(), ilist.end(), getAllocator());
            tmp.swap(*this);
        } else if (ilist.size() > size()) {
            std::copy_n(ilist.begin(), size(), start_);
            const SizeType added = ilist.size() - size();
            std::uninitialized_copy_n(ilist.begin() + size(), added,
                                      start_ + size());
            finish_ += added;
        } else {
            auto iter = std::copy(ilist.begin(), ilist.end(), start_);
            erase(iter, finish_);
        }
    }

    /// Returns the allocator associated with the container.
    AllocatorType getAllocator() const {
        return allocator_;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Element access
    ////////////////////////////////////////////////////////////////////////////

    /// Subscript access to the data contained in the `Vector`.
    ///
    /// This operator allows for easy, array-style, data access.
    ///
    /// # Note
    ///
    /// All data accesses with this oprator is unchecked and `out_of_range`
    /// lookups are not defined.
    ///
    /// For checked lookups see `at()`.
    Reference operator[](SizeType n) noexcept {
        return *(start_ + n);
    }

    /// Subscript const access to the data contained in the `Vector`.
    ///
    /// This operator allows for easy, array-style, data access.
    ///
    /// # Note
    ///
    /// All data access with this operator is unchecked and `out_of_range`
    /// lookups are not defined.
    ///
    /// For checked lookups see `at()`.
    ConstReference operator[](SizeType n) const noexcept {
        return *(start_ + n);
    }

    /// This function provides for safer data access. The parameter is first
    /// checked that it is in the range of the `Vector`. The function throws
    /// `out_of_range` if the check fails.
    Reference at(SizeType n) {
        rangeCheck(n);
        return (*this)[n];
    }

    /// This function provides for safer data access. The parameter is first
    /// checked that it is in the range of the `Vector`. The function throws
    /// `out_of_range` if the check fails.
    ConstReference at(SizeType n) const {
        rangeCheck(n);
        return (*this)[n];
    }

    /// Returns a read/write reference to the data at the first element of the
    /// `Vector`.
    Reference front() noexcept {
        return *start_;
    }

    /// Returns a read-onlye reference to the data at the first element of the
    /// `Vector`.
    ConstReference front() const noexcept {
        return *start_;
    }

    /// Returns a read/write reference to the data at the last element of the
    /// `Vector`.
    Reference back() noexcept {
        return *(finish_ - 1);
    }

    /// Returns a read-only reference to the data at the last element of the
    /// `Vector`.
    ConstReference back() const noexcept {
        return *(finish_ - 1);
    }

    /// Returns a pointer such that `[data(), data() + size())` is a valid
    /// range. For a non-empty `Vector`, `data() == &front()`.
    T* data() noexcept {
        return start_;
    }

    /// Returns a const pointer such that `[data(), data() + size())` is a valid
    /// range. For a non-empty `Vector`, `data() == &front()`.
    const T* data() const noexcept {
        return start_;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////////

    /// Returns a read/write iterator that points to the first element in the
    /// `Vector`. Iteration is done in ordinary element order.
    ///
    /// If the `Vector` is empty, the returned iterator will be equal to
    /// `end()`.
    Iterator begin() noexcept {
        return start_;
    }

    /// Returns a read-only iterator that points to the first element in the
    /// `Vector`. Iteration is done in ordinary element order.
    ///
    /// If the `Vector` is empty, the returned iterator will be equal to
    /// `cend()`.
    ConstIterator begin() const noexcept {
        return start_;
    }

    /// Returns a read-only iterator that points to the first element in the
    /// `Vector`. Iteration is done in ordinary element order.
    ///
    /// If the `Vector` is empty, the returned iterator will be equal to
    /// `cend()`.
    ConstIterator cbegin() const noexcept {
        return start_;
    }

    /// Returns a read/write iterator that points one past the last element in
    /// the `Vector`. Iteration is done in ordinary element order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    Iterator end() noexcept {
        return finish_;
    }

    /// Returns a read-only iterator that points one past the last element in
    /// the `Vector`. Iteration is done in ordinary element order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    ConstIterator end() const noexcept {
        return finish_;
    }

    /// Returns a read-only iterator that points one past the last element in
    /// the `Vector`. Iteration is done in ordinary element order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    ConstIterator cend() const noexcept {
        return finish_;
    }

    /// Returns a read/write reverse iterator that points to the last element in
    /// the `Vector`. Iteration is done in reverse element order.
    ///
    /// If the `Vector` is empty, the returned iterator is equal to `rend()`.
    ReverseIterator rbegin() noexcept {
        return ReverseIterator(end());
    }

    /// Returns a read-only reverse iterator that points to the last element in
    /// the `Vector`. Iteration is done in reverse element order.
    ///
    /// If the `Vector` is empty, the returned iterator is equal to `rend()`.
    ConstReverseIterator rbegin() const noexcept {
        return ConstReverseIterator(end());
    }

    /// Returns a read-only reverse iterator that points to the last element in
    /// the `Vector`. Iteration is done in reverse element order.
    ///
    /// If the `Vector` is empty, the returned iterator is equal to `rend()`.
    ConstReverseIterator crbegin() const noexcept {
        return ConstReverseIterator(end());
    }

    /// Returns a read/write reverse iterator that points to one before the
    /// first element in the `Vector`. Iteration is done in reverse element
    /// order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    ReverseIterator rend() noexcept {
        return ReverseIterator(begin());
    }

    /// Returns a read-only reverse iterator that points to one before the
    /// first element in the `Vector`. Iteration is done in reverse element
    /// order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    ConstReverseIterator rend() const noexcept {
        return ConstReverseIterator(begin());
    }

    /// Returns a read-only reverse iterator that points to one before the
    /// first element in the `Vector`. Iteration is done in reverse element
    /// order.
    ///
    /// Attempting to access it will result in **undefined** behavior.
    ConstReverseIterator crend() const noexcept {
        return ConstReverseIterator(begin());
    }

    ////////////////////////////////////////////////////////////////////////////
    // Capacity
    ////////////////////////////////////////////////////////////////////////////

    /// Returns true if the `Vector` is empty.
    [[nodiscard]] bool empty() const noexcept {
        return size() == 0;
    }

    /// Returns the number of elements in the `Vector`.
    [[nodiscard]] SizeType size() const noexcept {
        return static_cast<SizeType>(finish_ - start_);
    }

    /// Returns the size() of the largest possible `Vector`.
    [[nodiscard]] SizeType maxSize() const noexcept {
        return ~SizeType(0);
    }

    /// Increase the capacity of the `Vector` to `new_cap`. If `new_cap` is
    /// greater than the current `capacity()`, new storage is allocated,
    /// otherwise the method does nothing.
    ///
    /// **reserve** does not change the size of the `Vector`.
    ///
    /// If `new_cap` is greater than `capacity()`, all iterators, including the
    /// pass the end iterator, and all references to the elements are
    /// invalidated. Otherwise, no iterators or references are invalidated.
    void reserve(SizeType new_cap) {
        if (new_cap > maxSize()) {
            throw std::length_error("Vector::reserve too large capacity");
        }
        if (new_cap > capacity()) {
            growShrinkAux(new_cap);
        }
    }

    /// Returns the total number of elements that `Vector` can hold before
    /// needing to allocate more memory.
    [[nodiscard]] SizeType capacity() const noexcept {
        return static_cast<SizeType>(end_of_storage_ - start_);
    }

    /// Requests the removal of unused capacity.
    ///
    /// It is a non-binding requests to reduce `capacity()` to `size()`. If
    /// reallocation occurs, all iterators, including the past the end iterator,
    /// and all references to the elements are invalidated. If no reallocation
    /// takes place, no iterators or references are invalidated.
    void shrink() {
        const SizeType old_size = size();
        growShrinkAux(old_size);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////////

    /// Erases all elements from the container. After this call, `size()`
    /// returns zero.
    ///
    /// Invalidates any references, pointers, or iterators referring to
    /// contained elements. Any past the end iterators are also invalidated.
    ///
    /// Leaving the `capacity()` of the `Vector` unchanged.
    void clear() noexcept {
        std::destroy(start_, finish_);
        finish_ = start_;
    }

    /// Inserts given `value` into `Vector` before `pos`.
    ///
    /// # Note
    ///
    /// This kind of operation could be expensive for a `Vector` and if it is
    /// frequently used then user should consider using `List`.
    Iterator insert(ConstIterator pos, const T& value) {
        // Guarantee that `value` is not a reference to an element of this
        // `Vector`
        addressCheck(std::addressof(value));

        Iterator p = begin() + (pos - cbegin());
        if (finish_ != end_of_storage_) {
            if (p == finish_) {
                new (finish_) T(value);
                ++finish_;
            } else {
                new (finish_) T(std::move(*(finish_ - 1)));
                ++finish_;
                std::move_backward(p, finish_ - 2, finish_ - 1);
                *p = value;
            }
        } else {
            p = insertExpandAux(p, value);
        }
        return p;
    }

    /// Inserts `value` before `pos`. rvalue version.
    ///
    /// # Note
    ///
    /// This kind of operation could be expensive for a `Vector` and if it is
    /// frequently used then user should consider using `List`.
    Iterator insert(ConstIterator pos, T&& value) {
        Iterator p = begin() + (pos - cbegin());
        if (finish_ != end_of_storage_) {
            if (p == finish_) {
                new (finish_) T(std::move(value));
                ++finish_;
            } else {
                new (finish_) T(std::move(*(finish_ - 1)));
                ++finish_;
                std::move_backward(p, finish_ - 2, finish_ - 1);
                *p = std::move(value);
            }
        } else {
            p = insertExpandAux(p, std::move(value));
        }
        return p;
    }

    /// Inserts `count` copies of the `value` before pos.
    Iterator insert(ConstIterator pos, SizeType count, const T& value) {
        Iterator p = begin() + (pos - begin());
        if (count == 0) {
            return p;
        } else if (count == 1) {
            return insert(pos, value);
        }

        if (finish_ + count <= end_of_storage_) {
            if (p == finish_) {
                std::uninitialized_fill_n(finish_, count, value);
            } else {
                const SizeType m = finish_ - p;
                if (count == m) {
                    std::uninitialized_move(p, finish_, finish_);
                    std::fill_n(p, count, value);
                } else if (count < m) {
                    std::uninitialized_move(finish_ - count, finish_, finish_);
                    std::move_backward(p, finish_ - count, finish_);
                    std::fill(p, finish_ - count, value);
                } else {
                    std::uninitialized_move(p, finish_, finish_ + (count - m));
                    std::fill(p, finish_, value);
                    std::uninitialized_fill_n(finish_, count - m, value);
                }
            }
            finish_ += count;
        } else {
            p = insertRangeExpandAux(p, count, value);
        }
        return p;
    }

    /// Inserts elements from range `[first, last)` before pos.
    template <
        typename Iter,
        std::enable_if_t<std::is_convertible_v<typename std::iterator_traits<
                                                   Iter>::iterator_category,
                                               std::input_iterator_tag>,
                         int> = 0>
    Iterator insert(ConstIterator pos, Iter first, Iter last) {
        if (first == last) {
            return begin() + (pos - cbegin());
        }

        const SizeType offset = pos - cbegin();
        insertRangeAux(
            begin() + offset, first, last,
            typename std::iterator_traits<Iter>::iterator_category{});
        return begin() + offset;
    }

    /// Inserts elements from initializer list `ilist` before pos.
    Iterator insert(ConstIterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /// Inserts a new element into the container before `pos`. The element is
    /// constructed through placement-new. The arguments `args...` are forwarded
    /// to the constructor as `std::forward<Args>(args)...`.
    ///
    /// If the new `size()` is greater than `capacity()`, all iteratos and
    /// references are invalidted. Otherwise, only the iterators and references
    /// before the insertion point remain valid. The past-the-end iterator is
    /// also invalidated.
    template <typename... Args>
    Iterator emplace(ConstIterator pos, Args&&... args) {
        Iterator p = begin() + (pos - cbegin());
        if (finish_ != end_of_storage_) {
            if (p == finish_) {
                new (finish_) T(std::forward<Args>(args)...);
                ++finish_;
            } else {
                new (finish_) T(std::move(*(finish_ - 1)));
                ++finish_;
                std::move_backward(p, finish_ - 2, finish_ - 1);
                *p = T(std::forward<Args>(args)...);
            }
        } else {
            p = insertExpandAux(p, std::forward<Args>(args)...);
        }
        return p;
    }

    /// Erases the element at `pos`. It will invalidate iterators and references
    /// at or after the point of the erase, including the `end()` iterator. The
    /// iterator `pos` must be valid and dereferenceable. Thus the `end()`
    /// iterator (which is valid, but is not dereferenceable) cannot be used as
    /// a value for `pos`.
    ///
    /// Iterator following the last removed element is returned. If `pos` refers
    /// to the last element, then the `end()` iterator is returned.
    Iterator erase(ConstIterator pos) {
        Iterator p = begin() + (pos - cbegin());
        if (p + 1 != end()) {
            std::move(p + 1, end(), p);
        }
        --finish_;
        std::destroy_at(finish_);
        return p;
    }

    /// Erases the elements in the range `[first, last)`. It will invalidate
    /// iterators and references at or after the point of the erase, including
    /// the `end()` iterator. The iterator `pos` must be valid and
    /// dereferenceable. Thus the `end()` iterator (which is valid, but is not
    /// dereferenceable) cannot be used as a value for `pos`.
    ///
    /// Iterator following the last removed element is returned. If `last ==
    /// end()` prior to removal, then the updated `end()` iterator is returned.
    /// If `[first, last)` is an empty range, the `last` is returned.
    Iterator erase(ConstIterator first, ConstIterator last) {
        Iterator pfirst = begin() + (first - cbegin()),
                 plast = begin() + (last - cbegin());
        if (pfirst != plast) {
            if (plast == end()) {
                std::destroy(pfirst, plast);
                finish_ = pfirst;
            } else {
                std::move(plast, end(), pfirst);
                std::destroy(pfirst + (end() - plast), finish_);
                finish_ = pfirst + (end() - plast);
            }
        }
        return pfirst;
    }

    /// Add data to the end of the `Vector`.
    ///
    /// This is a typical stack operation. The function creates an element at
    /// the end of the `Vector` and assigns the given data to it. Due to the
    /// nature of a `Vector` this operation can be done in constant time if the
    /// `Vector` has preallocated space available.
    void push_back(const ValueType& e) {
        if (finish_ == end_of_storage_) {
            growShrinkAux(capacity() * 2 + 1);
        }

        new (finish_) T(e);
        ++finish_;
    }

    /// Move version of `push_back()`.
    void push_back(ValueType&& e) {
        if (finish_ == end_of_storage_) {
            growShrinkAux(capacity() * 2 + 1);
        }

        new (finish_) T(std::move(e));
        ++finish_;
    }

    /// Appends a new element to the end of the container. The element is
    /// construted through `std::allocator_traits::construct`, which typically
    /// uses placement-new to construct the element in-place at the location
    /// provided by the container. The arguments `args...` are forwarded to the
    /// constructor as `std::forward<Args>(args)...`.
    ///
    /// If the new `size()` is greater than `capacity()` then all iterators and
    /// references (including the past-the-end iterator) are invalidated.
    /// Otherwise only pass-the-end iterator is invalidated.
    template <typename... Args>
    Reference emplace_back(Args&&... args) {
        if (finish_ == end_of_storage_) {
            growShrinkAux(capacity() * 2 + 1);
        }

        new (finish_) T(std::forward<Args>(args)...);
        ++finish_;
        return back();
    }

    /// Removes the last element.
    ///
    /// This is a typical stack operation. It shrinks the `Vector` by one.
    ///
    /// # Note
    ///
    /// No data is returned, and if the last element's data is needed, it should
    /// be retrieved before `pop_back()` is called.
    void pop_back() noexcept {
        --finish_;
        std::destroy_at(finish_);
    }

    /// Resizes the `Vector` to the specified number of elements. If the number
    /// is smaller than the `Vector`'s current size the `Vector` is truncated,
    /// otherwise default constructed elements are appended.
    void resize(SizeType new_size) {
        if (new_size < size()) {
            std::destroy(start_ + new_size, finish_);
        } else if (new_size > size() && new_size < capacity()) {
            std::uninitialized_default_construct_n(finish_, new_size - size());
        } else {
            const SizeType old_size = size();
            growShrinkAux(new_size);
            std::uninitialized_default_construct_n(finish_,
                                                   new_size - old_size);
        }
        finish_ = start_ + new_size;
    }

    /// Swaps data with another `Vector`.
    ///
    /// This exchanges the elements between two `Vector`s in constant time.
    /// (Tree pointers, so it should be quite fast.)
    ///
    /// # Note
    ///
    /// The global `std::swap()` function is specialized such that
    /// `std::swap(v1, v2)` will feed to this function.
    void swap(Vector& rhs) noexcept {
        using std::swap;

        auto a = std::move(rhs.allocator_);
        rhs.allocator_ = std::move(allocator_);
        allocator_ = std::move(a);

        swap(start_, rhs.start_);
        swap(finish_, rhs.finish_);
        swap(end_of_storage_, rhs.end_of_storage_);
    }

private:
    void addressCheck(ConstIterator p) {
        if (p >= start_ && p < finish_) {
            throw std::range_error("Vector:addressCheck failed");
        }
    }

    void rangeCheck(SizeType n) {
        if (n >= size()) {
            throw std::out_of_range("Vector::rangeCheck failed");
        }
    }

    void deallocate(Pointer p, SizeType sz) {
        if (p) {
            if constexpr (relocatable) {
                std::free(p);
            } else {
                allocator_.deallocate(p, sz);
            }
        }
    }

    Pointer allocate(SizeType sz) {
        if constexpr (relocatable) {
            if (sz == 0) {
                return Pointer();
            }

            // consistent with the behavior of allocator
            auto p = (Pointer)std::malloc(sz * sizeof(T));
            if (!p) {
                throw std::bad_alloc();
            }
            return p;
        } else {
            return sz != 0 ? allocator_.allocate(sz) : Pointer();
        }
    }

    void create(SizeType sz) {
        start_ = allocate(sz);
        finish_ = start_;
        end_of_storage_ = start_ + sz;
    }

    // When calling `push_back()` and `emplace_back()`, `size()` may already be
    // equal to `capacity()`. The memory region expands and pushes the new
    // element at the end.
    //
    // When calling `shrinkToFit()`, the memory should be shrinked to `size()`.
    void growShrinkAux(SizeType sz) {
        assert(sz >= size());

        const SizeType old_size = size();
        Pointer tmp;
        if constexpr (relocatable) {
            tmp = (Pointer)std::realloc(start_, sz * sizeof(T));
            if (!tmp) {
                throw std::bad_alloc();
            }
        } else if constexpr (std::is_trivially_copyable_v<T>) {
            tmp = allocate(sz);
            __builtin_memmove(tmp, start_, old_size * sizeof(T));
            deallocate(start_, end_of_storage_ - start_);
        } else {
            tmp = allocate(sz);
            try {
                std::uninitialized_copy(make_move_if_noexcept_iterator(start_),
                                        make_move_if_noexcept_iterator(finish_),
                                        tmp);
            } catch (...) {
                deallocate(tmp, sz);
                throw;
            }
            std::destroy(start_, finish_);
            deallocate(start_, end_of_storage_ - start_);
        }

        start_ = tmp;
        finish_ = tmp + old_size;
        end_of_storage_ = start_ + sz;
    }

    template <typename... Args>
    Iterator insertExpandAux(Iterator pos, Args&&... args) {
        const SizeType offset = pos - begin();
        const SizeType old_size = size(); // equal to capacity()
        const SizeType sz = old_size * 2 + 1;

        Pointer tmp = allocate(sz);
        if constexpr (std::is_trivially_copyable_v<T>) {
            __builtin_memmove(tmp, start_, offset * sizeof(T));
            tmp[offset] = T(std::forward<Args>(args)...);
            __builtin_memmove(tmp + offset + 1, start_ + offset,
                              (old_size - offset) * sizeof(T));
            deallocate(start_, old_size);
        } else {
            try {
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_),
                    make_move_if_noexcept_iterator(start_ + offset), tmp);
                new (tmp + offset) T(std::forward<Args>(args)...);
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_ + offset),
                    make_move_if_noexcept_iterator(finish_), tmp + offset + 1);
            } catch (...) {
                deallocate(tmp, sz);
                throw;
            }
            std::destroy(start_, finish_);
            deallocate(start_, end_of_storage_ - start_);
        }

        start_ = tmp;
        finish_ = start_ + old_size + 1;
        end_of_storage_ = start_ + sz;
        return start_ + offset;
    }

    Iterator insertRangeExpandAux(Iterator pos, SizeType count,
                                  const T& value) {
        const SizeType offset = pos - begin();
        const SizeType old_size = size();
        const SizeType sz = old_size + std::max(count, old_size) + 1;

        Pointer tmp = allocate(sz);
        if constexpr (std::is_trivially_copyable_v<T>) {
            __builtin_memmove(tmp, start_, offset * sizeof(T));
            std::fill_n(tmp + offset, count, value);
            __builtin_memmove(tmp + offset + count, start_ + offset,
                              (old_size - offset) * sizeof(T));
            deallocate(start_, old_size);
        } else {
            try {
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_),
                    make_move_if_noexcept_iterator(start_ + offset), tmp);
                std::uninitialized_fill_n(tmp + offset, count, value);
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_ + offset),
                    make_move_if_noexcept_iterator(finish_),
                    tmp + offset + count);
            } catch (...) {
                deallocate(tmp, sz);
                throw;
            }
            std::destroy(start_, finish_);
            deallocate(start_, end_of_storage_ - start_);
        }

        start_ = tmp;
        finish_ = start_ + old_size + count;
        end_of_storage_ = start_ + sz;
        return start_ + offset;
    }

    template <typename Iter>
    Iterator insertRangeExpandAux(Iterator pos, SizeType count, Iter first,
                                  Iter last) {
        const SizeType offset = pos - begin();
        const SizeType old_size = size();
        const SizeType sz = old_size + std::max(count, old_size) + 1;

        Pointer tmp = allocate(sz);
        if constexpr (std::is_trivially_copyable_v<T>) {
            __builtin_memmove(tmp, start_, offset * sizeof(T));
            std::copy(first, last, tmp + offset);
            __builtin_memmove(tmp + offset + count, start_ + offset,
                              (old_size - offset) * sizeof(T));
            deallocate(start_, old_size);
        } else {
            try {
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_),
                    make_move_if_noexcept_iterator(start_ + offset), tmp);
                std::uninitialized_copy(first, last, tmp + offset);
                std::uninitialized_copy(
                    make_move_if_noexcept_iterator(start_ + offset),
                    make_move_if_noexcept_iterator(finish_),
                    tmp + offset + count);
            } catch (...) {
                deallocate(tmp, sz);
                throw;
            }
            std::destroy(start_, finish_);
            deallocate(start_, end_of_storage_ - start_);
        }

        start_ = tmp;
        finish_ = start_ + old_size + count;
        end_of_storage_ = start_ + sz;
        return start_ + offset;
    }

    template <typename Iter>
    void insertRangeAux(Iterator pos, Iter first, Iter last,
                        std::input_iterator_tag) {
        if (pos == finish_) {
            while (first != last) {
                insert(end(), *first++);
            }
        } else {
            Vector tmp(first, last);
            insert(pos, make_move_if_noexcept_iterator(tmp.begin()),
                   make_move_if_noexcept_iterator(tmp.end()));
        }
    }

    template <typename Iter>
    void insertRangeAux(Iterator pos, Iter first, Iter last,
                        std::forward_iterator_tag) {
        const SizeType count = std::distance(first, last);
        if (finish_ + count <= end_of_storage_) {
            if (pos == finish_) {
                std::uninitialized_copy(first, last, finish_);
            } else {
                const SizeType m = finish_ - pos;
                if (count == m) {
                    std::uninitialized_move(pos, finish_, finish_);
                    std::copy(first, last, finish_);
                } else if (count < m) {
                    std::uninitialized_move(finish_ - count, finish_, finish_);
                    std::move_backward(pos, finish_ - count, finish_);
                    std::copy(first, last, pos);
                } else {
                    std::uninitialized_move(pos, finish_,
                                            finish_ + (count - m));
                    auto p1 = std::next(first, m);
                    std::copy(first, p1, pos);
                    std::uninitialized_copy(p1, last, finish_);
                }
            }
            finish_ += count;
        } else {
            insertRangeExpandAux(pos, count, first, last);
        }
    }

    // iterator extension
    template <typename Iter,
              typename ReturnType = std::conditional_t<
                  std::is_nothrow_move_constructible_v<
                      typename std::iterator_traits<Iter>::value_type>,
                  std::move_iterator<Iter>, Iter>>
    constexpr ReturnType make_move_if_noexcept_iterator(Iter it) noexcept {
        return ReturnType(it);
    }

    template <typename U, typename ReturnType = std::conditional_t<
                              std::is_nothrow_move_constructible_v<U>,
                              std::move_iterator<U*>, const U*>>
    constexpr ReturnType make_move_if_noexcept_iterator(U* p) noexcept {
        return ReturnType(p);
    }

private:
    [[no_unique_address]] AllocatorType allocator_;
    Pointer start_ = nullptr;
    Pointer finish_ = nullptr;
    Pointer end_of_storage_ = nullptr;
};

/// `Vector` equality comparison.
///
/// `Vector`s are considered equaivalent if their sizes are equal, and if
/// corresponding elements compare equal.
template <typename T, typename Allocator>
inline bool operator==(const Vector<T, Allocator>& x,
                       const Vector<T, Allocator>& y) {
    return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
}

/// Based on operator==
template <typename T, typename Allocator>
inline bool operator!=(const Vector<T, Allocator>& x,
                       const Vector<T, Allocator>& y) {
    return !(x == y);
}

/// `Vector` ordering relation.
///
/// This is a total ordering relation.
template <typename T, typename Allocator>
inline bool operator<(const Vector<T, Allocator>& x,
                      const Vector<T, Allocator>& y) {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

/// Based on operator<
template <typename T, typename Allocator>
inline bool operator>(const Vector<T, Allocator>& x,
                      const Vector<T, Allocator>& y) {
    return y < x;
}

/// Based on operator<
template <typename T, typename Allocator>
inline bool operator>=(const Vector<T, Allocator>& x,
                       const Vector<T, Allocator>& y) {
    return !(x < y);
}

/// Based on operator<
template <typename T, typename Allocator>
inline bool operator<=(const Vector<T, Allocator>& x,
                       const Vector<T, Allocator>& y) {
    return !(y < x);
}

/// Swap two `Vector`s
///
/// See `Vector::swap` for more information.
template <typename T, typename Allocator>
inline void swap(Vector<T, Allocator>& x, Vector<T, Allocator>& y) noexcept {
    x.swap(y);
}
} // namespace algo
