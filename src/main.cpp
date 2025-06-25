#include "pch.h"

#include "utilities/logger.h"
#include "editor/iapp.h"

int main()
{
    auto app = std::make_unique<IApp>();
    app->init();
    return 1;
}