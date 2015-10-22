#ifndef GUIOBJECT_HPP
#define GUIOBJECT_HPP

#include "Resources.hpp"
#include "Subsystem.hpp"

#include <string>
#include <vector>

class Gui;
class TileGraphic;
class GuiWindow;

/* GuiObject */
class GuiObject {
private:
    GuiObject(const GuiObject&);
    GuiObject& operator=(const GuiObject&);

public:
    typedef std::vector<GuiObject *> Children;

    GuiObject(Gui& gui, GuiObject *parent);
    GuiObject(Gui& gui, GuiObject *parent, int x, int y,
        int width, int height);
    virtual ~GuiObject();

    void remove_all_objects();
    void remove_object(GuiObject *object);
    void set_x(int x);
    void set_y(int y);
    virtual int get_x() const;
    virtual int get_y() const;
    virtual int get_client_x() const;
    virtual int get_client_y() const;
    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;

    void set_width(int width);
    void set_height(int height);
    int get_width() const;
    int get_height() const;

    bool get_visible() const;
    void set_visible(bool state);
    void set_focus();

    void set_tag(int tag);
    int get_tag() const;
    void set_ptr_tag(const void *ptr);
    const void *get_ptr_tag() const;

    void set_tooltip_text(const std::string& text);
    const std::string& get_tooltip_text() const;

    bool get_follow_alpha() const;
    void set_follow_alpha(bool state);

    void draw();

    GuiObject *get_parent() const;
    const Children& get_children() const;
    GuiObject *get_upmost_object(int x, int y);

    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);
    virtual bool keydown(int keycode, bool repeat);
    virtual bool keyup(int keycode);
    virtual bool keypress(InputData& input);
    virtual bool joymotion(int motion);
    virtual bool joybuttondown(int button);
    virtual bool joybuttonup(int button);

protected:
    Gui& gui;

private:
    int x;
    int y;
    int width;
    int height;
    bool visible;
    int tag;
    bool follow_alpha;
    const void *ptr;

    GuiObject *parent;
    Children children;
    std::string tooltip_text;

    void set_parent(GuiObject *obj);
    virtual void paint() = 0;
};

/* GuiWindow */
class GuiWindow : public GuiObject {
public:
    typedef bool (*OnKeyDown)(GuiWindow *sender, void *data, int keycode, bool repeat);
    typedef bool (*OnKeyUp)(GuiWindow *sender, void *data, int keycode);
    typedef bool (*OnJoyMotion)(GuiWindow *sender, void *data, int motion);
    typedef bool (*OnJoyButtonDown)(GuiWindow *sender, void *data, int button);
    typedef bool (*OnJoyButtonUp)(GuiWindow *sender, void *data, int button);

    GuiWindow(Gui& gui, GuiObject *parent);
    GuiWindow(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
        const std::string& title);

    virtual ~GuiWindow();

    void set_on_keydown(OnKeyDown on_key_down, void *on_key_down_data);
    void set_on_keyup(OnKeyUp on_key_up, void *on_key_up_data);
    void set_on_joymotion(OnJoyMotion on_joy_motion, void *on_joy_motion_data);
    void set_on_joybuttondown(OnJoyButtonDown on_joy_button_down, void *on_joy_button_down_data);
    void set_on_joybuttonup(OnJoyButtonUp on_joy_button_up, void *on_joy_button_up_data);

    void set_title(const std::string& title);
    const std::string& get_title() const;
    void show_screws(bool state);

    int get_client_width() const;
    int get_client_height() const;
    GuiObject *get_focused_object();
    void set_focused_object(GuiObject *object);
    void set_draw_window(bool state);

    virtual bool can_have_mouse_events() const;
    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);
    virtual bool keydown(int keycode, bool repeat);
    virtual bool keyup(int keycode);
    virtual bool joymotion(int motion);
    virtual bool joybuttondown(int button);
    virtual bool joybuttonup(int button);

protected:
    virtual int get_client_x() const;
    virtual int get_client_y() const;

private:
    GuiObject *focused_object;
    OnKeyDown on_key_down;
    void *on_key_down_data;
    OnKeyUp on_key_up;
    void *on_key_up_data;
    OnJoyMotion on_joy_motion;
    void *on_joy_motion_data;
    OnJoyButtonDown on_joy_button_down;
    void *on_joy_button_down_data;
    OnJoyButtonUp on_joy_button_up;
    void *on_joy_button_up_data;
    std::string title;
    bool screws;
    bool draw_window;

    bool mouse_is_down;
    bool moving_valid;
    int distance_x;
    int distance_y;
    int window_title_height;
    Icon *screw1, *screw2, *screw3, *screw4;

    virtual void paint();
    void prepare();
};

/* GuiBox */
class GuiBox : public GuiObject {
public:
    GuiBox(Gui& gui, GuiObject *parent);
    GuiBox(Gui& gui, GuiObject *parent, int x, int y, int width, int height);

    virtual ~GuiBox();
    void set_color(float bg_r, float bg_g, float bg_b);
    void set_filled(bool state);

private:
    bool filled;
    float bg_r;
    float bg_g;
    float bg_b;

    virtual void paint();
};

/* GuiLabel */
class GuiLabel : public GuiObject {
public:
    GuiLabel(Gui& gui, GuiObject *parent);
    GuiLabel(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
        const std::string& caption);

    virtual ~GuiLabel();

    void set_caption(const std::string& caption);
    const std::string& get_caption() const;
    void set_color(float bg_r, float bg_g, float bg_b);
    void set_follow_alpha(bool state);

private:
    bool follow_alpha;
    float bg_r;
    float bg_g;
    float bg_b;
    std::string caption;

    virtual void paint();
};

/* GuiButton */
class GuiButton : public GuiObject {
public:
    typedef void (*OnClick)(GuiButton *sender, void *data);
    enum Alignment {
        AlignmentCenter = 0,
        AlignmentLeft,
        AlignmentRight
    };

    GuiButton(Gui& gui, GuiObject *parent);
    GuiButton(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
        const std::string& caption, OnClick on_click, void *on_click_data);

    virtual ~GuiButton();

    void set_caption(const std::string& caption);
    const std::string& get_caption() const;
    void show_bolts(bool state);
    void set_color(float r, float g, float b);
    void reset_color();
    OnClick get_on_click_func();
    void *get_on_click_data();
    void set_alignment(Alignment align);
    void set_offset_x(int x);
    void set_offset_y(int y);

    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;
    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);

protected:
    virtual int get_client_x() const;
    virtual int get_client_y() const;

private:
    std::string caption;
    bool mouse_is_down;
    OnClick on_click;
    void *on_click_data;
    bool mouse_is_in_button;
    bool bolts;
    Icon *bolt;
    float text_r;
    float text_g;
    float text_b;
    Alignment align;
    int offset_x;
    int offset_y;

    virtual void paint();
    void prepare();
};

/* GuiCheckbox */
class GuiCheckbox : public GuiObject {
public:
    enum CheckBoxStyle {
        CheckBoxStyleQuad = 0,
        CheckBoxStyleCircle
    };

    typedef void (*OnClick)(GuiCheckbox *sender, void *data, bool state);

    GuiCheckbox(Gui& gui, GuiObject *parent);
    GuiCheckbox(Gui& gui, GuiObject *parent, int x, int y, const std::string& caption,
        bool state, OnClick on_click, void *on_click_data);

    virtual ~GuiCheckbox();

    void set_caption(const std::string& caption);
    const std::string& get_caption() const;
    void set_style(CheckBoxStyle style);
    void set_state(bool state);
    bool get_state() const;

    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;
    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);

private:
    std::string caption;
    bool state;
    bool mouse_is_down;
    bool mouse_is_in_button;
    OnClick on_click;
    void *on_click_data;
    CheckBoxStyle style;

    Icon *on;
    Icon *off;
    Icon *half;

    virtual void paint();

    void load_icons();
};

/* GuiTextbox */
class GuiTextbox : public GuiObject {
public:
    GuiTextbox(Gui& gui, GuiObject *parent);
    GuiTextbox(Gui& gui, GuiObject *parent, int x, int y, int width,
        const std::string& text);

    virtual ~GuiTextbox();

    void set_text(const std::string& text);
    const std::string& get_text() const;
    void set_caret_position(int pos);
    int get_caret_position() const;
    void set_locked(bool state);
    bool get_locked() const;
    void set_hide_characters(bool state);
    bool get_hide_characters() const;

    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;
    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);
    virtual bool keyup(int keycode);
    virtual bool keypress(InputData& input);

private:
    int start_position;
    int caret_position;
    std::string text;
    bool locked;
    bool hide_characters;

    void recalc();
    void set_cursor_pos_from_mouse(int x);
    virtual void paint();
};

/* GuiPicture */
class GuiPicture : public GuiObject {
public:
    GuiPicture(Gui& gui, GuiObject *parent);
    GuiPicture(Gui& gui, GuiObject *parent, int x, int y, TileGraphic *graphic);

    virtual ~GuiPicture();

    void set_picture(TileGraphic *graphic);

private:
    TileGraphic *graphic;

    virtual void paint();
};

/* GuiFrame */
class GuiFrame : public GuiObject {
public:
    GuiFrame(Gui& gui, GuiObject *parent);
    GuiFrame(Gui& gui, GuiObject *parent, int x, int y, int width, int height);

    virtual ~GuiFrame();

private:
    virtual void paint();
};

/* GuiTab */
class GuiTab : public GuiObject {
public:
    GuiTab(Gui& gui, GuiObject *parent);
    GuiTab(Gui& gui, GuiObject *parent, int x, int y, int width, int height);

    virtual ~GuiTab();

    GuiFrame *create_tab(const std::string& name);
    void select_tab(int index);

private:
    struct Tab {
        GuiTab *owner;
        int tab_number;
        GuiButton *button;
        GuiFrame *tab;
    };

    typedef std::vector<Tab *> Tabs;

    Tab *current_tab;
    int tab_button_height;
    int current_button_x;
    Tabs tabs;

    virtual void paint();

    static void static_tab_clicked(GuiButton *sender, void *data);
};

/* GuiScroll */
class GuiScroll : public GuiObject {
public:
    typedef void (*ValueChanged)(GuiScroll *sender, void *data, int value);
    static const int Size = 11;

    GuiScroll(Gui& gui, GuiObject *parent);
    GuiScroll(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
        int min_value, int max_value, int initial_value,
        ValueChanged on_value_changed, void *on_value_changed_data);

    virtual ~GuiScroll();

    void set_value(int value);
    int get_value() const;
    void set_min_value(int value);
    int get_min_value() const;
    void set_max_value(int value);
    int get_max_value() const;
    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;

protected:
    ValueChanged on_value_changed;
    void *on_value_changed_data;
    int min_value;
    int max_value;
    int current_value;
    bool draw_block;

    void recalc();
    static void static_down_button_clicked(GuiButton *sender, void *data);
    static void static_up_button_clicked(GuiButton *sender, void *data);
};

/* GuiHScroll */
class GuiHScroll : public GuiScroll {
public:
    GuiHScroll(Gui& gui, GuiObject *parent);
    GuiHScroll(Gui& gui, GuiObject *parent, int x, int y, int width,
        int min_value, int max_value, int initial_value,
        GuiScroll::ValueChanged on_value_changed, void *on_value_changed_data);

    virtual ~GuiHScroll();

    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);

private:
    GuiButton *left_button;
    GuiButton *right_button;
    Icon *left_arrow;
    Icon *right_arrow;
    int blockpos;
    bool moving;
    int origin_x;

    virtual void paint();

    void prepare();
    void calc_blockpos();
};

/* GuiVScroll */
class GuiVScroll : public GuiScroll {
public:
    GuiVScroll(Gui& gui, GuiObject *parent);
    GuiVScroll(Gui& gui, GuiObject *parent, int x, int y, int height,
        int min_value, int max_value, int initial_value,
        GuiScroll::ValueChanged on_value_changed, void *on_value_changed_data);

    virtual ~GuiVScroll();

    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);

private:
    GuiButton *up_button;
    GuiButton *down_button;
    Icon *up_arrow;
    Icon *down_arrow;
    int blockpos;
    bool moving;
    int origin_y;

    virtual void paint();

    void prepare();
    void calc_blockpos();
};

/* GuiListboxEntry */
class GuiListboxEntry : public GuiObject {
public:
    enum DrawType {
        DrawTypeNormal = 0,
        DrawTypeSelected,
        DrawTypeTitle
    };

    static const int Spc = 5;

    class Column {
    public:
        Column(const std::string& text, int width) : text(text), width(width) { }
        ~Column() { }

        std::string text;
        int width;
    };

    typedef std::vector<Column> Columns;

    GuiListboxEntry(Gui& gui, GuiObject *parent);
    GuiListboxEntry(Gui& gui, GuiObject *parent, const std::string& text);
    GuiListboxEntry(Gui& gui, GuiObject *parent, const std::string& text, int width);
    GuiListboxEntry(Gui& gui, GuiObject *parent, const Column& column);

    virtual ~GuiListboxEntry();

    Columns& get_columns();
    void add_column(const Column& column);
    void add_column(const std::string& text, int width);

    void draw(int x, int y, int width, DrawType draw_type);

private:
    Columns columns;

    virtual void paint();
    void draw_column(const Column& column, int x, int y, int max_width);
};

/* GuiListbox */
class GuiListbox : public GuiObject {
public:
    typedef void (*OnItemSelected)(GuiListbox *sender, void *data, int index);
    typedef void (*OnTitleClicked)(GuiListbox *sender, void *data, int index);

    GuiListbox(Gui& gui, GuiObject *parent);
    GuiListbox(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
        const std::string& title, OnItemSelected on_item_selected,
        void *on_item_selected_data);

    virtual ~GuiListbox();

    void set_on_title_clicked(OnTitleClicked on_title_clicked, void *on_title_clicked_data);
    void set_top_index(int index);
    int get_top_index() const;
    void set_selected_index(int index);
    int get_selected_index() const;
    int get_entry_count() const;
    GuiListboxEntry *get_entry(int index);
    GuiListboxEntry *add_entry(const std::string& text);
    GuiListboxEntry *add_entry(const std::string& text, int width);
    GuiListboxEntry *add_entry(const GuiListboxEntry::Column& column);
    GuiListboxEntry *get_title_bar();
    void show_title_bar(bool state);
    void remove_entry(int index);
    void clear();

    virtual bool can_have_focus() const;
    virtual bool can_have_mouse_events() const;
    virtual bool mousedown(int button, int x, int y);
    virtual bool mousemove(int x, int y);
    virtual bool mouseup(int button, int x, int y);

private:
    typedef std::vector<GuiListboxEntry *> Entries;

    OnItemSelected on_item_selected;
    void *on_item_selected_data;
    OnTitleClicked on_title_clicked;
    void *on_title_clicked_data;
    Entries entries;
    int start_index;
    int selected_index;
    GuiScroll *sb;
    bool mouse_is_down;
    GuiListboxEntry *title_bar;
    bool title_bar_visible;

    virtual void paint();
    void my_down_mousemove(int x, int y, bool from_mousemove);
    void setup(const std::string& title);
    void recalc();
    void select_from_mouse(int x, int y, bool from_mousemove);

    static void static_scroll_changed(GuiScroll *sender, void *data, int value);
};

#endif
