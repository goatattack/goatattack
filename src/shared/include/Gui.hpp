#ifndef GUI_HPP
#define GUI_HPP

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

    Gui(Resources& resources, Subsystem& subsystem, Font *font) throw (GuiException, ResourcesException);
    virtual ~Gui();

    void run() throw (Exception);
    void leave();

    virtual void idle() throw (Exception) = 0;
    virtual void on_input_event(const InputData& input) = 0;

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
    size_t get_stack_count() const;

    Resources& get_resources() const;
    Subsystem& get_subsystem() const;
    Font *get_font() const;

    GuiWindow *push_window(int x, int y, int width, int height, const std::string& title);
    void pop_window();

    GuiBox *create_box(GuiObject *parent, int x, int y, int width,
        int height) throw (GuiException);

    GuiLabel *create_label(GuiObject *parent, int x, int y,
        const std::string& caption) throw (GuiException);

    GuiButton *create_button(GuiObject *parent, int x, int y, int width, int height,
        const std::string& caption, GuiButton::OnClick on_click,
        void *on_click_data) throw (GuiException);

    GuiCheckbox *create_checkbox(GuiObject *parent, int x, int y,
        const std::string& caption, bool state, GuiCheckbox::OnClick on_click,
        void *on_click_data) throw (GuiException);

    GuiTextbox *create_textbox(GuiObject *parent, int x, int y, int width,
        const std::string& text) throw (GuiException);

    GuiPicture *create_picture(GuiObject *parent, int x, int y,
        TileGraphic *graphic) throw (GuiException);

    GuiFrame *create_frame(GuiObject *parent, int x, int y, int width,
        int height) throw (GuiException);

    GuiTab *create_tab(GuiObject *parent, int x, int y, int width, int height)
        throw (GuiException);

    GuiHScroll *create_hscroll(GuiObject *parent, int x, int y, int width,
        int min_value, int max_value, int initial_value,
        GuiScroll::ValueChanged on_value_changed, void *on_value_changed_data)
        throw (GuiException);

    GuiVScroll *create_vscroll(GuiObject *parent, int x, int y, int height,
        int min_value, int max_value, int initial_value,
        GuiScroll::ValueChanged on_value_changed, void *on_value_changed_data)
        throw (GuiException);

    GuiListbox *create_listbox(GuiObject *parent, int x, int y, int width,
        int height, const std::string& title, GuiListbox::OnItemSelected on_item_selected,
        void *on_item_selected_data) throw (GuiException);

    GuiWindow *get_current_window() const;
    bool is_active(GuiObject *object) const;
    float get_alpha(GuiObject *object) const;
    void set_focus(GuiObject *object);
    bool has_focus(GuiObject *object) const;
    bool get_blink_on() const;
    void set_mousepointer(Mousepointer mouse);

    MessageBoxResponse show_messagebox(MessageBoxIcon icon, const std::string& title, const std::string& text);
    MessageBoxResponse show_questionbox(const std::string& title, const std::string& text);
    MessageBoxResponse show_inputbox(const std::string& title, std::string& text, bool password);

private:
    typedef std::deque<GuiWindow *> WindowStack;

    Resources& resources;
    Subsystem& subsystem;
    Font *font;
    GuiWindow *current_window;
    GuiObject *active_object;
    bool blink_on;
    bool running;
    bool mouse_is_down;
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
    gametime_t tooltip_init;
    MessageBoxResponse last_response;
    GuiTextbox *input_box;
    std::string last_entered;

    void idleloop(int stack_counter) throw (Exception);
    void set_current_window();
    void check_parent(GuiObject *parent) throw (GuiException);
    GuiWindow *get_window_of_object(GuiObject *object) const;
    bool process_mousemove();
    bool process_mousedown(int button);
    bool process_mouseup(int button);
    bool process_keyinput(InputData& input);
    bool process_keydown(InputData& input);
    bool process_keyup(InputData& input);
    bool process_joymotion(InputData& input);
    bool process_joybuttondown(InputData& input);
    bool process_joybuttonup(InputData& input);
    void reset_blinker();
    void destroy_tooltip();
    void set_tooltip(GuiObject *object);
    void idle_tooltip();

    /* messagebox helpers */
    static void static_close_click(GuiButton *sender, void *data);
    static void static_cancel_click(GuiButton *sender, void *data);
    void cancel_click();
    static void static_ok_click(GuiButton *sender, void *data);
    void ok_click();
    static void static_yes_click(GuiButton *sender, void *data);
    void yes_click();
    static void static_no_click(GuiButton *sender, void *data);
    void no_click();
    static void static_yes_input_box_click(GuiButton *sender, void *data);
    void yes_input_box_click();
};

#endif
