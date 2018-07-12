#include "pch.h"

#include "MainPage.h"
#include <array>
#include <d3d11_4.h>
#include <dxgi1_6.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Dx11Xaml::implementation;

MainPage::MainPage() {
  InitializeComponent();
  Loaded({this, &MainPage::OnLoaded});
}

void MainPage::OnLoaded([[maybe_unused]] IInspectable const &,
                        [[maybe_unused]] RoutedEventArgs const &) {
  const std::array<D3D_FEATURE_LEVEL, 4> feature_levels{
      D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0};
  com_ptr<ID3D11Device> device{};
  com_ptr<ID3D11DeviceContext> context{};
  check_hresult(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
      feature_levels.data(), static_cast<UINT>(feature_levels.size()), D3D11_SDK_VERSION,
      device.put(), nullptr, context.put()));

  com_ptr<IDXGIFactory2> dxgi_factory{};
  check_hresult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,
                                   IID_PPV_ARGS(dxgi_factory.put())));

  DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
  swapchain_desc.Width = static_cast<UINT>(swapchainPanel().ActualWidth());
  swapchain_desc.Height = static_cast<UINT>(swapchainPanel().ActualHeight());
  swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapchain_desc.SampleDesc.Count = 1;
  swapchain_desc.SampleDesc.Quality = 0;
  swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapchain_desc.BufferCount = 2;
  swapchain_desc.Scaling = DXGI_SCALING_STRETCH;
  swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

  com_ptr<IDXGISwapChain1> swapchain{};
  check_hresult(dxgi_factory->CreateSwapChainForComposition(
      device.get(), &swapchain_desc, nullptr, swapchain.put()));

  check_hresult(swapchainPanel().as<ISwapChainPanelNative>()->SetSwapChain(
      swapchain.get()));

  com_ptr<ID3D11Texture2D> back_buffer{};
  check_hresult(swapchain->GetBuffer(0, IID_PPV_ARGS(back_buffer.put())));

  D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
  rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  rtv_desc.Format = swapchain_desc.Format;
  com_ptr<ID3D11RenderTargetView> rtv{};
  check_hresult(
      device->CreateRenderTargetView(back_buffer.get(), &rtv_desc, rtv.put()));
  const auto rtvs = rtv.get();
  context->OMSetRenderTargets(1, &rtvs, nullptr);

  const std::array<float, 4> clear_color{0, 0, 0, 1};
  context->ClearRenderTargetView(rtv.get(), clear_color.data());

  swapchain->Present(0, 0);
}
