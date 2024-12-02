#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <functional>

template <typename T, typename Comparator = std::less<T>>
class SortedArray {
    std::vector<T> data;
    Comparator comp;
public:
    SortedArray(Comparator comp = Comparator()) : comp(comp) {}

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    auto upper_bound(T value)
    {
        return std::upper_bound(data.begin(), data.end(), value, comp);
    }

    void insert(T&& value) 
    {
        auto pos = std::lower_bound(data.begin(), data.end(), value, comp);
        data.insert(pos, std::forward<T>(value));
    }

    void batch_insert(std::vector<T>&& batch)
    {
        std::sort(batch.begin(), batch.end());
        data.reserve(data.size() + batch.size());
        std::move(batch.begin(), batch.end(), std::back_inserter(data));
        std::inplace_merge(data.begin(), data.begin() + data.size() - batch.size(), data.end());
    }

    void erase(std::vector<T>::iterator it)
    {
        data.erase(it);
    }

    void erase(std::vector<T>::iterator a, std::vector<T>::iterator b)
    {
        data.erase(a, b);
    }

    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }

    auto operator[] (size_t index) const { return data[index]; }
    auto& operator[] (size_t index) { return data[index]; }
};
