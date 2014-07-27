// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#include "browser/browser_main_parts.h"

#include "browser/browser_context.h"
#include "browser/web_ui_controller_factory.h"
#include "net/proxy/proxy_resolver_v8.h"

#if defined(USE_AURA)
#include "ui/gfx/screen.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#endif

#if defined(USE_AURA) && defined(USE_X11)
#include "chrome/browser/ui/libgtk2ui/gtk2_ui.h"
#include "ui/views/linux_ui/linux_ui.h"
#include "ui/wm/core/wm_state.h"
#endif

#if defined(TOOLKIT_VIEWS)
#include "browser/views/views_delegate.h"
#endif

#if defined(OS_WIN)
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/l10n/l10n_util_win.h"
#include "ui/gfx/platform_font_win.h"
#endif

namespace brightray {

#if defined(OS_WIN)
namespace {

// gfx::Font callbacks
void AdjustUIFont(LOGFONT* logfont) {
  l10n_util::AdjustUIFont(logfont);
}

int GetMinimumFontSize() {
  return 10;
}

}  // namespace
#endif

BrowserMainParts::BrowserMainParts() {
}

BrowserMainParts::~BrowserMainParts() {
}

void BrowserMainParts::PreEarlyInitialization() {
#if defined(OS_MACOSX)
  IncreaseFileDescriptorLimit();
#endif

#if defined(USE_AURA) && defined(USE_X11)
  views::LinuxUI::SetInstance(BuildGtk2UI());
#endif
}

void BrowserMainParts::ToolkitInitialized() {
#if defined(USE_AURA) && defined(USE_X11)
  views::LinuxUI::instance()->Initialize();
  wm_state_.reset(new wm::WMState);
#endif

#if defined(TOOLKIT_VIEWS)
  views_delegate_.reset(new ViewsDelegate);
#endif

#if defined(OS_WIN)
  gfx::PlatformFontWin::adjust_font_callback = &AdjustUIFont;
  gfx::PlatformFontWin::get_minimum_font_size_callback = &GetMinimumFontSize;
#endif
}

void BrowserMainParts::PreMainMessageLoopStart() {
#if defined(OS_MACOSX)
  InitializeMainNib();
#endif
}

void BrowserMainParts::PreMainMessageLoopRun() {
  browser_context_.reset(CreateBrowserContext());
  browser_context_->Initialize();

  web_ui_controller_factory_.reset(
      new WebUIControllerFactory(browser_context_.get()));
  content::WebUIControllerFactory::RegisterFactory(
      web_ui_controller_factory_.get());
}

void BrowserMainParts::PostMainMessageLoopRun() {
  browser_context_.reset();
}

int BrowserMainParts::PreCreateThreads() {
#if defined(USE_AURA)
  gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE,
                                 views::CreateDesktopScreen());
#endif

  InitProxyResolverV8();
  return 0;
}

BrowserContext* BrowserMainParts::CreateBrowserContext() {
  return new BrowserContext;
}

void BrowserMainParts::InitProxyResolverV8() {
#if defined(OS_WIN)
  net::ProxyResolverV8::CreateIsolate();
#else
  net::ProxyResolverV8::RememberDefaultIsolate();
#endif
}

}  // namespace brightray
