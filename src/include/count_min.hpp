#pragma once
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "frequency_count.hpp"

#include "xxhash.h"

namespace utility {

/*
 * https://www.sciencedirect.com/science/article/abs/pii/S0196677403001913?via%3Dihub
 * The parameters w and d can be chosen by setting w=⌈e/ε⌉ and d=⌈ln1/δ⌉, where the error in answering a query is
 * within a factor of ε with probability at least 1 - δ.
 * Error rate chosen is 1% with a 99% confidence
 * Width: 272
 * Depth: 5
 */
#define COUNT_MIN_WIDTH 272
#define COUNT_MIN_DEPTH 5

template <typename T>
class CountMinSketch : public IFrequencyCount<T> {
public:
	CountMinSketch();
	~CountMinSketch();

	size_t Query(const T &element) override;
	void AddElement(const T &element) override;
	void Clear() override;

private:
	size_t ComputeHash(int i, const T &element);
	std::array<std::array<size_t, COUNT_MIN_WIDTH>, COUNT_MIN_DEPTH> counters;
	std::array<uint64_t, COUNT_MIN_DEPTH> seeds;
	std::array<XXH64_state_t *, COUNT_MIN_DEPTH> hashes;
	size_t count;
};

// ── Implementations ──

template <typename T>
CountMinSketch<T>::CountMinSketch() : count {0} {
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		hashes[i] = XXH64_createState();
		seeds[i] = random();
		XXH64_reset(hashes[i], seeds[i]);
	}
}

template <typename T>
CountMinSketch<T>::~CountMinSketch() {
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		XXH64_freeState(hashes[i]);
	}
}

template <typename T>
size_t CountMinSketch<T>::Query(const T &element) {
	size_t answer = SIZE_MAX;
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		int index = ComputeHash(i, element);
		answer = std::min(counters[i][index], answer);
	}
	return answer;
}

template <typename T>
void CountMinSketch<T>::AddElement(const T &element) {
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		int index = ComputeHash(i, element);
		counters[i][index]++;
	}
}

template <typename T>
void CountMinSketch<T>::Clear() {
	for (auto &a : counters) {
		a.fill(0);
	}
	count = 0;
}

template <typename T>
size_t CountMinSketch<T>::ComputeHash(int i, const T &element) {
	auto &hash = hashes[i];
	XXH64_reset(hash, seeds[i]);
	if (std::is_same<T, std::string>::value) {
		// Hash the actual string contents
		XXH64_update(hash, element.data(), element.size());
	} else {
		// For POD types (int, float, etc.) memcpy is fine
		XXH64_update(hash, &element, sizeof(T));
	}
	return XXH64_digest(hash) % COUNT_MIN_WIDTH;
}

} // namespace utility
