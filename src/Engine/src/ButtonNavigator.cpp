/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ButtonNavigator.hpp"

ButtonNavigator::ButtonNavigator(Gui& gui, Configuration& config)
    : gui(gui), config(config), on_cancel(0), on_cancel_data(0),
      joymotion_up(false), joymotion_down(false) { }

ButtonNavigator::~ButtonNavigator() { }

void ButtonNavigator::install_handlers(GuiWindow *window,
    OnCancel on_cancel, void *on_cancel_data)
{
    if (window) {
        window->set_on_keydown(static_window_keydown, this);
        window->set_on_joymotion(static_window_joymotion, this);
        window->set_on_joybuttondown(static_window_joybutton_down, this);
    }
    this->on_cancel = on_cancel;
    this->on_cancel_data = on_cancel_data;
}

GuiButton *ButtonNavigator::add_button(GuiButton *btn) {
    navigator_buttons.push_back(btn);
    if (current_index < 0) {
        current_index = 0;
    }

    return btn;
}

void ButtonNavigator::clear() {
    navigator_buttons.clear();
    current_index = -1;
}

void ButtonNavigator::set_button_focus() {
    for (Buttons::iterator it = navigator_buttons.begin();
        it != navigator_buttons.end(); it++)
    {
        (*it)->reset_color();
    }
    if (current_index >= 0 && current_index < static_cast<int>(navigator_buttons.size())) {
        navigator_buttons[current_index]->set_color(1.0f, 0.5f, 0.5f);
    }
}

const ButtonNavigator::Buttons& ButtonNavigator::get_buttons() const {
    return navigator_buttons;
}

bool ButtonNavigator::static_window_keydown(GuiWindow *sender, void *data, int keycode, bool repeat) {
    return (reinterpret_cast<ButtonNavigator *>(data))->window_keydown(keycode, repeat);
}

bool ButtonNavigator::window_keydown(int keycode, bool repeat) {
    binding.extract_from_config(config);
    for (int i = 0; i < KeyBinding::MaxBindings; i++) {
        if (binding.down[i].device == MappedKey::DeviceKeyboard && binding.down[i].param == keycode) {
            set_button_focus_down();
            return true;
        }
        if (binding.up[i].device == MappedKey::DeviceKeyboard && binding.up[i].param == keycode) {
            set_button_focus_up();
            return true;
        }
        if (binding.fire[i].device == MappedKey::DeviceKeyboard && binding.fire[i].param == keycode) {
            do_current_button_click();
            return true;
        }
        if (binding.escape[i].device == MappedKey::DeviceKeyboard && binding.escape[i].param == keycode) {
            if (on_cancel) {
                on_cancel(on_cancel_data);
            }
            return true;
        }
    }

    return false;
}

bool ButtonNavigator::static_window_joymotion(GuiWindow *sender, void *data, int motion) {
    return (reinterpret_cast<ButtonNavigator *>(data))->window_joymotion(motion);
}

bool ButtonNavigator::window_joymotion(int motion) {
    if (motion & InputData::InputJoyDirectionDown && !joymotion_down) {
        joymotion_down = true;
        set_button_focus_down();
    } else {
        joymotion_down = false;
    }
    if (motion & InputData::InputJoyDirectionUp && !joymotion_up) {
        joymotion_up = true;
        set_button_focus_up();
    } else {
        joymotion_up = false;
    }

    return false;
}

bool ButtonNavigator::static_window_joybutton_down(GuiWindow *sender, void *data, int button) {
    return (reinterpret_cast<ButtonNavigator *>(data))->window_joybutton_down(button);
}

bool ButtonNavigator::window_joybutton_down(int button) {
    binding.extract_from_config(config);
    for (int i = 0; i < KeyBinding::MaxBindings; i++) {
        if (binding.down[i].device == MappedKey::DeviceJoyButton && binding.down[i].param == button) {
            set_button_focus_down();
            return true;
        }
        if (binding.up[i].device == MappedKey::DeviceJoyButton && binding.up[i].param == button) {
            set_button_focus_up();
            return true;
        }
        if (binding.fire[i].device == MappedKey::DeviceJoyButton && binding.fire[i].param == button) {
            do_current_button_click();
            return true;
        }
        if (binding.escape[i].device == MappedKey::DeviceJoyButton && binding.escape[i].param == button) {
            if (on_cancel) {
                on_cancel(on_cancel_data);
            }
            return true;
        }
    }

    return false;
}

void ButtonNavigator::set_button_focus_up() {
    current_index--;
    if (current_index < 0) {
        current_index = static_cast<int>(navigator_buttons.size()) - 1;
    }
    set_button_focus();
}

void ButtonNavigator::set_button_focus_down() {
    current_index++;
    if (current_index > static_cast<int>(navigator_buttons.size()) - 1) {
        current_index = 0;
    }
    set_button_focus();
}

void ButtonNavigator::do_current_button_click() {
    if (current_index >= 0 && current_index < static_cast<int>(navigator_buttons.size())) {
        GuiButton *btn = navigator_buttons[current_index];
        GuiVirtualButton::OnClick on_click = btn->get_on_click_func();
        if (on_click) {
            on_click(btn, btn->get_on_click_data());
        }
    }
}
