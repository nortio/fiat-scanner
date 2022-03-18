#pragma once
#include "font.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_internal.h"
extern ImGuiID main_dock;
extern ImGuiID sidebar_dock;
extern ImGuiID sidebarBottom;

void renderUi(SDL_Window *gWindow);
