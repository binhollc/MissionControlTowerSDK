#if defined(_WIN32) || defined(_WIN64)
    #ifdef BUILD_BMC_SDK
        #define BMC_SDK __declspec(dllexport)
    #elif defined(USE_STATIC_LIBS)
        #define BMC_SDK
    #else
        #define BMC_SDK __declspec(dllimport)
    #endif
#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
    #define BMC_SDK __attribute__((visibility("default")))
#else
    #define BMC_SDK
#endif

#ifndef NDEBUG
    #define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
    #define DEBUG_MSG(str) do { } while ( false )
#endif
