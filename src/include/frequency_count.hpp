#pragma once
#include <cstddef>

namespace utility {

#define DATASET_MIN_SKETCH 10000

template <typename T>
class IFrequencyCount {
public:
	virtual ~IFrequencyCount() = default;
	virtual size_t Query(const T &element) = 0;
	virtual void AddElement(const T &element) = 0;
	virtual void Clear() = 0;
};

template <typename T>
class FrequencyCount {
private:
	IFrequencyCount<T> *frequency_counter;

public:
	explicit FrequencyCount(size_t dataset_size);
	~FrequencyCount() {
		delete frequency_counter;
	}
	size_t Query(const T &element);
	void AddElement(const T &element);
	void Clear();
};
} // namespace utility

// Included AFTER class definitions to break circular dependency
#include "count_min.hpp"
#include "multiset.hpp"

namespace utility {
template <typename T>
FrequencyCount<T>::FrequencyCount(size_t dataset_size) {
	if (dataset_size >= DATASET_MIN_SKETCH) {
		frequency_counter = new CountMinSketch<T>();
	} else {
		frequency_counter = new Multiset<T>();
	}
}

template <typename T>
size_t FrequencyCount<T>::Query(const T &element) {
	return frequency_counter->Query(element);
}

template <typename T>
void FrequencyCount<T>::AddElement(const T &element) {
	frequency_counter->AddElement(element);
}

template <typename T>
void FrequencyCount<T>::Clear() {
	frequency_counter->Clear();
}

} // namespace utility
