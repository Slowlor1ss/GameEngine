#ifndef UTILS_HPP
#define UTILS_HPP

namespace utils {

    template <typename T, typename P>
    inline bool AllEqual(T val, P item) { return val == item; }
    template <typename T, typename P, typename... Args>
    inline bool AllEqual(T val, P item, Args... otherItems) {
        return val == item && AllEqual(val, otherItems...);
    }

    template <typename T, typename P>
    constexpr bool AnyEqual(T val, P item) { return val == item; }
    template <typename T, typename P, typename... Args>
    constexpr bool AnyEqual(T val, P item, Args... otherItems) {
        return val == item || AnyEqual(val, otherItems...);
    }

    template <typename... Args>
    inline bool AnyNull(Args... ptrs) { return AnyEqual(nullptr, ptrs...); }

    //
    //Array functions
    //
    template<typename T>
    inline void CopyArray(T* dst, const T* src, size_t size) {
        for (size_t i = 0; i < size; ++i) dst[i] = src[i];
    }
    template<typename T>
    inline bool CompareArray(const T* a1, const T* a2, size_t size) {
        for (size_t i = 0; i < size; ++i) if (a1[i] != a2[i]) return false;
        return true;
    }
    template<typename T, typename U>
    inline void FillArray(T* arr, const U& val, size_t size) {
        for (size_t i = 0; i < size; ++i) arr[i] = static_cast<T>(val);
    }

    //https://www.geeksforgeeks.org/program-multiplication-array-elements/
    //Implementation
    namespace productImpl {
        //http://retis.sssup.it/~lipari/courses/oosd2010-2/07.metaprogramming-handout.pdf
        template<size_t> struct int2type {};

        //Prevents branching and stops recursion
        template <typename T>
        constexpr int ProductImpl(const T* arr, int2type<0>) { return arr[0]; }

        //Uses recursion num keeps track of where we are in the array, once it's 0 it will stop the recursion
        template <typename T, size_t num>
        inline T ProductImpl(const T* arr, int2type<num>) {
            return arr[0] * ProductImpl(arr + 1, int2type<num - 1>());
        }
    }

    //Product of whole array
    template <size_t num, typename T>
    inline T ArrayProduct(const T* arr) {
        return productImpl::ProductImpl(arr, productImpl::int2type<num - 1>());
    }

    //allows casting to another type & doesn't use recursion
    //requires *= operator
    template<typename T, typename R = T>
    inline R ArrayProduct(const T* arr, size_t size) {
        R prod = 1;
        for (size_t i = 0; i < size; ++i) prod *= arr[i];
        return prod;
    }

    //
    // End Array Functions
    //

    //https://stackoverflow.com/questions/12301916/equality-compare-stdweak-ptr
    //note that this is not consistent with shared_ptr::operator==
    //because that compares the object pointers,
    //and two shared pointers with the same control block can point to different objects
    //TODO: more complex than i thought should give it a better look when i have more time
    template <typename T, typename U>
    inline bool equals(const std::weak_ptr<T>& t, const std::weak_ptr<U>& u)
    {
        return !t.owner_before(u) && !u.owner_before(t);
    }

    template <typename T, typename U>
    inline bool equals(const std::weak_ptr<T>& t, const std::shared_ptr<U>& u)
    {
        return !t.owner_before(u) && !u.owner_before(t);
    }





}// namespace utils
#endif // UTILS_HPP
