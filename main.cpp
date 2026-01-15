#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Entry {
    uint16_t id;
    uint16_t category;
};

struct Section {
    std::vector<Entry> entries;
};

constexpr size_t kSectionCount{ 30 };

const std::vector<std::string> kSectionNames = {
    "Fighting Stances",
    "Taunts",
    "Finishers",
    "Standing Front Grapples",
    "Standing Front Grapples Groggy",
    "Standing Strikes",
    "Standing Back Grapples",
    "Ground Upper Grapples",
    "Ground Lower Grapples",
    "Ground Strikes",
    "Rope Down Grapples",
    "Turnbuckle Upper Front Grapples",
    "Turnbuckle Upper Back Grapples",
    "Turnbuckle Lower Grapples",
    "Turnbuckle Lower Running Strikes",
    "Aerial Strikes Stand",
    "Aerial Strikes Down",
    "Jump Down Over Strikes",
    "Jump Off Rope Strikes",
    "Running Front Grapples",
    "Running Strikes",
    "Running Back Grapples",
    "Running Counter Grapples",
    "Double Team Moves",
    "Favorites",
    "Winning Moves",
    "Ring In Moves",
    "Ring Out Moves",
    "Combo Strikes Basic",
    "Combo Strikes Final"
};

// 9999.dat has unknown data at the end, we preserve it as is
std::vector<char> trailingData;

static std::vector<Section> LoadSections(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<Section> sections;
    trailingData.clear();

    if (!file) return sections;

    uint32_t sectionCount{ 0 };

    while (file && sectionCount < kSectionCount) {
        uint32_t count{ 0 };
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        if (!file) break;

        Section section;
        for (uint32_t i = 0; i < count; ++i) {
            Entry e{ 0,0 };
            file.read(reinterpret_cast<char*>(&e.id), sizeof(e.id));
            file.read(reinterpret_cast<char*>(&e.category), sizeof(e.category));
            if (!file) break;
            section.entries.push_back(e);
        }

        if (!section.entries.empty()) {
            sections.push_back(section);
            ++sectionCount;
        }
    }

    // Read the rest of the file into trailingData
    if (file) {
        std::streampos currentPos = file.tellg();
        file.seekg(0, std::ios::end);
        std::streampos endPos = file.tellg();
        std::streamsize size = endPos - currentPos;

        if (size > 0) {
            trailingData.resize(static_cast<size_t>(size));
            file.seekg(currentPos);
            file.read(trailingData.data(), size);
        }
    }

    return sections;
}

static void SaveSections(const std::string& path, const std::vector<Section>& sections) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if (!file) {
        std::cerr << "Failed to open file for writing\n";
        return;
    }

    for (size_t i = 0; i < sections.size(); ++i) {
        const auto& section = sections[i];
        uint32_t count = static_cast<uint32_t>(section.entries.size());
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        for (const auto& entry : section.entries) {
            file.write(reinterpret_cast<const char*>(&entry.id), sizeof(entry.id));
            file.write(reinterpret_cast<const char*>(&entry.category), sizeof(entry.category));
        }
        // Section 26 has 12 extra 00 bytes for some reason
        if (i == 25) {
            constexpr uint8_t kExtraBytes[12] = { 00 };
            file.write(reinterpret_cast<const char*>(kExtraBytes), sizeof(kExtraBytes));
        }
    }

    // Append unmodified trailing data
    if (!trailingData.empty()) {
        file.write(trailingData.data(), trailingData.size());
    }
}

int main() {
    // Init GLFW
    if (!glfwInit()) return -1;

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "SD2 9999 Editor", nullptr, nullptr);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Custom font
    ImFontConfig font_config;
    ImFont* customFont = io.Fonts->AddFontFromFileTTF(
        "font/Inconsolata-Regular.ttf",
        24.0f,
        &font_config
    );

    if (!customFont) {
        std::cerr << "Failed to load font\n";
    }

    // File to load, section data and default selection
    static std::string filePath = "9999.dat";
    static std::vector<Section> sections = LoadSections(filePath);
    static int selectedSection = -1;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui window position and size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));

        // Section window
        ImGui::Begin("Sections", nullptr, 
                      ImGuiWindowFlags_NoCollapse | 
                      ImGuiWindowFlags_NoResize | 
                      ImGuiWindowFlags_NoMove);

        ImGui::Indent(5.0f);
        
        // Section list
        for (size_t i = 0; i < sections.size(); ++i) {
            std::string label = std::format("{:02}", i + 1);
            if (i < kSectionNames.size()) {
                label += " " + kSectionNames[i];
            }

            // Select/deselect section
            if (ImGui::Selectable(label.c_str(), selectedSection == static_cast<int>(i))) {
                if (selectedSection == static_cast<int>(i)) {
                    selectedSection = -1;
                }
                else {
                    selectedSection = static_cast<int>(i);
                    ImGui::SetScrollY(0.0f); // Scroll up to avoid visual issues
                }
            }

            // Show entries for selected section
            if (selectedSection == static_cast<int>(i)) {
                auto& entries = sections[i].entries;

                // Moves table
                ImGui::Indent();
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6.0f, 4.0f));
                if (ImGui::BeginTable("Moves Table", 5))
                {
                    // Table header
                    ImGui::TableSetupColumn("Move", ImGuiTableColumnFlags_WidthFixed, 50.0f);
                    ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 55.0f);
                    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 55.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 66.0f);
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
                    ImGui::TableHeadersRow();
                    ImGui::PopStyleColor(3);

                    // Table rows
                    for (size_t j = 0; j < entries.size(); ++j) {
                        ImGui::PushID(static_cast<int>(j));
                        ImGui::TableNextRow();

                        // Column 1: Move
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%03zu", j);

                        // Column 2: ID
                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputScalar(
                            "##ID",
                            ImGuiDataType_U16,
                            &entries[j].id,
                            nullptr,
                            nullptr,
                            "%04X",
                            ImGuiInputTextFlags_CharsHexadecimal
                        );

                        // Column 3: Type
                        ImGui::TableSetColumnIndex(2);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::InputScalar(
                            "##Category",
                            ImGuiDataType_U16,
                            &entries[j].category,
                            nullptr,
                            nullptr,
                            "%04X",
                            ImGuiInputTextFlags_CharsHexadecimal
                        );

                        // Column 4: Add
                        ImGui::TableSetColumnIndex(3);
                        if (ImGui::Button(" Add ")) {
                            entries.insert(entries.begin() + j + 1, { 0x0000, 0x0000 });
                        }

                        // Column 5: Delete
                        ImGui::TableSetColumnIndex(4);
                        if (ImGui::Button(" Delete ")) {
                            entries.erase(entries.begin() + j);
                            --j;
                        }
                        ImGui::PopID();
                    }
                    ImGui::EndTable();
                    ImGui::PopStyleVar();
                }
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                ImGui::Unindent();
                ImGui::Separator();
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        // "Save File" button
        if (ImGui::Button("Save File")) {
            SaveSections(filePath, sections);
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Unindent();
        ImGui::End();

        // Rendering
        ImGui::Render();
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}