#include "multiset.hpp"

namespace utility {

template <typename T>
size_t Multiset<T>::Query(T element) {
	return set.count(element);
}

template <typename T>
void Multiset<T>::AddElement(T element) {
	set.insert(element);
}

template <typename T>
void Multiset<T>::Clear() {
	set.clear();
}
} // namespace utility
