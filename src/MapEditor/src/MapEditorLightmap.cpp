#include "MapEditor.hpp"
#include "TileGraphicGL.hpp"
#include "Utils.hpp"

#include <png.h>
#include <cerrno>

void MapEditor::static_calculate_light_click(GuiButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->calculate_light(false);
}

void MapEditor::static_calculate_light_pixel_click(GuiButton *sender, void *data) {
    (reinterpret_cast<MapEditor *>(data))->calculate_light(true);
}

void MapEditor::calculate_light(bool pixel_precise) {
    if (wmap) {
        EditableMap::Lights& lights = wmap->get_light_sources();
        if (!lights.size()) {
            show_messagebox(Gui::MessageBoxIconExclamation, "Light Sources",
                "No light sources found in this map.");
            return;
        }

        /* name set? */
        if (!wmap->get_name().length()) {
            show_messagebox(Gui::MessageBoxIconExclamation, "No Map Name",
                "Please choose a map name in the map properties.");
            return;
        }

        /* trace all points */
        Tileset *ts = wmap->get_tileset_ptr();
        if (!ts) {
            show_messagebox(Gui::MessageBoxIconExclamation, "No Tileset",
                "Please select a tileset in the map properties.");
            return;
        }

        int vw = subsystem.get_view_width();
        int vh = subsystem.get_view_height();
        int ww = 127;
        int wh = 60;
        win_compile = push_window(vw / 2 - ww / 2, vh / 2 - wh / 2, ww, wh, "Compiling");
        lbl_compile = create_label(win_compile, Spc, Spc, "0%");

        int mw = wmap->get_width();
        int mh = wmap->get_height();
        int tw = ts->get_tile_width();
        int th = ts->get_tile_height();
        lightmap_w = mw * tw;
        lightmap_h = mh * th;
        int mapw = mw * tw;
        int maph = mh * th;
        short **pdeco = wmap->get_decoration();

        if (lightmap_w % LightMapSize) {
            lightmap_w = ((lightmap_w / LightMapSize) + 1) * LightMapSize;
        }
        if (lightmap_h % LightMapSize) {
            lightmap_h = ((lightmap_h / LightMapSize) + 1) * LightMapSize;
        }

        /* create whole lightmap */
        lightmap = new unsigned char *[lightmap_h];

        /* create thread */
        if (pixel_precise) {
            for (int y = 0; y < lightmap_h; y++) {
                lightmap[y] = new unsigned char[lightmap_w * 4];
                for (int x = 0; x < lightmap_w; x++) {
                    lightmap[y][x * 4 + 0] = 0;
                    lightmap[y][x * 4 + 1] = 0;
                    lightmap[y][x * 4 + 2] = 0;
                    if (x >= mapw || y >= maph) {
                        lightmap[y][x * 4 + 3] = 0;
                    } else {
                        int index = pdeco[y / th][x / tw];
                        if (index < 0) {
                            lightmap[y][x * 4 + 3] = 0;
                        } else {
                            TileGraphic *tg = ts->get_tile(index)->get_tilegraphic();
                            TileGraphicGL *tggl = static_cast<TileGraphicGL *>(tg);
                            if (tggl->get_bytes_per_pixel(0) < 4) {
                                lightmap[y][x * 4 + 3] = 255;
                            } else {
                                unsigned char *p = tggl->get_picture_array(0);
                                p += (((y % th) * 4 * tw) + ((x % tw) * 4));
                                lightmap[y][x * 4 + 3] = p[3];
                            }
                        }
                    }

                }
            }
            compile_thread = new CompileThreadPixel(wmap, lightmap);
        } else {
            for (int y = 0; y < lightmap_h; y++) {
                lightmap[y] = new unsigned char[lightmap_w * 4];
                for (int x = 0; x < lightmap_w; x++) {
                    lightmap[y][x * 4 + 0] = 0;
                    lightmap[y][x * 4 + 1] = 0;
                    lightmap[y][x * 4 + 2] = 0;
                    if (x >= mapw || y >= maph) {
                        lightmap[y][x * 4 + 3] = 0;
                    } else {
                        if (pdeco[y / th][x / tw] < 0) {
                            lightmap[y][x * 4 + 3] = 0;
                        } else {
                            lightmap[y][x * 4 + 3] = 255;
                        }
                    }

                }
            }
            compile_thread = new CompileThreadBlock(wmap, lightmap);
        }
    }
}

void MapEditor::finalise_lightmap() {
    delete compile_thread;
    compile_thread = 0;

    /* write PNG */
    create_directory("maps", home_workdir);
    std::string filename = home_workdir + dir_separator +
        "maps" + dir_separator + wmap->get_name() + ".lmp";

    FILE *f = fopen(filename.c_str(), "wb");
    if (!f) {
        show_messagebox(Gui::MessageBoxIconError, "Light Map",
            "Saving failed: " + std::string(strerror(errno)));
        return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, f);
    png_set_IHDR(png_ptr, info_ptr, lightmap_w, lightmap_h, 8, PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);

    /* delete image after writing each row */
    for (int y = 0; y < lightmap_h; y++) {
        png_write_row(png_ptr, lightmap[y]);
        delete[] lightmap[y];
    }
    delete[] lightmap;

    /* close file */
    png_write_end(png_ptr, 0);
    if (info_ptr) {
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    }
    if (png_ptr) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
    fclose(f);

    if (wmap) {
        wmap->create_lightmap();
    }

    pop_window();
}

