//
// Created by Matthew Good on 4/8/21.
//

#ifndef GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H
#define GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H

#include "Array.h"

template<typename T>
class AutoResizingArray {
    Array<T> array;
    size_t pageSize = 2;
    size_t index = 0;
public:

    AutoResizingArray() = default;

    AutoResizingArray(const AutoResizingArray<T> &other) : array(other.array), pageSize(other.pageSize), index(other.index) {}

    template<typename U>
    AutoResizingArray(const AutoResizingArray<U> &other) : array(other.array), pageSize(other.pageSize), index(other.index) {}

    AutoResizingArray<T> &operator=(const AutoResizingArray<T> &other) {
        array = other.array;
        pageSize = other.pageSize;
        index = other.index;
        return *this;
    }

    template<typename U>
    AutoResizingArray<T> &operator=(const AutoResizingArray<U> &other) {
        array = other.array;
        pageSize = other.pageSize;
        index = other.index;
        return *this;
    }

    AutoResizingArray(AutoResizingArray<T> &&other) : array(std::move(other.array)) {
        std::swap(pageSize, other.pageSize);
        std::swap(index, other.index);
    }

    template<typename U>
    AutoResizingArray(AutoResizingArray<U> &&other) : array(std::move(other.array)) {
        std::swap(pageSize, other.pageSize);
        std::swap(index, other.index);
    }

    AutoResizingArray<T> &operator=(AutoResizingArray<T> &&other) {
        array = std::move(other.array);
        std::swap(pageSize, other.pageSize);
        std::swap(index, other.index);
        return *this;
    }

    template<typename U>
    AutoResizingArray<T> &operator=(AutoResizingArray<U> &&other) {
        array = std::move(other.array);
        std::swap(pageSize, other.pageSize);
        std::swap(index, other.index);
        return *this;
    }

    bool isEmpty() const {
        return array.isEmpty();
    }

    size_t getCapacity() const {
        return array.getCapacity();
    }

    void setPageSize(size_t newPageSize) {
        pageSize = newPageSize;
    }

    template <typename U>
    void copyFrom(AutoResizingArray<U> & array_) {
        array.copyFrom(array_.array);
    }

    template <typename U>
    void copyTo(AutoResizingArray<U> & array_) {
        array.copyTo(array_.array);
    }

    template <typename U>
    void moveFrom(AutoResizingArray<U> & array_) {
        array.moveFrom(array_.array);
    }

    template <typename U>
    void moveTo(AutoResizingArray<U> & array_) {
        array.moveTo(array_.array);
    }

    T & operator[] (int index_) const {
        return array[index_];
    }

    void add(const T & data) {
        size_t capacity = array.getCapacity();
        if (capacity == index) array.resize(capacity + pageSize);
        array[index++] = data;
    }

    T & peek() {
        return array[index-1];
    }

    T & remove() {
        T & val = peek();
        size_t capacity = array.getCapacity() - pageSize;
        if (capacity == index) array.resize(capacity);
        index--;
        return val;
    }

    T *getData() const {
        return array.getData();
    }
};

#endif //GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H
