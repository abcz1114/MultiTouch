//
// Created by Matthew Good on 24/7/21.
//

#ifndef GRAPHICAL_TOOL_KIT_MEMORYALLOCATOR_H
#define GRAPHICAL_TOOL_KIT_MEMORYALLOCATOR_H

#include <type_traits>
#include <new> // std::bad_alloc
#include <algorithm>
#include "../DiligentLog/Log.h"

namespace MemoryAllocator {
    template<class T>
    struct is_primitive {
        static constexpr bool value = !std::is_class<T>::value;
    };

    template<class T>
    constexpr bool is_primitive<T>::value;

    template<typename T>
    class MemoryAllocation {
        T *data;
        size_t capacity;
        size_t capacityInBytes;
        size_t sizeof_T;

    public:
        static_assert(
                is_primitive<T>::value
                || (
                        std::is_trivial<T>::value
                        || std::is_default_constructible<T>::value
                ),
                "class T must have a public constructor T()"
        );

        MemoryAllocation() {
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
            sizeof_T = sizeof(T);
        }

        MemoryAllocation(const MemoryAllocation<T> &other) {
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
            sizeof_T = sizeof(T);
            copyFrom(other);
        }

        template<typename U>
        MemoryAllocation(const MemoryAllocation<U> &other) {
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
            sizeof_T = sizeof(T);
            copyFrom(other);
        }

        MemoryAllocation &operator=(const MemoryAllocation<T> &other) {
            deallocate();
            copyFrom(other);
            return *this;
        }

        template<typename U>
        MemoryAllocation &operator=(const MemoryAllocation<U> &other) {
            deallocate();
            copyFrom(other);
            return *this;
        }

        MemoryAllocation(MemoryAllocation<T> &&other) {
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
            sizeof_T = sizeof(T);
            std::swap(data, other.data);
            std::swap(capacity, other.capacity);
        }

        template<typename U>
        MemoryAllocation(MemoryAllocation<U> &&other) {
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
            sizeof_T = sizeof(T);
            moveFrom(other);
        }

        MemoryAllocation &operator=(MemoryAllocation<T> &&other) {
            deallocate();
            std::swap(data, other.data);
            std::swap(capacity, other.capacity);
            return *this;
        }

        template<typename U>
        MemoryAllocation &operator=(MemoryAllocation<U> &&other) {
            moveFrom(other);
            return *this;
        }

        template<typename U>
        void copyFrom(MemoryAllocation<U> &memoryAllocation) {
            static_assert(std::is_copy_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                          "T must be copy constructible and convertible from U");
            MemoryAllocation<T> tmp;
            tmp.allocate(memoryAllocation.capacity);
            if (is_primitive<T>::value && std::is_same<U, T>::value) {
                // copy primitive if same type
                memcpy(tmp.data, memoryAllocation.data, memoryAllocation.capacityInBytes);
            } else {
                for (size_t i = 0; i < capacity; ++i) {
                    tmp.data[i] = static_cast<T>(memoryAllocation.data[i]);
                }
            }
            std::swap(data, tmp.data);
            std::swap(capacity, tmp.capacity);
        }

        template<typename U>
        void copyTo(MemoryAllocation<U> &memoryAllocation) {
            static_assert(std::is_copy_constructible<U>::value &&
                          std::is_convertible<T, U>::value,
                          "U must be copy constructible and convertible from T");
            MemoryAllocation<U> tmp;
            tmp.allocate(capacity);
            if (is_primitive<U>::value && std::is_same<U, T>::value) {
                // copy primitive if same type
                memcpy(tmp.data, data, capacityInBytes);
            } else {
                for (size_t i = 0; i < capacity; ++i) {
                    tmp.data[i] = static_cast<T>(data[i]);
                }
            }
            std::swap(memoryAllocation.data, tmp.data);
            std::swap(memoryAllocation.capacity, tmp.capacity);
        }

        template<typename U>
        void moveFrom(MemoryAllocation<U> &memoryAllocation) {
            static_assert(std::is_move_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                          "T must be move constructible and convertible from U");
            if (capacity != memoryAllocation.capacity) {
                // manually resize to avoid looping twice
                // completely recreate memoryAllocation
                deallocate();
                allocate(memoryAllocation.capacity);
            }
            for (size_t i = 0; i < capacity; ++i) {
                data[i] = std::move(memoryAllocation.data[i]);
            }
            memoryAllocation.deallocate();
        }

        template<typename U>
        void moveTo(MemoryAllocation<U> &memoryAllocation) {
            static_assert(std::is_move_constructible<U>::value &&
                          std::is_convertible<T, U>::value,
                          "U must be move constructible and convertible from T");
            if (capacity != memoryAllocation.capacity) {
                // manually resize to avoid looping twice
                // completely recreate memoryAllocation
                memoryAllocation.deallocate();
                memoryAllocation.allocate(capacity);
            }
            for (size_t i = 0; i < capacity; ++i) {
                memoryAllocation.data[i] = std::move(data[i]);
            }
            deallocate();
        }

        ~MemoryAllocation() {
            deallocate();
        }

        T &operator[](size_t index) const {
            if (capacity <= index) {
                Log::Error_And_Throw("index out of range: index: ", index, ", capacity: ", capacity);
            }
            return data[index];
        }

        T *getData() const {
            return data;
        }

        size_t getCapacity() const {
            return capacity;
        }

        size_t getCapacityInBytes() const {
            return capacityInBytes;
        }

        size_t getSizeofT() const {
            return sizeof_T;
        }

        void allocate(size_t newCapacity) {
            if (data != nullptr) {
                Log::Error_And_Throw(
                        "attempting to allocate an already allocated pointer "
                        "would result in memory leak, "
                        "please call deallocate() first"
                );
            }
            data = new T[newCapacity];
            capacity = newCapacity;
            capacityInBytes = sizeof_T * newCapacity;
            if (is_primitive<T>::value) {
                // initialize array to zero if primitive
                memset(data, 0, capacityInBytes);
            }
        }
        
        void allocate(size_t newCapacity, const T & initializer) {
            if (data != nullptr) {
                Log::Error_And_Throw(
                        "attempting to allocate an already allocated pointer "
                        "would result in memory leak, "
                        "please call deallocate() first"
                );
            }
            data = new T[newCapacity];
            capacity = newCapacity;
            capacityInBytes = sizeof_T * newCapacity;
            for (size_t i = 0; i < capacity; i++) {
                data[i] = initializer;
            }
        }

        void reallocate(size_t newCapacity) {
            // dont reallocate if we dont need to
            if (newCapacity == capacity) return;
            if (newCapacity <= 0) deallocate();
            else if (data == nullptr) {
                allocate(newCapacity);
            } else {
                MemoryAllocation<T> tmp;
                tmp.allocate(newCapacity);
                size_t min_capacity = std::min(capacity, tmp.capacity);
                for (size_t i = 0; i < min_capacity; i++) {
                    tmp.data[i] = std::move(data[i]);
                }
                delete[] data;
                data = tmp.data;
                tmp.data = nullptr;
                capacity = tmp.capacity;
                tmp.capacity = 0;
                capacityInBytes = sizeof_T * newCapacity;
                tmp.capacityInBytes = 0;
            }
        }
        
        void reallocate(size_t newCapacity, const T & initializer) {
            if (newCapacity <= 0) deallocate();
            else if (data == nullptr) {
                allocate(newCapacity, initializer);
            } else {
                MemoryAllocation<T> tmp;
                tmp.allocate(newCapacity);
                size_t min_capacity = std::min(capacity, tmp.capacity);
                for (size_t i = 0; i < min_capacity; i++) {
                    tmp.data[i] = std::move(data[i]);
                }
                for (size_t i = min_capacity; i < tmp.capacity; i++) {
                    tmp.data[i] = initializer;
                }
                delete[] data;
                data = tmp.data;
                tmp.data = nullptr;
                capacity = tmp.capacity;
                tmp.capacity = 0;
                capacityInBytes = sizeof_T * newCapacity;
                tmp.capacityInBytes = 0;
            }
        }

        void deallocate() {
            delete[] data;
            data = nullptr;
            capacity = 0;
            capacityInBytes = 0;
        }
    };
};

#endif //GRAPHICAL_TOOL_KIT_MEMORYALLOCATOR_H
