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

#include "TextMessageSystem.hpp"
#include "Icon.hpp"

TextMessageSystem::TextMessage::TextMessage(Icon& icon, const std::string& player,
    const std::string& text)
    : icon(&icon), icon_width(icon.get_tile()->get_tilegraphic()->get_width()),
      player(player), text(text), duration(0.0), delete_me(false) { }

TextMessageSystem::TextMessage::TextMessage(Icon& icon, const std::string& text)
    : icon(&icon), icon_width(icon.get_tile()->get_tilegraphic()->get_width()),
      text(text), duration(0.0), delete_me(false) { }

TextMessageSystem::TextMessageSystem() { }

TextMessageSystem::~TextMessageSystem() {
    for (TextMessages::iterator it = text_messages.begin();
        it != text_messages.end(); it++)
    {
        delete *it;
    }
}

void TextMessageSystem::add_text_msg(const std::string& player, const std::string& msg) {
    text_messages.push_back(new TextMessage(player, msg));
}

void TextMessageSystem::add_text_msg(Icon& icon, const std::string& player, const std::string& msg) {
    text_messages.push_back(new TextMessage(icon, player, msg));
}

void TextMessageSystem::add_text_msg(const std::string& msg) {
    text_messages.push_back(new TextMessage(msg));
}
