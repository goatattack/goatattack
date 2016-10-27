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

#ifndef _I18N_HPP_
#define _I18N_HPP_

#include "I18NTexts.hpp"

#include <string>
#include <cstdio>
#include <vector>
#include <iostream>

class I18N {
public:
    typedef void (*Callback)(void *data);

    enum Language {
        LanguageEnglish,
        LanguageGerman,
        LanguageFrench,
        LanguagePortuguese
    };

    static const char *Languages[];

    I18N(std::ostream& stream);
    I18N(std::ostream& stream, Language language);

    void change(Language language);
    void register_callback(Callback cb, void *data);
    void unregister_callback(Callback cb);

    std::string operator()(I18NText id) {
        return get_text(id);
    }

    template<typename T1> std::string operator()(I18NText id, const T1& p1) const {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        return s;
    }

    template<typename T1, typename T2> std::string operator()(I18NText id, const T1& p1, const T2& p2) const {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        replace(s, "${2}", 4, p2);
        return s;
    }

    template<typename T1, typename T2, typename T3> std::string operator()(I18NText id, const T1& p1, const T2& p2, const T3& p3) const {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        replace(s, "${2}", 4, p2);
        replace(s, "${3}", 4, p3);
        return s;
    }

    template<typename T> void replace(std::string& s, int idx, const T& p) {
        char buffer[16];
        sprintf(buffer, "${%d}", idx);
        replace(s, buffer, 4, p);
    }

private:
    struct Cb {
        Cb(Callback cb, void *data) : cb(cb), data(data) { }

        Callback cb;
        void *data;
    };

    typedef std::vector<Cb> Cbs;

    struct Text {
        I18NText id;
        const char *text;
    };

    std::ostream& stream;
    const Text *current_language;
    mutable int fallback_counter;

    Language language;
    Cbs cbs;

    static const Text all_texts_english[];
    static const Text all_texts_german[];
    static const Text all_texts_french[];
    static const Text all_texts_portuguese[];

    void init(Language language);
    const char *get_text(I18NText id) const;
    const char *get_text_fallback(I18NText id) const;
    void replace(std::string& s, const char *w, size_t wl, const char *p) const;
    void replace(std::string& s, const char *w, size_t wl, const std::string& p) const;
    void replace(std::string& s, const char *w, size_t wl, int p) const;
    void replace(std::string& s, const char *w, size_t wl, long p) const;
    void replace(std::string& s, const char *w, size_t wl, double p) const;
};

#endif