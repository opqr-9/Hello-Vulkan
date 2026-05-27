#pragma once
#include "GLFWGeneral.hpp"
#include "Camera.hpp"

enum class InputAction {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    LOOK_UP,
    LOOK_DOWN,
    LOOK_LEFT,
    LOOK_RIGHT,
    ZOOM_IN,
    ZOOM_OUT,
    TOGGLE_UI_MODE,
};

struct InputState {
    glm::vec2 cursorPosition = { 0.0f, 0.0f };
    glm::vec2 cursorDelta = { 0.0f, 0.0f };
    bool mouseButtons[3] = { false, false, false };
    float scrollDelta = 0.0f;

    struct TouchPoint {
        int id;
        glm::vec2 position;
        glm::vec2 delta;
    };
    std::vector<TouchPoint> touchPoints;

    void resetDeltas() {
        cursorDelta = { 0.0f, 0.0f };
        scrollDelta = 0.0f;
        for (auto& touch : touchPoints) {
            touch.delta = { 0.0f, 0.0f };
        }
    }
};

class InputSystem {
public:
    static void Initialize();
    static void InitAction();
    static void Shutdown();

    static void Update(float deltaTime);

    static void RegisterActionCallback(InputAction action, std::function<void(float)> callback);

    static bool ProcessInputEvent(void* event);

    static InputState& GetInputState();

    static bool IsImGuiCapturingKeyboard();
    static bool IsImGuiCapturingMouse();

private:
    static InputState inputState;
    static std::unordered_map<InputAction, std::function<void(float)>> actionCallbacks;
};

inline InputState InputSystem::inputState;
inline std::unordered_map<InputAction, std::function<void(float)>> InputSystem::actionCallbacks;

static bool mouseCaptureMode = false;

// GLFW callback functions
static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < 3) {
        InputState& state = InputSystem::GetInputState();
        state.mouseButtons[button] = action;
        io.AddMouseButtonEvent(button, action);
    }
}

static void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    InputState& state = InputSystem::GetInputState();

    // Calculate delta from last position
    glm::vec2 newPos(static_cast<float>(xpos), static_cast<float>(ypos));
    state.cursorDelta = newPos - state.cursorPosition;
    state.cursorPosition = newPos;

    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(xpos, ypos);
}

static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    InputState& state = InputSystem::GetInputState();
    state.scrollDelta = static_cast<float>(yoffset);
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    ImGuiIO& io = ImGui::GetIO();
    // Map GLFW keys to our input actions
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        bool pressed = (action == GLFW_PRESS);

        // Toggle mouse capture mode with Escape key
        if (key == GLFW_KEY_ESCAPE && pressed) {
            mouseCaptureMode = !mouseCaptureMode;

            if (mouseCaptureMode) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        if (key ==GLFW_KEY_BACKSPACE && pressed) {
            io.AddKeyEvent(ImGuiKey_Backspace, action);
        }
        // Map other keys to actions
        // ...
    }
}

void InputSystem::Initialize() {

    // Set up GLFW callbacks
    glfwSetMouseButtonCallback(pWindow, glfwMouseButtonCallback);
    glfwSetCursorPosCallback(pWindow, glfwCursorPosCallback);
    glfwSetScrollCallback(pWindow, glfwScrollCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    // Initially capture the cursor for camera control
    mouseCaptureMode = true;
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    InputSystem::InitAction();
}

void InputSystem::InitAction()
{
    actionCallbacks.emplace(InputAction::MOVE_FORWARD, [](float deltaTime) {mainCamera->MoveForward(deltaTime); });
    actionCallbacks.emplace(InputAction::MOVE_BACKWARD, [](float deltaTime) {mainCamera->MoveBackward(deltaTime); });
    actionCallbacks.emplace(InputAction::MOVE_LEFT, [](float deltaTime) {mainCamera->MoveLeft(deltaTime); });
    actionCallbacks.emplace(InputAction::MOVE_RIGHT, [](float deltaTime) {mainCamera->MoveRight(deltaTime); });
    actionCallbacks.emplace(InputAction::MOVE_UP, [](float deltaTime) {mainCamera->MoveUp(deltaTime); });
    actionCallbacks.emplace(InputAction::MOVE_DOWN, [](float deltaTime) {mainCamera->MoveDown(deltaTime); });

    actionCallbacks.emplace(InputAction::LOOK_UP, [](float detla) {mainCamera->LookUp(detla); });
    actionCallbacks.emplace(InputAction::LOOK_DOWN, [](float detla) {mainCamera->LookDown(detla); });
    actionCallbacks.emplace(InputAction::LOOK_LEFT, [](float detla) {mainCamera->LookLeft(detla); });
    actionCallbacks.emplace(InputAction::LOOK_RIGHT, [](float detla) {mainCamera->LookRight(detla); });
}

void InputSystem::Update(float deltaTime) {
    // Poll for input events
    glfwPollEvents();

    // Update key states for continuous actions (like movement)
    if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_FORWARD); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_BACKWARD); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_LEFT); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_RIGHT); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_UP); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }

    if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (auto it = actionCallbacks.find(InputAction::MOVE_DOWN); it != actionCallbacks.end()) {
            it->second(deltaTime);
        }
    }
    if (mouseCaptureMode)
    {
        if (inputState.cursorDelta.x < 0)
        {
            if (auto it = actionCallbacks.find(InputAction::LOOK_LEFT); it != actionCallbacks.end()) {
                it->second(-inputState.cursorDelta.x);
            }
        }
        if (inputState.cursorDelta.x > 0)
        {
            if (auto it = actionCallbacks.find(InputAction::LOOK_RIGHT); it != actionCallbacks.end()) {
                it->second(inputState.cursorDelta.x);
            }
        }
        if (inputState.cursorDelta.y < 0)
        {
            if (auto it = actionCallbacks.find(InputAction::LOOK_UP); it != actionCallbacks.end()) {
                it->second(-inputState.cursorDelta.y);
            }
        }
        if (inputState.cursorDelta.y > 0)
        {
            if (auto it = actionCallbacks.find(InputAction::LOOK_DOWN); it != actionCallbacks.end()) {
                it->second(inputState.cursorDelta.y);
            }
        }
    }
    

    //std::cout << mainCamera->dir.x << " " << mainCamera->dir.y << " " << mainCamera->dir.z << std::endl;

    // ... other keys ...

    // Reset delta values after processing
    inputState.resetDeltas();
}

InputState& InputSystem::GetInputState() {
    return inputState;
}

bool InputSystem::IsImGuiCapturingKeyboard() {
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool InputSystem::IsImGuiCapturingMouse() {
    return ImGui::GetIO().WantCaptureMouse;
}
