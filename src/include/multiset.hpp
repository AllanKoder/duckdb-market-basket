#pragma once

#include "frequency_count.hpp"
#include <set>
namespace utility {
template <typename T>
class Multiset : FrequencyCount<T> {
public:
	Multiset();
	size_t Query(T element) override;
	void AddElement(T element) override;
	void Clear() override;

private:
	std::multiset<T> set;
};
} // namespace utility
