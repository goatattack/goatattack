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

#ifndef _GUI_HPP_
#define _GUI_HPP_

#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "GuiObject.hpp"
#include "Timing.hpp"

#include <deque>

class GuiException : public Exception {
public:
    GuiException(const char *msg) : Exception(msg) { }
    GuiException(const std::string& msg) : Exception(msg) { }
};

struct GuiResponse {
    GuiResponse() : quit(false) { }
    bool quit;
};

class Gui {
private:
    Gui(const Gui&);
    Gui& operator=(const Gui&);

public:
    static const int Spc = 12;

    enum MessageBoxResponse {
        MessageBoxResponseCancel = 0,
        MessageBoxResponseOk,
        MessageBoxResponseYes,
        MessageBoxResponseNo
    };

    enum MessageBoxIcon {
        MessageBoxIconNone = 0,
        MessageBoxIconQuestion,
        MessageBoxIconInformation,
        MessageBoxIconExclamation,
        MessageBoxIconError
    };

    enum Mousepointer {
        MousepointerDefault = 0,
        MousepointerMove
    };

    Gui(Resources& resources, Subsystem& subsystem, Font *font);
    ~Gui();

    void run();
    void leave();

    virtual void idle() = 0;
    virtual void on_input_event(const InputData& input) = 0;
    virtual void on_leave() = 0;

    void load_resources();
    void link_mouse(const Gui& gui);
    void link_mouse(int *pmousex, int *pmousey);
    void unlink_mouse();
    int *get_pmousex() const;
    int *get_pmousey() const;
    int get_mouse_x() const;
    int get_mouse_y() const;
    void set_mouse_x(int v);
    void set_mouse_y(int v);
    void set_mouse_xy(int x, int y);
    size_t get_stack_count() const;

    Resources& get_resources() const;
    Subsystem& get_subsystem() const;
    Font *get_font() const;

    GuiWindow *push_window(int x, int y, int width, int height, const std::string& title);
    void pop_window();

    GuiBox *create_box(GuiObject *parent, int x, int y, int width, int height);
    GuiLabel *create_label(GuiObject *parent, int x, int y, const std::string& caption);

    GuiButton *create_button(GuiObject *parent, int x, int y, int width, int height,
        const std::string& caption, GuiVirtualButton::OnClick on_click,
        void *on_click_data);

    GuiRoundedButton *create_rounded_button(GuiObject *parent, int x, int y, int width,
        int height, const std::string& caption, GuiVirtualButton::OnClick on_click,
        void *on_click_data);

    GuiCheckbox *create_checkbox(GuiObject *parent, int x, int y,
        const std::string& caption, bool state, GuiCheckbox::OnClick on_click,
        void *on_click_data);

    GuiTextbox *create_textbox(GuiObject *parent, int x, int y, int width,
        const std::string& text);

    GuiPicture *create_picture(GuiObject *parent, int x, int y,
        TileGraphic *graphic);

    GuiFrame *create_frame(GuiObject *parent, int x, int y, int width, int height);
    GuiTab *create_tab(GuiObject *parent, int x, int y, int width, int height);

    GuiHScroll *create_hscroll(GuiObject *parent, int x, int y, int width,
        int min_value, int max_value, int initial_value,
        GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data);

    GuiVScroll *create_vscroll(GuiObject *parent, int x, int y, int height,
        int min_value, int max_value, int initial_value,
        GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data);

    GuiListbox *create_listbox(GuiObject *parent, int x, int y, int width,
        int height, const std::string& title, GuiListbox::OnItemSelected on_item_selected,
        void *on_item_selected_data);

    GuiListbox *create_listbox(GuiObject *parent, int x, int y, int width,
        int height, Icon *icon, int icon_width, const std::string& title,
        GuiListbox::OnItemSelected on_item_selected, void *on_item_selected_data);

    GuiWindow *get_current_window() const;
    bool is_active(GuiObject *object) const;
    float get_alpha(GuiObject *object) const;
    void set_focus(GuiObject *object);
    bool has_focus(GuiObject *object) const;
    bool get_blink_on() const;
    bool get_tick_on() const;
    void set_mousepointer(Mousepointer mouse);
    void reset_blinker();

    MessageBoxResponse show_messagebox(MessageBoxIcon icon, const std::string& title, const std::string& text);
    MessageBoxResponse show_questionbox(const std::string& title, const std::string& text);
    MessageBoxResponse show_inputbox(const std::string& title, std::string& text, bool password);

private:
    typedef std::deque<GuiWindow *> WindowStack;

    Resources& resources;
    Subsystem& subsystem;
    I18N& i18n;
    Font *font;
    GuiWindow *current_window;
    GuiObject *active_object;
    bool blink_on;
    bool tick_on;
    bool running;
    bool mouse_is_down;
    bool mouse_is_visible;
    int local_mousex;
    int local_mousey;
    int *pmousex;
    int *pmousey;
    GuiBox *tooltip;
    GuiObject *tooltip_object;
    int tooltip_x;
    int tooltip_y;

    WindowStack windows;
    InputData input;

    Icon *mouse_default;
    Icon *mouse_move;
    Icon *mouse;
    Icon *msg_question;
    Icon *msg_information;
    Icon *msg_exclamation;
    Icon *msg_error;
    gametime_t now;
    gametime_t last;
    gametime_t last_tick;
    gametime_t tooltip_init;
    MessageBoxResponse last_response;
    GuiTextbox *input_box;
    std::string last_entered;

    void idleloop(int stack_counter);
    void set_current_window();
    void check_parent(GuiObject *parent);
    GuiWindow *get_window_of_object(GuiObject *object) const;
    bool process_mousemove();
    bool process_mousedown(int button);
    bool process_mouseup(int button);
    bool process_mousewheel(int x, int y);
    bool process_keyinput(InputData& input);
    bool process_keydown(InputData& input);
    bool process_keyup(InputData& input);
    bool process_joymotion(InputData& input);
    bool process_joybuttondown(InputData& input);
    bool process_joybuttonup(InputData& input);
    void destroy_tooltip();
    void set_tooltip(GuiObject *object);
    void idle_tooltip();

    /* messagebox helpers */
    static void static_close_click(GuiVirtualButton *sender, void *data);
    static void static_cancel_click(GuiVirtualButton *sender, void *data);
    void cancel_click();
    static void static_ok_click(GuiVirtualButton *sender, void *data);
    void ok_click();
    static void static_yes_click(GuiVirtualButton *sender, void *data);
    void yes_click();
    static void static_no_click(GuiVirtualButton *sender, void *data);
    void no_click();
    static void static_yes_input_box_click(GuiVirtualButton *sender, void *data);
    void yes_input_box_click();
};

#endif
