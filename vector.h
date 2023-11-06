//
// Created by romka on 04.11.2023.
//

#ifndef MYVECTOR_VECTOR_H
#define MYVECTOR_VECTOR_H

#include <cstddef>
#include <iostream>
#include <new>
#include <stdexcept>


namespace vct {

    template <typename T>
    class vector {

    private:

        T* array_;

        size_t size_;
        size_t capacity_;

        static const size_t multiplyFactor = 2;

    private:

        void free(size_t index) {
            for (size_t i = 0; i < index; ++i) {
                (array_ + i)->~T();
            }
            operator delete(array_);
        }

        void reallocation(size_t new_cap) {
            if (new_cap == 0) {
                capacity_ = 0;
                array_ = nullptr;
                return;
            }
            size_t index = 0;
            auto tmp = static_cast<T*>(operator new(sizeof(T) * new_cap));

            try {
                for (size_t i = 0; i < size_; ++i) {
                    new (tmp + i) T(std::move(array_[i]));
                    index = i;
                }
            }
            catch (...) {
                for (size_t i = 0; i < index + 1; ++i) {
                    (tmp + i)->~T();
                }
                operator delete(tmp);
                throw;
            }
            free(size_);
            capacity_ = new_cap;
            std::swap(array_, tmp);
        }

        void reallocation() {
            if (capacity_ == 0) {
                ++capacity_;
            }
            size_t index = 0;
            capacity_ *= multiplyFactor;
            auto tmp = static_cast<T*>(operator new(sizeof(T) * capacity_));

            try {
                for (size_t i = 0; i < size_; ++i) {
                    new (tmp + i) T(std::move(array_[i]));
                    index = i;
                }
            }
            catch(...) {
                for (size_t i = 0; i < index + 1; ++i) {
                    (tmp + i)->~T();
                }
                operator delete(tmp);
                capacity_ /= multiplyFactor;
                throw;
            }
            free(size_);
            std::move(array_, tmp);
        }

    public:

        vector() noexcept: array_(nullptr), size_(0), capacity_(0) {}

        explicit vector(size_t size): size_(size), capacity_(size) {
            if (size == 0) {
                array_ = nullptr;
            } else {
                array_ = static_cast<T*> (operator new(sizeof(T) * size));
                size_t index = 0;

                try {
                    for (size_t i = 0; i < size; ++i) {
                        new (array_ + i) T();
                        index = i;
                    }
                }
                catch (...) {
                    free(index + 1);
                    throw;
                }
            }
        }

        vector(size_t size, const T& value) : size_(size), capacity_(size) {
            if (size == 0) {
                array_ = nullptr;
            } else {
                array_ = static_cast<T*> (operator new(sizeof(T) * size));
                size_t index = 0;

                try {
                    for (size_t i = 0; i < size; ++i) {
                        new (array_ + i) T(value);
                        index = i;
                    }
                }
                catch (...) {
                    free(index + 1);
                    throw;
                }
            }
        }

        vector(const vector& other) : size_(other.size_), capacity_(other.capacity_) {
            if (size_ == 0) {
                array_ = nullptr;
            } else {
                array_ = static_cast<T*> (operator new(sizeof(T) * size_));
                size_t index = 0;

                try {
                    for (size_t i = 0; i < size_; ++i) {
                        new (array_ + i) T(other.array_[i]);
                        index = i;
                    }
                }
                catch (...) {
                    free(index + 1);
                    throw;
                }
            }
        }

        vector(vector&& other) noexcept: size_(other.size_), capacity_(other.capacity_) {
            if (other.size_ == 0) {
                array_ = nullptr;
            } else {
                array_ = other.array_;
            }
            other.array_ = nullptr;
            other.size_ = 0;
        }



        ~vector() {
            for (size_t i = 0; i < size_; ++i) {
                (array_ + i)->~T();
            }
            operator delete(array_);
        }



        [[nodiscard]] size_t size() const noexcept {
            return size_;
        }

        [[nodiscard]] size_t capacity() const noexcept {
            return capacity_;
        }

        [[nodiscard]] bool empty() const noexcept {
            return size_ == 0;
        }



        void swap(const vector& other) noexcept {
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
            std::swap(array_, other.array_);
        }

        void clear() noexcept {
            for (size_t i = 0; i < size_; ++i) {
                (array_ + i)->~T();
            }
            size_ = 0;
        }

        void pop_back() noexcept {
            (array_ + (size_--) - 1)->~T();
        }

        void shrink_to_fit() {
            if (size_ == 0) {
                operator delete(array_);
            }
            reallocation(size_);
        }

        void reserve(size_t new_cap) {
            if (new_cap > capacity_) {
                reallocation(new_cap);
            }
        }

        void resize(size_t new_size) {
            if (new_size <= size_) {
                for (size_t i = new_size; i < size_; ++i) {
                    (array_ + i)->~T();
                }
                size_ = new_size;
                return;
            }
            if (new_size > capacity_) {
                reallocation(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                new (array_ + i) T();
            }
            size_ = new_size;
        }

        void resize(size_t new_size, const T& value) {
            if (new_size <= size_) {
                for (size_t i = new_size; i < size_; ++i) {
                    (array_ + i)->~T();
                }
                size_ = new_size;
                return;
            }
            if (new_size > capacity_) {
                reallocation(new_size);
            }
            for (size_t i = size_; i < new_size; ++i) {
                new (array_ + i) T(std::move(value));
            }
            size_ = new_size;
        }

        template <typename... Args>
        void emplace_back(Args&&... args) {
            if (size_ >= capacity_) {
                reallocation();
            }
            new (array_ + (size_++)) T(std::forward<Args>(args)...);
        }

        void push_back(const T& value) {
            if (size_ >= capacity_) {
                reallocation();
            }
            try {
                new (array_ + (size_++)) T(value);
            }
            catch (...) {
                (array_ + size_)->~T();
                throw;
            }
        }

        void push_back(T&& value) {
            if (size_ >= capacity_) {
                reallocation();
            }
            try {
                new (array_ + (size_++)) T(std::move(value));
            }
            catch (...) {
                (array_ + size_)->~T();
                throw;
            }
        }



        T& at(size_t index) {
            if (index >= size_) {
                throw std::out_of_range("VectorOutOfRange");
            }
            return array_[index];
        }

        const T& at(size_t index) const {
            if (index >= size_) {
                throw std::out_of_range("VectorOutOfRange");
            }
            return array_[index];
        }

        T& front() noexcept {
            return array_[0];
        }

        T& back() noexcept {
            return array_[size_ - 1];
        }

        const T& front() const noexcept {
            return array_[0];
        }

        const T& back() const noexcept {
            return array_[size_ - 1];
        }

        T* data() noexcept {
            return array_;
        }

        const T* data() const noexcept {
            return array_;
        }



        vector& operator=(const vector& other) {
            if (this == &other) {
                return *this;
            }
            auto tmp = other;
            swap(tmp);
            return *this;
        }

        vector& operator=(vector&& other) noexcept {
            if (this == &other) {
                return *this;
            }
            auto tmp = std::move(other);
            swap(tmp);
            return *this;
        }

        T& operator[](size_t index) {
            return array_[index];
        }

        const T& operator[](size_t index) const {
            return array_[index];
        }

        bool operator<(const vector& other) const {
            for (size_t i = 0; i < std::min(size_, other.size_); ++i) {
                if (array_[i] != other.array_[i]) {
                    return array_[i] < other.array_[i];
                }
            }
            return size_ < other.size_;
        }

        bool operator>(const vector& other) const {
            return other < *this;
        }

        bool operator==(const vector& other) const {
            return (*this <= other) && (*this >= other);
        }
    };
}

#endif //MYVECTOR_VECTOR_H
