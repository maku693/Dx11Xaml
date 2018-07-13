#include "pch.h"

#include "MainPage.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
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
  check_hresult(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
      feature_levels.data(), static_cast<UINT>(feature_levels.size()),
      D3D11_SDK_VERSION, device.put(), nullptr, context.put()));

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

  check_hresult(dxgi_factory->CreateSwapChainForComposition(
      device.get(), &swapchain_desc, nullptr, swapchain.put()));

  check_hresult(swapchainPanel().as<ISwapChainPanelNative>()->SetSwapChain(
      swapchain.get()));

  com_ptr<ID3D11Texture2D> back_buffer{};
  check_hresult(swapchain->GetBuffer(0, IID_PPV_ARGS(back_buffer.put())));

  D3D11_RENDER_TARGET_VIEW_DESC rtv_desc{};
  rtv_desc.Format = swapchain_desc.Format;
  rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
  check_hresult(
      device->CreateRenderTargetView(back_buffer.get(), &rtv_desc, rtv.put()));

  D3D11_TEXTURE2D_DESC depth_buffer_desc{};
  depth_buffer_desc.Width = swapchain_desc.Width;
  depth_buffer_desc.Height = swapchain_desc.Height;
  depth_buffer_desc.MipLevels = 1;
  depth_buffer_desc.ArraySize = 1;
  depth_buffer_desc.Format = DXGI_FORMAT_R16_TYPELESS;
  depth_buffer_desc.SampleDesc.Count = 1;
  depth_buffer_desc.SampleDesc.Quality = 0;
  depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  com_ptr<ID3D11Texture2D> depth_buffer{};
  check_hresult(device->CreateTexture2D(&depth_buffer_desc, nullptr, depth_buffer.put()));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
  dsv_desc.Format = DXGI_FORMAT_D16_UNORM;
  dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  check_hresult(device->CreateDepthStencilView(depth_buffer.get(), &dsv_desc, dsv.put()));

  const std::array<ID3D11RenderTargetView *, 1> rtvs{rtv.get()};
  context->OMSetRenderTargets(static_cast<UINT>(rtvs.size()), rtvs.data(), dsv.get());

  CompositionTarget::Rendering({this, &MainPage::OnRendering});
}

void MainPage::OnRendering([[maybe_unused]] IInspectable const &,
                           [[maybe_unused]] IInspectable const &) {
  context->ClearRenderTargetView(rtv.get(), clear_color.data());
  context->ClearDepthStencilView(dsv.get(), D3D11_CLEAR_DEPTH, 1, 0);
  check_hresult(swapchain->Present(1, 0));
}

const std::array<float, 4> MainPage::clear_color{0, 0, 0, 1};
