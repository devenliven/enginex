#include "pch.h"

#include "utilities/logger.h"
#include "editor/app.h"
// #include "editor/bootstrap.h"

int main()
{
    auto app = std::make_shared<App>();
    app->init(app);
    return 1;
}