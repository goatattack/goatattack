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

#include "I18N.hpp"

I18N::I18N() throw () : language(language) {
    init();
}

I18N::I18N(Language language) throw () : language(language) {
    init();
}

void I18N::init() {
    const Text *current_language = 0;

    switch (language) {
        case LanguageEnglish:
            current_language = all_texts_english;
            break;

        case LanguageGerman:
            current_language = all_texts_german;
            break;

        case LanguageFrench:
            current_language = all_texts_french;
            break;
    }

    if (current_language) {
        while (current_language->id) {
            texts[current_language->id] = current_language->text;
            current_language++;
        }
    }
}

const char *I18N::get_text(id_t id) const {
    TextMap::const_iterator it = texts.find(id);
    if (it != texts.end()) {
        return it->second;
    }

    return "";
}

void I18N::replace(std::string& s, const char *w, size_t wl, const char *p) {
    std::string::size_type pos = 0;
    while ((pos = s.find(w)) != std::string::npos) {
        s.replace(pos, wl, p);
    }
}

void I18N::replace(std::string& s, const char *w, size_t wl, const std::string& p) {
    replace(s, w, wl, p.c_str());
}

void I18N::replace(std::string& s, const char *w, size_t wl, int p) {
    char buffer[64];
    sprintf(buffer, "%d", p);
    replace(s, w, wl, buffer);
}

void I18N::replace(std::string& s, const char *w, size_t wl, double p) {
    char buffer[64];
    sprintf(buffer, "%f", p);
    replace(s, w, wl, buffer);
}