#include <catch2/catch_test_macros.hpp>

import cosinet.perceptron;

TEST_CASE("perceptron returns something", "[perceptron]")
{
    cosinet::perceptron perceptron;
    auto activation = perceptron.evaluate({0.3, 0.5});
}
