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

#include "Characterset.hpp"

#include <cstdlib>

const int Characterset::Width = 32;
const int Characterset::Height = 32;
const CollisionBox Characterset::Colbox(7, 0, 18, 18);
const CollisionBox Characterset::DamageColbox(2, 0, 28, 22);
const int Characterset::FlagOffsetX = 7;
const int Characterset::FlagOffsetY = -64;
const int Characterset::FlagDropOffsetX = 7;
const int Characterset::FlagDropOffsetY = -41;
const int Characterset::CoinOffsetX = 0;
const int Characterset::CoinOffsetY = -64;
const int Characterset::CoinDropOffsetX = 0;
const int Characterset::CoinDropOffsetY = -41;
const int Characterset::ProjectileOffsetY = 5;
const char *Characterset::SpawnAnimation = "player_spawn";
const char *Characterset::JumpSound = "jump";

Characterset::Characterset(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, MovableException)
    : Properties(filename + ".characterset", zip), Movable(subsystem)
{
    try {
        read_base_informations(*this);

        suppress_shot_animation = (atoi(get_value("suppress_shot_animation").c_str()) != 0);

        create_character(CharacterAnimationStanding, filename + "_standing.png", get_speed(*this, "standing", 30), get_one_shot(*this, "standing", false), zip);
        create_character(CharacterAnimationRunning, filename + "_running.png", get_speed(*this, "running", 30), get_one_shot(*this, "running", false), zip);
        create_character(CharacterAnimationSliding, filename + "_sliding.png", get_speed(*this, "sliding", 30), get_one_shot(*this, "sliding", false), zip);
        create_character(CharacterAnimationJumping, filename + "_jumping.png", get_speed(*this, "jumping", 30), get_one_shot(*this, "jumping", false), zip);
        create_character(CharacterAnimationFalling, filename + "_falling.png", get_speed(*this, "falling", 30), get_one_shot(*this, "falling", false), zip);
        create_character(CharacterAnimationLanding, filename + "_landing.png", get_speed(*this, "landing", 30), get_one_shot(*this, "landing", false), zip);
        create_character(CharacterAnimationShooting, filename + "_shooting.png", get_speed(*this, "shooting", 30), get_one_shot(*this, "shooting", false), zip);

        create_armor_overlay(CharacterAnimationStanding, filename + "_standing_armor_overlay.png", get_speed(*this, "standing", 30), get_one_shot(*this, "standing", false), zip);
        create_armor_overlay(CharacterAnimationRunning, filename + "_running_armor_overlay.png", get_speed(*this, "running", 30), get_one_shot(*this, "running", false), zip);
        create_armor_overlay(CharacterAnimationSliding, filename + "_sliding_armor_overlay.png", get_speed(*this, "sliding", 30), get_one_shot(*this, "sliding", false), zip);
        create_armor_overlay(CharacterAnimationJumping, filename + "_jumping_armor_overlay.png", get_speed(*this, "jumping", 30), get_one_shot(*this, "jumping", false), zip);
        create_armor_overlay(CharacterAnimationFalling, filename + "_falling_armor_overlay.png", get_speed(*this, "falling", 30), get_one_shot(*this, "falling", false), zip);
        create_armor_overlay(CharacterAnimationLanding, filename + "_landing_armor_overlay.png", get_speed(*this, "landing", 30), get_one_shot(*this, "landing", false), zip);
        create_armor_overlay(CharacterAnimationShooting, filename + "_shooting_armor_overlay.png", get_speed(*this, "shooting", 30), get_one_shot(*this, "shooting", false), zip);

        create_rifle_overlay(CharacterAnimationStanding, filename + "_standing_overlay.png", get_speed(*this, "standing", 30), get_one_shot(*this, "standing", false), zip);
        create_rifle_overlay(CharacterAnimationRunning, filename + "_running_overlay.png", get_speed(*this, "running", 30), get_one_shot(*this, "running", false), zip);
        create_rifle_overlay(CharacterAnimationSliding, filename + "_sliding_overlay.png", get_speed(*this, "sliding", 30), get_one_shot(*this, "sliding", false), zip);
        create_rifle_overlay(CharacterAnimationJumping, filename + "_jumping_overlay.png", get_speed(*this, "jumping", 30), get_one_shot(*this, "jumping", false), zip);
        create_rifle_overlay(CharacterAnimationFalling, filename + "_falling_overlay.png", get_speed(*this, "falling", 30), get_one_shot(*this, "falling", false), zip);
        create_rifle_overlay(CharacterAnimationLanding, filename + "_landing_overlay.png", get_speed(*this, "landing", 30), get_one_shot(*this, "landing", false), zip);
        create_rifle_overlay(CharacterAnimationShooting, filename + "_shooting_overlay.png", get_speed(*this, "shooting", 30), get_one_shot(*this, "shooting", false), zip);
    } catch (const Exception& e) {
        throw MovableException(std::string(e.what()) + ": " + filename);
    }
}

Characterset::~Characterset() {
    cleanup();
}

Tile *Characterset::get_tile(Direction direction, CharacterAnimation animation) {
    return tiles[direction][animation];
}

Tile *Characterset::get_armor_overlay(Direction direction, CharacterAnimation animation) {
    return armor_overlays[direction][animation];
}

Tile *Characterset::get_rifle_overlay(Direction direction, CharacterAnimation animation) {
    return rifle_overlays[direction][animation];
}

bool Characterset::get_suppress_shot_animation() const {
    return suppress_shot_animation;
}

void Characterset::create_character(CharacterAnimation type,
    const std::string& filename, int animation_speed, bool one_shot, ZipReader *zip) throw (Exception)
{
    PNG png(filename, zip);
    tiles[DirectionRight][type] = create_tile(png, animation_speed, one_shot, false);
    png.flip_h();
    tiles[DirectionLeft][type] = create_tile(png, animation_speed, one_shot, true);
}

void Characterset::create_armor_overlay(CharacterAnimation type,
    const std::string& filename, int animation_speed, bool one_shot, ZipReader *zip) throw (Exception)
{
    PNG png(filename, zip);
    armor_overlays[DirectionRight][type] = create_tile(png, animation_speed, one_shot, false);
    png.flip_h();
    armor_overlays[DirectionLeft][type] = create_tile(png, animation_speed, one_shot, true);
}

void Characterset::create_rifle_overlay(CharacterAnimation type,
    const std::string& filename, int animation_speed, bool one_shot, ZipReader *zip) throw (Exception)
{
    PNG png(filename, zip);
    rifle_overlays[DirectionRight][type] = create_tile(png, animation_speed, one_shot, false);
    png.flip_h();
    rifle_overlays[DirectionLeft][type] = create_tile(png, animation_speed, one_shot, true);
}

void Characterset::cleanup() {
    for (int i = 0; i < _CharacterAnimationMAX; i++) {
        delete tiles[DirectionRight][i];
        delete tiles[DirectionLeft][i];
        delete armor_overlays[DirectionRight][i];
        delete armor_overlays[DirectionLeft][i];
        delete rifle_overlays[DirectionRight][i];
        delete rifle_overlays[DirectionLeft][i];
    }
}
