#pragma once

#include "MainPage.g.h"

namespace winrt::Dx11Xaml::implementation {
struct MainPage : MainPageT<MainPage> {
  MainPage();

  void OnLoaded([[maybe_unused]] IInspectable const &,
                [[maybe_unused]] Windows::UI::Xaml::RoutedEventArgs const &);

  void OnRendering([[maybe_unused]] IInspectable const &,
                   [[maybe_unused]] IInspectable const &);

  com_ptr<ID3D11Device> device;
  com_ptr<ID3D11DeviceContext> context;
  com_ptr<IDXGISwapChain1> swapchain;
  com_ptr<ID3D11RenderTargetView> rtv;
  com_ptr<ID3D11DepthStencilView> dsv;

  static const std::array<float, 4> clear_color;
};
} // namespace winrt::Dx11Xaml::implementation

namespace winrt::Dx11Xaml::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::Dx11Xaml::factory_implementation
