#include <catch2/catch_test_macros.hpp>

import cosinet.perceptron;
import cosinet.vector;

TEST_CASE("perceptron returns something", "[perceptron]")
{
    cosinet::perceptron perceptron;
    auto activation = perceptron.evaluate({0.3, 0.5});
}

TEST_CASE("vector supports named members, indexing and contiguous storage", "[vector]")
{
    cosinet::vector<float, 26> v{};

    v.a = 3.0f;
    v.b = 5.0f;
    v.z = 26.0f;

    SECTION("named members and indexed access share storage")
    {
        REQUIRE(v[0] == 3.0f);
        REQUIRE(v[1] == 5.0f);
        REQUIRE(v[25] == 26.0f);

        v[0] = 10.0f;
        REQUIRE(v.a == 10.0f);
    }

    SECTION("data provides contiguous writable storage")
    {
        auto* data = v.data();

        REQUIRE(data[0] == v.a);
        REQUIRE(data[1] == v.b);
        REQUIRE(data[25] == v.z);

        data[25] = 42.0f;
        REQUIRE(v.z == 42.0f);
    }

    SECTION("copying produces independent storage")
    {
        auto copy = v;

        REQUIRE(copy.a == v.a);
        REQUIRE(copy.z == v.z);

        copy.a = 100.0f;

        REQUIRE(v.a == 3.0f);
        REQUIRE(copy[0] == 100.0f);
        REQUIRE(copy.data() != v.data());
    }

    SECTION("assignment preserves correct member bindings")
    {
        cosinet::vector<float, 26> other{};
        other = v;

        REQUIRE(other.a == 3.0f);
        REQUIRE(other.z == 26.0f);

        other.b = 99.0f;

        REQUIRE(other[1] == 99.0f);
        REQUIRE(v.b == 5.0f);
    }
}