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

#include "Gui.hpp"

namespace {

    const float WindowAlphaActive = 0.8f;
    const float WindowAlphaInactive = 0.55f;
    const ms_t CaretBlinkInterval = 500;
    const ms_t TickInterval = 500;

}

Gui::Gui(Resources& resources, Subsystem& subsystem, Font *font)
    : resources(resources), subsystem(subsystem), i18n(subsystem.get_i18n()),
      font(font), current_window(0), active_object(0), blink_on(true), tick_on(true),
      running(false), mouse_is_down(false), mouse_is_visible(true), local_mousex(0),
      local_mousey(0), pmousex(&local_mousex), pmousey(&local_mousey), tooltip(0),
      tooltip_object(0), tooltip_x(0), tooltip_y(0)
{
    if (!font) {
        throw GuiException(i18n(I18N_NO_FONT));
    }

    *pmousex = subsystem.get_view_width() / 2;
    *pmousey = subsystem.get_view_height() / 2;
    load_resources();

    get_now(last);
    get_now(last_tick);
    now = last;
}

Gui::~Gui() {
    for (WindowStack::iterator it = windows.begin(); it != windows.end(); it++) {
        delete *it;
    }

    destroy_tooltip();
}

void Gui::run() {
    running = true;
    idleloop(-1);
    on_leave();
}

void Gui::leave() {
    running = false;
}

void Gui::load_resources() {
    mouse = resources.get_icon("mouse");
    mouse_default = resources.get_icon("mouse");
    mouse_move = resources.get_icon("mouse_move");
    set_mousepointer(MousepointerDefault);

    msg_question = resources.get_icon("msg_question");
    msg_information = resources.get_icon("msg_information");
    msg_exclamation = resources.get_icon("msg_exclamation");
    msg_error = resources.get_icon("msg_error");
}

void Gui::link_mouse(const Gui& gui) {
    link_mouse(gui.get_pmousex(), gui.get_pmousey());
}

void Gui::link_mouse(int *pmousex, int *pmousey) {
    this->pmousex = pmousex;
    this->pmousey = pmousey;
}

void Gui::unlink_mouse() {
    pmousex = &local_mousex;
    pmousey = &local_mousey;
}

int *Gui::get_pmousex() const {
    return pmousex;
}

int *Gui::get_pmousey() const {
    return pmousey;
}

int Gui::get_mouse_x() const {
    return *pmousex;
}

int Gui::get_mouse_y() const {
    return *pmousey;
}

void Gui::set_mouse_x(int v) {
    *pmousex = v;
    subsystem.set_mouse_position(v, *pmousey);
}

void Gui::set_mouse_y(int v) {
    *pmousey = v;
    subsystem.set_mouse_position(*pmousex, v);
}

void Gui::set_mouse_xy(int x, int y) {
    *pmousex = x;
    *pmousey = y;
    subsystem.set_mouse_position(x, y);
}

size_t Gui::get_stack_count() const {
    return windows.size();
}

Resources& Gui::get_resources() const {
    return resources;
}

Subsystem& Gui::get_subsystem() const {
    return subsystem;
}

Font *Gui::get_font() const {
    return font;
}

GuiWindow *Gui::push_window(int x, int y, int width, int height, const std::string& title) {
    GuiWindow *win = new GuiWindow(*this, 0, x, y, width, height, title);
    windows.push_back(win);
    set_current_window();
    destroy_tooltip();

    return win;
}

void Gui::pop_window() {
    size_t sz = windows.size();
    if (sz) {
        delete windows[sz - 1];
        windows.pop_back();
        set_current_window();
        active_object = 0;
        destroy_tooltip();
    }
}

GuiBox *Gui::create_box(GuiObject *parent, int x, int y, int width, int height) {
    check_parent(parent);
    return new GuiBox(*this, parent, x, y, width, height);
}

GuiLabel *Gui::create_label(GuiObject *parent, int x, int y, const std::string& caption) {
    check_parent(parent);
    return new GuiLabel(*this, parent, x, y, 0, 0, caption);
}

GuiButton *Gui::create_button(GuiObject *parent, int x, int y, int width,
    int height, const std::string& caption, GuiVirtualButton::OnClick on_click,
    void *on_click_data)
{
    check_parent(parent);
    return new GuiButton(*this, parent, x, y, width, height, caption, on_click, on_click_data);
}

GuiRoundedButton *Gui::create_rounded_button(GuiObject *parent, int x, int y, int width,
    int height, const std::string& caption, GuiVirtualButton::OnClick on_click,
    void *on_click_data)
{
    check_parent(parent);
    return new GuiRoundedButton(*this, parent, x, y, width, height, caption,
        on_click, on_click_data);
}

GuiCheckbox *Gui::create_checkbox(GuiObject *parent, int x, int y,
    const std::string& caption, bool state, GuiCheckbox::OnClick on_click,
    void *on_click_data)
{
    check_parent(parent);
    return new GuiCheckbox(*this, parent, x, y, caption, state, on_click, on_click_data);
}

GuiTextbox *Gui::create_textbox(GuiObject *parent, int x, int y, int width,
    const std::string& text)
{
    check_parent(parent);
    return new GuiTextbox(*this, parent, x, y, width, text);
}

GuiPicture *Gui::create_picture(GuiObject *parent, int x, int y, TileGraphic *graphic) {
    check_parent(parent);
    return new GuiPicture(*this, parent, x, y, graphic);
}

GuiFrame *Gui::create_frame(GuiObject *parent, int x, int y, int width, int height) {
    check_parent(parent);
    return new GuiFrame(*this, parent, x, y, width, height);
}

GuiTab *Gui::create_tab(GuiObject *parent, int x, int y, int width, int height) {
    check_parent(parent);
    return new GuiTab(*this, parent, x, y, width, height);
}

GuiHScroll *Gui::create_hscroll(GuiObject *parent, int x, int y, int width,
    int min_value, int max_value, int initial_value,
    GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data)
{
    check_parent(parent);
    return new GuiHScroll(*this, parent, x, y, width, min_value, max_value,
    initial_value, on_value_changed, on_value_changed_data);
}

GuiVScroll *Gui::create_vscroll(GuiObject *parent, int x, int y, int height,
    int min_value, int max_value, int initial_value,
    GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data)
{
    check_parent(parent);
    return new GuiVScroll(*this, parent, x, y, height, min_value, max_value,
        initial_value, on_value_changed, on_value_changed_data);
}

GuiListbox *Gui::create_listbox(GuiObject *parent, int x, int y, int width,
    int height, const std::string& title, GuiListbox::OnItemSelected on_item_selected,
    void *on_item_selected_data)
{
    check_parent(parent);
    return new GuiListbox(*this, parent, x, y, width, height, title,
        on_item_selected, on_item_selected_data);
}

GuiListbox *Gui::create_listbox(GuiObject *parent, int x, int y, int width,
    int height, Icon *icon, int icon_width, const std::string& title,
    GuiListbox::OnItemSelected on_item_selected, void *on_item_selected_data)
{
    check_parent(parent);
    return new GuiListbox(*this, parent, x, y, width, height, icon, icon_width, title,
        on_item_selected, on_item_selected_data);
}

GuiWindow *Gui::get_current_window() const {
    return current_window;
}

bool Gui::is_active(GuiObject *object) const {
    if (object) {
        object = get_window_of_object(object);
        GuiObject *co = static_cast<GuiObject *>(current_window);
        return ((object->get_parent() && object->get_parent() == co) || object == co);
    }

    return true;
}

float Gui::get_alpha(GuiObject *object) const {
    if (object && !object->get_follow_alpha()) {
        return 1.0f;
    }

    return (is_active(object) ? WindowAlphaActive : WindowAlphaInactive);
}

void Gui::set_focus(GuiObject *object) {
    if (object && object->can_have_focus()) {
        GuiWindow *win = get_window_of_object(object);
        if (win) {
            reset_blinker();
            win->set_focused_object(object);
        }
    }
}

bool Gui::has_focus(GuiObject *object) const {
    if (object) {
        GuiWindow *win = get_window_of_object(object);
        return (is_active(object) && win->get_focused_object() == object);
    }

    return false;
}

bool Gui::get_blink_on() const {
    return blink_on;
}

bool Gui::get_tick_on() const {
    return tick_on;
}

void Gui::set_mousepointer(Mousepointer mouse) {
    switch (mouse) {
        case MousepointerDefault:
            this->mouse = mouse_default;
            break;

        case MousepointerMove:
            this->mouse = mouse_move;
            break;
    }
}

Gui::MessageBoxResponse Gui::show_messagebox(MessageBoxIcon icon, const std::string& title, const std::string& text) {
    Icon *msg_icon = 0;

    switch (icon) {
        case MessageBoxIconNone:
            break;

        case MessageBoxIconQuestion:
            msg_icon = msg_question;
            break;

        case MessageBoxIconInformation:
            msg_icon = msg_information;
            break;

        case MessageBoxIconExclamation:
            msg_icon = msg_exclamation;
            break;

        case MessageBoxIconError:
            msg_icon = msg_error;
            break;
    };

    int icw = 0;
    int ics = 0;
    if (msg_icon) {
        TileGraphic *tg = msg_icon->get_tile()->get_tilegraphic();
        icw = tg->get_width();
        ics = 5;
    }

    int tiw = font->get_text_width(title) + 30;
    int tew = icw + ics + font->get_text_width(text) + 30;
    int max_tw = (tew > tiw ? tew : tiw);
    std::string caption_ok(i18n(I18N_BUTTON_OK));
    int bw = get_font()->get_text_width(caption_ok) + 24;
    int bh = 18;
    int width = (max_tw > bw ? max_tw : bw );
    int height = 80;
    int x = subsystem.get_view_width() / 2 - width / 2;
    int y = subsystem.get_view_height() / 2 - height / 2;

    GuiWindow *window = push_window(x, y, width, height, title);
    if (msg_icon) {
        create_picture(window, 15, 13, msg_icon->get_tile()->get_tilegraphic());
    }
    create_label(window, 15 + icw + ics, 15, text);
    create_button(window, width / 2- bw / 2, height - bh - 26, bw, bh, caption_ok, static_ok_click, this);

    idleloop(static_cast<int>(windows.size()) - 1);

    return last_response;
}

Gui::MessageBoxResponse Gui::show_questionbox(const std::string& title, const std::string& text) {
    int icw = 0;
    int ics = 0;
    TileGraphic *tg = msg_question->get_tile()->get_tilegraphic();
    icw = tg->get_width();
    ics = 5;

    int tiw = font->get_text_width(title) + 2 * Spc;
    int tew = icw + ics + font->get_text_width(text) + 2 * Spc;
    int max_tw = (tew > tiw ? tew : tiw);
    int bw = 55;
    int bh = 18;
    int width = (max_tw > bw ? max_tw : bw );
    int height = 80;
    int x = subsystem.get_view_width() / 2 - width / 2;
    int y = subsystem.get_view_height() / 2 - height / 2;

    GuiWindow *window = push_window(x, y, width, height, title);
    create_picture(window, Spc, Spc - 2, tg);
    create_label(window, Spc + icw + ics, Spc, text);

    /* place buttons */
    bh = 18;
    std::string caption_no(i18n(I18N_BUTTON_NO));
    width = window->get_client_width();
    height = window->get_client_height();
    bw = get_font()->get_text_width(caption_no) + 24;

    std::string caption_yes(i18n(I18N_BUTTON_YES));
    int bw2 = get_font()->get_text_width(caption_yes) + 24;
    if (bw2 > bw) {
        bw = bw2;
    }

    create_button(window, width - bw - Spc, height - bh - Spc, bw, bh, caption_no, static_no_click, this);
    create_button(window, width - 2 * bw - Spc - 5, height - bh - Spc, bw, bh, caption_yes, static_yes_click, this);

    idleloop(static_cast<int>(windows.size()) - 1);

    return last_response;
}

Gui::MessageBoxResponse Gui::show_inputbox(const std::string& title, std::string& text, bool password) {
    int width = 280;
    int height = 80;
    int x = subsystem.get_view_width() / 2 - width / 2;
    int y = subsystem.get_view_height() / 2 - height / 2;

    GuiWindow *window = push_window(x, y, width, height, title);
    width = window->get_client_width();
    input_box = create_textbox(window, Spc, Spc, width - 2 * Spc, text);
    input_box->set_focus();
    input_box->set_type(password ? GuiTextbox::TypeHidden : GuiTextbox::TypeNormal);
    height = window->get_height() - window->get_client_height();
    window->set_height(height + 3 * Spc + input_box->get_height() + 18);

    /* place buttons */
    int bh = 18;
    std::string caption_no(i18n(I18N_BUTTON_CANCEL));
    width = window->get_client_width();
    height = window->get_client_height();
    int bw = get_font()->get_text_width(caption_no) + 24;

    std::string caption_yes(i18n(I18N_BUTTON_OK));
    int bw2 = get_font()->get_text_width(caption_yes) + 24;
    if (bw2 > bw) {
        bw = bw2;
    }

    create_button(window, width - bw - Spc, height - bh - Spc, bw, bh, caption_no, static_no_click, this);
    create_button(window, width - 2 * bw - Spc - 5, height - bh - Spc, bw, bh, caption_yes, static_yes_input_box_click, this);

    last_entered = text;
    idleloop(static_cast<int>(windows.size()) - 1);
    text = last_entered;
    return last_response;
}

void Gui::idleloop(int stack_counter) {
    while (running && static_cast<int>(windows.size()) > stack_counter) {
        /* update caret blinker */
        get_now(now);
        if (diff_ms(last, now) > CaretBlinkInterval) {
            blink_on = !blink_on;
            last = now;
        }

        /* update independent blinker */
        if (diff_ms(last_tick, now) > TickInterval) {
            tick_on = !tick_on;
            last_tick = now;
        }

        /* go */
        while (subsystem.get_input(input)) {
            switch (input.data_type) {
                case InputData::InputDataTypeQuit:
                    running = false;
                    break;

                case InputData::InputDataTypeMouseMove:
                {
                    int zm = subsystem.get_zoom_factor();
                    *pmousex = input.param1 / zm;
                    *pmousey = input.param2 / zm;
                    int vw = subsystem.get_view_width();
                    int vh = subsystem.get_view_height();
                    bool warp = false;
                    if (*pmousex > vw - 2) {
                        *pmousex = vw - 2;
                        warp = true;
                    }
                    if (*pmousex < 1) {
                        *pmousex = 1;
                        warp = true;
                    }
                    if (*pmousey > vh - 2) {
                        *pmousey = vh - 2;
                        warp = true;
                    }
                    if (*pmousey < 1) {
                        *pmousey = 1;
                        warp = true;
                    }
                    if (warp && subsystem.is_fullscreen()) {
                        subsystem.set_mouse_position(*pmousex, *pmousey);
                    }

                    if (!process_mousemove()) {
                        on_input_event(input);
                    }
                    break;
                }

                case InputData::InputDataMouseLeftWindow:
                    mouse_is_visible = false;
                    break;

                case InputData::InputDataMouseEnteredWindow:
                    mouse_is_visible = true;
                    break;

                case InputData::InputDataTypeMouseLeftDown:
                    if (!process_mousedown(0)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseLeftUp:
                    if (!process_mouseup(0)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseMiddleDown:
                    if (!process_mousedown(1)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseMiddleUp:
                    if (!process_mouseup(1)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseRightDown:
                    if (!process_mousedown(2)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseRightUp:
                    if (!process_mouseup(2)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeMouseWheel:
                    if (!process_mousewheel(input.param1, input.param2)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeKeyDown:
                case InputData::InputDataTypeText:
                {
                    bool processed_kd = false;
                    if (input.data_type == InputData::InputDataTypeKeyDown) {
                        processed_kd = process_keydown(input);
                    }

                    bool processed_ki = process_keyinput(input);

                    if (!processed_kd && !processed_ki) {
                        on_input_event(input);
                    }
                    break;
                }

                case InputData::InputDataTypeKeyUp:
                    if (!process_keyup(input)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeJoyMotion:
                    if (!process_joymotion(input)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeJoyButtonDown:
                    if (!process_joybuttondown(input)) {
                        on_input_event(input);
                    }
                    break;

                case InputData::InputDataTypeJoyButtonUp:
                    if (!process_joybuttonup(input)) {
                        on_input_event(input);
                    }
                    break;

                default:
                    break;
            }
        }

        /* drawing */
        subsystem.begin_drawings();

        idle();
        idle_tooltip();
        subsystem.reset_color();
        for (WindowStack::iterator it = windows.begin(); it != windows.end(); it++) {
            GuiWindow *o = *it;
            o->draw();
        }
        subsystem.reset_color();

        /* draw tooltip */
        if (tooltip) {
            tooltip->draw();
        }
        subsystem.reset_color();

        if (windows.size() && mouse_is_visible) {
            subsystem.draw_icon(mouse, *pmousex - mouse->get_hotspot_x(), *pmousey - mouse->get_hotspot_y());
        }

        subsystem.end_drawings();
    }
}

void Gui::set_current_window() {
    size_t sz = windows.size();
    if (sz) {
        current_window = windows[sz - 1];
    } else {
        current_window = 0;
    }
}

void Gui::check_parent(GuiObject *parent) {
    if (!parent) {
        throw GuiException(i18n(I18N_NO_PARENT));
    }
}

GuiWindow *Gui::get_window_of_object(GuiObject *object) const {
    GuiWindow *window = 0;
    if (object) {
        while (object->get_parent()) {
            object = object->get_parent();
        }
        window = static_cast<GuiWindow *>(object);
    }

    return window;
}

bool Gui::process_mousemove() {
    bool processed = false;

    if (mouse_is_down) {
        if (active_object) {
            active_object->mousemove(*pmousex, *pmousey);
        }
    } else {
        if (current_window) {
            GuiObject *obj = current_window->get_upmost_object(*pmousex, *pmousey);
            while (obj && !obj->can_have_mouse_events()) {
                obj = obj->get_parent();
            }
            if (obj) {
                processed = obj->mousemove(*pmousex, *pmousey);
            }
            set_tooltip(obj);
        }
    }

    return processed;
}

bool Gui::process_mousedown(int button) {
    bool processed = false;

    if (current_window) {
        GuiObject *obj = current_window->get_upmost_object(*pmousex, *pmousey);
        if (obj) {
            while (obj && !obj->can_have_mouse_events()) {
                obj = obj->get_parent();
            }
            if (obj) {
                set_focus(obj);
                processed = obj->mousedown(button, *pmousex, *pmousey);
            }
        }
        active_object = obj;
        if (!tooltip) {
            destroy_tooltip();
        }
    }
    mouse_is_down = true;

    return processed;
}

bool Gui::process_mouseup(int button) {
    bool processed = false;

    if (mouse_is_down) {
        if (active_object) {
            processed = active_object->mouseup(button, *pmousex, *pmousey);
        }
    } else {
        if (current_window) {
            GuiObject *obj = current_window->get_upmost_object(*pmousex, *pmousey);
            while (obj && !obj->can_have_mouse_events()) {
                obj = obj->get_parent();
            }
            if (obj) {
                processed = obj->mouseup(button, *pmousex, *pmousey);
            }
        }
    }
    active_object = 0;
    mouse_is_down = false;

    return processed;
}

bool Gui::process_mousewheel(int x, int y) {
    bool processed = false;

    if (current_window) {
        GuiObject *obj = current_window->get_upmost_object(*pmousex, *pmousey);
        if (obj) {
            while (obj && !obj->can_have_mouse_events()) {
                obj = obj->get_parent();
            }
            if (obj) {
                processed = obj->mousehweel(x, y);
            }
        }
    }

    return processed;
}

bool Gui::process_keyinput(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->keypress(input);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                reset_blinker();
                processed = object->keypress(input);
            }
        }
    }

    return processed;
}

bool Gui::process_keydown(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->keydown(input.keycode, input.key_repeat != 0);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                processed = object->keydown(input.keycode, input.key_repeat != 0);
            }
        }
    }

    return processed;
}

bool Gui::process_keyup(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->keyup(input.keycode);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                processed = object->keyup(input.keycode);
            }
        }
    }

    return processed;
}

bool Gui::process_joymotion(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->joymotion(input.param1);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                processed = object->joymotion(input.param1);
            }
        }
    }

    return processed;
}

bool Gui::process_joybuttondown(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->joybuttondown(input.param1);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                object->joybuttondown(input.param1);
            }
        }
    }

    return processed;
}

bool Gui::process_joybuttonup(InputData& input) {
    bool processed = false;

    if (current_window) {
        GuiWindow *cw = current_window;
        processed = cw->joybuttonup(input.param1);
        if (cw == current_window && !processed) {
            GuiObject *object = current_window->get_focused_object();
            if (object) {
                object->joybuttonup(input.param1);
            }
        }
    }

    return processed;
}

void Gui::reset_blinker() {
    get_now(last);
    blink_on = true;
    last = now;
}

void Gui::static_close_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->pop_window();
}

/* messagebox response callbacks */
void Gui::static_cancel_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->cancel_click();
}

void Gui::cancel_click() {
    last_response = MessageBoxResponseCancel;
    pop_window();
}

void Gui::static_ok_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->ok_click();
}

void Gui::ok_click() {
    last_response = MessageBoxResponseOk;
    pop_window();
}

void Gui::static_yes_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->yes_click();
}

void Gui::yes_click() {
    last_response = MessageBoxResponseYes;
    pop_window();
}

void Gui::static_no_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->no_click();
}

void Gui::no_click() {
    last_response = MessageBoxResponseNo;
    pop_window();
}

void Gui::static_yes_input_box_click(GuiVirtualButton *sender, void *data) {
    (reinterpret_cast<Gui *>(data))->yes_input_box_click();
}

void Gui::yes_input_box_click() {
    last_response = MessageBoxResponseYes;
    last_entered = input_box->get_text();
    pop_window();
}

void Gui::set_tooltip(GuiObject *object) {
    if (object != tooltip_object) {
        if (!tooltip) {
            get_now(tooltip_init);
        }
        destroy_tooltip();
        tooltip_object = object;
    }
}

void Gui::destroy_tooltip() {
    if (tooltip) {
        delete tooltip;
        tooltip = 0;
    }
    tooltip_object = 0;
}

void Gui::idle_tooltip() {
    if (!tooltip && tooltip_object) {
        const std::string& tooltip_text = tooltip_object->get_tooltip_text();
        if (tooltip_text.length()) {
            if (diff_ms(tooltip_init, now) > 750) {
                tooltip_x = *pmousex;
                tooltip_y = *pmousey;
                Font *f = resources.get_font("normal");
                int mrg = 2;
                int tw = f->get_text_width(tooltip_text);
                int th = f->get_font_height();
                int ww = tw + 3 * mrg;
                int wh = th + 2 * mrg;
                tooltip = new GuiBox(*this, 0, tooltip_x, tooltip_y, ww, wh);
                tooltip->set_filled(true);
                tooltip->set_color(1.0f, 1.0f, 0.75f);
                tooltip->set_follow_alpha(false);
                GuiLabel *lbl = new GuiLabel(*this, tooltip, mrg, mrg, tw, th, tooltip_text);
                lbl->set_follow_alpha(false);
            }
        }
    }
}
