#include <glad.h>
#include "Algorithm.h"
#include "easyimgui.h"
#include <GLFW/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>

// Define a simple 3D vector struct
struct Vec3 {
  float x, y, z;
  Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

  // Apply rotation around the Y axis (Theta)
  Vec3 rotateY(float angle) const {
    float rad = glm::radians(angle);
    float cos_angle = cos(rad);
    float sin_angle = sin(rad);
    return Vec3(x * cos_angle - z * sin_angle, y,
                x * sin_angle + z * cos_angle);
  }

  // Apply rotation around the X axis (Phi)
  Vec3 rotateX(float angle) const {
    float rad = glm::radians(angle);
    float cos_angle = cos(rad);
    float sin_angle = sin(rad);
    return Vec3(x, y * cos_angle - z * sin_angle,
                y * sin_angle + z * cos_angle);
  }

  // Apply rotation around the Z axis (Delta)
  Vec3 rotateZ(float angle) const {
    float rad = glm::radians(angle);
    float cos_angle = cos(rad);
    float sin_angle = sin(rad);
    return Vec3(x * cos_angle - y * sin_angle, x * sin_angle + y * cos_angle,
                z);
  }

  // Apply translation
  Vec3 translate(float tx, float ty, float tz) const {
    return Vec3(x + tx, y + ty, z + tz);
  }
};

// Parameters for the control panel
struct ProjectionParameters {
  bool perspective = true;
  Vec3 vrp = Vec3(0, 0, 0); // View Reference Point (VRP)
  float theta = 0.0f;       // Rotation around Y-axis
  float phi = 0.0f;         // Rotation around X-axis
  float delta = 0.0f;       // Rotation around Z-axis
  Vec3 prp = Vec3(0, 0, 1000); // Projection Reference Point (PRP)
  ImVec2 cw = ImVec2(400, 300); // Window Center (CW) - Adjusted for centering
};

// Function to apply rotation (combined rotation around all axes)
Vec3 RotateVertex(const Vec3 &vertex, float theta, float phi, float delta) {
  // Apply rotations around each axis
  Vec3 rotated = vertex.rotateY(theta);
  rotated = rotated.rotateX(phi);
  rotated = rotated.rotateZ(delta);
  return rotated;
}

// Function to apply perspective projection with respect to the PRP
ImVec2 ProjectVertex(const Vec3 &vertex, const ProjectionParameters &params) {
  if (params.perspective) {
    float d = params.prp.z; // Use PRP's z-coordinate as the distance
    return ImVec2(
        d * (vertex.x - params.prp.x) / (d + vertex.z - params.prp.z) + params.cw.x,
        d * (vertex.y - params.prp.y) / (d + vertex.z - params.prp.z) + params.cw.y
    );
  } else {
    return ImVec2(vertex.x + params.cw.x, vertex.y + params.cw.y);
  }
}

// Function to draw a line between two points
void DrawLine(ImDrawList *draw_list, const ImVec2 &canvas_pos,
              const ImVec2 &p1, const ImVec2 &p2, uint32_t color) {
  draw_list->AddLine(p1 + canvas_pos, p2 + canvas_pos, color, 1.0f);
}

void DrawControlPanel(ProjectionParameters &params) {
  ImGui::Begin("Parameter Settings");

  // Projection type
  ImGui::Text("Projection Type");
  if (ImGui::RadioButton("Perspective", params.perspective)) {
    params.perspective = true;
  }
  if (ImGui::RadioButton("Parallel", !params.perspective)) {
    params.perspective = false;
  }

  // VRP
  ImGui::Text("View Reference Point VRP (WC)");
  ImGui::SliderFloat3("VRP", (float *)&params.vrp, -100.0f, 100.0f);

  // View reference coordinate system rotation angles
  ImGui::Text("View Reference Coordinate System Rotation Angles");
  ImGui::SliderAngle("Theta (Y-axis)", &params.theta, -360.0f,
                     360.0f); // Adjusted range
  ImGui::SliderAngle("Phi (X-axis)", &params.phi, -360.0f,
                     360.0f); // Adjusted range
  ImGui::SliderAngle("Delta (Z-axis)", &params.delta, -360.0f,
                     360.0f); // Adjusted range

  // PRP
  ImGui::Text("Projection Reference Point PRP (VRC)");
  ImGui::SliderFloat3("PRP", (float *)&params.prp, -100.0f, 100.0f);

  // CW
  ImGui::Text("Window Center CW (VRC)");
  ImGui::SliderFloat2("CW", (float *)&params.cw, -500.0f,
                      500.0f); // Make sure to adjust this accordingly

  // Apply and Restore buttons
  if (ImGui::Button("Apply")) {
    // Apply changes
  }
  ImGui::SameLine();
  if (ImGui::Button("Restore")) {
    // Restore default values
    params = ProjectionParameters();
  }

  ImGui::End();
}

void Draw3DHouse(ImDrawList *draw_list, const ImVec2 &canvas_pos,
                 const ProjectionParameters &params) {
  // Define the vertices for the house
  std::vector<Vec3> vertices = {
      Vec3(-50, -50, -200), Vec3(50, -50, -200), Vec3(50, 50, -200),
      Vec3(-50, 50, -200),  Vec3(-50, -50, -100), Vec3(50, -50, -100),
      Vec3(50, 50, -100),    Vec3(-50, 50, -100),  Vec3(0, 100, -150) // Roof peak
  };

  // Apply transformations based on parameters
  for (auto &vertex : vertices) {
    vertex = RotateVertex(vertex, params.theta, params.phi, params.delta);
    vertex.x += params.vrp.x;
    vertex.y += params.vrp.y;
    vertex.z += params.vrp.z;
  }

  // Project vertices to 2D
  std::vector<ImVec2> projectedVertices;
  for (const auto &vertex : vertices) {
    projectedVertices.push_back(ProjectVertex(vertex, params));
  }

  // Draw the wireframe of the house
  std::vector<std::pair<int, int>> edges = {
      {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Base
      {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top
      {0, 4}, {1, 5}, {2, 6}, {3, 7}, // Sides
      {3, 8}, {2, 8}, {7, 8}, {6, 8}  // Roof
  };

  for (const auto &edge : edges) {
    DrawLine(draw_list, canvas_pos, projectedVertices[edge.first],
             projectedVertices[edge.second], IM_COL32(255, 255, 255, 255));
  }
}

int main() {
  // Initialize GLFW and ImGui
  GLFWwindow *window = InitGLFWAndImGui("3D House", 800, 600);
  if (!window)
    return -1;

  ProjectionParameters params;

  while (!glfwWindowShouldClose(window)) {
    // Start the ImGui frame
    BeginImGuiFrame();

    // Draw the control panel
    DrawControlPanel(params);

    // Create a new ImGui window for the 3D house
    ImGui::Begin("3D House Drawing");

    // Get the draw list and canvas position
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();

    // Draw the 3D house with transformations
    Draw3DHouse(draw_list, canvas_pos, params);

    // End the ImGui window
    ImGui::End();

    // End the ImGui frame and render
    EndImGuiFrame(window);

    // Poll for and process events
    glfwPollEvents();
  }

  // Cleanup
  CleanupGLFWAndImGui(window);
  return 0;
}