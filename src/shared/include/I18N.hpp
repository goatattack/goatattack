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
#include <map>
#include <cstdio>

class I18N {
public:
    enum Language {
        LanguageEnglish,
        LanguageGerman,
        LanguageFrench
    };

    typedef unsigned int id_t;

    I18N() throw ();
    I18N(Language language) throw ();

    std::string operator()(id_t id) {
        return get_text(id);
    }

    template<typename T1> std::string operator()(id_t id, const T1& p1) {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        return s;
    }


    template<typename T1, typename T2> std::string operator()(id_t id, const T1& p1, const T2& p2) {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        replace(s, "${2}", 4, p2);
        return s;
    }

    template<typename T1, typename T2, typename T3> std::string operator()(id_t id, const T1& p1, const T2& p2, const T3& p3) {
        std::string s(get_text(id));
        replace(s, "${1}", 4, p1);
        replace(s, "${2}", 4, p2);
        replace(s, "${3}", 4, p3);
        return s;
    }

private:
    Language language;

    typedef std::map<id_t, const char*> TextMap;

    struct Text {
        id_t id;
        const char *text;
    };

    static const Text all_texts_english[];
    static const Text all_texts_german[];
    static const Text all_texts_french[];

    TextMap texts;

    void init();
    const char *get_text(id_t id) const;

    void replace(std::string& s, const char *w, size_t wl, const char *p);
    void replace(std::string& s, const char *w, size_t wl, const std::string& p);
    void replace(std::string& s, const char *w, size_t wl, int p);
    void replace(std::string& s, const char *w, size_t wl, double p);
};

#endif