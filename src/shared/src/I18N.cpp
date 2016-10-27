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

const char *I18N::Languages[] = {
    "English",
    "Deutsch",
    "Français",
    "Português",
    0
};

static const int FallbackCounterMax = 5;

I18N::I18N(std::ostream& stream)
    : stream(stream), current_language(all_texts_english), fallback_counter(0)
{
    init(LanguageEnglish);
}

I18N::I18N(std::ostream& stream, Language language)
    : stream(stream), current_language(all_texts_english), fallback_counter(0)
{
    init(language);
}

void I18N::init(Language language) {
    this->language = language;
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

        case LanguagePortuguese:
            current_language = all_texts_portuguese;
            break;
    }
}

void I18N::change(Language language) {
    if (language != this->language) {
        init(language);
        for (Cbs::iterator it = cbs.begin(); it != cbs.end(); it++) {
            it->cb(it->data);
        }
    }
}

void I18N::register_callback(Callback cb, void *data) {
    cbs.push_back(Cb(cb, data));
}

void I18N::unregister_callback(Callback cb) {
    for (Cbs::iterator it = cbs.begin(); it != cbs.end(); it++) {
        if (it->cb == cb) {
            cbs.erase(it);
            break;
        }
    }
}

const char *I18N::get_text(I18NText id) const {
    if (id > I18N_NONE && id < _I18N_FINISHED_) {
        const Text& text = current_language[id - 1];
        if (text.id == id) {
            return text.text;
        } else {
            return get_text_fallback(id);
        }
    }

    return "???";
}

const char *I18N::get_text_fallback(I18NText id) const {
    fallback_counter++;
    if (fallback_counter == FallbackCounterMax) {
        fallback_counter = 0;
        stream << std::endl;
        stream << "----------------------------------------" << std::endl;
        stream << " WARNING: the i18n is inconstistent." << std::endl;
        stream << " your game client seems to be outdated." << std::endl;
        stream << " you should update as soon as possible." << std::endl;
        stream << "----------------------------------------" << std::endl;
        stream << std::endl;
    }
    const Text *texts = current_language;
    while (texts->text) {
        if (texts->id == id) {
            return texts->text;
        }
        texts++;
    }

    return "???";
}

void I18N::replace(std::string& s, const char *w, size_t wl, const char *p) const {
    std::string::size_type pos = 0;
    while ((pos = s.find(w)) != std::string::npos) {
        s.replace(pos, wl, p);
    }
}

void I18N::replace(std::string& s, const char *w, size_t wl, const std::string& p) const {
    replace(s, w, wl, p.c_str());
}

void I18N::replace(std::string& s, const char *w, size_t wl, int p) const {
    char buffer[64];
    sprintf(buffer, "%d", p);
    replace(s, w, wl, buffer);
}

void I18N::replace(std::string& s, const char *w, size_t wl, long p) const {
    char buffer[64];
    sprintf(buffer, "%ld", p);
    replace(s, w, wl, buffer);
}

void I18N::replace(std::string& s, const char *w, size_t wl, double p) const {
    char buffer[64];
    sprintf(buffer, "%f", p);
    replace(s, w, wl, buffer);
}