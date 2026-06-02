#include "count_min.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace utility {

template <typename T>
CountMinSketch<T>::CountMinSketch() : count {0} {
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		hashes[i] = XXH64_createState();
		XXH64_reset(hashes[i], random());
	}
}

template <typename T>
size_t CountMinSketch<T>::Query(T element) {
	size_t answer = SIZE_MAX;
	for (int i = 0; i < COUNT_MIN_DEPTH; i++) {
		int index = ComputeHash(i, element);
		answer = std::min(counters[i][index], answer);
	}
	return answer;
}

template <typename T>
void CountMinSketch<T>::AddElement(T element) {
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
size_t CountMinSketch<T>::ComputeHash(int i, T element) {
	auto &hash = hashes[i];

	std::vector<char> bytes(sizeof(T));
	std::memcpy(bytes.data(), &element, sizeof(T));

	XXH64_update(hash, bytes.data(), bytes.size());
	return XXH64_digest(hash) % COUNT_MIN_WIDTH;
}

} // namespace utility
