#include "Algorithm.h"
#include "easyimgui.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <vector>

// Function to draw rectangles
void DrawRectangle(ImDrawList *draw_list, const ImVec2 &top_left,
                   const ImVec2 &bottom_right, ImU32 color,
                   float thickness = 1.0f) {
  draw_list->AddRect(top_left, bottom_right, color, 0.0f, 0, thickness);
}

// Function to perform viewport transformation
ImVec2 ViewportTransform(const ImVec2 &point, const ImVec2 &windowTopLeft,
                         const ImVec2 &windowSize, const ImVec2 &viewportTopLeft,
                         const ImVec2 &viewportSize) {
  ImVec2 scale(viewportSize.x / windowSize.x, viewportSize.y / windowSize.y);
  ImVec2 offset = viewportTopLeft - windowTopLeft * scale;
  return ImVec2(point.x * scale.x + offset.x, point.y * scale.y + offset.y);
}

void DrawControlPoints(ImDrawList* draw_list, const std::vector<ControlPoint>& controlPoints, ImU32 color, float radius) {
    for (const auto& cp : controlPoints) {
        draw_list->AddCircleFilled(cp.position, radius, color);
    }
}

void UpdateControlPoints(std::vector<ControlPoint>& controlPoints, ImVec2 windowTopLeft, ImVec2 windowSize) {
    for (auto& cp : controlPoints) {
        if (cp.isDragging) {
            cp.position = ImVec2(
                std::max(windowTopLeft.x, std::min(windowTopLeft.x + windowSize.x, cp.position.x)),
                std::max(windowTopLeft.y, std::min(windowTopLeft.y + windowSize.y, cp.position.y))
            );
        }
    }
}

int main() {
  // Initialize GLFW and ImGui
  GLFWwindow *window =
      InitGLFWAndImGui("Viewport Transformation Demo");
  if (!window)
    return -1;

  // Define window and viewport dimensions
  ImVec2 windowSize(300, 196);
  ImVec2 viewportSize(300, 196);

  // Define rectangles in world coordinates
  std::vector<std::pair<ImVec2, ImVec2>> rectangles = {
      {ImVec2(50, 50), ImVec2(150, 250)},
      {ImVec2(100, 120), ImVec2(200, 170)},
      {ImVec2(120, 30), ImVec2(180, 80)}};

  // Window open flags
  bool showWorldWindow = true;
  bool showViewportWindow = true;

  // Declare windowTopLeft before the if block
  ImVec2 windowTopLeft;
  ImVec2 windowBottomRight;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    BeginImGuiFrame();

    // Control panel
    ImGui::Begin("Control Panel");

    for (size_t i = 0; i < rectangles.size(); ++i) {
        ImVec2 rectSize = rectangles[i].second - rectangles[i].first;
        ImGui::SliderFloat2(("Rectangle " + std::to_string(i + 1) + " Position").c_str(),
                            (float *)&rectangles[i].second, -100.0f, 400.0f);
    }

    ImGui::Checkbox("Show World Coordinates", &showWorldWindow);
    ImGui::Checkbox("Show Viewport Coordinates", &showViewportWindow);

    ImGui::End();

    // Blue window (world coordinates)
    if (showWorldWindow) {
      ImGui::Begin("World Coordinates", &showWorldWindow, ImGuiWindowFlags_NoResize);
      ImDrawList *draw_list = ImGui::GetWindowDrawList();
      ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

      // Set background to white
      ImVec2 canvas_size = ImGui::GetContentRegionAvail();
      draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

      // Draw window rectangle
      windowTopLeft = canvas_pos;
      windowBottomRight = ImVec2(canvas_pos.x + windowSize.x, canvas_pos.y + windowSize.y);
      DrawRectangle(draw_list, windowTopLeft, windowBottomRight,
                    IM_COL32(0, 0, 255, 255), 2.0f);

      // Draw rectangles in world coordinates
      for (const auto &rect : rectangles) {
        DrawRectangle(draw_list, windowTopLeft + rect.first,
                      windowTopLeft + rect.second, IM_COL32(0, 0, 0, 255), 1.0f);
      }

      ImGui::End();
    }

    // Green window (viewport coordinates)
    if (showViewportWindow) {
      ImGui::Begin("Viewport Coordinates", &showViewportWindow, ImGuiWindowFlags_NoResize);
      ImDrawList *draw_list = ImGui::GetWindowDrawList();
      ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

      // Set background to white
      ImVec2 canvas_size = ImGui::GetContentRegionAvail();
      draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

      // Draw viewport rectangle
      ImVec2 viewportTopLeft = canvas_pos;
      ImVec2 viewportBottomRight = ImVec2(viewportTopLeft.x + viewportSize.x, viewportTopLeft.y + viewportSize.y);
      DrawRectangle(draw_list, viewportTopLeft, viewportBottomRight, IM_COL32(0, 255, 0, 255), 2.0f);

      // Draw transformed rectangles in viewport
      for (const auto &rect : rectangles) {
        ImVec2 transformedTopLeft = ViewportTransform(windowTopLeft + rect.first, windowTopLeft, windowSize, viewportTopLeft, viewportSize);
        ImVec2 transformedBottomRight = ViewportTransform(windowTopLeft + rect.second, windowTopLeft, windowSize, viewportTopLeft, viewportSize);

        // Clip to viewport
        transformedTopLeft.x = std::max(transformedTopLeft.x, viewportTopLeft.x);
        transformedTopLeft.y = std::max(transformedTopLeft.y, viewportTopLeft.y);
        transformedBottomRight.x = std::min(transformedBottomRight.x, viewportBottomRight.x);
        transformedBottomRight.y = std::min(transformedBottomRight.y, viewportBottomRight.y);

        // Ensure the transformed rectangle is at least partially inside the viewport before drawing it
        if (transformedTopLeft.x < transformedBottomRight.x && transformedTopLeft.y < transformedBottomRight.y) {
          DrawRectangle(draw_list, transformedTopLeft, transformedBottomRight, IM_COL32(0, 0, 0, 255), 1.0f);
        }
      }

      ImGui::End();
    }

    EndImGuiFrame(window);
  }

  CleanupGLFWAndImGui(window);
  return 0;
}