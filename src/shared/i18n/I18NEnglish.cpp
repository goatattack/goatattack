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

const I18N::Text I18N::all_texts_english[] = {
    { I18N_BUTTON_OK, "Ok" },
    { I18N_BUTTON_CANCEL, "Cancel" },
    { I18N_BUTTON_CLOSE, "Close" },
    { I18N_BUTTON_START_SERVER, "Start Server" },
    { I18N_MAINMENU_PLAY, "Play" },
    { I18N_MAINMENU_LAN, "Create Local LAN Server" },
    { I18N_MAINMENU_PACKAGES, "List Loaded Packages" },
    { I18N_MAINMENU_OPTIONS, "Options And Settings" },
    { I18N_MAINMENU_CREDITS, "Credits" },
    { I18N_MAINMENU_QUIT, "Quit Game" },
    { I18N_MAINMENU_VERSION, "Current Version: ${1}" },
    { I18N_MAINMENU_INTERNET, "Internet" },
    { I18N_MAINMENU_INTERNET_SERVERS, "Internet servers:" },
    { I18N_MAINMENU_SERVER_NAME, "Server Name" },
    { I18N_MAINMENU_CONNECT, "Connect" },
    { I18N_MAINMENU_REFRESH, "Refresh" },
    { I18N_MAINMENU_PLAYERS, "Players" },
    { I18N_MAINMENU_PING, "Ping" },
    { I18N_MAINMENU_LAN_TITLE, "LAN" },
    { I18N_MAINMENU_LAN_SERVERS, "LAN servers:" },
    { I18N_MAINMENU_CUSTOM_SERVER, "Custom Server" },
    { I18N_MAINMENU_ENTER_HOSTNAME, "Enter here the hostname or IP address and port:" },
    { I18N_MAINMENU_ADDRESS, "address:" },
    { I18N_MAINMENU_PORT, "port:" },
    { I18N_MAINMENU_PASSWORD, "password:" },
    { I18N_MAINMENU_ENTER_PASSWORD, "Enter password:" },
    { I18N_MAINMENU_SERVER_NAME2, "server name:" },
    { I18N_MAINMENU_GAME_MODE, "game mode:" },
    { I18N_MAINMENU_GM_DM, "death match" },
    { I18N_MAINMENU_GM_TDM, "team death match" },
    { I18N_MAINMENU_GM_CTF, "capture the flag" },
    { I18N_MAINMENU_GM_SR, "speed race" },
    { I18N_MAINMENU_GM_CTC, "catch the coin" },
    { I18N_MAINMENU_GM_GOH, "goat of the hill" },
    { I18N_MAINMENU_MAX_PLAYERS, "max. players:" },
    { I18N_MAINMENU_WARMUP, "warm-up:" },
    { I18N_MAINMENU_DURATION, "duration:" },
    { I18N_MAINMENU_IN_SECONDS, "(in seconds)" },
    { I18N_MAINMENU_IN_MINUTES, "(in minutes)" },
    { I18N_MAINMENU_SELECT_MAP, "select map:" },
    { I18N_MAINMENU_INVALID_SERVERNAME, "Please enter a valid server name." },
    { I18N_MAINMENU_MAX_PLAYERS_TITLE, "Max. Players" },
    { I18N_MAINMENU_INVALID_GENERIC_NUMBER, "Please enter a valid number. (${1})" },
    { I18N_MAINMENU_DURATION2, "Duration" },
    { I18N_MAINMENU_WARMUP2, "Warm-Up" },
    { I18N_MAINMENU_SELECT_MAP2, "Select Map" },
    { I18N_MAINMENU_SELECT_MAP3, "Please select a map." },
    { I18N_MAINMENU_PACKAGE, "Package" },
    { I18N_MAINMENU_HASH, "Hash" },
    { I18N_MAINMENU_CREDITS_CODE, "code:" },
    { I18N_MAINMENU_CREDITS_GFX, "graphics:" },
    { I18N_MAINMENU_CREDITS_MAPS, "maps:" },
    { I18N_MAINMENU_CREDITS_MUSIC, "music:" },
    { I18N_MAINMENU_CREDITS_THANKS, "special thanks to:" },
    { I18N_MAINMENU_FREDERIC, "and of course, frederic the goat" },

    { I18N_OPTIONS_PLAYER, "Player" },
    { I18N_OPTIONS_GRAPHICS_AND_SOUND, "Graphics And Sound" },
    { I18N_OPTIONS_CONTROLLER, "Controller And Keyboard" },
    { I18N_OPTIONS_SPECTATE, "Spectate" },
    { I18N_OPTIONS_SKIP_SONG, "Skip Song" },
    { I18N_OPTIONS_RETURN, "Return To Main Menu" },

    { I18N_VERSION_MISMATCH_TITLE, "Version Mismatch" },
    { I18N_VERSION_MISMATCH_MESSAGE, "Both protocols differ, you cannot connect to this server." },
    { I18N_SERVER_TITLE, "Server" },
    { I18N_SELECT_SERVER, "Please select a server first." },
    { I18N_ERROR_TITLE, "Error" },
    { I18N_PORT_TITLE, "Port" },
    { I18N_SERVER_UNKNOWN, "Server not found or unknown." },
    { I18N_INVALID_PORT, "Enter valid port number." },
    { I18N_WRONG_MAPTYPE, "Wrong Map Type" },
    { I18N_WRONG_MAPTYPE2, "This map can't be played in this mode." },

    { 0, 0 }
};
