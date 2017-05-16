#ifndef INPUT_HPP
#define INPUT_HPP
#define _USE_INPUT_
#ifdef _USE_INPUT_

#include <SFML/Window.hpp>
#include <vector>
#include <stdint.h>

// input state
#define RELEASED 0
#define PRESSED 1
#define RELEASED_NOW 2
#define PRESSED_NOW 3

// keys
#define KUP         0
#define KRIGHT      1
#define KDOWN       2
#define KLEFT       3

#define KSHOOT      4
#define KJUMP       5
#define KRUN        6
#define KSTART      7
#define KSELECT     8

#define KLEFTCLICK  9

#define KCOUNT      10

// pad binding
#define JOYSTICK_DEAD_ZONE 10
#define JOYSTICK_SETTING_OFFSET 20
#define JOYSTICK_SETTING_TYPE 100

struct PadBind
{
    PadBind();
    PadBind(int32_t value);
    bool isButton;
    int32_t id;
    sf::Joystick::Axis axis;
    bool axis_positive;
};

struct InputKey
{
    InputKey()
    {
        state = 4;
        k_def = false;
        b_def = false;
        m_def = false;
    }
    int8_t state;
    // keyboard bind
    sf::Keyboard::Key k_id;
    bool k_def;
    // pad bind
    PadBind b_id;
    bool b_def;
    // mouse bind
    sf::Mouse::Button m_id;
    bool m_def;
};

class Input
{
    public:
        Input();
        ~Input();

        void init();
        void update(const sf::Window *window = NULL); // window for mouse position

        void enableMouse(bool b);
        bool isMouseEnabled();
        sf::Vector2i getMousePosition();

        void reset();
        void reset(int8_t k);
        void resetState();
        void resetState(int8_t k);
        bool isPressed(int8_t k) const;
        bool isPressedNow(int8_t k) const;
        bool isReleased(int8_t k) const;
        bool isReleasedNow(int8_t k) const;

        void bindKey(int8_t k, sf::Keyboard::Key id);
        void unbingKey(int8_t k);
        void bindPadButton(int8_t k, PadBind id);
        void unbingPadButton(int8_t k);
        void bindMouse(int8_t k, sf::Mouse::Button id);
        void unbingMouse(int8_t k);

        bool keyIsBinded(int8_t k) const;
        bool mouseIsBinded(int8_t k) const;
        bool padButtonIsBinded(int8_t k) const;

        sf::Keyboard::Key getKeyID(int8_t k) const;
        sf::Mouse::Button getMouseID(int8_t k) const;
        int32_t getPadButtonID(int8_t k) const;
        PadBind getPadBind(int8_t k) const;

        std::string getKeyString(int8_t k) const;
        std::string getMouseString(int8_t k) const;
        std::string getPadButtonString(int8_t k) const;

        int32_t get_pad_id() const;
        bool pad_isConnected() const;
        bool pad_isConnected(const int32_t &id) const;
        void pad_connect(sf::Event &event);
        void pad_disconnect(sf::Event &event);
        bool pad_auto_connect();

        void saveState();
        void loadState();
    protected:
        bool mouse_enabled;
        sf::Vector2i mouse_pos;
        int32_t pad_id;
        bool isPadPressed(const PadBind &bbind);
        std::vector<InputKey> keys;
        std::vector<InputKey> save;
};

#endif
#endif // INPUT_HPP
