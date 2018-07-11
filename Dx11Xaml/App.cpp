#include "pch.h"

#include "App.h"
#include "MainPage.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Dx11Xaml;
using namespace Dx11Xaml::implementation;

App::App() {
  InitializeComponent();
  Suspending({this, &App::OnSuspending});
}

void App::OnLaunched(LaunchActivatedEventArgs const &e) {
  Frame rootFrame{nullptr};

  auto content = Window::Current().Content();
  if (content != nullptr) {
    rootFrame = content.try_as<Frame>();
  }

  if (rootFrame == nullptr) {
    // Create a Frame to act as the navigation context and associate it with
    // a SuspensionManager key
    rootFrame = Frame();

    rootFrame.NavigationFailed({this, &App::OnNavigationFailed});

    if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated) {
      // Restore the saved session state only when appropriate, scheduling the
      // final launch steps after the restore is complete
    }

    Window::Current().Content(rootFrame);
  }

  if (e.PrelaunchActivated() == false) {
    if (rootFrame.Content() == nullptr) {
      // When the navigation stack isn't restored navigate to the first page,
      // configuring the new page by passing required information as a
      // navigation parameter
      rootFrame.Navigate(xaml_typename<Dx11Xaml::MainPage>(),
                         box_value(e.Arguments()));
    }

    Window::Current().Activate();
  }
}

void App::OnSuspending([[maybe_unused]] IInspectable const &sender,
                       [[maybe_unused]] SuspendingEventArgs const &e) {
  auto deferral = e.SuspendingOperation().GetDeferral();
  // TODO: Save application state and stop any background activity
  deferral.Complete();
}

void App::OnNavigationFailed(IInspectable const &,
                             NavigationFailedEventArgs const &e) {
  throw hresult_error(E_FAIL,
                      L"Failed to load Page " + e.SourcePageType().Name);
}
