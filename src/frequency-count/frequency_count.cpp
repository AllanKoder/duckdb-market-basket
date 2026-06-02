#include <cstddef>
#include <memory>
#include "frequency_count.hpp"
#include "count_min.hpp"
#include "multiset.hpp"

namespace utility {
template <typename T>
FrequencyCount<T>::FrequencyCount(size_t dataset_size) {
	if (dataset_size >= DATASET_MIN_SKETCH) {
		frequency_counter = std::make_unique<CountMinSketch<T>>;
	} else {
		frequency_counter = std::make_unique<Multiset<T>>;
	}
}

template <typename T>
size_t FrequencyCount<T>::Query(T element) {
	return frequency_counter->Query(element);
}

template <typename T>
void FrequencyCount<T>::AddElement(T element) {
	frequency_counter->AddElement(element);
}

template <typename T>
void FrequencyCount<T>::Clear() {
	frequency_counter->Clear();
}
} // namespace utility
