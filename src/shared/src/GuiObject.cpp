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

#include "GuiObject.hpp"
#include "Gui.hpp"
#include "UTF8.hpp"

#include <cstdlib>

/* ****************************************************** */
/* GuiObject                                              */
/* ****************************************************** */
GuiObject::GuiObject(Gui& gui, GuiObject *parent)
    : gui(gui), x(0), y(0), width(0), height(0), visible(true), tag(0),
      follow_alpha(true), ptr(0)
{
    set_parent(parent);
}

GuiObject::GuiObject(Gui& gui, GuiObject *parent, int x, int y, int width, int height)
    : gui(gui), x(x), y(y), width(width), height(height), visible(true), tag(0),
      follow_alpha(true), ptr(0)
{
    set_parent(parent);
}

GuiObject::~GuiObject() {
    remove_all_objects();
}

void GuiObject::remove_all_objects() {
    for (Children::iterator it = children.begin(); it != children.end(); it++) {
        GuiObject *obj = *it;
        object_removing(obj);
        delete obj;
    }
    children.clear();
}

void GuiObject::remove_object(GuiObject *object) {
    for (Children::iterator it = children.begin(); it != children.end(); it++) {
        GuiObject *obj = *it;
        if (obj == object) {
            object_removing(obj);
            delete obj;
            children.erase(it);
            break;
        }
    }
}

void GuiObject::set_x(int x) {
    this->x = x;
}

void GuiObject::set_y(int y) {
    this->y = y;
}

int GuiObject::get_x() const {
    if (parent) {
        return x + parent->get_x();
    }

    return x;
}

int GuiObject::get_y() const {
    if (parent) {
        return y + parent->get_y();
    }

    return y;
}

int GuiObject::get_client_x() const {
    if (parent) {
        return x + parent->get_client_x();
    }

    return x;
}

int GuiObject::get_client_y() const {
    if (parent) {
        return y + parent->get_client_y();
    }

    return y;
}

bool GuiObject::can_have_focus() const {
    return false;
}

bool GuiObject::can_have_mouse_events() const {
    return false;
}

void GuiObject::set_width(int width) {
    this->width = width;
}

void GuiObject::set_height(int height) {
    this->height = height;
}

int GuiObject::get_width() const {
    return width;
}

int GuiObject::get_height() const {
    return height;
}

bool GuiObject::get_visible() const {
    return visible;
}

void GuiObject::set_visible(bool state) {
    visible = state;
}

void GuiObject::set_focus() {
    gui.set_focus(this);
}

void GuiObject::set_focus_on_first_child() {
    for (Children::iterator it = children.begin(); it != children.end(); it++) {
        GuiObject *obj = *it;
        if (obj->can_have_focus()) {
            gui.set_focus(obj);
            break;
        }
    }
}

void GuiObject::set_tag(int tag) {
    this->tag = tag;
}

int GuiObject::get_tag() const {
    return tag;
}

void GuiObject::set_ptr_tag(const void *ptr) {
    this->ptr = ptr;
}

const void *GuiObject::get_ptr_tag() const {
    return ptr;
}

void GuiObject::set_tooltip_text(const std::string& text) {
    tooltip_text = text;
}

const std::string& GuiObject::get_tooltip_text() const {
    return tooltip_text;
}

bool GuiObject::get_follow_alpha() const {
    return follow_alpha;
}

void GuiObject::set_follow_alpha(bool state) {
    follow_alpha = state;
}

void GuiObject::draw() {
    if (visible) {
        paint();
        for (Children::iterator it = children.begin(); it != children.end(); it++) {
            GuiObject *obj = *it;
            obj->draw();
        }
    }
}

GuiObject *GuiObject::get_parent() const {
    return parent;
}

const GuiObject::Children& GuiObject::get_children() const {
    return children;
}

GuiObject *GuiObject::get_upmost_object(int x, int y) {
    for (Children::iterator it = children.begin(); it != children.end(); it++) {
        GuiObject *child = *it;
        if (child->get_visible()) {
            GuiObject *subchild = child->get_upmost_object(x, y);
            if (subchild) {
                return subchild;
            }
        }
    }

    if (get_visible()) {
        int cx = (parent ? get_client_x() : get_x());
        int cy = (parent ? get_client_y() : get_y());
        if (x >= cx && x <= cx + get_width() - 1 &&
            y >= cy && y <= cy + get_height() - 1)
        {
            return this;
        }
    }

    return 0;
}

bool GuiObject::mousedown(int button, int x, int y) { return false; }

bool GuiObject::mousemove(int x, int y) { return false; }

bool GuiObject::mouseup(int button, int x, int y) { return false; }

bool GuiObject::keypress(InputData& input) { return false; }

bool GuiObject::keydown(int keycode, bool repeat) { return false; }

bool GuiObject::keyup(int keycode) { return false; }

bool GuiObject::joymotion(int motion) { return false; }

bool GuiObject::joybuttondown(int button) { return false; }

bool GuiObject::joybuttonup(int button) { return false; }

void GuiObject::set_parent(GuiObject *obj) {
    parent = obj;
    if (obj) {
        obj->children.push_back(this);
    }
}

void GuiObject::object_removing(GuiObject *obj) { }

/* ****************************************************** */
/* GuiWindow                                              */
/* ****************************************************** */
GuiWindow::GuiWindow(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), focused_object(0), on_key_down(0),
      on_key_down_data(0), on_key_up(0), on_key_up_data(0),
      on_joy_motion(0), on_joy_motion_data(0), on_joy_button_down(0),
      on_joy_button_down_data(0), on_joy_button_up(0), on_joy_button_up_data(0),
      cancelable(false), on_cancel(0), on_cancel_data(0), title(), screws(true)
{
    prepare();
}

GuiWindow::GuiWindow(Gui& gui, GuiObject *parent, int x, int y,
    int width, int height, const std::string& title)
    : GuiObject(gui, parent, x, y, width, height), focused_object(0),
      on_key_down(0), on_key_down_data(0), on_key_up(0), on_key_up_data(0),
      on_joy_motion(0), on_joy_motion_data(0), on_joy_button_down(0),
      on_joy_button_down_data(0), on_joy_button_up(0), on_joy_button_up_data(0),
      cancelable(false), on_cancel(0), on_cancel_data(0), title(title),
      screws(true), invisible(false)
{
    prepare();
}

GuiWindow::~GuiWindow() { }

void GuiWindow::set_on_keydown(OnKeyDown on_key_down, void *on_key_down_data) {
    this->on_key_down = on_key_down;
    this->on_key_down_data = on_key_down_data;
}

void GuiWindow::set_on_keyup(OnKeyUp on_key_up, void *on_key_up_data) {
    this->on_key_up = on_key_up;
    this->on_key_up_data = on_key_up_data;
}

void GuiWindow::set_on_joymotion(OnJoyMotion on_joy_motion, void *on_joy_motion_data) {
    this->on_joy_motion = on_joy_motion;
    this->on_joy_motion_data = on_joy_motion_data;
}

void GuiWindow::set_on_joybuttondown(OnJoyButtonDown on_joy_button_down, void *on_joy_button_down_data) {
    this->on_joy_button_down = on_joy_button_down;
    this->on_joy_button_down_data = on_joy_button_down_data;
}

void GuiWindow::set_on_joybuttonup(OnJoyButtonUp on_joy_button_up, void *on_joy_button_up_data) {
    this->on_joy_button_up = on_joy_button_up;
    this->on_joy_button_up_data = on_joy_button_up_data;
}

void GuiWindow::set_title(const std::string& title) {
    this->title = title;
}

const std::string& GuiWindow::get_title() const {
    return title;
}

void GuiWindow::show_screws(bool state) {
    screws = state;
}

int GuiWindow::get_client_width() const {
    return get_width() - 2;
}

int GuiWindow::get_client_height() const {
    return get_height() - 2 - window_title_height;
}

GuiObject *GuiWindow::get_focused_object() {
    return focused_object;
}

void GuiWindow::set_focused_object(GuiObject *object) {
    if (object) {
        if (object->can_have_focus()) {
            focused_object = object;
        }
    }
}

void GuiWindow::set_invisible(bool state) {
    invisible = state;
}

void GuiWindow::set_cancelable(bool state) {
    cancelable = state;
}

void GuiWindow::set_cancel_callback(OnCallback on_cancel, void *on_cancel_data) {
    this->on_cancel = on_cancel;
    this->on_cancel_data = on_cancel_data;
}

bool GuiWindow::can_have_mouse_events() const {
    return true;
}

bool GuiWindow::mousedown(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = true;
        moving_valid = (y <= get_client_y());
        distance_x = x - get_x();
        distance_y = y - get_y();
    }

    return true;
}

bool GuiWindow::mousemove(int x, int y) {
    if (!invisible && mouse_is_down && moving_valid) {
        set_x(x - distance_x);
        set_y(y - distance_y);
    }

    return true;
}

bool GuiWindow::mouseup(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = false;
    }

    return true;
}

bool GuiWindow::keydown(int keycode, bool repeat) {
    if (cancelable && keycode == 27) {
        if (on_cancel) {
            on_cancel(this, on_cancel_data);
        } else {
            gui.pop_window();
        }
    } else if (on_key_down) {
        return on_key_down(this, on_key_down_data, keycode, repeat);
    }

    return false;
}

bool GuiWindow::keyup(int keycode) {
    if (on_key_up) {
        return on_key_up(this, on_key_up_data, keycode);
    }

    return false;
}

bool GuiWindow::joymotion(int motion) {
    if (on_joy_motion) {
        return on_joy_motion(this, on_joy_motion_data, motion);
    }

    return false;
}

bool GuiWindow::joybuttondown(int button) {
    if (on_joy_button_down) {
        return on_joy_button_down(this, on_joy_button_down_data, button);
    }

    return false;
}

bool GuiWindow::joybuttonup(int button) {
    if (on_joy_button_up) {
        return on_joy_button_up(this, on_joy_button_up_data, button);
    }

    return false;
}

int GuiWindow::get_client_x() const {
    return GuiObject::get_x() + 1;
}

int GuiWindow::get_client_y() const {
    return GuiObject::get_y() + window_title_height;
}


void GuiWindow::object_removing(GuiObject *object) {
    if (object == focused_object) {
        focused_object = 0;
    }
}

void GuiWindow::paint() {
    if (!invisible) {
        Subsystem& s = gui.get_subsystem();
        Font *f = gui.get_font();
        int x = GuiObject::get_x();
        int y = GuiObject::get_y();
        int width = get_width();
        int height = get_height();

        /* set alpha */
        float alpha = gui.get_alpha(this);

        /* draw shadow */
        s.set_color(0.0f, 0.0f, 0.0f, 0.2f);
        s.draw_box(x + 7, y + 7, width, height);

        /* draw window */
        s.set_color(0.5f, 0.5f, 1.0f, alpha);
        s.draw_box(x, y, width, height);

        s.set_color(0.0f, 0.0f, 0.35f, alpha);
        s.draw_box(x + 1, y + 1, width - 2, height - 2);

        /* draw title bar */
        int tw = f->get_text_width(title);
        if (gui.is_active(this)) {
            s.set_color(0.05f, 0.05f, 0.15f, alpha);
            s.set_color(0.0f, 0.0f, 0.35f, alpha);
            s.draw_box(x + 1, y + 1, width - 2, window_title_height);
        }
        s.set_color(1.0f, 1.0f, 1.0f, alpha);
        s.draw_text(f, x + width / 2 - (tw / 2), y + 2, title);

        /* draw screws */
        if (screws) {
            s.draw_icon(screw1, get_client_x() + 3, get_client_y() + 3);
            s.draw_icon(screw2, get_client_x() + 3, get_y() + get_height() - 3 - 8);
            s.draw_icon(screw3, get_x() + get_width() - 3 - 8, get_client_y() + 3);
            s.draw_icon(screw4, get_x() + get_width() - 3 - 8, get_y() + get_height() - 3 - 8);
        }

        /* done */
        s.reset_color();
    }
}

void GuiWindow::prepare() {
    mouse_is_down = false;

    window_title_height = gui.get_font()->get_font_height() + 2;

    char buffer[32];
    sprintf(buffer, "screw%d", rand() % 2 + 1);
    screw1 = gui.get_resources().get_icon(buffer);

    sprintf(buffer, "screw%d", rand() % 2 + 1);
    screw2 = gui.get_resources().get_icon(buffer);

    sprintf(buffer, "screw%d", rand() % 2 + 1);
    screw3 = gui.get_resources().get_icon(buffer);

    sprintf(buffer, "screw%d", rand() % 2 + 1);
    screw4 = gui.get_resources().get_icon(buffer);
}

/* ****************************************************** */
/* GuiBox                                                 */
/* ****************************************************** */
GuiBox::GuiBox(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), filled(false),
      bg_r(0.5f), bg_g(0.5f), bg_b(1.0f) { }

GuiBox::GuiBox(Gui& gui, GuiObject *parent, int x, int y, int width, int height)
    : GuiObject(gui, parent, x, y, width, height), filled(false),
      bg_r(0.5f), bg_g(0.5f), bg_b(1.0f) { }

GuiBox::~GuiBox() { }

void GuiBox::set_color(float bg_r, float bg_g, float bg_b) {
    this->bg_r = bg_r;
    this->bg_g = bg_g;
    this->bg_b = bg_b;
}

void GuiBox::set_filled(bool state) {
    filled = state;
}

void GuiBox::paint() {
    int x = get_client_x();
    int y = get_client_y();
    int width = get_width();
    int height = get_height();
    Subsystem& s = gui.get_subsystem();

    /* set alpha */
    float alpha = gui.get_alpha(this);

    /* draw box */
    s.set_color(bg_r, bg_g, bg_b, alpha);
    if (filled) {
        s.draw_box(x, y, width, height);
    } else {
        s.draw_box(x, y, width, 1);
        s.draw_box(x, y + height - 1, width, 1);
        s.draw_box(x, y, 1, height);
        s.draw_box(x + width - 1, y, 1, height);
    }

    /* done */
    s.reset_color();
}

/* ****************************************************** */
/* GuiLabel                                               */
/* ****************************************************** */
GuiLabel::GuiLabel(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), follow_alpha(true),
      bg_r(1.0f), bg_g(1.0f), bg_b(1.0f) { }

GuiLabel::GuiLabel(Gui& gui, GuiObject *parent, int x, int y,
    int width, int height, const std::string& caption)
    : GuiObject(gui, parent, x, y, width, height),
      follow_alpha(true), bg_r(1.0f), bg_g(1.0f), bg_b(1.0f), caption(caption) { }

GuiLabel::~GuiLabel() { }

void GuiLabel::set_caption(const std::string& caption) {
    this->caption = caption;
}

const std::string& GuiLabel::get_caption() const {
    return caption;
}

void GuiLabel::set_follow_alpha(bool state) {
    follow_alpha = state;
}

void GuiLabel::set_color(float bg_r, float bg_g, float bg_b) {
    this->bg_r = bg_r;
    this->bg_g = bg_g;
    this->bg_b = bg_b;
}

void GuiLabel::paint() {
    int x = get_client_x();
    int y = get_client_y();
    Subsystem& s = gui.get_subsystem();

    s.set_color(bg_r, bg_g, bg_b, (follow_alpha ? gui.get_alpha(this) : 1.0f));
    s.draw_text(gui.get_font(), x, y, caption);
    s.reset_color();
}

/* ****************************************************** */
/* GuiVirtualButton                                       */
/* ****************************************************** */
GuiVirtualButton::GuiVirtualButton(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), caption(), mouse_is_down(false),
      on_click(0), on_click_data(0), align(AlignmentCenter),
      offset_x(0), offset_y(0) { }

GuiVirtualButton::GuiVirtualButton(Gui& gui, GuiObject *parent, int x, int y,
    int width, int height, const std::string& caption,
    OnClick on_click, void *on_click_data)
    : GuiObject(gui, parent, x, y, width, height),
      caption(caption), mouse_is_down(false),
      on_click(on_click), on_click_data(on_click_data), align(AlignmentCenter),
      offset_x(0), offset_y(0) { }

GuiVirtualButton::~GuiVirtualButton() { }

void GuiVirtualButton::set_caption(const std::string& caption) {
    this->caption = caption;
}

const std::string& GuiVirtualButton::get_caption() const {
    return caption;
}

GuiVirtualButton::OnClick GuiVirtualButton::get_on_click_func() {
    return on_click;
}

void *GuiVirtualButton::get_on_click_data() {
    return on_click_data;
}

void GuiVirtualButton::set_alignment(Alignment align) {
    this->align = align;
}

void GuiVirtualButton::set_offset_x(int x) {
    offset_x = x;
}

void GuiVirtualButton::set_offset_y(int y) {
    offset_y = y;
}

bool GuiVirtualButton::can_have_focus() const {
    return true;
}

bool GuiVirtualButton::can_have_mouse_events() const {
    return true;
}

bool GuiVirtualButton::mousedown(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = true;
        mouse_is_in_button = true;
    }

    return true;
}

bool GuiVirtualButton::mousemove(int x, int y) {
    int cx = GuiObject::get_client_x();
    int cy = GuiObject::get_client_y();
    int w = get_width();
    int h = get_height();

    mouse_is_in_button = (x >= cx && x < cx + w && y >= cy && y < cy + h);

    return true;
}

bool GuiVirtualButton::mouseup(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = false;
        if (mouse_is_in_button) {
            if (on_click) {
                on_click(this, on_click_data);
            }
        }
    }

    return true;
}

int GuiVirtualButton::get_client_x() const {
    int ofs = (mouse_is_down && mouse_is_in_button ? 1 : 0);
    return GuiObject::get_client_x() + ofs;
}

int GuiVirtualButton::get_client_y() const {
    int ofs = (mouse_is_down && mouse_is_in_button ? 1 : 0);
    return GuiObject::get_client_y() + ofs;
}

/* ****************************************************** */
/* GuiButton                                              */
/* ****************************************************** */
GuiButton::GuiButton(Gui& gui, GuiObject *parent)
    : GuiVirtualButton(gui, parent), bolts(true),
      text_r(1.0f), text_g(1.0f), text_b(1.0f)
{
    prepare();
}

GuiButton::GuiButton(Gui& gui, GuiObject *parent, int x, int y,
    int width, int height, const std::string& caption,
    GuiVirtualButton::OnClick on_click, void *on_click_data)
    : GuiVirtualButton(gui, parent, x, y, width, height, caption, on_click, on_click_data),
      bolts(true), text_r(1.0f), text_g(1.0f), text_b(1.0f)
{
    prepare();
}

GuiButton::~GuiButton() { }

void GuiButton::show_bolts(bool state) {
    bolts = state;
}

void GuiButton::set_color(float r, float g, float b) {
    text_r = r;
    text_g = g;
    text_b = b;
}

void GuiButton::reset_color() {
    text_r = text_g = text_b = 1.0f;
}

void GuiButton::paint() {
    Subsystem& s = gui.get_subsystem();
    Font *f = gui.get_font();
    int x = GuiObject::get_client_x();
    int y = GuiObject::get_client_y();
    int width = get_width();
    int height = get_height();

    /* set alpha */
    float alpha = gui.get_alpha(this);

    /* draw box */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, width, height);

    if (mouse_is_down && mouse_is_in_button) {
        s.set_color(0.1f, 0.1f, 0.2f, alpha);
    } else {
        s.set_color(0.2f, 0.2f, 0.6f, alpha);
    }
    s.draw_box(x + 1, y + 1, width - 2, height - 2);

    /* pressed offset */
    int ofs = (mouse_is_down && mouse_is_in_button ? 1 : 0) + 1;

    /* draw bolts */
    if (bolts) {
        if (mouse_is_down && mouse_is_in_button) {
            s.set_color(0.75f, 0.75f, 0.75f, alpha);
        } else {
            s.set_color(1.0f, 1.0f, 1.0f, alpha);
        }
        s.draw_icon(bolt, x + ofs + 2, y + ofs + 2);
        s.draw_icon(bolt, x + width + ofs - 4 - 4, y + ofs + 2);
        s.draw_icon(bolt, x + ofs + 2, y + height + ofs - 4 - 4);
        s.draw_icon(bolt, x + width + ofs - 4 - 4, y + height + ofs - 4 - 4);
    }

    /* draw caption */
    width -= 2;
    height -= 2;
    int tw = f->get_text_width(caption);
    int th = f->get_font_height() + 2;
    s.set_color(text_r, text_b, text_b, alpha);
    switch (align) {
        case AlignmentCenter:
            s.draw_text(f, x + offset_x + ofs + width / 2 - tw / 2, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;

        case AlignmentLeft:
            s.draw_text(f, x + offset_x + ofs, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;

        case AlignmentRight:
            s.draw_text(f, x + offset_x + ofs + width - tw, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;
    }

    /* done */
    s.reset_color();
}

void GuiButton::prepare() {
    bolt = gui.get_resources().get_icon("bolt");
}


/* ****************************************************** */
/* GuiRoundedButton                                       */
/* ****************************************************** */
GuiRoundedButton::GuiRoundedButton(Gui& gui, GuiObject *parent)
    : GuiVirtualButton(gui, parent),  bolts(true), bolt(0),
    mb_ul(0), mb_ur(0), mb_ll(0), mb_lr(0),
    mb_ul_pressed(0), mb_ur_pressed(0), mb_ll_pressed(0), mb_lr_pressed(0)
{
    prepare();
}

GuiRoundedButton::GuiRoundedButton(Gui& gui, GuiObject *parent, int x, int y,
    int width, int height, const std::string& caption,
    GuiVirtualButton::OnClick on_click, void *on_click_data)
    : GuiVirtualButton(gui, parent, x, y, width, height, caption, on_click, on_click_data),
      bolts(true), bolt(0), mb_ul(0), mb_ur(0), mb_ll(0), mb_lr(0),
      mb_ul_pressed(0), mb_ur_pressed(0), mb_ll_pressed(0), mb_lr_pressed(0)
{
    prepare();
}

GuiRoundedButton::~GuiRoundedButton() { }


void GuiRoundedButton::show_bolts(bool state) {
    bolts = state;
}

void GuiRoundedButton::paint() {
    Subsystem& s = gui.get_subsystem();
    Font *f = gui.get_font();
    int x = GuiObject::get_client_x();
    int y = GuiObject::get_client_y();
    int width = get_width();
    int height = get_height();

    /* set alpha */
    float alpha = gui.get_alpha(this);

    Icon *mb_upper_left;
    Icon *mb_upper_right;
    Icon *mb_lower_left;
    Icon *mb_lower_right;

    if (mouse_is_down && mouse_is_in_button) {
        mb_upper_left = mb_ul_pressed;
        mb_upper_right = mb_ur_pressed;
        mb_lower_left = mb_ll_pressed;
        mb_lower_right = mb_lr_pressed;
    } else {
        mb_upper_left = mb_ul;
        mb_upper_right = mb_ur;
        mb_lower_left = mb_ll;
        mb_lower_right = mb_lr;
    }

    /* draw edges */
    s.set_color(1.0f, 1.0f, 1.0f, alpha);
    TileGraphic *tg_ul = mb_upper_left->get_tile()->get_tilegraphic();
    s.draw_icon(mb_upper_left, x, y);
    TileGraphic *tg_ll = mb_lower_left->get_tile()->get_tilegraphic();
    s.draw_icon(mb_lower_left, x, y + height - tg_ll->get_height());
    TileGraphic *tg_ur = mb_upper_right->get_tile()->get_tilegraphic();
    s.draw_icon(mb_upper_right, x + width - tg_ur->get_width(), y);
    TileGraphic *tg_lr = mb_lower_right->get_tile()->get_tilegraphic();
    s.draw_icon(mb_lower_right, x + width - tg_lr->get_width(), y + height - tg_lr->get_height());
    s.reset_color();

    /* draw border lines */
    int brd_len = width - tg_ul->get_width() - tg_ur->get_width();
    int brd_height = height - tg_ul->get_height() - tg_ll->get_height();
    int brd_x = x + tg_ul->get_width();
    int brd_y = y + tg_ul->get_height();
    s.set_color(0.145f, 0.6f, 0.141f, alpha);
    s.draw_box(brd_x, y, brd_len, 1);
    s.draw_box(brd_x, y + height - 1, brd_len, 1);
    s.draw_box(x, brd_y, 1, brd_height);
    s.draw_box(x + width - 1, brd_y, 1, brd_height);
    s.reset_color();

    /* fill gaps */
    if (mouse_is_down && mouse_is_in_button) {
        s.set_color(0.031f, 0.2f, 0.004f, alpha);
    } else {
        s.set_color(0.05f, 0.33f, 0.008f, alpha);
    }
    s.draw_box(x + 1, y + tg_ul->get_height(), width - 2, brd_height);
    s.draw_box(x + tg_ul->get_width(), y + 1, brd_len, tg_ul->get_height() - 1);
    s.draw_box(x + tg_ul->get_width(), y + height - tg_ll->get_height(), brd_len, tg_ul->get_height() - 1);
    s.reset_color();

    /* pressed offset */
    int ofs = (mouse_is_down && mouse_is_in_button ? 1 : 0) + 1;

    /* draw bolts */
    if (bolts) {
        if (mouse_is_down && mouse_is_in_button) {
            s.set_color(0.75f, 0.75f, 0.75f, alpha);
        } else {
            s.set_color(1.0f, 1.0f, 1.0f, alpha);
        }
        s.draw_icon(bolt, x + ofs + 4, y + ofs + 4);
        s.draw_icon(bolt, x + width + ofs - 6 - 4, y + ofs + 4);
        s.draw_icon(bolt, x + ofs + 4, y + height + ofs - 6 - 4);
        s.draw_icon(bolt, x + width + ofs - 6 - 4, y + height + ofs - 6 - 4);
    }

    /* draw caption */
    width -= 2;
    int tw = f->get_text_width(caption);
    int th = f->get_font_height() + 2;
    s.set_color(1.0f, 1.0f, 1.0f, alpha);
    switch (align) {
        case AlignmentCenter:
            s.draw_text(f, x + offset_x + ofs + width / 2 - tw / 2, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;

        case AlignmentLeft:
            s.draw_text(f, x + offset_x + ofs, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;

        case AlignmentRight:
            s.draw_text(f, x + offset_x + ofs + width - tw, y + offset_y + ofs + height / 2 - th / 2, caption);
            break;
    }

    /* done */
    s.reset_color();
}

void GuiRoundedButton::prepare() {
    bolt = gui.get_resources().get_icon("bolt");
    mb_ul = gui.get_resources().get_icon("mb_ul");
    mb_ur = gui.get_resources().get_icon("mb_ur");
    mb_ll = gui.get_resources().get_icon("mb_ll");
    mb_lr = gui.get_resources().get_icon("mb_lr");
    mb_ul_pressed = gui.get_resources().get_icon("mb_ul_pressed");
    mb_ur_pressed = gui.get_resources().get_icon("mb_ur_pressed");
    mb_ll_pressed = gui.get_resources().get_icon("mb_ll_pressed");
    mb_lr_pressed = gui.get_resources().get_icon("mb_lr_pressed");
}

/* ****************************************************** */
/* GuiCheckbox                                            */
/* ****************************************************** */
GuiCheckbox::GuiCheckbox(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), caption(), state(false),
      mouse_is_down(false), mouse_is_in_button(false),
      on_click(0), on_click_data(0), style(CheckBoxStyleQuad)
{
    load_icons();
}

GuiCheckbox::GuiCheckbox(Gui& gui, GuiObject *parent, int x, int y,
    const std::string& caption, bool state, OnClick on_click, void *on_click_data)
    : GuiObject(gui, parent, x, y, gui.get_font()->get_text_width(caption) + 9 + 5,
      gui.get_font()->get_font_height()), caption(caption),
      state(state), mouse_is_down(false), mouse_is_in_button(false),
      on_click(on_click), on_click_data(on_click_data), style(CheckBoxStyleQuad)
{
    load_icons();
}

GuiCheckbox::~GuiCheckbox() { }

void GuiCheckbox::set_caption(const std::string& caption) {
    this->caption = caption;
}

const std::string& GuiCheckbox::get_caption() const {
    return caption;
}

void GuiCheckbox::set_style(CheckBoxStyle style) {
    this->style = style;
    load_icons();
}

void GuiCheckbox::set_state(bool state) {
    this->state = state;
}

bool GuiCheckbox::get_state() const {
    return state;
}

bool GuiCheckbox::can_have_focus() const {
    return true;
}

bool GuiCheckbox::can_have_mouse_events() const {
    return true;
}

bool GuiCheckbox::mousedown(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = true;
        mouse_is_in_button = true;
    }

    return true;
}

bool GuiCheckbox::mousemove(int x, int y) {
    int cx = get_client_x();
    int cy = get_client_y();
    int w = get_width();
    int h = get_height();

    mouse_is_in_button = (x >= cx && x < cx + w && y >= cy && y < cy + h);

    return true;
}

bool GuiCheckbox::mouseup(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = false;
        if (mouse_is_in_button) {
            state = !state;
            if (on_click) {
                on_click(this, on_click_data, state);
            }
        }
    }

    return true;
}

void GuiCheckbox::paint() {
    int x = get_client_x();
    int y = get_client_y();

    Icon *icon = 0;
    if (mouse_is_in_button && mouse_is_down) {
        icon = half;
    } else if (state) {
        icon = on;
    } else {
        icon = off;
    }
    int iw = 9;
    int ih = 9;

    Subsystem& s = gui.get_subsystem();

    s.set_color(1.0f, 1.0f, 1.0f, gui.get_alpha(this));
    s.draw_icon(icon, x, y);

    Font *f = gui.get_font();
    x += iw + 5;
    y += (ih / 2 - (f->get_font_height() - 2) / 2);
    s.draw_text(f, x, y, caption);
    s.reset_color();
}

void GuiCheckbox::load_icons() {
    switch (style) {
        case CheckBoxStyleQuad:
            on = gui.get_resources().get_icon("checkbox_checked");
            off = gui.get_resources().get_icon("checkbox_unchecked");
            half = gui.get_resources().get_icon("checkbox_half");
            break;

        case CheckBoxStyleCircle:
            on = gui.get_resources().get_icon("option_on");
            off = gui.get_resources().get_icon("option_off");
            half = gui.get_resources().get_icon("option_half");
            break;
    }
}

/* ****************************************************** */
/* GuiTextbox                                             */
/* ****************************************************** */
GuiTextbox::GuiTextbox(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), start_position(0), caret_position(0),
      text(), locked(false), type(TypeNormal), mouse_is_down(false) { }

GuiTextbox::GuiTextbox(Gui& gui, GuiObject *parent, int x, int y,
    int width, const std::string& text)
    : GuiObject(gui, parent, x, y, width, gui.get_font()->get_font_height() + 2),
      start_position(0), caret_position(utf8_strlen(text.c_str())),
      text(text), locked(false), type(TypeNormal), mouse_is_down(false)
{
    recalc();
}

GuiTextbox::~GuiTextbox() { }

void GuiTextbox::set_type(Type type) {
    this->type = type;
}

GuiTextbox::Type GuiTextbox::get_type() const {
    return type;
}

void GuiTextbox::set_text(const std::string& text) {
    this->text = text;
    caret_position = utf8_strlen(text.c_str());
}

const std::string& GuiTextbox::get_text() const {
    return text;
}

void GuiTextbox::set_caret_position(int pos) {
    int sz = static_cast<int>(utf8_strlen(text.c_str()));
    caret_position = pos;
    if (caret_position > sz) {
        caret_position = sz;
    }
    recalc();
}

int GuiTextbox::get_caret_position() const {
    return caret_position;
}

void GuiTextbox::set_locked(bool state) {
    locked = state;
}

bool GuiTextbox::get_locked() const {
    return locked;
}

bool GuiTextbox::can_have_focus() const {
    return true;
}

bool GuiTextbox::can_have_mouse_events() const {
    return true;
}

bool GuiTextbox::mousedown(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = true;
        set_cursor_pos_from_mouse(x);
    }

    return true;
}

bool GuiTextbox::mousemove(int x, int y) {
    set_cursor_pos_from_mouse(x);

    return true;
}

bool GuiTextbox::mouseup(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = false;
    }

    return true;
}

bool GuiTextbox::keyup(int keycode) {
    return true;
}

bool GuiTextbox::keypress(InputData& input) {
    if (!locked) {
        switch (input.data_type) {
            case InputData::InputDataTypeKeyDown:
            {
                switch (input.key_type) {
                    case InputData::InputKeyTypeHome:
                    {
                        caret_position = 0;
                        recalc();
                        break;
                    }

                    case InputData::InputKeyTypeEnd:
                    {
                        caret_position = static_cast<int>(utf8_strlen(text.c_str()));
                        recalc();
                        break;
                    }

                    case InputData::InputKeyTypeLeft:
                    {
                        if (caret_position) {
                            caret_position--;
                            recalc();
                        }
                        break;
                    }

                    case InputData::InputKeyTypeRight:
                    {
                        if (caret_position < static_cast<int>(utf8_strlen(text.c_str()))) {
                            caret_position++;
                            recalc();
                        }
                        break;
                    }

                    case InputData::InputKeyTypeDelete:
                    {
                        const char *tptr = text.c_str();
                        if (static_cast<int>(utf8_strlen(tptr)) > caret_position) {
                            int pos = static_cast<int>(utf8_real_char_pos(tptr, caret_position));
                            int seqlen = static_cast<int>(utf8_sequence_len(&tptr[pos]));
                            text.erase(pos, seqlen);
                            recalc();
                        }
                        break;
                    }

                    case InputData::InputKeyTypeBackspace:
                    {
                        if (caret_position) {
                            caret_position--;
                            const char *tptr = text.c_str();
                            int pos = static_cast<int>(utf8_real_char_pos(tptr, caret_position));
                            int seqlen = static_cast<int>(utf8_sequence_len(&tptr[pos]));
                            text.erase(pos, seqlen);
                        }
                        break;
                    }

                    default:
                        break;
                }
                break;
            }

            case InputData::InputDataTypeText:
            {
                bool ok = true;
                const char *p = reinterpret_cast<const char *>(input.text);
                if (p) {
                    if (type == TypeInteger) {
                        if (strlen(p) != 1 || *p < '0' || *p > '9') {
                            ok = false;
                        }
                    }

                    if (ok) {
                        int pos = static_cast<int>(utf8_real_char_pos(text.c_str(), caret_position));
                        text.insert(pos, p);
                        caret_position += utf8_strlen(p);
                        recalc();
                    }
                }
                break;
            }

            default:
                break;
        }
    }

    return true;
}

void GuiTextbox::recalc() {
    if (caret_position < start_position) {
        start_position = caret_position;
    }

    const char *s = text.c_str();

    if (caret_position > start_position) {
        Font *f = gui.get_font();
        const char *tptr = text.c_str();
        int cw = 0;
        int pos = start_position;
        while (pos < caret_position) {
            const Font::Character *chr = f->get_character(utf8_real_char_ptr(tptr, pos));
            cw += chr->advance;
            pos++;
        }

        int w = get_width() - 2 - 2;

        if (cw >= w) {
            int pos = caret_position;
            cw = 0;
            while (pos) {
                pos--;
                cw += f->get_char_width(utf8_real_char_ptr(tptr, pos));
                if (cw >= w) {
                    start_position = pos + 2;
                    break;
                }
            }
        }
    }
}

void GuiTextbox::set_cursor_pos_from_mouse(int x) {
    if (mouse_is_down) {
        Font *f = gui.get_font();
        int cx = get_client_x();
        int rp = cx + get_width() - 2 - 2;
        int pos = start_position;
        int caret = caret_position;
        const char *tptr = text.c_str();
        int sz = utf8_strlen(tptr);
        int curx = cx + 2;
        while (pos < sz + 1 && curx <= rp) {
            bool ok = false;
            int advance = 0;
            if (pos < sz) {
                const char *str = utf8_real_char_ptr(tptr, pos);
                const Font::Character *chr = f->get_character(str);
                advance = chr->advance;
                if (x >= curx && x < curx + chr->advance) {
                    ok = true;
                }
            } else {
                if (x >= curx) {
                    ok = true;
                }
            }
            if (ok) {
                if (caret_position != pos) {
                    caret_position = pos;
                    gui.reset_blinker();
                }
                break;
            }
            curx += advance;
            pos++;
        }
        recalc();
    }
}

void GuiTextbox::paint() {
    float alpha = gui.get_alpha(this);
    Subsystem& s = gui.get_subsystem();
    Font *f = gui.get_font();
    int fh = f->get_font_height();
    int x = get_client_x();
    int y = get_client_y();
    int w = get_width();
    int h = get_height();
    int rp = x + w - 2;

    /* draw box */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, w, h);
    s.set_color(0.0f, 0.0f, 0.0f, alpha);
    s.draw_box(x + 1, y + 1, w - 2, h - 2);
    s.set_color(1.0f, 1.0f, 1.0f, alpha);

    /* draw text */
    int pos = start_position;
    int caret = caret_position;
    const char *tptr = text.c_str();
    int sz = utf8_strlen(tptr);
    y++;
    x += 2;
    int cp = x;
    bool caret_drawn = false;
    int caretx = cp;
    const char *p = text.c_str();
    while (pos < sz) {
        const char *str = (type == TypeHidden ? "*" : utf8_real_char_ptr(tptr, pos));
        const Font::Character *chr = f->get_character(str);
        if (cp + chr->advance >= rp) {
            break;
        }
        if (pos == caret && !caret_drawn) {
            caret_drawn = true;
            caretx = cp;
        }
        cp = s.draw_char(f, cp, y, str);
        pos++;
    }
    if (!caret_drawn) {
        caretx = cp;
    }
    if (gui.has_focus(this) && !locked) {
        if (gui.get_blink_on()) {
            s.draw_box(caretx, y + 1, 1, fh - 2);
        }
    }

    /* done */
    s.reset_color();
}

/* ****************************************************** */
/* GuiPicture                                             */
/* ****************************************************** */
GuiPicture::GuiPicture(Gui& gui, GuiObject *parent) : GuiObject(gui, parent) { }

GuiPicture::GuiPicture(Gui& gui, GuiObject *parent, int x, int y, TileGraphic *graphic)
    : GuiObject(gui, parent, x, y, (graphic ? graphic->get_width() : 0), (graphic ? graphic->get_height() : 0)),
      graphic(graphic) { }

GuiPicture::~GuiPicture() { }

void GuiPicture::set_picture(TileGraphic *graphic) {
    this->graphic = graphic;
    if (graphic) {
        set_width(graphic->get_width());
        set_height(graphic->get_height());
    } else {
        set_width(0);
        set_height(0);
    }
}

void GuiPicture::paint() {
    if (graphic) {
        Subsystem& s = gui.get_subsystem();
        s.set_color(1.0f, 1.0f, 1.0f, gui.get_alpha(this));
        gui.get_subsystem().draw_tilegraphic(graphic, get_client_x(), get_client_y());
        s.reset_color();
    }
}

/* ****************************************************** */
/* GuiFrame                                               */
/* ****************************************************** */
GuiFrame::GuiFrame(Gui& gui, GuiObject *parent) : GuiObject(gui, parent) { }

GuiFrame::GuiFrame(Gui& gui, GuiObject *parent, int x, int y, int width, int height)
    : GuiObject(gui, parent, x, y, width, height) { }

GuiFrame::~GuiFrame() { }

void GuiFrame::paint() { }

/* ****************************************************** */
/* GuiTab                                                 */
/* ****************************************************** */
GuiTab::GuiTab(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), current_tab(0),
      tab_button_height(gui.get_font()->get_font_height() * 2),
      current_button_x(0) { }

GuiTab::GuiTab(Gui& gui, GuiObject *parent, int x, int y, int width, int height)
    : GuiObject(gui, parent, x, y, width, height), current_tab(0),
      tab_button_height(gui.get_font()->get_font_height() * 2),
      current_button_x(0) { }

GuiTab::~GuiTab() {
    for (Tabs::iterator it = tabs.begin(); it != tabs.end(); it++) {
        delete *it;
    }
}

GuiFrame *GuiTab::create_tab(const std::string& name) {
    Font *f = gui.get_font();
    int x = current_button_x;
    int y = 0;
    int width = f->get_text_width(name) + 24; //16;
    Tab *tab = new Tab;
    tab->owner = this;
    tab->tab_number = static_cast<int>(tabs.size());
    tab->button = gui.create_button(this, x, y, width, tab_button_height, name, static_tab_clicked, tab);
    tab->tab = gui.create_frame(this, 10, tab_button_height + 10, get_width() - 20, get_height() - 20 - tab_button_height);
    tab->tab->set_visible(false);
    tabs.push_back(tab);

    if (!current_tab) {
        current_tab = tab;
        tab->tab->set_visible(true);
        tab->tab->set_focus_on_first_child();
    }

    current_button_x += width - 1;

    return tab->tab;
}

void GuiTab::select_tab(int index) {
    int sz = static_cast<int>(tabs.size());
    if (index >= 0 && index < sz) {
        for (Tabs::iterator it = tabs.begin(); it != tabs.end(); it++) {
            Tab *tab = *it;
            tab->tab->set_visible(false);
        }
        Tab *tab = tabs[index];
        tab->tab->set_visible(true);
        tab->tab->set_focus_on_first_child();
    }
}

void GuiTab::static_tab_clicked(GuiVirtualButton *sender, void *data) {
    Tab *tab = reinterpret_cast<Tab *>(data);
    tab->owner->select_tab(tab->tab_number);
}

void GuiTab::paint() {
    int x = get_client_x();
    int y = get_client_y() + tab_button_height - 1;
    int width = get_width();
    int height = get_height() - tab_button_height;
    Subsystem& s = gui.get_subsystem();

    /* set alpha */
    float alpha = gui.get_alpha(this);

    /* draw window */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, width, height);

    s.set_color(0.0f, 0.0f, 0.35f, alpha);
    s.draw_box(x + 1, y + 1, width - 2, height - 2);
    s.reset_color();
}

/* ****************************************************** */
/* GuiVirtualScroll                                       */
/* ****************************************************** */
GuiVirtualScroll::GuiVirtualScroll(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), on_value_changed(0), on_value_changed_data(0),
      min_value(0), max_value(100), current_value(0) { }

GuiVirtualScroll::GuiVirtualScroll(Gui& gui, GuiObject *parent, int x, int y, int width, int height,
    int min_value, int max_value, int initial_value, ValueChanged on_value_changed,
    void *on_value_changed_data)
    : GuiObject(gui, parent, x, y, width, height), on_value_changed(on_value_changed),
      on_value_changed_data(on_value_changed_data), min_value(min_value),
      max_value(max_value), current_value(initial_value)
{
    if (min_value > max_value) {
        std::swap(min_value, max_value);
    }
    recalc();
}

GuiVirtualScroll::~GuiVirtualScroll() { }

void GuiVirtualScroll::set_value(int value) {
    int old_value = current_value;
    current_value = value;
    recalc();
    if (current_value != old_value) {
        if (on_value_changed) {
            on_value_changed(this, on_value_changed_data, current_value);
        }
    }
}

int GuiVirtualScroll::get_value() const {
    return current_value;
}

void GuiVirtualScroll::set_min_value(int value) {
    if (value <= max_value) {
        min_value = value;
        recalc();
    }
}

int GuiVirtualScroll::get_min_value() const {
    return min_value;
}

void GuiVirtualScroll::set_max_value(int value) {
    if (value >= min_value) {
        max_value = value;
        recalc();
    }
}

int GuiVirtualScroll::get_max_value() const {
    return max_value;
}

bool GuiVirtualScroll::can_have_focus() const {
    return true;
}

bool GuiVirtualScroll::can_have_mouse_events() const {
    return true;
}

void GuiVirtualScroll::recalc() {
    if (current_value < min_value) {
        current_value = min_value;
    }

    if (current_value > max_value) {
        current_value = max_value;
    }
}


void GuiVirtualScroll::static_down_button_clicked(GuiVirtualButton *sender, void *data) {
    GuiVirtualScroll *obj = reinterpret_cast<GuiVirtualScroll *>(data);
    obj->set_value(obj->get_value() - 1);
}

void GuiVirtualScroll::static_up_button_clicked(GuiVirtualButton *sender, void *data) {
    GuiVirtualScroll *obj = reinterpret_cast<GuiVirtualScroll *>(data);
    obj->set_value(obj->get_value() + 1);
}

/* ****************************************************** */
/* GuiHScroll                                             */
/* ****************************************************** */
GuiHScroll::GuiHScroll(Gui& gui, GuiObject *parent) : GuiVirtualScroll(gui, parent),
    left_button(0), right_button(0), left_arrow(0), right_arrow(0)
{
    prepare();
}

GuiHScroll::GuiHScroll(Gui& gui, GuiObject *parent, int x, int y, int width,
    int min_value, int max_value, int initial_value,
    GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data)
    : GuiVirtualScroll(gui, parent, x, y, width, Size, min_value, max_value, initial_value,
      on_value_changed, on_value_changed_data)
{
    prepare();

    left_button = gui.create_button(this, 0, 0, Size, Size, "", static_down_button_clicked, this);
    left_button->show_bolts(false);
    gui.create_picture(left_button, 1, 1, left_arrow->get_tile()->get_tilegraphic());

    right_button = gui.create_button(this, width - Size, 0, Size, Size, "", static_up_button_clicked, this);
    right_button->show_bolts(false);
    gui.create_picture(right_button, 1, 1, right_arrow->get_tile()->get_tilegraphic());
}

GuiHScroll::~GuiHScroll() { }

bool GuiHScroll::mousedown(int button, int x, int y) {
    if (button == 0) {
        int blocksize = Size;
        calc_blockpos();
        if (x >= blockpos && x < blockpos + blocksize) {
            origin_x = x - blockpos;
            moving = true;
        }
    }

    return true;
}

bool GuiHScroll::mousemove(int x, int y) {
    if (moving) {
        x -= origin_x + get_client_x() + left_button->get_width();
        int range = max_value - min_value;
        int blocksize = Size;
        int length = get_width() - left_button->get_width() - right_button->get_width() - blocksize;
        int value = 0;
        if (length) {
            value = x * range / length;
        }
        value += min_value;
        set_value(value);
    }

    return true;
}

bool GuiHScroll::mouseup(int button, int x, int y) {
    if (button == 0) {
        moving = false;
    }

    return true;
}

void GuiHScroll::paint() {
    Subsystem& s = gui.get_subsystem();
    float alpha = gui.get_alpha(this);
    int x = get_client_x();
    int y = get_client_y();
    int width = get_width();
    int height = get_height();
    int block_size = Size;

    /* draw window */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, width, height);

    s.set_color(0.0f, 0.0f, 0.35f, alpha);
    s.draw_box(x + 1, y + 1, width - 2, height - 2);

    /* draw block */
    calc_blockpos();
    if (draw_block) {
        s.set_color(0.5f, 0.5f, 1.0f, alpha);
        s.draw_box(blockpos, y, block_size, height);

        s.set_color(0.2f, 0.2f, 0.6f, alpha);
        s.draw_box(blockpos + 1, y + 1, block_size - 2, height - 2);
    }

    /* done */
    s.reset_color();
}

void GuiHScroll::prepare() {
    left_arrow = gui.get_resources().get_icon("left_scroll");
    right_arrow = gui.get_resources().get_icon("right_scroll");
    moving = false;
}

void GuiHScroll::calc_blockpos() {
    int x = get_client_x();
    int block_size = Size;
    int lbw = left_button->get_width() - 1;
    int rbw = right_button->get_width() - 1;
    int width = get_width() - lbw - rbw - block_size;
    int range = max_value - min_value;
    draw_block = (range > 0);
    if (range == 0) {
        range = 1;
    }
    int value = current_value - min_value;

    blockpos = x + lbw + (range ? width * value / range : 0);
}

/* ****************************************************** */
/* GuiVScroll                                             */
/* ****************************************************** */
GuiVScroll::GuiVScroll(Gui& gui, GuiObject *parent) : GuiVirtualScroll(gui, parent),
    up_button(0), down_button(0), up_arrow(0), down_arrow(0)
{
    prepare();
}

GuiVScroll::GuiVScroll(Gui& gui, GuiObject *parent, int x, int y, int height,
    int min_value, int max_value, int initial_value,
    GuiVirtualScroll::ValueChanged on_value_changed, void *on_value_changed_data)
    : GuiVirtualScroll(gui, parent, x, y, Size, height, min_value, max_value,
      initial_value, on_value_changed, on_value_changed_data)
{
    prepare();

    up_button = gui.create_button(this, 0, 0, Size, Size, "", static_down_button_clicked, this);
    up_button->show_bolts(false);
    gui.create_picture(up_button, 1, 1, up_arrow->get_tile()->get_tilegraphic());

    down_button = gui.create_button(this, 0, height - Size, Size, Size, "", static_up_button_clicked, this);
    down_button->show_bolts(false);
    gui.create_picture(down_button, 1, 1, down_arrow->get_tile()->get_tilegraphic());
}

GuiVScroll::~GuiVScroll() { }

bool GuiVScroll::mousedown(int button, int x, int y) {
    if (button == 0) {
        int blocksize = Size;
        calc_blockpos();
        if (y >= blockpos && y < blockpos + blocksize) {
            origin_y = y - blockpos;
            moving = true;
        }
    }

    return true;
}

bool GuiVScroll::mousemove(int x, int y) {
    if (moving) {
        y -= origin_y + get_client_y() + up_button->get_height();
        int range = max_value - min_value;
        int blocksize = Size;
        int length = get_height() - up_button->get_height() - down_button->get_height() - blocksize;
        int value = 0;
        if (length) {
            value = y * range / length;
        }
        value += min_value;
        set_value(value);
    }

    return true;
}

bool GuiVScroll::mouseup(int button, int x, int y) {
    if (button == 0) {
        moving = false;
    }

    return true;
}

void GuiVScroll::paint() {
    Subsystem& s = gui.get_subsystem();
    float alpha = gui.get_alpha(this);
    int x = get_client_x();
    int y = get_client_y();
    int width = get_width();
    int height = get_height();
    int block_size = Size;

    /* draw window */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, width, height);

    s.set_color(0.0f, 0.0f, 0.35f, alpha);
    s.draw_box(x + 1, y + 1, width - 2, height - 2);

    /* draw block */
    calc_blockpos();
    if (draw_block) {
        s.set_color(0.5f, 0.5f, 1.0f, alpha);
        s.draw_box(x, blockpos, width, block_size);

        s.set_color(0.2f, 0.2f, 0.6f, alpha);
        s.draw_box(x + 1, blockpos + 1, width - 2, block_size - 2);
    }

    /* done */
    s.reset_color();
}

void GuiVScroll::prepare() {
    up_arrow = gui.get_resources().get_icon("up_scroll");
    down_arrow = gui.get_resources().get_icon("down_scroll");
    moving = false;
}

void GuiVScroll::calc_blockpos() {
    int y = get_client_y();
    int block_size = Size;
    int ubh = up_button->get_height() - 1;
    int dbh = down_button->get_height() - 1;
    int height = get_height() - ubh - dbh - block_size;
    int range = max_value - min_value;
    draw_block = (range > 0);
    if (range == 0) {
        range = 1;
    }
    int value = current_value - min_value;

    int block_y = (range ? height * value / range : 0);

    blockpos = y + ubh + block_y;
}

/* ****************************************************** */
/* GuiListboxEntry                                        */
/* ****************************************************** */
GuiListboxEntry::GuiListboxEntry(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent) { }

GuiListboxEntry::GuiListboxEntry(Gui& gui, GuiObject *parent,
    const std::string& text)
    : GuiObject(gui, parent, 0, 0, 0, 0)
{
    add_column(text, 0);
}

GuiListboxEntry::GuiListboxEntry(Gui& gui, GuiObject *parent,
    const std::string& text, int width)
    : GuiObject(gui, parent, 0, 0, 0, 0)
{
    add_column(text, width);
}

GuiListboxEntry::GuiListboxEntry(Gui& gui, GuiObject *parent,
    const GuiListboxEntry::Column& column)
    : GuiObject(gui, parent, 0, 0, 0, 0)
{
    add_column(column);
}

GuiListboxEntry::~GuiListboxEntry() { }

GuiListboxEntry::Columns& GuiListboxEntry::get_columns() {
    return columns;
}

void GuiListboxEntry::add_column(const Column& column) {
    columns.push_back(column);
}

void GuiListboxEntry::add_column(const std::string& text, int width) {
    add_column(Column(text, width));
}

void GuiListboxEntry::draw(int x, int y, int width, DrawType draw_type) {
    Subsystem& s = gui.get_subsystem();
    float alpha = gui.get_alpha(this);
    Font *f = gui.get_font();

    if (draw_type == DrawTypeSelected) {
        int th = f->get_font_height();

        s.set_color(0.5f, 0.5f, 0.5f, alpha);
        s.draw_box(x, y, width, th);
    } else if (draw_type == DrawTypeTitle) {
        int th = f->get_font_height();

        s.set_color(0.5f, 0.5f, 1.0f, alpha);
        s.draw_box(x, y, width, th);
    }

    s.set_color(1.0f, 1.0f, 1.0f, alpha);

    size_t sz = columns.size();
    int width0 = width;
    for (size_t i = 1; i < sz; i++) {
        width0 -= columns[i].width + Spc;
    }
    draw_column(columns[0], x, y, width0);
    x += width0 + Spc;
    for (size_t i = 1; i < sz; i++) {
        int w = columns[i].width;
        draw_column(columns[i], x, y, w);
        x += columns[i].width + Spc;
    }

    s.reset_color();
}

void GuiListboxEntry::paint() { }

void GuiListboxEntry::draw_column(const Column& column, int x, int y, int max_width) {
    Subsystem& s = gui.get_subsystem();
    s.draw_clipped_text(gui.get_font(), x, y, max_width, column.text);
}

/* ****************************************************** */
/* GuiListbox                                             */
/* ****************************************************** */
GuiListbox::GuiListbox(Gui& gui, GuiObject *parent)
    : GuiObject(gui, parent), on_item_selected(0), on_item_selected_data(0),
      on_title_clicked(0), on_title_clicked_data(0)
{
    setup("");
}

GuiListbox::GuiListbox(Gui& gui, GuiObject *parent, int x, int y, int width,
    int height, const std::string& title, OnItemSelected on_item_selected,
    void *on_item_selected_data)
    : GuiObject(gui, parent, x, y, width, height),
      on_item_selected(on_item_selected),
      on_item_selected_data(on_item_selected_data),
      on_title_clicked(0), on_title_clicked_data(0)
{
    setup(title);
}

GuiListbox::~GuiListbox() { }

void GuiListbox::set_on_title_clicked(OnTitleClicked on_title_clicked,
    void *on_title_clicked_data)
{
    this->on_title_clicked = on_title_clicked;
    this->on_title_clicked_data = on_title_clicked_data;
}

void GuiListbox::set_top_index(int index) {
    if (index >= 0) {
        int max_entries = sb->get_max_value();
        if (index <= max_entries) {
            start_index = index;
        } else {
            start_index = max_entries;
        }
        sb->set_value(start_index);
    }
}

int GuiListbox::get_top_index() const {
    return start_index;
}

void GuiListbox::set_selected_index(int index) {
    int old_index = selected_index;
    int sz = static_cast<int>(entries.size());
    if (index < 0) {
        index = -1;
    }
    if (index >= sz) {
        index = sz - 1;
    }
    selected_index = index;

    if (selected_index != old_index && on_item_selected) {
        if (index >= 0) {
            on_item_selected(this, on_item_selected_data, selected_index);
        }
    }
}

int GuiListbox::get_entry_count() const {
    return static_cast<int>(entries.size());
}

int GuiListbox::get_selected_index() const {
    return selected_index;
}

GuiListboxEntry *GuiListbox::get_entry(int index) {
    int sz = static_cast<int>(entries.size());
    if (index >= 0 && index < sz) {
        return entries[index];
    }

    return 0;
}

GuiListboxEntry *GuiListbox::add_entry(const std::string& text) {
    return add_entry(GuiListboxEntry::Column(text, 0));
}

GuiListboxEntry *GuiListbox::add_entry(const std::string& text, int width) {
    return add_entry(GuiListboxEntry::Column(text, width));
}

GuiListboxEntry *GuiListbox::add_entry(const GuiListboxEntry::Column& column) {
    GuiListboxEntry *entry = new GuiListboxEntry(gui, this, column);
    entries.push_back(entry);
    recalc();

    return entry;
}

GuiListboxEntry *GuiListbox::get_title_bar() {
    return title_bar;
}

void GuiListbox::show_title_bar(bool state) {
    title_bar_visible = state;
}

void GuiListbox::remove_entry(int index) {
    int sz = static_cast<int>(entries.size());
    if (index >=0 && index < sz) {
        remove_object(entries[index]);
        entries.erase(entries.begin() + index);
    }
}

void GuiListbox::clear() {
    size_t sz = entries.size();
    for (size_t i = 0; i < sz; i++) {
        remove_object(entries[i]);
    }
    entries.clear();
}

bool GuiListbox::can_have_focus() const {
    return true;
}

bool GuiListbox::can_have_mouse_events() const {
    return true;
}

bool GuiListbox::mousedown(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = true;
        my_down_mousemove(x, y, false);
    }

    return true;
}

bool GuiListbox::mousemove(int x, int y) {
    my_down_mousemove(x, y, true);

    return true;
}

bool GuiListbox::mouseup(int button, int x, int y) {
    if (button == 0) {
        mouse_is_down = false;
    }

    return true;
}

void GuiListbox::paint() {
    Subsystem& s = gui.get_subsystem();
    float alpha = gui.get_alpha(this);
    int x = get_client_x();
    int y = get_client_y();
    int width = get_width();
    int height = get_height();

    /* draw window */
    s.set_color(0.5f, 0.5f, 1.0f, alpha);
    s.draw_box(x, y, width, height);

    s.set_color(0.0f, 0.0f, 0.35f, alpha);
    s.draw_box(x + 1, y + 1, width - 2, height - 2);

    /* draw entries */
    x++;
    y++;
    width -= 2;
    height -= 2;
    int sz = static_cast<int>(entries.size());
    int index = start_index;
    int th = gui.get_font()->get_font_height();
    int toth = 0;
    if (title_bar_visible) {
        title_bar->draw(x, y, width - sb->get_width() + 1, GuiListboxEntry::DrawTypeTitle);
        toth += th;
        y += th;
    }
    while (index < sz) {
        toth += th;
        if (toth > height) {
            break;
        }
        GuiListboxEntry *entry = entries[index];
        entry->draw(x, y, width - sb->get_width() + 1, (index == selected_index ? GuiListboxEntry::DrawTypeSelected : GuiListboxEntry::DrawTypeNormal));
        y += th;
        index++;
    }
}

void GuiListbox::my_down_mousemove(int x, int y, bool from_mousemove) {
    if (mouse_is_down) {
        if (y >= get_client_y() + 1) {
            select_from_mouse(x, y, from_mousemove);
        }
    }
}

void GuiListbox::setup(const std::string& title) {
    sb = gui.create_vscroll(this, get_width() - GuiVirtualScroll::Size, 0, get_height(),
        0, 0, 0, static_scroll_changed, this);

    title_bar = new GuiListboxEntry(gui, this, title);
    title_bar_visible = false;

    start_index = 0;
    selected_index = -1;
    mouse_is_down = false;
    recalc();
}

void GuiListbox::recalc() {
    int max_entries = static_cast<int>(entries.size());
    int fh = gui.get_font()->get_font_height();
    int height = get_height() - 2 - (title_bar_visible ? fh : 0);
    int visible_entries = height / fh;
    max_entries -= visible_entries;
    if (max_entries < 0) {
        max_entries = 0;
    }
    sb->set_max_value(max_entries);
}

void GuiListbox::select_from_mouse(int x, int y, bool from_mousemove) {
    int fh = gui.get_font()->get_font_height();
    int height = get_height() - 2 - (title_bar_visible ? fh : 0);
    y -= get_client_y() + 1 + (title_bar_visible ? fh : 0);
    int index = y / fh;
    int visible_entries = height / fh;

    /* title bar column click? */
    if (title_bar_visible && y >= -fh && y < 0) {
        if (!from_mousemove) {
            GuiListboxEntry::Columns& columns = title_bar->get_columns();
            int width = get_width() - 2 - sb->get_width() + 1;
            int index = columns.size() - 1;
            int col_right = width + (get_client_x() + 1);
            for (GuiListboxEntry::Columns::reverse_iterator it = columns.rbegin();
                it != columns.rend(); it++)
            {
                GuiListboxEntry::Column& column = *it;
                if (x >= col_right - column.width && x <= (col_right + GuiListboxEntry::Spc)) {
                    if (on_title_clicked) {
                        on_title_clicked(this, on_title_clicked_data, index);
                    }
                    return;
                }
                col_right -= (column.width + GuiListboxEntry::Spc);
                index--;
            }
            index = 0;
            if (on_title_clicked) {
                on_title_clicked(this, on_title_clicked_data, index);
            }
        }
        return;
    }

    /* item click? */
    if (index < 0 || index > visible_entries - 1) {
        return;
    }
    set_selected_index(index + start_index);
}

void GuiListbox::static_scroll_changed(GuiVirtualScroll *sender, void *data, int value) {
    (reinterpret_cast<GuiListbox *>(data))->set_top_index(value);
}
