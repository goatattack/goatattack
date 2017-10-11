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

#ifndef _RESOURCES_HPP_
#define _RESOURCES_HPP_

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "ZipReader.hpp"
#include "Directory.hpp"
#include "Tileset.hpp"
#include "Object.hpp"
#include "Characterset.hpp"
#include "NPC.hpp"
#include "Animation.hpp"
#include "Map.hpp"
#include "Background.hpp"
#include "Font.hpp"
#include "Icon.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "Shader.hpp"

#include <string>
#include <map>
#include <vector>
#include <cstring>

class ResourcesException : public Exception {
public:
    ResourcesException(const char *msg) : Exception(msg) { }
    ResourcesException(const std::string& msg) : Exception(msg) { }
};

class ResourcesMissingException : public ResourcesException {
public:
    ResourcesMissingException(const char *msg) : ResourcesException(msg) { }
    ResourcesMissingException(const std::string& msg) : ResourcesException(msg) { }
};

class Tournament;

class Resources {
private:
    Resources(const Resources&);
    Resources& operator=(const Resources&);

public:
    struct NonDownloadableMainPak {
        const char *name;
        bool check;
    };

    static const NonDownloadableMainPak NonDownloadableMainPaks[];

    struct LoadedPak {
        LoadedPak(const std::string& pak_name, const std::string& pak_short_name,
            const std::string& pak_hash, bool from_home_dir)
            : pak_name(pak_name), pak_short_name(pak_short_name), pak_hash(pak_hash),
              from_home_dir(from_home_dir) { }

        std::string pak_name;
        std::string pak_short_name;
        std::string pak_hash;
        bool from_home_dir;

        bool operator==(const char *short_name) const {
            return (!strcmp(pak_short_name.c_str(), short_name));
        }
    };

    typedef std::vector<LoadedPak> LoadedPaks;

    struct ResourceObject {
        ResourceObject(void *object, bool base_resource) : object(object), base_resource(base_resource) { }
        void *object;
        bool base_resource;
    };

    typedef std::vector<ResourceObject> ResourceObjects;

    Resources(Subsystem& subystem, const std::string& resource_directory, bool skip_maps = false, bool paks_only = false);
    virtual ~Resources();

    void reload_resources();
    void update_tile_index(double diff, Tileset *tileset);
    const std::string& get_resource_directory() const;

    Tileset *get_tileset(const std::string& name);
    Object *get_object(const std::string& name);
    Characterset *get_characterset(const std::string& name);
    NPC *get_npc(const std::string& name);
    Animation *get_animation(const std::string& name);
    Map *get_map(const std::string& name);
    Background *get_background(const std::string& name);
    Font *get_font(const std::string& name);
    Icon *get_icon(const std::string& name);
    Sound *get_sound(const std::string& name);
    Music *get_music(const std::string& name);
    Properties *get_game_settings(const std::string& name);
    Shader *get_shader(const std::string& name);
    Icon *get_flag(const std::string& name, bool no_throw = false);

    ResourceObjects& get_tilesets();
    ResourceObjects& get_objects();
    ResourceObjects& get_charactersets();
    ResourceObjects& get_npcs();
    ResourceObjects& get_animations();
    ResourceObjects& get_maps();
    ResourceObjects& get_backgrounds();
    ResourceObjects& get_fonts();
    ResourceObjects& get_icons();
    ResourceObjects& get_sounds();
    ResourceObjects& get_musics();
    ResourceObjects& get_game_settings();
    ResourceObjects& get_shaders();
    ResourceObjects& get_flags();

    const LoadedPaks& get_loaded_paks() const;

private:
    Subsystem& subsystem;
    I18N& i18n;
    std::string resource_directory;
    bool skip_maps;
    bool paks_only;
    FT_Library ft;

    LoadedPaks loaded_paks;
    ResourceObjects tilesets;
    ResourceObjects objects;
    ResourceObjects charactersets;
    ResourceObjects npcs;
    ResourceObjects animations;
    ResourceObjects maps;
    ResourceObjects backgrounds;
    ResourceObjects fonts;
    ResourceObjects icons;
    ResourceObjects sounds;
    ResourceObjects musics;
    ResourceObjects game_settings;
    ResourceObjects shaders;
    ResourceObjects flags;

    void read_tilesets(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_objects(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_charactersets(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_npcs(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_animations(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_maps(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_backgrounds(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_fonts(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_icons(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_sounds(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_musics(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_game_settings(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_shaders(const std::string& directory, ZipReader *zip, bool base_resource);
    void read_flags(const std::string& directory, ZipReader *zip, bool base_resource);

    void load_resources(bool home_paks_only);
    void read_all(const std::string& fdir, ZipReader *fzip, bool base_resource);
    void destroy_resources(bool home_paks_only);
    void prepare_resources();
};

#endif
