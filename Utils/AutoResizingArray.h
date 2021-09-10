//
// Created by Matthew Good on 4/8/21.
//

#ifndef GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H
#define GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H

#include "Array.h"

template<typename T>
class AutoResizingArray {
    Array<T> array;
    size_t minCapacity = 0;
    size_t pageSize = 2;
    size_t elementCount = 0;
public:

    AutoResizingArray() = default;

    AutoResizingArray(size_t pageSize, size_t minCapacity) {
        this->pageSize = pageSize;
        setMinCapacity(minCapacity);
    };

    AutoResizingArray(size_t pageSize) {
        this->pageSize = pageSize;
    };

    AutoResizingArray(const AutoResizingArray<T> &other) : array(other.array), minCapacity(other.minCapacity), pageSize(other.pageSize), elementCount(other.elementCount) {}

    template<typename U>
    AutoResizingArray(const AutoResizingArray<U> &other) : array(other.array), minCapacity(other.minCapacity), pageSize(other.pageSize), elementCount(other.elementCount) {}

    AutoResizingArray<T> &operator=(const AutoResizingArray<T> &other) {
        array = other.array;
        minCapacity = other.minCapacity;
        pageSize = other.pageSize;
        elementCount = other.elementCount;
        return *this;
    }

    template<typename U>
    AutoResizingArray<T> &operator=(const AutoResizingArray<U> &other) {
        array = other.array;
        minCapacity = other.minCapacity;
        pageSize = other.pageSize;
        elementCount = other.elementCount;
        return *this;
    }

    AutoResizingArray(AutoResizingArray<T> &&other) : array(std::move(other.array)) {
        std::swap(minCapacity, other.minCapacity);
        std::swap(pageSize, other.pageSize);
        std::swap(elementCount, other.elementCount);
    }

    template<typename U>
    AutoResizingArray(AutoResizingArray<U> &&other) : array(std::move(other.array)) {
        std::swap(minCapacity, other.minCapacity);
        std::swap(pageSize, other.pageSize);
        std::swap(elementCount, other.elementCount);
    }

    AutoResizingArray<T> &operator=(AutoResizingArray<T> &&other) {
        array = std::move(other.array);
        std::swap(minCapacity, other.minCapacity);
        std::swap(pageSize, other.pageSize);
        std::swap(elementCount, other.elementCount);
        return *this;
    }

    template<typename U>
    AutoResizingArray<T> &operator=(AutoResizingArray<U> &&other) {
        array = std::move(other.array);
        std::swap(minCapacity, other.minCapacity);
        std::swap(pageSize, other.pageSize);
        std::swap(elementCount, other.elementCount);
        return *this;
    }

    bool isEmpty() const {
        return elementCount == 0;
    }

    size_t getCapacity() const {
        return array.getCapacity();
    }
    
    void setMinCapacity(const size_t & capacity) {
        if (minCapacity != capacity) {
            minCapacity = capacity;
            if (array.getCapacity() < minCapacity) array.resize(minCapacity);
            if (elementCount > minCapacity) elementCount = minCapacity;
        }
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

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & operator[] (size_t index) {
        return array[index];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    const T & operator[] (size_t index) const {
        return array[index];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T operator[] (size_t index) {
        return array[index];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    const T operator[] (size_t index) const {
        return array[index];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & add(const T & data) {
        size_t capacity = array.getCapacity();
        if (capacity == elementCount && capacity >= minCapacity) {
            array.resize(capacity + pageSize);
        }
        array[elementCount] = data;
        return array[elementCount++];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T add(T data) {
        size_t capacity = array.getCapacity();
        if (capacity == elementCount && capacity >= minCapacity) {
            array.resize(capacity + pageSize);
        }
        array[elementCount] = data;
        return array[elementCount++];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & peek() {
        return array[elementCount-1];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    const T & peek() const {
        return array[elementCount-1];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T peek() {
        return array[elementCount-1];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    const T peek() const {
        return array[elementCount-1];
    }
    
    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T && remove() {
        T && val = std::move(peek());
        elementCount--;
        size_t capacity = array.getCapacity() - pageSize;
        if (capacity == elementCount && capacity >= minCapacity) array.resize(capacity);
        return std::move(val);
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T remove() {
        T val = peek();
        elementCount--;
        size_t capacity = array.getCapacity() - pageSize;
        if (capacity == elementCount && capacity >= minCapacity) array.resize(capacity);
        return val;
    }

    T *getData() {
        return array.getData();
    }

    const T *getData() const {
        return array.getData();
    }
    
    size_t size() const {
        return elementCount;
    }

    T* begin() {
        return array.begin();
    }

    const T* begin() const {
        return array.begin();
    }

    T* end() {
        return begin() + size();
    }

    const T* end() const {
        return begin() + size();
    }

    void clear() {
        array.resize(0);
        array.resize(minCapacity);
        elementCount = 0;
    }
};

#endif //GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H
