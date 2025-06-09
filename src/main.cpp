#include "stdafx.h"
#include "app.hpp"

int main()
{
    try {
        App app;
        return app.Run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
}