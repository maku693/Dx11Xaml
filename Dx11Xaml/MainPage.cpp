#include "pch.h"
#include "MainPage.h"
#include <array>
#include <d3d11_4.h>
#include <dxgi1_6.h>

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Dx11Xaml::implementation;

MainPage::MainPage()
{
	InitializeComponent();

	com_ptr<IDXGIFactory> dxgi_factory{};
	check_hresult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgi_factory.put())));

	com_ptr<IDXGIAdapter> dxgi_adapter{};
	dxgi_factory.as<IDXGIFactory6>()->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(dxgi_adapter.put()));

	const std::array<D3D_FEATURE_LEVEL, 1> feature_levels{ D3D_FEATURE_LEVEL_11_0 };
	com_ptr<ID3D11Device> device{};
	com_ptr <ID3D11DeviceContext> context{};
	D3D_FEATURE_LEVEL feature_level{};

	D3D11CreateDevice(dxgi_adapter.get(), D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUGGABLE, feature_levels.data(), feature_levels.size(), D3D11_SDK_VERSION, device.put(), &feature_level, context.put());
}
