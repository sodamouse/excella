#define COMFYG_IMPLEMENTATION
#include "comfyg.hpp"
#include <iostream>

int main()
{
    const char** stringOption = Comfyg::config_str("string_option", {});
    bool* boolOption = Comfyg::config_bool("boolean option", {});
    int* intOption = Comfyg::config_int("intOption", {});
    double* doubleOption = Comfyg::config_double("DoubleOption", {});

    if (!Comfyg::load_config_file("config.conf"))
    {
        std::cerr << "Something went wrong!\n";
        return 1;
    }

    std::cout << *stringOption << '\n';
    std::cout << *boolOption << '\n';
    std::cout << *intOption << '\n';
    std::cout << *doubleOption << '\n';

    return 0;
}
