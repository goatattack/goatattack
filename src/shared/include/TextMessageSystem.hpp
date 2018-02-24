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

#ifndef _TEXTMESSAGESYSTEM_HPP_
#define _TEXTMESSAGESYSTEM_HPP_

#include <string>
#include <vector>

class Icon;

class TextMessageSystem {
public:
    TextMessageSystem();
    ~TextMessageSystem();

    void add_text_msg(const std::string& player, const std::string& msg);
    void add_text_msg(Icon& icon, const std::string& player, const std::string& msg);
    void add_text_msg(const std::string& msg);

protected:
    struct TextMessage {
        TextMessage(const std::string& player, const std::string& text)
            : icon(0), icon_width(0), player(player), text(text), duration(0.0),
              delete_me(false) { }

        TextMessage(Icon& icon, const std::string& player, const std::string& text);

        TextMessage(const std::string& text)
            : icon(0), icon_width(0), text(text), duration(0.0), delete_me(false) { }

        TextMessage(Icon& icon, const std::string& text);

        Icon *icon;
        int icon_width;
        std::string player;
        std::string text;
        double duration;
        bool delete_me;
    };

    typedef std::vector<TextMessage *> TextMessages;

    TextMessages text_messages;
};

#endif
