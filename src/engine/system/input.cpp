#include "input.hpp"
#ifdef _USE_INPUT_
#include <SFML/Graphics.hpp>

#include "../mlib/mlib.hpp"

PadBind::PadBind():PadBind(999)
{

}

PadBind::PadBind(int32_t value)
{
    if(value >= JOYSTICK_SETTING_TYPE)
    {
        isButton = true;
        id = value - JOYSTICK_SETTING_TYPE;
    }
    else
    {
        isButton = false;
        if(value < 0)
        {
            switch(- (value + JOYSTICK_SETTING_OFFSET))
            {
                case 0: axis = sf::Joystick::X; break;
                case 1: axis = sf::Joystick::Y; break;
                case 2: axis = sf::Joystick::Z; break;
                case 3: axis = sf::Joystick::R; break;
                case 4: axis = sf::Joystick::U; break;
                case 5: axis = sf::Joystick::V; break;
                case 6: axis = sf::Joystick::PovX; break;
                case 7: axis = sf::Joystick::PovY; break;
            }
            axis_positive = false;
        }
        else
        {
            switch(value - JOYSTICK_SETTING_OFFSET)
            {
                case 0: axis = sf::Joystick::X; break;
                case 1: axis = sf::Joystick::Y; break;
                case 2: axis = sf::Joystick::Z; break;
                case 3: axis = sf::Joystick::R; break;
                case 4: axis = sf::Joystick::U; break;
                case 5: axis = sf::Joystick::V; break;
                case 6: axis = sf::Joystick::PovX; break;
                case 7: axis = sf::Joystick::PovY; break;
            }
            axis_positive = true;
        }
    }
}

Input::Input()
{
    mouse_enabled = false;
    mouse_pos = sf::Vector2i(-1, -1);
    pad_id = 8;
    while(keys.size() < KCOUNT) keys.push_back(InputKey());
    while(save.size() < KCOUNT) save.push_back(InputKey());
}

Input::~Input()
{
    //dtor
}

void Input::init()
{
    // keyboard
    bindKey(KUP, sf::Keyboard::Z);
    bindKey(KRIGHT, sf::Keyboard::D);
    bindKey(KDOWN, sf::Keyboard::S);
    bindKey(KLEFT, sf::Keyboard::Q);
    bindKey(KSHOOT, sf::Keyboard::Left);
    bindKey(KJUMP, sf::Keyboard::Down);
    bindKey(KRUN, sf::Keyboard::Right);
    bindKey(KSTART, sf::Keyboard::Return);
    bindKey(KSELECT, sf::Keyboard::Space);

    // mouse
    bindMouse(KLEFTCLICK, sf::Mouse::Left);

    // pad
    bindPadButton(KUP, PadBind(27));
    bindPadButton(KRIGHT, PadBind(26));
    bindPadButton(KDOWN, PadBind(-27));
    bindPadButton(KLEFT, PadBind(-26));
    bindPadButton(KSHOOT, PadBind(101)); // B
    bindPadButton(KJUMP, PadBind(102)); // X
    bindPadButton(KRUN, PadBind(100)); // Y
    bindPadButton(KSTART, PadBind(107)); // start
    bindPadButton(KSELECT, PadBind(106)); // back
}

bool Input::readEvent(sf::Event &event)
{
    switch(event.type)
    {
        case sf::Event::KeyPressed:
            for(InputKey &i : keys)
                if(i.k_id == event.key.code)
                {
                    if(i.state == RELEASED)
                    {
                        i.state = PRESSED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
        case sf::Event::KeyReleased:
            for(InputKey &i : keys)
                if(i.k_id == event.key.code)
                {
                    if(i.state == PRESSED)
                    {
                        i.state = RELEASED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
         case sf::Event::MouseButtonPressed:
            for(InputKey &i : keys)
                if(i.m_id == event.mouseButton.button)
                {
                    if(i.state == RELEASED)
                    {
                        i.state = PRESSED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
        case sf::Event::MouseButtonReleased:
            for(InputKey &i : keys)
                if(i.m_id == event.mouseButton.button)
                {
                    if(i.state == PRESSED)
                    {
                        i.state = RELEASED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
        case sf::Event::JoystickButtonPressed:
            if(event.joystickButton.joystickId != pad_id)
                return false;
            for(InputKey &i : keys)
                if(i.b_id.isButton && i.b_id.id == event.joystickButton.button)
                {
                    if(i.state == RELEASED)
                    {
                        i.state = PRESSED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
        case sf::Event::JoystickButtonReleased:
            if(event.joystickButton.joystickId != pad_id)
                return false;
            for(InputKey &i : keys)
                if(i.b_id.isButton && i.b_id.id == event.joystickButton.button)
                {
                    if(i.state == PRESSED)
                    {
                        i.state = RELEASED_NOW;
                        i.updated = true;
                    }
                    return true;
                }
            break;
        case sf::Event::JoystickMoved:
            if(event.joystickButton.joystickId != pad_id)
                return false;
            for(InputKey &i : keys)
                if(!i.b_id.isButton && i.b_id.axis == event.joystickMove.axis)
                {
                    if(i.b_id.axis_positive)
                    {
                        if(i.state == RELEASED && event.joystickMove.position > JOYSTICK_DEAD_ZONE)
                        {
                            i.state = PRESSED_NOW;
                            i.updated = true;
                            return true;
                        }
                        else if(i.state == PRESSED && event.joystickMove.position <= JOYSTICK_DEAD_ZONE)
                        {
                            i.state = RELEASED_NOW;
                            i.updated = true;
                            return true;
                        }
                    }
                    else
                    {
                        if(i.state == RELEASED && event.joystickMove.position < -JOYSTICK_DEAD_ZONE)
                        {
                            i.state = PRESSED_NOW;
                            i.updated = true;
                            return true;
                        }
                        else if(i.state == PRESSED && event.joystickMove.position >= -JOYSTICK_DEAD_ZONE)
                        {
                            i.state = RELEASED_NOW;
                            i.updated = true;
                            return true;
                        }
                    }
                }
            break;
        case sf::Event::JoystickConnected:
            pad_connect(event);
            return true;
        case sf::Event::JoystickDisconnected:
            pad_disconnect(event);
            return true;
        default:
            break;
    }
    return false;
}

void Input::update(const sf::Window *window, bool focus)
{
    if(mouse_enabled && window && focus)
    {
        mouse_pos = sf::Mouse::getPosition(*window);
        sf::Vector2u wsize = window->getSize();
        if(mouse_pos.x < 0 || mouse_pos.y < 0 || mouse_pos.x >= (int)wsize.x || mouse_pos.y >= (int)wsize.y)
        {
            mouse_pos.x = -1;
            mouse_pos.y = -1;
        }
    }
    else
    {
        mouse_pos.x = -1;
        mouse_pos.y = -1;
    }

    for(InputKey &i : keys)
    {
        if(i.updated)
        {
            i.updated = false;
        }
        else
        {
            switch(i.state)
            {
                case RELEASED_NOW:
                    i.state = RELEASED;
                    break;
                case PRESSED_NOW:
                    i.state = PRESSED;
                    break;
                case RELEASED:
                case PRESSED:
                    break;
                default:
                    i.state = RELEASED; // failsafe, in case of error
            }
        }
    }
}

void Input::enableMouse(bool b)
{
    mouse_enabled = true;
}

bool Input::isMouseEnabled()
{
    return mouse_enabled;
}

sf::Vector2i Input::getMousePosition()
{
    return mouse_pos;
}

void Input::reset()
{
    for(int8_t i = 0; i < KCOUNT; ++i) keys[i].state = RELEASED;
}

void Input::reset(int8_t k)
{
    if(k >= 0 || k < KCOUNT) keys[k].state = RELEASED;
}

void Input::resetState()
{
    for(int8_t i = 0; i < KCOUNT; ++i) save[i].state = RELEASED;
}

void Input::resetState(int8_t k)
{
    if(k >= 0 || k < KCOUNT) save[k].state = RELEASED;
}

bool Input::isPressed(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return (keys[k].state == PRESSED || keys[k].state == PRESSED_NOW);
}

bool Input::isPressedNow(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return (keys[k].state == PRESSED_NOW);
}

bool Input::isReleased(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return (keys[k].state == RELEASED || keys[k].state == RELEASED_NOW);
}

bool Input::isReleasedNow(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return (keys[k].state == RELEASED_NOW);
}

void Input::bindKey(int8_t k, sf::Keyboard::Key id)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].k_id = id;
    keys[k].k_def = true;
}

void Input::unbingKey(int8_t k)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].k_def = false;
}

void Input::bindPadButton(int8_t k, PadBind id)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].b_id = id;
    keys[k].b_def = true;
}

void Input::unbingPadButton(int8_t k)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].b_def = false;
}

void Input::bindMouse(int8_t k, sf::Mouse::Button id)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].m_id = id;
    keys[k].m_def = true;
}

void Input::unbingMouse(int8_t k)
{
    if(k < 0 || k >= KCOUNT) return;
    keys[k].m_def = false;
}

bool Input::keyIsBinded(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return keys[k].k_def;
}

bool Input::mouseIsBinded(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return keys[k].m_def;
}

bool Input::padButtonIsBinded(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return false;
    return keys[k].b_def;
}

sf::Keyboard::Key Input::getKeyID(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return sf::Keyboard::KeyCount;
    return keys[k].k_id;
}

sf::Mouse::Button Input::getMouseID(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return sf::Mouse::ButtonCount;
    return keys[k].m_id;
}

int32_t Input::getPadButtonID(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return 0;
    if(keys[k].b_id.isButton) return JOYSTICK_SETTING_TYPE + keys[k].b_id.id;
    else return (JOYSTICK_SETTING_OFFSET + keys[k].b_id.axis) * ((keys[k].b_id.axis_positive*2)-1);
}

PadBind Input::getPadBind(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return PadBind();
    return keys[k].b_id;
}

std::string Input::getKeyString(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return "";
    if(keys[k].k_def)
    {
        switch(keys[k].k_id)
        {
            case sf::Keyboard::Unknown: return "Unknown";
            case sf::Keyboard::A: return "A";
            case sf::Keyboard::B: return "B";
            case sf::Keyboard::C: return "C";
            case sf::Keyboard::D: return "D";
            case sf::Keyboard::E: return "E";
            case sf::Keyboard::F: return "F";
            case sf::Keyboard::G: return "G";
            case sf::Keyboard::H: return "H";
            case sf::Keyboard::I: return "I";
            case sf::Keyboard::J: return "J";
            case sf::Keyboard::K: return "K";
            case sf::Keyboard::L: return "L";
            case sf::Keyboard::M: return "M";
            case sf::Keyboard::N: return "N";
            case sf::Keyboard::O: return "O";
            case sf::Keyboard::P: return "P";
            case sf::Keyboard::Q: return "Q";
            case sf::Keyboard::R: return "R";
            case sf::Keyboard::S: return "S";
            case sf::Keyboard::T: return "T";
            case sf::Keyboard::U: return "U";
            case sf::Keyboard::V: return "V";
            case sf::Keyboard::W: return "W";
            case sf::Keyboard::X: return "X";
            case sf::Keyboard::Y: return "Y";
            case sf::Keyboard::Z: return "Z";
            case sf::Keyboard::Num0: return "0";
            case sf::Keyboard::Num1: return "1";
            case sf::Keyboard::Num2: return "2";
            case sf::Keyboard::Num3: return "3";
            case sf::Keyboard::Num4: return "4";
            case sf::Keyboard::Num5: return "5";
            case sf::Keyboard::Num6: return "6";
            case sf::Keyboard::Num7: return "7";
            case sf::Keyboard::Num8: return "8";
            case sf::Keyboard::Num9: return "9";
            case sf::Keyboard::Escape: return "Escape";
            case sf::Keyboard::LControl: return "LCtrl";
            case sf::Keyboard::LShift: return "LShift";
            case sf::Keyboard::LAlt: return "LAlt";
            case sf::Keyboard::LSystem: return "LSys";
            case sf::Keyboard::RControl: return "RCtrl";
            case sf::Keyboard::RShift: return "RShift";
            case sf::Keyboard::RAlt: return "RAlt";
            case sf::Keyboard::RSystem: return "RSys";
            case sf::Keyboard::Menu: return "Menu";
            case sf::Keyboard::LBracket: return "[";
            case sf::Keyboard::RBracket: return "]";
            case sf::Keyboard::SemiColon: return ";";
            case sf::Keyboard::Comma: return ",";
            case sf::Keyboard::Period: return ".";
            case sf::Keyboard::Quote: return "\"";
            case sf::Keyboard::Slash: return "/";
            case sf::Keyboard::BackSlash: return "\\";
            case sf::Keyboard::Tilde: return "~";
            case sf::Keyboard::Equal: return "=";
            case sf::Keyboard::Dash: return "—";
            case sf::Keyboard::Space: return "Space";
            case sf::Keyboard::Return: return "Return";
            case sf::Keyboard::BackSpace: return "BackSpc";
            case sf::Keyboard::Tab: return "Tab";
            case sf::Keyboard::PageUp: return "PageUp";
            case sf::Keyboard::PageDown: return "PageDown";
            case sf::Keyboard::End: return "End";
            case sf::Keyboard::Home: return "Home";
            case sf::Keyboard::Insert: return "Ins";
            case sf::Keyboard::Delete: return "Del";
            case sf::Keyboard::Add: return "Num+";
            case sf::Keyboard::Subtract: return "Num-";
            case sf::Keyboard::Multiply: return "Num*";
            case sf::Keyboard::Divide: return "Num/";
            case sf::Keyboard::Left: return "Left";
            case sf::Keyboard::Right: return "Right";
            case sf::Keyboard::Up: return "Up";
            case sf::Keyboard::Down: return "Down";
            case sf::Keyboard::Numpad0: return "Num0";
            case sf::Keyboard::Numpad1: return "Num1";
            case sf::Keyboard::Numpad2: return "Num2";
            case sf::Keyboard::Numpad3: return "Num3";
            case sf::Keyboard::Numpad4: return "Num4";
            case sf::Keyboard::Numpad5: return "Num5";
            case sf::Keyboard::Numpad6: return "Num6";
            case sf::Keyboard::Numpad7: return "Num7";
            case sf::Keyboard::Numpad8: return "Num8";
            case sf::Keyboard::Numpad9: return "Num9";
            case sf::Keyboard::F1: return "F1";
            case sf::Keyboard::F2: return "F2";
            case sf::Keyboard::F3: return "F3";
            case sf::Keyboard::F4: return "F4";
            case sf::Keyboard::F5: return "F5";
            case sf::Keyboard::F6: return "F6";
            case sf::Keyboard::F7: return "F7";
            case sf::Keyboard::F8: return "F8";
            case sf::Keyboard::F9: return "F9";
            case sf::Keyboard::F10: return "F10";
            case sf::Keyboard::F11: return "F11";
            case sf::Keyboard::F12: return "F12";
            case sf::Keyboard::F13: return "F13";
            case sf::Keyboard::F14: return "F14";
            case sf::Keyboard::F15: return "F15";
            case sf::Keyboard::Pause: return "Pause";
            default: return "<none>";
        }

    }
    else return "<none>";
}

std::string Input::getMouseString(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return "";
    if(keys[k].m_def)
    {
        switch(keys[k].m_id)
        {
            case sf::Mouse::Left: return "Mouse1";
            case sf::Mouse::Right: return "Mouse2";
            case sf::Mouse::Middle: return "Mouse3";
            case sf::Mouse::XButton1: return "Mouse4";
            case sf::Mouse::XButton2: return "Mouse5";
            default: return "<none>";
        }
    }
    else return "<none>";
}

std::string Input::getPadButtonString(int8_t k) const
{
    if(k < 0 || k >= KCOUNT) return "";
    if(keys[k].b_def)
    {
        if(keys[k].b_id.isButton) return "Button" + std::to_string(keys[k].b_id.id);
        else
        {
            if(keys[k].b_id.axis_positive == false)
            {
                switch(keys[k].b_id.axis)
                {
                    case sf::Joystick::X: return "AxisX-";
                    case sf::Joystick::Y: return "AxisY-";
                    case sf::Joystick::Z: return "AxisZ-";
                    case sf::Joystick::R: return "AxisR-";
                    case sf::Joystick::U: return "AxisU-";
                    case sf::Joystick::V: return "AxisV-";
                    case sf::Joystick::PovX: return "AxisPovX-";
                    case sf::Joystick::PovY: return "AxisPovY-";
                }
            }
            else
            {
                switch(keys[k].b_id.axis)
                {
                    case sf::Joystick::X: return "AxisX+";
                    case sf::Joystick::Y: return "AxisY+";
                    case sf::Joystick::Z: return "AxisZ+";
                    case sf::Joystick::R: return "AxisR+";
                    case sf::Joystick::U: return "AxisU+";
                    case sf::Joystick::V: return "AxisV+";
                    case sf::Joystick::PovX: return "AxisPovX+";
                    case sf::Joystick::PovY: return "AxisPovY+";
                }
            }
        }
    }
    return "<none>";
}

int32_t Input::get_pad_id() const
{
    return pad_id;
}

bool Input::pad_isConnected() const
{
    if(pad_id == 8) return false;
    return sf::Joystick::isConnected(pad_id);
}

bool Input::pad_isConnected(const int32_t &id) const
{
    return sf::Joystick::isConnected(id);
}

void Input::pad_connect(sf::Event &event)
{
    if(event.type != sf::Event::JoystickConnected || pad_id != 8) return;
    pad_id = event.joystickConnect.joystickId;
    Out = "Input: Current gamepad set to #" + std::to_string(pad_id) + "\n";
}

void Input::pad_disconnect(sf::Event &event)
{
    if(event.type == sf::Event::JoystickDisconnected && pad_id == event.joystickConnect.joystickId) pad_auto_connect();
}

bool Input::pad_auto_connect()
{
    for(int32_t i = 0; i < 8; ++i)
        if(pad_isConnected(i))
        {
            pad_id = i;
            Out = "Input: Current gamepad set to #" + std::to_string(pad_id) + "\n";
            return true;
        }
    pad_id = 8;
    Out = "Input: No gamepad detected\n";
    return false;
}

bool Input::isPadPressed(const PadBind &bbind)
{
    if(!pad_isConnected()) return false;
    if(bbind.isButton) return sf::Joystick::isButtonPressed(pad_id, bbind.id);
    if((bbind.axis_positive && sf::Joystick::getAxisPosition(pad_id, bbind.axis) > JOYSTICK_DEAD_ZONE)
       || (!bbind.axis_positive && sf::Joystick::getAxisPosition(pad_id, bbind.axis) < - JOYSTICK_DEAD_ZONE)) return true;
    return false;
}

void Input::saveState()
{
    save = keys;
}

void Input::loadState()
{
    keys = save;
}

#endif
