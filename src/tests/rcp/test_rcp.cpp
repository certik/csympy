#include "csympy_rcp.h"

using CSymPy::RCP;
using CSymPy::Ptr;
using CSymPy::rcp;

#define UNIQUE_PTR_DEBUG

template<class T>
class UniquePtr {
public:
    inline explicit UniquePtr( T *ptr ) : ptr_(ptr) {
        CSYMPY_ASSERT(ptr != nullptr)
    }
#ifndef UNIQUE_PTR_DEBUG
    ~UniquePtr() { delete ptr_; }
#endif
    // Copy constructor and assignment are disabled
    inline UniquePtr(const UniquePtr<T>& ptr) = delete;
    UniquePtr<T>& operator=(const UniquePtr<T>& ptr) = delete;
    // Move constructor and assignment
    inline UniquePtr(UniquePtr&&) = default;
    UniquePtr<T>& operator=(UniquePtr&&) = default;
    inline T* operator->() const { return ptr_; }
    inline T& operator*() const { return *ptr_; }
    inline const Ptr<T> ptr() const {
#ifdef UNIQUE_PTR_DEBUG
        return ptr_.ptr();
#else
        return Ptr<T>(ptr_);
#endif
    }
private:
#ifdef UNIQUE_PTR_DEBUG
    RCP<T> ptr_;
#else
    T *ptr_;
#endif
};

int main(int argc, char* argv[])
{

    Ptr<int> a;
    {
        //RCP<int> b = rcp(new int(1));
        UniquePtr<int> b(new int(1));
        *b = 5;
        a = b.ptr();
        std::cout << *a << std::endl;
        std::cout << *b << std::endl;
    }
    std::cout << *a << std::endl;
    *a = 5;

    return 0;
}
