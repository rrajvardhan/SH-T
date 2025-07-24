#include "Editor.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL_events.h>

Editor::Editor(World& world, ServiceContext& ctx) : _world(world), _ctx(ctx)
{

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.FontGlobalScale = 1.0f;
  io.Fonts->Clear();
  ImFont* customFont = io.Fonts->AddFontFromFileTTF("assets/default.ttf", 20.0f);
  if (!customFont)
  {
    LOG_ERROR("[Editor] Failed to load default font!");
  }

  ImGui::StyleColorsClassic();

  ImGuiStyle& style  = ImGui::GetStyle();
  ImVec4*     colors = style.Colors;

  colors[ImGuiCol_Text]                      = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
  colors[ImGuiCol_TextDisabled]              = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  colors[ImGuiCol_WindowBg]                  = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_ChildBg]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                   = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
  colors[ImGuiCol_Border]                    = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_BorderShadow]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]                   = ImVec4(0.11f, 0.11f, 0.13f, 1.00f);
  colors[ImGuiCol_FrameBgHovered]            = ImVec4(0.11f, 0.11f, 0.13f, 1.00f);
  colors[ImGuiCol_FrameBgActive]             = ImVec4(0.11f, 0.11f, 0.13f, 1.00f);
  colors[ImGuiCol_TitleBg]                   = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TitleBgActive]             = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]          = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
  colors[ImGuiCol_MenuBarBg]                 = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
  colors[ImGuiCol_ScrollbarBg]               = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
  colors[ImGuiCol_ScrollbarGrab]             = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
  colors[ImGuiCol_ScrollbarGrabHovered]      = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
  colors[ImGuiCol_ScrollbarGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
  colors[ImGuiCol_CheckMark]                 = ImVec4(0.56f, 0.56f, 0.78f, 1.00f);
  colors[ImGuiCol_SliderGrab]                = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
  colors[ImGuiCol_SliderGrabActive]          = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
  colors[ImGuiCol_Button]                    = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
  colors[ImGuiCol_ButtonHovered]             = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
  colors[ImGuiCol_ButtonActive]              = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_Header]                    = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_HeaderHovered]             = ImVec4(0.17f, 0.16f, 0.22f, 0.87f);
  colors[ImGuiCol_HeaderActive]              = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
  colors[ImGuiCol_Separator]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
  colors[ImGuiCol_SeparatorHovered]          = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
  colors[ImGuiCol_SeparatorActive]           = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
  colors[ImGuiCol_ResizeGrip]                = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  colors[ImGuiCol_ResizeGripHovered]         = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
  colors[ImGuiCol_ResizeGripActive]          = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
  colors[ImGuiCol_InputTextCursor]           = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
  colors[ImGuiCol_TabHovered]                = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_Tab]                       = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TabSelected]               = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_TabSelectedOverline]       = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TabDimmed]                 = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TabDimmedSelected]         = ImVec4(0.08f, 0.08f, 0.11f, 1.00f);
  colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.53f, 0.53f, 0.87f, 0.00f);
  colors[ImGuiCol_DockingPreview]            = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg]            = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram]             = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]      = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]             = ImVec4(0.27f, 0.27f, 0.38f, 1.00f);
  colors[ImGuiCol_TableBorderStrong]         = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
  colors[ImGuiCol_TableBorderLight]          = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
  colors[ImGuiCol_TableRowBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]             = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
  colors[ImGuiCol_TextLink]                  = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
  colors[ImGuiCol_TextSelectedBg]            = ImVec4(0.31f, 0.31f, 0.45f, 1.00f);
  colors[ImGuiCol_TreeLines]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
  colors[ImGuiCol_DragDropTarget]            = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavCursor]                 = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
  colors[ImGuiCol_NavWindowingHighlight]     = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]         = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]          = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

  style.FramePadding     = ImVec2(6, 6);
  style.ItemSpacing      = ImVec2(8, 6);
  style.ItemInnerSpacing = ImVec2(6, 6);
  style.WindowPadding    = ImVec2(8, 8);
  style.IndentSpacing    = 4.0f;

  style.FrameRounding     = 6.0f;
  style.GrabRounding      = 12.0f;
  style.ScrollbarRounding = 3.0f;
  style.ScrollbarSize     = 0.0f;
  style.WindowRounding    = 0.0f;
  style.TabRounding       = 12.0f;

  style.WindowBorderSize = 1.0f;
  style.FrameBorderSize  = 0.0f;
  style.TabBorderSize    = 0.0f;

  ImGui_ImplSDLRenderer2_CreateDeviceObjects();
  ImGui_ImplSDL2_InitForSDLRenderer(_ctx.graphics->getWindow(), _ctx.graphics->getRenderer());
  ImGui_ImplSDLRenderer2_Init(_ctx.graphics->getRenderer());

  _btexture = new TextureBrowser(ctx.texture, "assets/textures");
  _bscript  = new ScriptBrowser("scripts", world.getScriptSystem());
  _baudio   = new AudioBrowser("assets/sounds", *ctx.audio);
}

void
Editor::events(SDL_Event* event)
{
  ImGui_ImplSDL2_ProcessEvent(event);
}

bool _showImguiEditor = false;
void
Editor::update()
{
  if (_ctx.input->keyPressed(SDL_SCANCODE_F11))
  {
    _showImguiEditor = !_showImguiEditor;
  }

  if (_ctx.input->keyPressed(SDL_SCANCODE_F1))
  {
    _active = !_active;
  }
}

void
Editor::render()
{
  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui::NewFrame();

  renderDockspace();
  renderControls();
  renderEntityPanel();
  renderGamePanel();

  _bscript->draw();
  _baudio->draw();
  _btexture->draw();

  if (_showImguiEditor)
  {
    ImGui::ShowDemoWindow();
  }

  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _ctx.graphics->getRenderer());
}
