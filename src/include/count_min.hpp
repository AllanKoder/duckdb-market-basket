#pragma once

#include "frequency_count.hpp"
#include <array>
#include <xxhash.h>

namespace utility {
/*
 * https://www.sciencedirect.com/science/article/abs/pii/S0196677403001913?via%3Dihub
 * The parameters w and d can be chosen by setting w=⌈e/ε⌉ and d=⌈ln1/δ⌉, where the error in answering a query is within
 a factor of ε with probability δ.
 * Error rate chosen is 1% with a 99% confidence
 * Width: 272
 * Depth: 5
 */
#define COUNT_MIN_WIDTH 272
#define COUNT_MIN_DEPTH 5

template <typename T>
class CountMinSketch : FrequencyCount<T> {
public:
	CountMinSketch();
	size_t Query(T element) override;
	void AddElement(T element) override;
	void Clear() override;

private:
	size_t ComputeHash(int i, T element);

	std::array<std::array<size_t, COUNT_MIN_WIDTH>, COUNT_MIN_DEPTH> counters;
	std::array<XXH64_state_t *, COUNT_MIN_DEPTH> hashes;
	size_t count;
};

} // namespace utility
