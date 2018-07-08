#pragma once

#include "MainPage.g.h"

namespace winrt::Dx11Xaml::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::Dx11Xaml::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
