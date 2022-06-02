#ifndef UTILS_HPP
#define UTILS_HPP
#include <functional>

namespace utils {

    //struct written together with ruben in prog2
    struct DelayedCallback{
        DelayedCallback() = default;
        DelayedCallback(float interval, std::function<void()> func, int nTimes = 1);

        int nTimes{ 1 };	//If it happens once or multiple times (-1 for inf)
        bool finished{ false };
        float interval{ 2.f };	//The times it takes for the timer to execute its behavior or whatever

        float accumulatedTime{ 0.f };

        std::function<void()> func;

        void Update(float deltaTime);
        void Reset();

    private:
        int startNTimes{ 0 };
    };

    inline DelayedCallback::DelayedCallback(float interval, std::function<void()> func, int nTimes)
        : nTimes{ nTimes }, interval{ interval }, func{ func }, startNTimes(nTimes){
	    if (nTimes == 0)
		    finished = true;
    }

    inline void DelayedCallback::Update(float deltaTime){
        if (finished) return;

        accumulatedTime += deltaTime;
        if (accumulatedTime >= interval)
        {
            //our timer ticked 
            func();
            if (nTimes > 0)
				--nTimes;

            if (nTimes == 0) finished = true;
            else  accumulatedTime -= interval;
        }
    }

    inline void DelayedCallback::Reset(){
        nTimes = startNTimes;
        accumulatedTime = 0;
        finished = false;
    }


/*=============================================================================*/
// Stolen from: Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/

/* --- CONSTANTS --- */
constexpr auto g_PI = 3.14159265358979323846;
constexpr auto g_PI_2 = 1.57079632679489661923;
constexpr auto g_PI_4 = 0.785398163397448309616;

/* --- FUNCTIONS --- */

    /*! An accurate inverse square root*/
    inline float InvSqrt(float f) //sqrtf not defined as constexpr
    {
        return 1.0f / sqrtf(f);
    }
    /*! An fast inverse square root, not fully accurate. Implementation based on Quake III Arena*/
    /*! Reference: https://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/ */
    inline float InvSqrtFst(float f)
    {
        const float xHalf = 0.5f * f;
        int i = *reinterpret_cast<int*>(&f);
        i = 0x5f3759df - (i >> 1);
        f = *reinterpret_cast<float*>(&i);
        f = f * (1.5f - xHalf * f * f);
        return f;
    }
    /*! Function to square a number */
    template<typename T, typename = std::enable_if<std::is_pod<T>::value>>
    constexpr auto Square(T v)
    {
        return v * v;
    }
    /*! Function to convert degrees to radians */
    constexpr float ToRadians(const float angle)
    {
        return angle * (static_cast<float>(g_PI) / 180.f);
    }
    /*! Function to convert radians to degrees */
    constexpr float ToDegrees(const float angle)
    {
        return angle * (180.f / static_cast<float>(g_PI));
    }
    /*! Template function to clamp between a minimum and a maximum value -> in STD since c++17 */
    template<typename T>
    constexpr T Clamp(const T a, T min, T max)
    {
        if (a < min)
            return min;

        if (a > max)
            return max;

        return a;
    }

    /*! Template function to clamp between a minimum and a maximum value*/
    template<typename T>
    constexpr T ClampRef(T& a, T min, T max)
    {
        if (a < min)
            a = min;

        if (a > max)
            a = max;

        return a;
    }

    /*! Random Integer */
    inline int randomInt(int max = 1)
    {
        return rand() % max;
    }
    inline int randomInt(int min, int max)
    {
        return min + (rand() % (max - min + 1));
    }

    /*! Random Float */
    inline float randomFloat(float max = 1.f)
    {
        return max * (float(rand()) / RAND_MAX);
    }

    /*! Random Float */
    inline float randomFloat(float min, float max)
    {
        auto range = max - min;
        return (range * (float(rand()) / RAND_MAX)) + min;
    }

    /*! Random Binomial Float */
    inline float randomBinomial(float max = 1.f)
    {
        return randomFloat(max) - randomFloat(max);
    }

    /*! Linear Interpolation */
    /*inline float Lerp(float v0, float v1, float t)
    { return (1 - t) * v0 + t * v1;	}*/
    template<typename T>
    inline T Lerp(T v0, T v1, float t)
    {
        return (1 - t) * v0 + t * v1;
    }

    /*! Smooth Step */
    inline float smoothStep(float edge0, float edge1, float x)
    {
        // Scale, bias and saturate x to 0..1 range
        x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        // Evaluate polynomial
        return x * x * (3 - 2 * x);
    }

    /*! Sign Function*/
    template <typename T>  int sign(T val)
    {
        return (T(0) < val) - (val < T(0));
    }
/*=============================================================================*/
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/



    //https://stackoverflow.com/questions/17333/what-is-the-most-effective-way-for-float-and-double-comparison
    inline bool ApproximatelyEqual(float a, float b, float epsilon = FLT_EPSILON)
    {
        return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    inline bool definitelyGreaterThan(float a, float b, float epsilon = FLT_EPSILON)
    {
        return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    inline bool definitelyLessThan(float a, float b, float epsilon = FLT_EPSILON)
    {
        return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    inline bool GreaterThanWithMargin(float a, float b, float margin)
    {
        return a - b > margin;
    }

    inline bool LessThanWithMargin(float a, float b, float margin)
    {
        return b - a > margin;
    }

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
    //Wrote these in the start just for fun
    //but std::array should probably be used instead
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
