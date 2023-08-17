#ifdef BUILD_BMC_SDK  // This macro is defined when building the library
    #define BMC_SDK __declspec(dllexport)
#else
    #define BMC_SDK __declspec(dllimport)
#endif
