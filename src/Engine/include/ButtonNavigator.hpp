#ifndef BUTTONNAVIGATOR_HPP
#define BUTTONNAVIGATOR_HPP

#include "Gui.hpp"
#include "Configuration.hpp"
#include "KeyBinding.hpp"

#include <vector>

class ButtonNavigator {
private:
    ButtonNavigator(const ButtonNavigator&);
    ButtonNavigator& operator=(const ButtonNavigator&);

public:
    typedef void (*OnCancel)(void *data);

    ButtonNavigator(Gui& gui, Configuration& config);
    virtual ~ButtonNavigator();

    void install_handlers(GuiWindow *window,
        OnCancel on_cancel, void *on_cancel_data);

    void add_button(GuiButton *btn);
    void clear();
    void set_button_focus();

private:
    typedef std::vector<GuiButton *> NavigatorButtons;

    Gui& gui;
    Configuration& config;
    OnCancel on_cancel;
    void *on_cancel_data;
    bool joymotion_up;
    bool joymotion_down;

    NavigatorButtons navigator_buttons;
    int current_index;
    KeyBinding binding;

    static bool static_window_keydown(GuiWindow *sender, void *data, int keycode, bool repeat);
    bool window_keydown(int keycode, bool repeat);

    static bool static_window_joymotion(GuiWindow *sender, void *data, int motion);
    bool window_joymotion(int motion);

    static bool static_window_joybutton_down(GuiWindow *sender, void *data, int button);
    bool window_joybutton_down(int button);

    void set_button_focus_up();
    void set_button_focus_down();
    void do_current_button_click();
};

#endif