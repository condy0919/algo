#define CATCH_CONFIG_MAIN
#include "vector.hpp"
#include <catch2/catch.hpp>
#include <memory>
#include <string>

using namespace algo;

TEST_CASE("types") {
    struct foo {};
    using Vec = Vector<foo>;
    REQUIRE(std::is_same_v<Vec::ValueType, foo>);
    REQUIRE(std::is_same_v<Vec::Pointer, foo*>);
    REQUIRE(std::is_same_v<Vec::Reference, foo&>);
    REQUIRE(std::is_same_v<Vec::ConstReference, const foo&>);
    REQUIRE(std::is_same_v<Vec::Iterator, foo*>);
    REQUIRE(std::is_same_v<Vec::ConstIterator, const foo*>);
    REQUIRE(std::is_same_v<Vec::SizeType, std::size_t>);
    REQUIRE(std::is_same_v<Vec::DifferenceType, std::ptrdiff_t>);
    REQUIRE(std::is_same_v<Vec::AllocatorType, std::allocator<foo>>);
}

TEST_CASE("default constructor") {
    Vector<int> vec;
    REQUIRE(vec.empty());
    REQUIRE(vec.begin() == vec.end());
}

TEST_CASE("constructor with an allocator") {
    Vector<int> vec{std::allocator<int>()};
}

TEST_CASE("constructor with n copies with specified value") {
    Vector<int> vec(5, 12);
    REQUIRE(vec[0] == 12);
    REQUIRE(vec[1] == 12);
    REQUIRE(vec[2] == 12);
    REQUIRE(vec[3] == 12);
    REQUIRE(vec[4] == 12);
}

TEST_CASE("constructor with n defaulted copies") {
    Vector<int> vec(5);
}

TEST_CASE("constructor with ranges") {
    const Vector<int> list = {1, 2, 3, 4};

    Vector<int> vec(list.begin(), list.end());
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("copy constructor") {
    const Vector<int> cp = {1, 2, 3, 4};

    Vector<int> vec(cp);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
    REQUIRE(vec[3] == 4);
}

TEST_CASE("move constructor") {
    Vector<int> v1 = {1, 2, 3, 4};
    Vector<int> v2(std::move(v1));
    REQUIRE(v1.empty());
    REQUIRE(v2.size() == 4);
}

TEST_CASE("copy assignment") {
    Vector<int> empty;
    const Vector<int> xs = {1, 2, 3};

    SECTION("trivial types copy assignment") {
        empty = xs;
        REQUIRE(empty.size() == 3);
        REQUIRE(xs.size() == 3);
        REQUIRE(empty[0] == xs[0]);
        REQUIRE(empty[1] == xs[1]);
        REQUIRE(empty[2] == xs[2]);
    }

    Vector<std::string> empty2;
    const Vector<std::string> ys = {"foo", "bar", "baz"};

    SECTION("non-trivial types copy assignment") {
        empty2 = ys;
        REQUIRE(empty2.size() == 3);
        REQUIRE(ys.size() == 3);
        REQUIRE(empty2[0] == ys[0]);
        REQUIRE(empty2[1] == ys[1]);
        REQUIRE(empty2[2] == ys[2]);
    }
}

TEST_CASE("move assignment") {
    Vector<int> empty;
    Vector<int> xs = {1, 2, 3};

    SECTION("trivial types move assignment") {
        empty = std::move(xs);
        REQUIRE(empty.size() == 3);
        REQUIRE(xs.empty());
        REQUIRE(empty[0] == 1);
        REQUIRE(empty[1] == 2);
        REQUIRE(empty[2] == 3);
    }

    Vector<std::string> empty2;
    Vector<std::string> ys = {"foo", "bar", "baz"};

    SECTION("non-trivial types move assignment") {
        empty2 = std::move(ys);
        REQUIRE(empty2.size() == 3);
        REQUIRE(ys.empty());
        REQUIRE(empty2[0] == "foo");
        REQUIRE(empty2[1] == "bar");
        REQUIRE(empty2[2] == "baz");
    }
}

TEST_CASE("initialize list assignment") {
    Vector<int> empty;

    SECTION("trivial types initialize list assignment") {
        empty = {1, 2, 3};
        REQUIRE(empty.size() == 3);
        REQUIRE(empty[0] == 1);
        REQUIRE(empty[1] == 2);
        REQUIRE(empty[2] == 3);
    }

    Vector<std::string> empty2;

    SECTION("non-trivial types initialize list assignment") {
        empty2 = {"foo", "bar", "baz"};
        REQUIRE(empty2.size() == 3);
        REQUIRE(empty2[0] == "foo");
        REQUIRE(empty2[1] == "bar");
        REQUIRE(empty2[2] == "baz");
    }
}

TEST_CASE("assign with n copies") {
    Vector<int> vec;

    SECTION("trivial types assign") {
        vec.assign(3, -1);
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == -1);
        REQUIRE(vec[1] == -1);
        REQUIRE(vec[2] == -1);

        vec.assign(1, 0);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec[0] == 0);
    }

    Vector<std::string> svec;

    SECTION("non-trivial types assign") {
        svec.assign(3, "foo");
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "foo");
        REQUIRE(svec[2] == "foo");

        svec.assign(1, "baz");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec[0] == "baz");
    }
}

TEST_CASE("assign with range elemtns") {
    Vector<int> vec;
    const Vector<int> xs = {-1, 0, 1};

    const int K = 417;
    const int A[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
    const int B[] = {K, K, K, K, K};
    const std::size_t N = sizeof(A) / sizeof(int);
    const std::size_t M = sizeof(B) / sizeof(int);

    SECTION("assign from pointer range") {
        vec.assign(A, A + N);
        REQUIRE(vec.size() == N);
        REQUIRE(std::equal(vec.begin(), vec.end(), A));
    }

    SECTION("trivial types assign") {
        vec.assign(xs.begin(), xs.end());
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == -1);
        REQUIRE(vec[1] == 0);
        REQUIRE(vec[2] == 1);

        vec.assign(xs.begin() + 1, xs.end());
        REQUIRE(vec.size() == 2);
        REQUIRE(vec[0] == 0);
        REQUIRE(vec[1] == 1);
    }

    Vector<std::string> svec;
    const Vector<std::string> ys = {"foo", "bar", "baz"};

    SECTION("non-trivial types assign") {
        svec.assign(ys.begin(), ys.end());
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "bar");
        REQUIRE(svec[2] == "baz");

        svec.assign(ys.begin() + 1, ys.end());
        REQUIRE(svec.size() == 2);
        REQUIRE(svec[0] == "bar");
        REQUIRE(svec[1] == "baz");
    }
}

TEST_CASE("assgin with an initializer list") {
    Vector<int> vec;

    SECTION("trivial types assign") {
        vec.assign({1, 2, 3});
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 2);
        REQUIRE(vec[2] == 3);

        vec.assign({1, 1, 1, 1});
        REQUIRE(vec.size() == 4);
        REQUIRE(vec.front() == 1);
        REQUIRE(vec.back() == 1);
    }

    Vector<std::string> svec;

    SECTION("non-trivial types assign") {
        svec.assign({"foo", "bar"});
        REQUIRE(svec.size() == 2);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "bar");

        svec.assign({"baz"});
        REQUIRE(svec.size() == 1);
        REQUIRE(svec[0] == "baz");
    }
}

TEST_CASE("out of range access") {
    Vector<int> empty;
    REQUIRE_THROWS(empty.at(10));
}

TEST_CASE("front & back") {
    Vector<int> v = {1, 2, 3, 4};
    REQUIRE(v.front() == 1);
    REQUIRE(v.back() == 4);
}

TEST_CASE("begin & end") {
    Vector<int> v = {1, 2, 3, 4};
    REQUIRE(std::accumulate(v.begin(), v.end(), 0) == 10);
    REQUIRE(std::distance(v.begin(), v.end()) == 4);
}

TEST_CASE("rbegin & rend") {
    Vector<int> v = {1, 2, 3, 4};
    auto iter = v.rbegin();
    REQUIRE(*iter++ == 4);
    REQUIRE(*iter++ == 3);
    REQUIRE(*iter++ == 2);
    REQUIRE(*iter++ == 1);
}

TEST_CASE("reserve") {
    Vector<int> v = {1, 2, 3, 4};
    v.reserve(10);
    REQUIRE(v.capacity() >= 10);
    REQUIRE(v[0] == 1);
    REQUIRE(v[1] == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(v[3] == 4);

    Vector<std::string> svec;
    REQUIRE(svec.empty());
    svec.push_back("foo");
    svec.push_back("bar");
    REQUIRE(svec.size() == 2);

    svec.reserve(10);
    REQUIRE(svec[0] == "foo");
    REQUIRE(svec[1] == "bar");
}

TEST_CASE("clear") {
    Vector<int> v = {1, 2, 3, 4};
    REQUIRE(v.size() == 4);

    v.clear();
    REQUIRE(v.empty());
}

TEST_CASE("shrink") {
    Vector<std::string> svec;
    REQUIRE(svec.empty());

    svec.push_back("foo");
    svec.push_back("bar");
    REQUIRE(svec.size() == 2);
    REQUIRE(svec.capacity() >= 2);

    svec.push_back("baz");
    REQUIRE(svec.size() == 3);
    REQUIRE(svec.capacity() >= 3);

    svec.shrink();
    REQUIRE(svec.size() == 3);
    REQUIRE(svec.capacity() == 3);
}

TEST_CASE("erase") {
    Vector<int> vec = {1, 2, 3, 4, 5};
    SECTION("erase one trivial element") {
        auto iter = vec.erase(vec.begin());
        REQUIRE(*iter == 2);
        REQUIRE(vec.size() == 4);
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 3);
        REQUIRE(vec[2] == 4);
        REQUIRE(vec[3] == 5);
    }
    SECTION("erase the last trivial element") {
        auto iter = vec.erase(vec.begin() + 4);
        REQUIRE(iter == vec.end());
        REQUIRE(vec.size() == 4);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 2);
        REQUIRE(vec[2] == 3);
        REQUIRE(vec[3] == 4);
    }
    SECTION("erase trivial elements in range [first, last)") {
        auto iter = vec.erase(vec.begin() + 1, vec.begin() + 3);
        REQUIRE(*iter == 4);
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 4);
        REQUIRE(vec[2] == 5);
    }
    SECTION("erase all") {
        vec.erase(vec.cbegin(), vec.cend());
        REQUIRE(vec.empty());
    }

    Vector<std::string> svec = {"foo", "bar", "baz"};
    SECTION("erase one non-trivial element") {
        auto iter = svec.erase(svec.begin());
        REQUIRE(*iter == "bar");
        REQUIRE(svec.size() == 2);
        REQUIRE(svec[0] == "bar");
        REQUIRE(svec[1] == "baz");
    }
    SECTION("erase the last non-trivial element") {
        auto iter = svec.erase(svec.begin() + 2);
        REQUIRE(iter == svec.end());
        REQUIRE(svec.size() == 2);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "bar");
    }
    SECTION("erase non-trivial elements in range [first, last)") {
        auto iter = svec.erase(svec.begin() + 1, svec.begin() + 2);
        REQUIRE(*iter == "baz");
        REQUIRE(svec.size() == 2);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "baz");
    }
}

TEST_CASE("erase from gcc tests") {
    const int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const int A1[] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const int A2[] = {0, 2, 3, 4, 10, 11, 12, 13, 14, 15};
    const int A3[] = {0, 2, 3, 4, 10, 11};
    const int A4[] = {4, 10, 11};
    const int A5[] = {4, 10};
    const unsigned int N = sizeof(A) / sizeof(int);
    const unsigned int N1 = sizeof(A1) / sizeof(int);
    const unsigned int N2 = sizeof(A2) / sizeof(int);
    const unsigned int N3 = sizeof(A3) / sizeof(int);
    const unsigned int N4 = sizeof(A4) / sizeof(int);
    const unsigned int N5 = sizeof(A5) / sizeof(int);

    SECTION("test01") {
        using vec_type = Vector<int>;
        using iterator_type = vec_type::Iterator;
        vec_type v(A, A + N);

        iterator_type it1 = v.erase(v.begin() + 1);
        REQUIRE(it1 == v.begin() + 1);
        REQUIRE(v.size() == N1);
        REQUIRE(std::equal(v.begin(), v.end(), A1));

        iterator_type it2 = v.erase(v.begin() + 4, v.begin() + 9);
        REQUIRE(it2 == v.begin() + 4);
        REQUIRE(v.size() == N2);
        REQUIRE(std::equal(v.begin(), v.end(), A2));

        iterator_type it3 = v.erase(v.begin() + 6, v.end());
        REQUIRE(it3 == v.begin() + 6);
        REQUIRE(v.size() == N3);
        REQUIRE(std::equal(v.begin(), v.end(), A3));

        iterator_type it4 = v.erase(v.begin(), v.begin() + 3);
        REQUIRE(it4 == v.begin());
        REQUIRE(v.size() == N4);
        REQUIRE(std::equal(v.begin(), v.end(), A4));

        iterator_type it5 = v.erase(v.begin() + 2);
        REQUIRE(it5 == v.begin() + 2);
        REQUIRE(v.size() == N5);
        REQUIRE(std::equal(v.begin(), v.end(), A5));

        iterator_type it6 = v.erase(v.begin(), v.end());
        REQUIRE(it6 == v.begin());
        REQUIRE(v.empty());
    }

    SECTION("test02") {
        using vec_type = Vector<Vector<int>>;
        using iterator_type = vec_type::Iterator;
        vec_type v, v1, v2, v3, v4, v5;

        for (int i : A) {
            v.push_back(Vector<int>(1, i));
        }
        for (int i : A1) {
            v1.push_back(Vector<int>(1, i));
        }
        for (int i : A2) {
            v2.push_back(Vector<int>(1, i));
        }
        for (int i : A3) {
            v3.push_back(Vector<int>(1, i));
        }
        for (int i : A4) {
            v4.push_back(Vector<int>(1, i));
        }
        for (int i : A5) {
            v5.push_back(Vector<int>(1, i));
        }

        iterator_type it1 = v.erase(v.begin() + 1);
        REQUIRE(it1 == v.begin() + 1);
        REQUIRE(v.size() == N1);
        REQUIRE(std::equal(v.begin(), v.end(), v1.begin()));

        iterator_type it2 = v.erase(v.begin() + 4, v.begin() + 9);
        REQUIRE(it2 == v.begin() + 4);
        REQUIRE(v.size() == N2);
        REQUIRE(std::equal(v.begin(), v.end(), v2.begin()));

        iterator_type it3 = v.erase(v.begin() + 6, v.end());
        REQUIRE(it3 == v.begin() + 6);
        REQUIRE(v.size() == N3);
        REQUIRE(std::equal(v.begin(), v.end(), v3.begin()));

        iterator_type it4 = v.erase(v.begin(), v.begin() + 3);
        REQUIRE(it4 == v.begin());
        REQUIRE(v.size() == N4);
        REQUIRE(std::equal(v.begin(), v.end(), v4.begin()));

        iterator_type it5 = v.erase(v.begin() + 2);
        REQUIRE(it5 == v.begin() + 2);
        REQUIRE(v.size() == N5);
        REQUIRE(std::equal(v.begin(), v.end(), v5.begin()));

        iterator_type it6 = v.erase(v.begin(), v.end());
        REQUIRE(it6 == v.begin());
        REQUIRE(v.empty());
    }
}

TEST_CASE("insert") {
    Vector<int> vec;
    REQUIRE(vec.empty());
    REQUIRE(vec.data() == nullptr);
    REQUIRE(vec.begin() == vec.end());

    SECTION("insert a trivial element at the beginning") {
        vec.insert(vec.begin(), 42);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.front() == vec.back());
        REQUIRE(vec[0] == 42);
    }
    SECTION("insert a trivial element at the end") {
        vec.insert(vec.end(), 42);
        REQUIRE(vec[0] == 42);
    }
    SECTION("insert more trivial elements") {
        vec.push_back(1);
        vec.insert(vec.begin(), 2);
        vec.insert(vec.end(), 3);
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 1);
        REQUIRE(vec[2] == 3);
    }
    SECTION("insert trivial elements in range") {
        int i01 = 5;
        int* pi01 = &i01;
        vec.insert(vec.begin(), pi01, pi01 + 1);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec[0] == 5);
    }

    Vector<std::string> svec;
    REQUIRE(svec.empty());
    REQUIRE(svec.data() == nullptr);
    REQUIRE(svec.begin() == svec.end());

    SECTION("insert a non-trivial element at the beginning") {
        svec.insert(svec.begin(), "first");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec.back() == "first");
    }
    SECTION("insert a non-trivial element at the end") {
        svec.insert(svec.end(), "last");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec[0] == "last");
    }
    SECTION("insert more non-trivial elements") {
        svec.push_back("42");
        svec.insert(svec.begin(), "21");
        svec.insert(svec.end(), "32");
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "21");
        REQUIRE(svec[1] == "42");
        REQUIRE(svec[2] == "32");
    }
    SECTION("insert non-trivial elements in range") {
        std::string s = "foo";
        std::string* p = &s;
        svec.insert(svec.begin(), p, p + 1);
        REQUIRE(svec.size() == 1);
        REQUIRE(svec[0] == "foo");
    }
}

TEST_CASE("emplace") {
    Vector<int> vec;
    REQUIRE(vec.empty());
    REQUIRE(vec.data() == nullptr);
    REQUIRE(vec.begin() == vec.end());

    SECTION("emplace a trivial element at the beginning") {
        vec.emplace(vec.begin(), 42);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.front() == vec.back());
        REQUIRE(vec[0] == 42);
    }
    SECTION("emplace a trivial element at the end") {
        vec.emplace(vec.end(), 42);
        REQUIRE(vec[0] == 42);
    }
    SECTION("emplace more trivial elements") {
        vec.push_back(1);
        vec.emplace(vec.begin(), 2);
        vec.emplace(vec.end(), 3);
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 2);
        REQUIRE(vec[1] == 1);
        REQUIRE(vec[2] == 3);
    }

    Vector<std::string> svec;
    REQUIRE(svec.empty());
    REQUIRE(svec.data() == nullptr);
    REQUIRE(svec.begin() == svec.end());

    SECTION("emplace a non-trivial element at the beginning") {
        svec.emplace(svec.begin(), "first");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec.back() == "first");
    }
    SECTION("emplace a non-trivial element at the end") {
        svec.emplace(svec.end(), "last");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec[0] == "last");
    }
    SECTION("emplace more non-trivial elements") {
        svec.push_back("42");
        svec.emplace(svec.begin(), "21");
        svec.emplace(svec.end(), "32");
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "21");
        REQUIRE(svec[1] == "42");
        REQUIRE(svec[2] == "32");
    }

    SECTION("test01 from gcc") {
        Vector<int> v;
        v.emplace(v.begin());
        REQUIRE(v.size() == 1);
        REQUIRE(v[0] == 0);
    }
}

TEST_CASE("emplace_back") {
    Vector<int> x{1, 2, 3, 4};
    Vector<int>::Reference r = x.emplace_back(5);
    REQUIRE(r == 5);
    REQUIRE(&r == &x.back());
}

TEST_CASE("insert n value") {
    Vector<int> vec;

    SECTION("insert a trivial element at the beginning") {
        vec.insert(vec.begin(), 1, 42);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.front() == 42);
    }
    SECTION("insert a trivial element at the end") {
        vec.insert(vec.end(), 1, 42);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.back() == 42);
    }
    SECTION("insert n trivial elements at the beginning") {
        vec.insert(vec.begin(), 10, 42);
        REQUIRE(vec.size() == 10);
        REQUIRE(vec.back() == 42);
    }
    SECTION("insert n trivial elements at the end") {
        vec.insert(vec.end(), 10, 42);
        REQUIRE(vec.size() == 10);
        REQUIRE(vec.back() == 42);
    }
    SECTION("insert n trivial elements at pos") {
        vec.push_back(1);
        vec.push_back(2);
        vec.insert(vec.begin() + 1, 3, -1);
        REQUIRE(vec.size() == 5);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == -1);
        REQUIRE(vec[2] == -1);
        REQUIRE(vec[3] == -1);
        REQUIRE(vec[4] == 2);
    }

    SECTION("insert multiplies") {
        Vector<int> v1, v2{5, 6};
        int n = 0;
        auto it = v1.insert(v1.cbegin(), n);
        it = v1.insert(v1.cbegin(), 1);
        it = v1.insert(v1.cbegin(), {2, 3});
        it = v1.insert(v1.cbegin(), 1, 4);
        it = v1.insert(v1.cbegin(), v2.begin(), v2.end());
        REQUIRE(v1.size() == 7);
        REQUIRE(v1[0] == 5);
        REQUIRE(v1[1] == 6);
        REQUIRE(v1[2] == 4);
        REQUIRE(v1[3] == 2);
        REQUIRE(v1[4] == 3);
        REQUIRE(v1[5] == 1);
        REQUIRE(v1[6] == 0);
    }

    Vector<std::string> svec;
    SECTION("insert a non-trivial element at the beginning") {
        svec.insert(svec.begin(), 1, "foo");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec.front() == "foo");
    }
    SECTION("insert a non-trivial element at the end") {
        svec.insert(svec.end(), 1, "foo");
        REQUIRE(svec.size() == 1);
        REQUIRE(svec.back() == "foo");
    }
    SECTION("insert n non-trivial elements at the beginning") {
        svec.insert(svec.begin(), 10, "foo");
        REQUIRE(svec.size() == 10);
        REQUIRE(svec.back() == "foo");
    }
    SECTION("insert n non-trivial elements at the end") {
        svec.insert(svec.end(), 10, "foo");
        REQUIRE(svec.size() == 10);
        REQUIRE(svec.back() == "foo");
    }
    SECTION("insert n non-trivial elements at pos") {
        svec.push_back("foo");
        svec.push_back("bar");
        svec.insert(svec.begin() + 1, 3, "baz");
        REQUIRE(svec.size() == 5);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "baz");
        REQUIRE(svec[2] == "baz");
        REQUIRE(svec[3] == "baz");
        REQUIRE(svec[4] == "bar");
    }
}

struct T {
    T() = default;
    T(const T&) = default;
    T(T&&) = delete;
};

class Bomb {
public:
    Bomb() = default;

    Bomb(const Bomb& b) : armed_(b.armed_) {
        tick();
    }

    Bomb(Bomb&& b) noexcept(false) : armed_(b.armed_) {
        tick();
        b.moved_from_ = true;
    }

    template <typename T>
    Bomb(T&) = delete;

    [[nodiscard]] bool moved() const noexcept {
        return moved_from_;
    }

    void armed(bool b) noexcept {
        armed_ = b;
    }

    [[nodiscard]] bool armed() const noexcept {
        return armed_;
    }

private:
    void tick() {
        if (armed_ && ticks_++) {
            throw 1;
        }
    }

    bool moved_from_ = false;
    bool armed_ = true;
    static int ticks_;
};

int Bomb::ticks_ = 0;

TEST_CASE("push_back") {
    const T val;
    Vector<T> x;
    x.push_back(val);

    SECTION("strong exception guarantee") {
        Vector<Bomb> v(2);
        v.resize(v.capacity());
        // sanity check
        for (auto& e : v) {
            REQUIRE(!e.moved());
        }
        Bomb defused;
        defused.armed(false);
        REQUIRE_THROWS(v.push_back(defused));
        // sanity check
        for (auto& e : v) {
            REQUIRE(!e.moved());
        }
    }
}

TEST_CASE("insert range elements") {
    Vector<int> vec;
    const Vector<int> xs = {11, 22, 33};

    SECTION("insert trivial elements at the beginning") {
        vec.insert(vec.begin(), xs.begin(), xs.end());
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 11);
        REQUIRE(vec[1] == 22);
        REQUIRE(vec[2] == 33);
    }
    SECTION("insert trivial elements at the end") {
        vec.insert(vec.end(), xs.begin(), xs.end());
        REQUIRE(vec.size() == 3);
        REQUIRE(vec[0] == 11);
        REQUIRE(vec[1] == 22);
        REQUIRE(vec[2] == 33);
    }
    SECTION("insert trivial elements at pos") {
        vec.push_back(1);
        vec.push_back(2);
        vec.insert(vec.begin() + 1, xs.begin(), xs.end());
        REQUIRE(vec.size() == 5);
        REQUIRE(vec[0] == 1);
        REQUIRE(vec[1] == 11);
        REQUIRE(vec[2] == 22);
        REQUIRE(vec[3] == 33);
        REQUIRE(vec[4] == 2);
    }

    Vector<std::string> svec;
    const Vector<std::string> ys = {"foo", "bar", "baz"};

    SECTION("insert non-trivial elements at the beginning") {
        svec.insert(svec.begin(), ys.begin(), ys.end());
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "bar");
        REQUIRE(svec[2] == "baz");
    }
    SECTION("insert non-trivial elements at the end") {
        svec.insert(svec.end(), ys.begin(), ys.end());
        REQUIRE(svec.size() == 3);
        REQUIRE(svec[0] == "foo");
        REQUIRE(svec[1] == "bar");
        REQUIRE(svec[2] == "baz");
    }
    SECTION("insert non-trivial elements at pos") {
        svec.push_back("first");
        svec.push_back("last");
        svec.insert(svec.begin() + 1, ys.begin(), ys.end());
        REQUIRE(svec.size() == 5);
        REQUIRE(svec[0] == "first");
        REQUIRE(svec[1] == "foo");
        REQUIRE(svec[2] == "bar");
        REQUIRE(svec[3] == "baz");
        REQUIRE(svec[4] == "last");
    }
}

TEST_CASE("Vectors can be sized and resized") {
    Vector<int> v(5);
    REQUIRE(v.size() == 5);
    REQUIRE(v.capacity() >= 5);

    SECTION("resizing bigger changes size and capacity") {
        v.resize(10);
        REQUIRE(v.size() == 10);
        REQUIRE(v.capacity() >= 10);
    }
    SECTION("resizing smaller changes size but not capacity") {
        v.resize(0);
        REQUIRE(v.empty());
        REQUIRE(v.capacity() >= 5);
    }
    SECTION("reserving bigger changes capacity but not size") {
        v.reserve(10);
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 10);
    }
    SECTION("reserving smaller does not change size or capacity") {
        v.reserve(0);
        REQUIRE(v.size() == 5);
        REQUIRE(v.capacity() >= 5);
    }
}
