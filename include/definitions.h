#ifdef BUILD_MCT_API  // This macro is defined when building the library
    #define MCT_API __declspec(dllexport)
#else
    #define MCT_API __declspec(dllimport)
#endif
