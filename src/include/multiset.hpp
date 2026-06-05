#pragma once
#include <set>
#include "frequency_count.hpp"

namespace utility {

template <typename T>
class Multiset : public IFrequencyCount<T> {
public:
	size_t Query(const T &element) override;
	void AddElement(const T &element) override;
	void Clear() override;

private:
	std::multiset<T> set;
};

// ── Implementations ──

template <typename T>
size_t Multiset<T>::Query(const T &element) {
	return set.count(element);
}

template <typename T>
void Multiset<T>::AddElement(const T &element) {
	set.insert(element);
}

template <typename T>
void Multiset<T>::Clear() {
	set.clear();
}

} // namespace utility
