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

    AutoResizingArray(size_t pageSize) {
        this->pageSize = pageSize;
    };
    
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

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & operator[] (int index_) {
        return array[index_];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    const T & operator[] (int index_) const {
        return array[index_];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T operator[] (int index_) {
        return array[index_];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    const T operator[] (int index_) const {
        return array[index_];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    void add(const T & data) {
        size_t capacity = array.getCapacity();
        if (capacity == index) array.resize(capacity + pageSize);
        array[index++] = data;
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    void add(T data) {
        size_t capacity = array.getCapacity();
        if (capacity == index) array.resize(capacity + pageSize);
        array[index++] = data;
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & peek() {
        return array[index-1];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    const T & peek() const {
        return array[index-1];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T peek() {
        return array[index-1];
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    const T peek() const {
        return array[index-1];
    }

    template<class T_REFERENCE = void, typename = typename std::enable_if<!std::is_pointer<T>::value, T_REFERENCE>::type>
    T & remove() {
        T & val = peek();
        size_t capacity = array.getCapacity() - pageSize;
        if (capacity == index) array.resize(capacity);
        index--;
        return val;
    }

    template<class T_POINTER = void, typename = typename std::enable_if<std::is_pointer<T>::value, T_POINTER>::type>
    T remove() {
        T val = peek();
        size_t capacity = array.getCapacity() - pageSize;
        if (capacity == index) array.resize(capacity);
        index--;
        return val;
    }

    T *getData() {
        return array.getData();
    }

    const T *getData() const {
        return array.getData();
    }
    
    size_t size() const {
        return index;
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
};

#endif //GRAPHICAL_TOOL_KIT_AUTORESIZINGARRAY_H
