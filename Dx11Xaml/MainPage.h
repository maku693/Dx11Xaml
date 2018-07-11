#pragma once

#include "MainPage.g.h"

namespace winrt::Dx11Xaml::implementation {
struct MainPage : MainPageT<MainPage> {
  MainPage();

  void OnLoaded([[maybe_unused]] IInspectable const &,
                [[maybe_unused]] Windows::UI::Xaml::RoutedEventArgs const &);
};
} // namespace winrt::Dx11Xaml::implementation

namespace winrt::Dx11Xaml::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::Dx11Xaml::factory_implementation
