#include "pch.h"

#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace DirectX;
using namespace Dx11Xaml::implementation;

MainPage::MainPage() {
  InitializeComponent();
  Loaded({this, &MainPage::OnLoaded});
}

void MainPage::OnLoaded([[maybe_unused]] IInspectable const &,
                        [[maybe_unused]] RoutedEventArgs const &) {
  LoadResources().Completed([this](auto, auto) {
    CompositionTarget::Rendering({this, &MainPage::OnRendering});
  });
}

void MainPage::OnRendering([[maybe_unused]] IInspectable const &,
                           [[maybe_unused]] IInspectable const &) {
  std::array<D3D11_VIEWPORT, 1> viewports{
      {0, 0, static_cast<FLOAT>(swapchainPanel().ActualWidth()),
       static_cast<FLOAT>(swapchainPanel().ActualHeight()), 0, 1}};
  context->RSSetViewports(static_cast<UINT>(viewports.size()),
                          viewports.data());

  const std::array<ID3D11RenderTargetView *, 1> rtvs{rtv.get()};
  context->OMSetRenderTargets(static_cast<UINT>(rtvs.size()), rtvs.data(),
                              dsv.get());

  const std::array<float, 4> clear_color{};
  context->ClearRenderTargetView(rtv.get(), clear_color.data());
  context->ClearDepthStencilView(dsv.get(),
                                 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

  const std::array<ID3D11Buffer *, 1> vertex_buffers{vertex_buffer.get()};
  const std::array<UINT, 1> vertex_buffer_strides{sizeof(XMFLOAT3)};
  const std::array<UINT, 1> vertex_buffer_offsets{0};
  context->IASetVertexBuffers(
      0, static_cast<UINT>(vertex_buffers.size()), vertex_buffers.data(),
      vertex_buffer_strides.data(), vertex_buffer_offsets.data());
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->IASetInputLayout(input_layout.get());

  context->VSSetShader(vertex_shader.get(), nullptr, 0);
  context->PSSetShader(pixel_shader.get(), nullptr, 0);

  context->Draw(3, 0);

  check_hresult(swapchain->Present(1, 0));
}

IAsyncAction MainPage::LoadResources() {
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
  depth_buffer_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
  depth_buffer_desc.SampleDesc.Count = 1;
  depth_buffer_desc.SampleDesc.Quality = 0;
  depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  com_ptr<ID3D11Texture2D> depth_buffer{};
  check_hresult(
      device->CreateTexture2D(&depth_buffer_desc, nullptr, depth_buffer.put()));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
  dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  check_hresult(
      device->CreateDepthStencilView(depth_buffer.get(), &dsv_desc, dsv.put()));

  const auto installed_folder = Package::Current().InstalledLocation();
  const auto vs_file =
      co_await installed_folder.GetFileAsync(L"VertexShader.cso");
  const auto ps_file =
      co_await installed_folder.GetFileAsync(L"PixelShader.cso");

  const auto vs_file_buffer = co_await FileIO::ReadBufferAsync(vs_file);
  std::vector<uint8_t> vs_file_data(vs_file_buffer.Length());
  DataReader::FromBuffer(vs_file_buffer).ReadBytes(vs_file_data);

  const auto ps_file_buffer = co_await FileIO::ReadBufferAsync(ps_file);
  std::vector<uint8_t> ps_file_data(ps_file_buffer.Length());
  DataReader::FromBuffer(ps_file_buffer).ReadBytes(ps_file_data);

  check_hresult(device->CreateVertexShader(
      vs_file_data.data(), vs_file_data.size(), nullptr, vertex_shader.put()));

  check_hresult(device->CreatePixelShader(
      ps_file_data.data(), ps_file_data.size(), nullptr, pixel_shader.put()));

  std::array<D3D11_INPUT_ELEMENT_DESC, 1> layout{
      {"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
       D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}};
  check_hresult(device->CreateInputLayout(
      layout.data(), static_cast<UINT>(layout.size()), vs_file_data.data(),
      vs_file_data.size(), input_layout.put()));

  D3D11_BUFFER_DESC vertex_buffer_desc{};
  vertex_buffer_desc.ByteWidth = sizeof(XMFLOAT3) * 3;
  vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  D3D11_SUBRESOURCE_DATA vertex_buffer_data{};
  std::array<XMFLOAT3, 3> vertices{
      {{-0.5, -0.5, 0}, {0, 0.5, 0}, {0.5, -0.5, 0}}};
  vertex_buffer_data.pSysMem = vertices.data();
  device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_data,
                       vertex_buffer.put());
}
