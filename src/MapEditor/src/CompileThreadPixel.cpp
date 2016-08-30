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

#include "CompileThreadPixel.hpp"
#include "TileGraphicGL.hpp"
#include "Scope.hpp"

#include <cmath>

CompileThreadPixel::CompileThreadPixel(EditableMap *wmap, unsigned char **lightmap)
    : CompileThread(wmap, lightmap)
{
    thread_start();
}

CompileThreadPixel::~CompileThreadPixel() {
    thread_join();
}

void CompileThreadPixel::thread() {
    EditableMap::Lights& lights = wmap->get_light_sources();
    Tileset *ts = wmap->get_tileset_ptr();
    size_t nlgt = lights.size();
    int mw = wmap->get_width();
    int mh = wmap->get_height();
    int tw = ts->get_tile_width();
    int th = ts->get_tile_height();
    int w = mw * tw;
    int h = mh * th;
    short **pmap = wmap->get_map();
    short **pdeco = wmap->get_decoration();
    Point pr;

    for (size_t i = 0; i < nlgt; i++) {
        {
            Scope<Mutex> lock(mtx);
            finished_percent = 100 * (i + 1) / nlgt;
        }
        int r = lights[i]->radius;
        int lmaxsq = r * r;
        int lx = lights[i]->x;
        int ly = lights[i]->y;
        Point p2(static_cast<float>(lx * tw + (tw / 2)), static_cast<float>(ly * th + (th / 2)));
        int lsx = static_cast<int>(p2.x) - r;
        int lsy = static_cast<int>(p2.y) - r;
        int lex = static_cast<int>(p2.x) + r;
        int ley = static_cast<int>(p2.y) + r;
        if (lsx < 0) lsx = 0;
        if (lsy < 0) lsy = 0;
        if (lex > w) lex = w;
        if (ley > h) ley = h;

        int txs = lsx / tw;
        int txe = lex / tw;
        int tys = lsy / th;
        int tye = ley / th;

        for (int y = lsy; y < ley; y++) {
            for (int x = lsx; x < lex; x++) {
                int dindex = pdeco[y / th][x / tw];
                if (dindex < 0) {
                    lightmap[y][x * 4 + 3] = 0;
                } else {
                    bool contact = false;
                    Point p1(static_cast<float>(x), static_cast<float>(y));
                    float xd = p2.x - p1.x;
                    float yd = p2.y - p1.y;
                    float dist = xd * xd + yd * yd;
                    if (dist < lmaxsq) {
                        for (int tx = txs; tx < txe; tx++) {
                            for (int ty = tys; ty < tye; ty++) {
                                short index = pmap[ty][tx];
                                if (index >= 0) {
                                    if (ts->get_tile(index)->is_light_blocking()) {
                                        TileGraphic *tg = ts->get_tile(index)->get_tilegraphic();
                                        TileGraphicGL *tggl = static_cast<TileGraphicGL *>(tg);
                                        if (tggl->get_bytes_per_pixel(0) < 4) {
                                            Point p1l(static_cast<float>(tx * tw), static_cast<float>(ty * th));
                                            Point p2l(static_cast<float>(tx * tw), static_cast<float>((ty + 1) * th - 0.5f));
                                            Point p1r(static_cast<float>((tx + 1) * tw - 0.5f), static_cast<float>(ty * th));
                                            Point p2r(static_cast<float>((tx + 1) * tw - 0.5f), static_cast<float>((ty + 1) * th - 0.5f));
                                            Point p1t(static_cast<float>(tx * tw), static_cast<float>(ty * th));
                                            Point p2t(static_cast<float>((tx + 1) * tw - 0.5f), static_cast<float>(ty * th));
                                            Point p1b(static_cast<float>(tx * tw), static_cast<float>((ty + 1) * th - 0.5f));
                                            Point p2b(static_cast<float>((tx + 1) * tw - 0.5f), static_cast<float>((ty + 1) * th - 0.5f));
                                            if (intersection(p1, p2, p1l, p2l, pr) ||
                                                intersection(p1, p2, p1r, p2r, pr) ||
                                                intersection(p1, p2, p1t, p2t, pr) ||
                                                intersection(p1, p2, p1b, p2b, pr))
                                            {
                                                contact = true;
                                                break;
                                            }
                                        } else {
                                            unsigned char *p = tggl->get_picture_array(0);
                                            for (int py = 0; py < th; py++) {
                                                for (int px = 0; px < tw; px++) {
                                                    if (p[3] == 255) {
                                                        Point p1v(static_cast<float>(tx * tw + px) + 0.5f, static_cast<float>(ty * th + py) - 0.5f);
                                                        Point p2v(static_cast<float>(tx * tw + px) + 0.5f, static_cast<float>(ty * th + py) + 0.5f);
                                                        Point p1h(static_cast<float>(tx * tw + px) - 0.5f, static_cast<float>(ty * th + py) + 0.5f);
                                                        Point p2h(static_cast<float>(tx * tw + px) + 0.5f, static_cast<float>(ty * th + py) + 0.5f);
                                                        if (intersection(p1, p2, p1v, p2v, pr) ||
                                                            intersection(p1, p2, p1h, p2h, pr))
                                                        {
                                                            contact = true;
                                                            break;
                                                        }
                                                    }
                                                    p += 4;
                                                }
                                                if (contact) {
                                                   break;
                                                }
                                            }
                                            if (contact) {
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            if (contact) {
                                break;
                            }
                        }
                    } else {
                        contact = true;
                    }
                    if (!contact) {
                        int v = static_cast<int>(sqrt(65025.0f * dist / lmaxsq));
                        if (v < lightmap[y][x * 4 + 3]) {
                            lightmap[y][x * 4 + 3] = v;
                        }
                    }
                }
            }
        }
    }

    Scope<Mutex> lock(mtx);
    finished = true;
}
