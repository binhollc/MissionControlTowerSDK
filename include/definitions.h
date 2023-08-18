#ifdef BUILD_BMC_SDK
    #define BMC_SDK __declspec(dllexport)
#else
    #define BMC_SDK __declspec(dllimport)
#endif

#ifndef NDEBUG
    #define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
    #define DEBUG_MSG(str) do { } while ( false )
#endif
