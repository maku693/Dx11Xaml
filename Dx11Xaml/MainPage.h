#pragma once

#include "MainPage.g.h"

namespace winrt::Dx11Xaml::implementation {
struct MainPage : MainPageT<MainPage> {
  MainPage();

  void OnLoaded([[maybe_unused]] IInspectable const &,
                [[maybe_unused]] Windows::UI::Xaml::RoutedEventArgs const &);

  void OnRendering([[maybe_unused]] IInspectable const &,
                   [[maybe_unused]] IInspectable const &);

  Windows::Foundation::IAsyncAction LoadResources();

  com_ptr<ID3D11Device> device;
  com_ptr<ID3D11DeviceContext> context;
  com_ptr<IDXGISwapChain1> swapchain;
  com_ptr<ID3D11RenderTargetView> rtv;
  com_ptr<ID3D11DepthStencilView> dsv;
  com_ptr<ID3D11VertexShader> vertex_shader;
  com_ptr<ID3D11PixelShader> pixel_shader;
  com_ptr<ID3D11InputLayout> input_layout;
  com_ptr<ID3D11Buffer> vertex_buffer;
};
} // namespace winrt::Dx11Xaml::implementation

namespace winrt::Dx11Xaml::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::Dx11Xaml::factory_implementation
