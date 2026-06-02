#pragma once

#include <cstddef>
#include <memory>

namespace utility {

#define DATASET_MIN_SKETCH 10000

template <typename T>
class IFrequencyCount {
private:
public:
	virtual ~IFrequencyCount() = default;
	virtual size_t Query(T element) = 0;
	virtual void AddElement(T element) = 0;
	virtual void Clear() = 0;
};

template <typename T>
class FrequencyCount {
private:
	std::unique_ptr<IFrequencyCount<T>> frequency_counter;

public:
	explicit FrequencyCount(size_t dataset_size);
	size_t Query(T element);
	void AddElement(T element);
	void Clear();
};
} // namespace utility
