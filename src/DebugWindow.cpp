#include "DebugWindow.h"
#include <iostream>
#include <sstream>

// Ugly hack for ImGui..... will disappear soon
static ImGuiIO dummy_io;

//---------------------------------------------------------
// DebugWindow()
//---------------------------------------------------------
DebugWindow::DebugWindow() : io(dummy_io)
{

}

//---------------------------------------------------------
// ~DebugWindow()
//---------------------------------------------------------
DebugWindow::~DebugWindow()
{

}

//---------------------------------------------------------
// init()
//---------------------------------------------------------
bool DebugWindow::init()
{
    if (!glfwInit()) {
        std::cout << "DebugWindow::init() Could not init glfw." << '\n';
        return false;
    }

    // Save back the current GLFW context (primary application should not know we are interacting with GLFW)
    GLFWwindow* returnContext = glfwGetCurrentContext();

    // Create window with graphics context
    debugWindow = glfwCreateWindow(600, 900, "Debug Window", nullptr, nullptr);
    if (debugWindow == nullptr) {
        std::cout << "DebugWindow::init() glfwCreateWindow() returned nullptr." << '\n';
    }

    glfwMakeContextCurrent(debugWindow);
    glfwSwapInterval(1); // Enable vsync

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(debugWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Return to the primary application's glfw context
    glfwMakeContextCurrent(returnContext);

    initialized = true;

    return initialized;
}


static void printToScreen()
{
    std::cout << "pressed" << '\n';
}

//---------------------------------------------------------
// draw()
//---------------------------------------------------------
void DebugWindow::draw()
{
    if (initialized) {
        // Save back glfwContext we enter with so we can swap back at the end of draw()
        GLFWwindow* returnContext = glfwGetCurrentContext();
        glfwMakeContextCurrent(debugWindow);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(debugWindow, &display_w, &display_h);

        //--------Begin IMGUI Window--------//
        ImGui::Begin("Debug Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(display_w, display_h));

        for (const auto& [label, f, lowerBound, upperBound] : registeredSliderFloats) {
            ImGui::SliderFloat(label.c_str(), &f, lowerBound, upperBound);
        }

        for (const auto& [label, buf, bufSize] : registeredInputTexts) {
            ImGui::InputText(label.c_str(), buf, bufSize);
        }

        for (const auto& [label, callback] : registeredButtons) {
            if (ImGui::Button(label.c_str()))
                callback();
        }

        ImGui::End();
        //--------End IMGUI Window----------//


        // Rendering
        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(debugWindow);

        glfwMakeContextCurrent(returnContext);
    }
    else {
        std::cout << "DebugWindow::draw() Function called but 'initialized' is false." << '\n';
    }
}

//---------------------------------------------------------
// addSliderFloat()
//---------------------------------------------------------
void DebugWindow::addSliderFloat(const char* label, float& f, float lowerBound, float upperBound)
{
    std::string registeredLabel = registerAndGetLabel(label);
    registeredSliderFloats.emplace_back(SliderFloat(registeredLabel, f, lowerBound, upperBound));
}

//---------------------------------------------------------
// addInputText()
//---------------------------------------------------------
void DebugWindow::addInputText(const char* label, char* buf, size_t bufSize)
{
    std::string registeredLabel = registerAndGetLabel(label);
    registeredInputTexts.emplace_back(InputText(registeredLabel, buf, bufSize));
}

//---------------------------------------------------------
// addButton()
//---------------------------------------------------------
void DebugWindow::addButton(const char* label, std::function<void(void)> callback)
{
    std::string registeredLabel = registerAndGetLabel(label);
    registeredButtons.emplace_back(Button(registeredLabel, callback));
}


//---------------------------------------------------------
// registerAndGetLabel()
//---------------------------------------------------------
std::string DebugWindow::registerAndGetLabel(const char* label)
{
    std::string strLabel(label);
    if (registeredLabels.find(strLabel) != registeredLabels.end()) {
        registeredLabels[strLabel]++;
        int num = registeredLabels[strLabel];
        strLabel.append(" (");
        strLabel.append(std::to_string(num));
        strLabel.append(")");
    }
    else {
        registeredLabels[strLabel] = 0;
    }

    return strLabel;
}