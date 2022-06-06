#pragma once

#include <memory>

/**
 * @brief Singleton template class
 * @tparam T class name
 **/
template<class T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton & operator=(Singleton &&) = delete;
    static T& GetInstance();
protected:
    Singleton() {} // Disallow instantiation outside of the class.
};

template<class T>
T& Singleton<T>::GetInstance() {
    static T instance;
    return instance;
}