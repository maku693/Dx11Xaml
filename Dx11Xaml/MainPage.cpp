#include "pch.h"

#include "MainPage.h"
#include <array>
#include <d3d11_4.h>
#include <dxgi1_6.h>

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Dx11Xaml::implementation;

MainPage::MainPage() {
  InitializeComponent();

  const std::array<D3D_FEATURE_LEVEL, 6> feature_levels{
      D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
  };
  com_ptr<ID3D11Device> device{};
  com_ptr<ID3D11DeviceContext> context{};
  check_hresult(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
      feature_levels.data(), feature_levels.size(), D3D11_SDK_VERSION,
      device.put(), nullptr, context.put()));

  com_ptr<IDXGIFactory2> dxgi_factory{};
  check_hresult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,
                                   IID_PPV_ARGS(dxgi_factory.put())));

  DXGI_SWAP_CHAIN_DESC1 desc{};
  desc.Width = 1280;
  desc.Height = 720;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
  desc.BufferCount = 2;
  desc.Scaling = DXGI_SCALING_STRETCH;
  desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

  com_ptr<IDXGISwapChain1> swapchain{};
  check_hresult(dxgi_factory->CreateSwapChainForComposition(
      device.get(), &desc, nullptr, swapchain.put()));
}
