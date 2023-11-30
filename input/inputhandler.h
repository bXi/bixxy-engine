#pragma once

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include "SDL3/SDL.h"

#include "utils/vectors.h"
#include "utils/helpers.h"

static const int DEADZONE = 8000;

enum class InputType { GAMEPAD, MOUSE_KB };
enum class Buttons { LEFT, RIGHT, UP, DOWN, ACCEPT, BACK, SHOOT, SWITCH_NEXT, SWITCH_PREV, RUN };
enum class Action { HELD, PRESSED };

class InputDevice {
private:
    InputType type;
    int gamepadID;

    std::map<Buttons, std::list<int>> mappingKB = {
        { Buttons::LEFT,  { SDLK_a , SDLK_LEFT}},
        { Buttons::RIGHT, { SDLK_d , SDLK_RIGHT}},
        { Buttons::UP,    { SDLK_w , SDLK_UP}},
        { Buttons::DOWN,  { SDLK_s , SDLK_DOWN}},

        { Buttons::ACCEPT, { SDLK_SPACE, SDLK_KP_ENTER, SDLK_RETURN }},
        { Buttons::BACK,   { SDLK_ESCAPE, SDLK_BACKSPACE }},

        { Buttons::SWITCH_NEXT, { SDLK_TAB }},
        { Buttons::SWITCH_PREV, { SDLK_BACKQUOTE }},

        { Buttons::RUN, { SDL_SCANCODE_LSHIFT}},

        { Buttons::SHOOT, { SDLK_LSHIFT }}

    };

    std::map<Buttons, std::list<int>> mappingGP = {
        { Buttons::ACCEPT, { SDL_GAMEPAD_BUTTON_SOUTH }},
        { Buttons::BACK,   { SDL_GAMEPAD_BUTTON_EAST }},

        { Buttons::LEFT,  { SDL_GAMEPAD_BUTTON_DPAD_LEFT }},
        { Buttons::RIGHT, { SDL_GAMEPAD_BUTTON_DPAD_RIGHT }},
        { Buttons::UP,    { SDL_GAMEPAD_BUTTON_DPAD_UP }},
        { Buttons::DOWN,  { SDL_GAMEPAD_BUTTON_DPAD_DOWN }},

        { Buttons::SWITCH_NEXT, { SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER }},
        { Buttons::SWITCH_PREV, { SDL_GAMEPAD_BUTTON_LEFT_SHOULDER }},

        { Buttons::RUN, {SDL_GAMEPAD_AXIS_RIGHT_TRIGGER}},
    };


    bool isButtonPressed(Buttons button);
    bool isButtonHeld(Buttons button);

    std::map<Buttons, float> pressedTimings = {
        {Buttons::LEFT,  0.0f},
        {Buttons::RIGHT, 0.0f},
        {Buttons::UP,    0.0f},
        {Buttons::DOWN,  0.0f},
    };

    float joystickCooldown = 0.10f;


public:
    void updateTimings();

	bool is(Buttons button, Action action);

    [[nodiscard]] int getGamepadID() const;

    explicit InputDevice(InputType _type);

    InputDevice(InputType _type, int _gamepadID);
    InputType getType();
};

class Input {
public:

    static void Init() { get()._init(); }

    static InputDevice* GetController(int index) { return get()._getController(index); }
    static void clear() { get()._clear(); }
    static std::vector<InputDevice*> GetAllInputs() { return get().inputs; }

    static void Update()
    {
        UpdateTimings();


        for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
            get().previousKeyboardState.at(i) = get().currentKeyboardState[i];
        }
        get().currentKeyboardState = SDL_GetKeyboardState(nullptr);


        get().previousMouseButtons = get().currentMouseButtons;
        get().currentMouseButtons = SDL_GetMouseState(nullptr, nullptr);

        for (auto& gamepad : get().gamepads) {
            gamepad.previousButtonState = gamepad.currentButtonState;

            for (int i = 0; i < SDL_GAMEPAD_BUTTON_MAX; ++i) {
                gamepad.currentButtonState[i] = SDL_GetGamepadButton(gamepad.gamepad, static_cast<SDL_GamepadButton>(i));
            }
        }

    }

    static void UpdateTimings()
    {
	    for (const auto&  i : get().inputs)
	    {
            i->updateTimings();
	    }
    }

    static float GetGamepadAxisMovement(int gamepadID, SDL_GamepadAxis axis)
    {

        //TODO fix this
        Sint16 x = SDL_GetGamepadAxis(get().gamepads[gamepadID].gamepad, axis);

        if (std::abs(x) < DEADZONE) {
            // Value is within the deadzone, ignore it
            x = 0;
        }


        return  ((float) x) / 32768.0f;
    }

    static bool GamepadButtonPressed(int gamepadID, int key)
    {

        auto gamepadinfo = get().gamepads[gamepadID];
        return gamepadinfo.currentButtonState[key] && !gamepadinfo.previousButtonState[key];
    }

    static bool GamepadButtonDown(int gamepadID, int key)
    {
        return SDL_GetGamepadButton(get().gamepads[gamepadID].gamepad, static_cast<SDL_GamepadButton>(key));
    }

    static bool KeyPressed(int key)
    {

        auto curState = get().currentKeyboardState;
        auto prevState = get().previousKeyboardState;

        auto scancode = SDL_GetScancodeFromKey(key);

        return curState[scancode] == 1 && prevState[scancode] == 0;
    }

    static bool KeyReleased(int key)
    {

        auto curState = get().currentKeyboardState;
        auto prevState = get().previousKeyboardState;

        auto scancode = SDL_GetScancodeFromKey(key);

        return curState[scancode] == 0 && prevState[scancode] == 1;
    }

    static bool KeyDown(int key)
    {

        auto scancode = SDL_GetScancodeFromKey(key);

        return SDL_GetKeyboardState(nullptr)[scancode] == 1;
    }


    static vf2d GetMousePosition()
    {
        float xMouse, yMouse;

        SDL_GetMouseState(&xMouse,&yMouse);

        return {xMouse, yMouse};
    }


    static bool MouseButtonPressed(int key)
    {
        auto keymask = (1 << ((key) - 1));
        return (get().currentMouseButtons & keymask) != 0 && (get().previousMouseButtons & keymask) == 0;
    }

    static bool MouseButtonDown(int key)
    {
        auto keymask = (1 << ((key) - 1));
        return (get().currentMouseButtons & keymask) != 0;
    }


private:
    std::vector<InputDevice*> inputs;
    void _init();
    InputDevice* _getController(int index);
    void _clear();


    const Uint8 *currentKeyboardState;
    std::vector<Uint8> previousKeyboardState;

    Uint32 currentMouseButtons = 0;
    Uint32 previousMouseButtons = 0;


    SDL_JoystickID* joystickIds = nullptr;

    struct gamepadInfo {
        SDL_Gamepad* gamepad;
        std::vector<bool> currentButtonState;
        std::vector<bool> previousButtonState;
    };

    std::vector<gamepadInfo> gamepads;
public:
    Input(const Input&) = delete;
    static Input& get() { static Input instance; return instance; }

private:
    Input() {
        SDL_PumpEvents();
        currentKeyboardState = SDL_GetKeyboardState(nullptr);
        previousKeyboardState.resize(SDL_NUM_SCANCODES);
    }
};


//*/