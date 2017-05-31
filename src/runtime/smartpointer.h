#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#include <cassert>
#include <type_traits>
#include <functional>

/**
 * 智能指针。
 * cocos的RefPtr限制了前置声明的语法，这里的智能指针更通用一些。
 */
template <typename T>
class SmartPointer
{
public:

    SmartPointer()
        : _ptr(nullptr)
    {

    }

    SmartPointer(SmartPointer<T> && other)
    {
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    SmartPointer(T * ptr)
        : _ptr(ptr)
    {
        if(_ptr)
        {
            _ptr->retain();
        }
    }

    SmartPointer(std::nullptr_t ptr)
        : _ptr(nullptr)
    {

    }

    SmartPointer(const SmartPointer<T> & other)
        : _ptr(const_cast<T*>(other._ptr))
    {
        if(_ptr)
        {
            _ptr->retain();
        }
    }

    ~SmartPointer()
    {
        if(_ptr)
        {
            _ptr->release();
        }
    }

    SmartPointer<T> & operator = (T * other)
    {
        if (other != _ptr)
        {
            if(other)
            {
                other->retain();
            }
            if(_ptr)
            {
                _ptr->release();
            }
            _ptr = other;
        }

        return *this;
    }

    SmartPointer<T> & operator = (const SmartPointer<T> & other)
    {
        return *this = const_cast<T*>(other._ptr);
    }

    SmartPointer<T> & operator = (SmartPointer<T> && other)
    {
        if (&other != this)
        {
            if(_ptr)
            {
                _ptr->release();
            }
            _ptr = other._ptr;
            other._ptr = nullptr;
        }

        return *this;
    }

    SmartPointer<T> & operator = (std::nullptr_t other)
    {
        if(_ptr)
        {
            _ptr->release();
            _ptr = nullptr;
        }
        return *this;
    }

    operator T * () const { return _ptr; }

    T & operator * () const
    {
        assert(_ptr && "Attempt to dereference a null pointer!");
        return *_ptr;
    }

    T * operator->() const
    {
        assert(_ptr && "Attempt to dereference a null pointer!");
        return _ptr;
    }

    T * get() const { return _ptr; }


    bool operator == (const SmartPointer<T> & other) const { return _ptr == other._ptr; }

    bool operator == (const T * other) const { return _ptr == other; }

    bool operator == (typename std::remove_const<T>::type * other) const { return _ptr == other; }

    bool operator == (const std::nullptr_t other) const { return _ptr == other; }


    bool operator != (const SmartPointer<T> & other) const { return _ptr != other._ptr; }

    bool operator != (const T * other) const { return _ptr != other; }

    bool operator != (typename std::remove_const<T>::type * other) const { return _ptr != other; }

    bool operator != (const std::nullptr_t other) const { return _ptr != other; }


    bool operator > (const SmartPointer<T> & other) const { return _ptr > other._ptr; }

    bool operator > (const T * other) const { return _ptr > other; }

    bool operator > (typename std::remove_const<T>::type * other) const { return _ptr > other; }


    bool operator < (const SmartPointer<T> & other) const { return _ptr < other._ptr; }

    bool operator < (const T * other) const { return _ptr < other; }

    bool operator < (typename std::remove_const<T>::type * other) const { return _ptr < other; }


    bool operator >= (const SmartPointer<T> & other) const { return _ptr >= other._ptr; }

    bool operator >= (const T * other) const { return _ptr >= other; }

    bool operator >= (typename std::remove_const<T>::type * other) const { return _ptr >= other; }


    bool operator <= (const SmartPointer<T> & other) const { return _ptr <= other._ptr; }

    bool operator <= (const T * other) const { return _ptr <= other; }

    bool operator <= (typename std::remove_const<T>::type * other) const { return _ptr <= other; }


    operator bool() const { return _ptr != nullptr; }

    void reset()
    {
        if(_ptr)
        {
            _ptr->release();
            _ptr = nullptr;
        }
    }

    void swap(SmartPointer<T> & other)
    {
        if (&other != this)
        {
            T * tmp = _ptr;
            _ptr = other._ptr;
            other._ptr = tmp;
        }
    }

    /**
     * This function assigns to this SmartPointer<T> but does not increase the reference count of the object pointed to.
     * Useful for assigning an object created through the 'new' operator to a SmartPointer<T>. Basically used in scenarios
     * where the SmartPointer<T> has the initial ownership of the object.
     *
     * E.G:
     *      SmartPointer<cocos2d::Image> image;
     *      image.weakAssign(new cocos2d::Image());
     *
     * Instead of:
     *      SmartPointer<cocos2d::Image> image;
     *      image = new cocos2d::Image();
     *      image->release();               // Required because new'd object already has a reference count of '1'.
     */
    void weakAssign(const SmartPointer<T> & other)
    {
        if(_ptr)
        {
            _ptr->release();
        }
        _ptr = other._ptr;
    }

private:
    T * _ptr;
};


template<class T> inline
bool operator<(const SmartPointer<T>& r, std::nullptr_t)
{
    return std::less<T*>()(r.get(), nullptr);
}

template<class T> inline
bool operator<(std::nullptr_t, const SmartPointer<T>& r)
{
    return std::less<T*>()(nullptr, r.get());
}

template<class T> inline
bool operator>(const SmartPointer<T>& r, std::nullptr_t)
{
    return nullptr < r;
}

template<class T> inline
bool operator>(std::nullptr_t, const SmartPointer<T>& r)
{
    return r < nullptr;
}

template<class T> inline
bool operator<=(const SmartPointer<T>& r, std::nullptr_t)
{
    return !(nullptr < r);
}

template<class T> inline
bool operator<=(std::nullptr_t, const SmartPointer<T>& r)
{
    return !(r < nullptr);
}

template<class T> inline
bool operator>=(const SmartPointer<T>& r, std::nullptr_t)
{
    return !(r < nullptr);
}

template<class T> inline
bool operator>=(std::nullptr_t, const SmartPointer<T>& r)
{
    return !(nullptr < r);
}

#endif // SMARTPOINTER_H
