#if defined(_WIN32) || defined(_WIN64)
    #define EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #define EXPORT
#endif

class EXPORT SampleLibrary {
    private:
        std::string device_address;
        std::string hw_version;
        std::string fw_version;
        std::string serial_number;
        bool open_ok;
    public:
        SampleLibrary() :
            device_address(""),
            open_ok(false),
            hw_version(""),
            fw_version(""),
            serial_number("") {};
        void get_address();
        void load_values();
};