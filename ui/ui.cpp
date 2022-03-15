#include "ui.h"

bool initial_layout_done;
ImGuiID main_dock;
ImGuiID sidebar_dock;
ImGuiID sidebarBottom;

void renderUi(SDL_Window *gWindow) {
  ImGui_ImplSDLRenderer_NewFrame();
  ImGui_ImplSDL2_NewFrame(gWindow);
  ImGui::NewFrame();

  ImGui::StyleColorsDark();

  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(viewport, ImGuiDockNodeFlags_PassthruCentralNode);

  if (!initial_layout_done) {
    initial_layout_done = true;

    // Try to find the nodes if there is a loaded state
    ImGuiDockNode *root_node = ImGui::DockBuilderGetNode(dockspace_id);
    // Do we have a tree?
    if (root_node && root_node->ChildNodes[0] && root_node->ChildNodes[0]->ChildNodes[0] && root_node->ChildNodes[0]->ChildNodes[1]) {
      sidebarBottom = root_node->ChildNodes[0]->ChildNodes[1]->ID;
      sidebar_dock = root_node->ChildNodes[0]->ID;
    } else {
      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGuiID dock_id_main = ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dock_id_main, viewport->GetWorkCenter());
      // ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Down, 0.20f, NULL, &dock_id_main);
      // ImGui::DockBuilderSetNodeSize(dock_id_bottom, ImVec2{viewport->GetWorkCenter().x, 100});
      ImGuiID dock_id_sidebar = ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.70f, NULL, &dock_id_main);
      ImGuiID dock_id_sidebarBottom = ImGui::DockBuilderSplitNode(dock_id_sidebar, ImGuiDir_Down, 0.70f, NULL, &dock_id_sidebar);
      // bottom_dock = dock_id_bottom;
      sidebar_dock = dock_id_sidebar;
      main_dock = dock_id_main;
      sidebarBottom = dock_id_sidebarBottom;
      ImGui::DockBuilderFinish(dockspace_id);
    }
  }
  
  ImGui::SetNextWindowDockID(main_dock, ImGuiCond_FirstUseEver);
  ImGui::Begin("Test");
  ImGui::End();

  ImGui::SetNextWindowDockID(sidebar_dock, ImGuiCond_FirstUseEver);
  ImGui::Begin("Settings");
  ImGui::End();

  ImGui::SetNextWindowDockID(sidebarBottom, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Tab1")) {
  }
  ImGui::End();

  ImGui::SetNextWindowDockID(sidebarBottom, ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Tab2")) {
  }
  ImGui::End();

  // Rendering ImGUI
  ImGui::ShowDemoWindow();

  ImGui::Render();

  ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}
