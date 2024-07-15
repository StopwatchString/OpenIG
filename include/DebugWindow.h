#ifndef DEBUG_WINDOW_H
#define DEBUG_WINDOW_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"
#include <gl/GL.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

class DebugWindow
{
public:
    DebugWindow();
    ~DebugWindow();

    bool init();
    void draw();

    void addSliderFloat(const char* label, float& f, float lowerBound, float upperBound);
    void addInputText(const char* label, char* buf, size_t bufSize);
    void addButton(const char* label, std::function<void(void)> callback);

private:
    /*
        For each type of Imgui input we want to be able to register, we create a struct of all
        the fields that the input type needs. Then we can build a struct from the corresponding add()
        call and save it back in a vector. Then when draw() is called, we can iterate through
        all registered values.
    */

    struct SliderFloat {
        std::string label;
        float& f;
        float lowerBound;
        float upperBound;
    };
    std::vector<SliderFloat> registeredSliderFloats;

    struct InputText {
        std::string label;
        char* buf;
        size_t bufSize;
    };
    std::vector<InputText> registeredInputTexts;

    // TODO:: Can I enable more than <void(void)> through templating? Is there a reason to?
    struct Button {
        std::string label;
        std::function<void(void)> callback;
    };
    std::vector<Button> registeredButtons;

    // This is a registery of label names used because Imgui uses labels to decide
    // what part of the gui you're interacting with. If two components have the same label,
    // then it will register input on both when you interact with either. registerAndGetLabel()
    // automatically deconflicts names.
    std::unordered_map<std::string, int> registeredLabels;
    std::string registerAndGetLabel(const char* label);


    bool initialized         { false };
    GLFWwindow* debugWindow  { nullptr };

    // Imgui Members
    ImGuiIO& io;
};

#endif