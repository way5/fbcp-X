#include "config.h"
#include "display.h"
#include "spi.h"

#include <memory.h>

#ifdef USE_SPLASH_SCREEN

using namespace std;

#ifndef USE_SPLASH_BUILTIN
splash_bitmap_t splashBitmap;
#endif
/**
 * @brief Load bitmap structure from file if not defined USE_SPLASH_BUILTIN
 *
 * @author SK
 * @return true
 * @return false
*/
bool loadSplash() {
    splashonclear = config->read<bool>("splash_on_clear", false);
    LOG("- splash_on_clear = %d", splashonclear);
    splashdelay = config->read<unsigned long long>("splash_delay", 3);
    LOG("- splash_delay = %llu sec", splashdelay);
    splashdelay *= 1000000ULL;
#ifndef USE_SPLASH_BUILTIN
    FILE * sf;
    char * pixb;
    string splash;
    config->readInto(splash, "splash", string(""));
    LOG("- bitmap: %s", splash.c_str());

    if(!splash.empty()) {
        uint32_t cp = 0;                     // file bytes total
        uint8_t cb = 2;                      // bytes per parameter
        uint32_t cbp = 0;                    // pixel buffer cursor
        char p = '\0';
        uintmax_t pixb = 0;
        // b has no effect?
        sf = fopen(splash.c_str(), "rb");

        if(sf == nullptr) {
            LOG("loadSplash: failed to open bitmap");
            return false;;
        }

        while(1) {
            if(fread(&p, 1, 1, sf) != 1) break; /* EOF */
            // first - 2 byte-long parameters
            if(cp < (8 + (SPI_BYTESPERPIXEL * 2))) {
                pixb = ((pixb << 8) | static_cast<uint8_t>(p));
                cb--;
                if(cb == 0) {
                    switch(cp) {
                        case 1:
                            splashBitmap.compressed = pixb;
                            // next parameter is two bytes long
                            cb = 2;
                            break;
                        case 3:
                            splashBitmap.width = pixb;
                            // next parameter is two bytes long
                            cb = 2;
                            break;
                        case 5:
                            splashBitmap.height = pixb;
                            if(splashBitmap.width != 0 && splashBitmap.height != 0) {
                                {
                                    size_t mems = (splashBitmap.width * splashBitmap.height * SPI_BYTESPERPIXEL);
                                    splashBitmap.bitmap = (uint8_t *)Malloc(mems, "loadSplash: unable to malloc");
                                    memset(splashBitmap.bitmap, 0, mems);
                                    LOG("  - %d byte(s) allocated for bitmap", mems);
                                }
                            } else {
                                LOG("loadSplash: wrong width, height or both");
                                fclose(sf);
                                return false;
                            }
                            // next parameter is two bytes long
                            cb = 2;
                            break;
                        case 7:
                            splashBitmap.alpha = pixb;
                            // next parameter is two bytes long
                            cb = SPI_BYTESPERPIXEL;
                            break;
                        default:
                            if(cp < (8 + SPI_BYTESPERPIXEL)) {
                                splashBitmap.bg_color = pixb;
                            } else {
                                splashBitmap.alpha_color = pixb;
                            }
                            // next parameter is two bytes long
                            cb = SPI_BYTESPERPIXEL;
                            break;
                    }
                }
                cp++;
                continue;
            }
            // pixels
            *(splashBitmap.bitmap + cbp) = static_cast<uint8_t>(p);
            cbp++;
        }
        LOG("- bitmap length: %d bytes", cbp);
        fclose(sf);
    } else {
        LOG("loadSplash: unable to read bitmap: %s", splash.c_str());
        return false;
    }
#endif                      // USE_SPLASH_BUILTIN
    // calculate position on screen
    splashBitmap.xpos = (((DISPLAY_WIDTH > splashBitmap.width)?(DISPLAY_WIDTH - splashBitmap.width):2)/2);
    splashBitmap.ypos = (((DISPLAY_HEIGHT > splashBitmap.height)?(DISPLAY_HEIGHT - splashBitmap.height):2)/2);
    LOG("- bitmap positioned at X0=%d Y0=%d", splashBitmap.xpos, splashBitmap.ypos);
    LOG("  - compressed: %d", splashBitmap.compressed);
    LOG("  - width: %d px", splashBitmap.width);
    LOG("  - height: %d px", splashBitmap.height);
    LOG("  - bg_color: 0x%X", splashBitmap.bg_color);
    LOG("  - alpha: %d", splashBitmap.alpha);
    LOG("  - alpha_color: 0x%X", splashBitmap.alpha_color);

    return true;
}

/**
 * @brief Cleanup
 *
 * @author SK
*/
void unloadSplash() {
#ifndef USE_SPLASH_BUILTIN
    LOG("- cleaninng up splash data");
    free(splashBitmap.bitmap);
#endif
}
#else                                                       // USE_SPLASH_SCREEN
bool loadSplash() {
    return true;
}
void unloadSplash() {}
#endif                                                      // USE_SPLASH_SCREEN

/**
 * @brief Clears entire screen
 *         splash is always centered
*/
void ClearScreen(bool init)
{
    uint32_t rowLength = (DISPLAY_WIDTH * SPI_BYTESPERPIXEL);
#ifdef USE_SPLASH_SCREEN
    uint8_t * fbufferRow = new uint8_t[rowLength];
    uint32_t xmin = (splashBitmap.xpos * SPI_BYTESPERPIXEL);
    uint32_t xmax = ((splashBitmap.xpos + splashBitmap.width) * SPI_BYTESPERPIXEL);
    uint16_t x = xmin;                                      // buffer current cursor position
    uint32_t c = 0;                                         // bitmap cursor position
    if(!splashTimeout && programRunning && !displayOff && !displayOffForced && !backlightOff) {
        LOG("- injecting splash bitmap. Using bitmap bg_color: 0x%X", splashBitmap.bg_color);
        memset(fbufferRow, splashBitmap.bg_color, rowLength);
    } else {
        if(programRunning) {
            LOG("- resuming normal operation");
        }
        memset(fbufferRow, 0, rowLength);
    }
#endif
    for (int y = 0; y < DISPLAY_HEIGHT; ++y)
    {
#ifdef USE_SPLASH_SCREEN
        if ((!displayOff && !displayOffForced && !backlightOff) && (!splashTimeout && programRunning) 
            && (firstpass | splashonclear) 
                && y >= splashBitmap.ypos && y < (splashBitmap.ypos + splashBitmap.height))
        {
            while (x < xmax)
            {
                fbufferRow[x] = *(splashBitmap.bitmap + c);
                x++;
                c++;
            }
            x = xmin;
        }
#else 
        memset(fbufferRow, 0, rowLength);
#endif
#ifdef DISPLAY_SPI_BUS_IS_16BITS_WIDE
        SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, 0, 0, 0, 0, (DISPLAY_WIDTH - 1) >> 8, 0, (DISPLAY_WIDTH - 1) & 0xFF);
        SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, 0, (uint8_t)(y >> 8), 0, (uint8_t)(y & 0xFF), 0, (DISPLAY_HEIGHT - 1) >> 8, 0, (DISPLAY_HEIGHT - 1) & 0xFF);
#elif defined(DISPLAY_SET_CURSOR_IS_8_BIT)
        SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, DISPLAY_WIDTH - 1);
        SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, (uint8_t)y, DISPLAY_HEIGHT - 1);
#else
        SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, 0, (DISPLAY_WIDTH - 1) >> 8, (DISPLAY_WIDTH - 1) & 0xFF);
        SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, (uint8_t)(y >> 8), (uint8_t)(y & 0xFF), (DISPLAY_HEIGHT - 1) >> 8, (DISPLAY_HEIGHT - 1) & 0xFF);
#endif
        SPITask *clearLine = AllocTask(rowLength);
        clearLine->cmd = DISPLAY_WRITE_PIXELS;
#ifdef USE_SPLASH_SCREEN
        memcpy(clearLine->data, fbufferRow, clearLine->size);
#else
        memset(clearLine->data, 0, clearLine->size);
#endif
        CommitTask(clearLine);
        RunSPITask(clearLine);
        DoneTask(clearLine);
        if(!init) {
            memset(fbufferRow, 0, rowLength);
#ifdef USE_SPLASH_SCREEN
        } else {
            memset(fbufferRow, splashBitmap.bg_color, rowLength);
#endif
        }
    }
#ifdef DISPLAY_SPI_BUS_IS_16BITS_WIDE
    SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, 0, 0, 0, 0, (DISPLAY_WIDTH - 1) >> 8, 0, (DISPLAY_WIDTH - 1) & 0xFF);
    SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, 0, 0, 0, 0, 0, (DISPLAY_HEIGHT - 1) >> 8, 0, (DISPLAY_HEIGHT - 1) & 0xFF);
#elif defined(DISPLAY_SET_CURSOR_IS_8_BIT)
    SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, DISPLAY_WIDTH - 1);
    SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, 0, DISPLAY_HEIGHT - 1);
#else
    SPI_TRANSFER(DISPLAY_SET_CURSOR_X, 0, 0, (DISPLAY_WIDTH - 1) >> 8, (DISPLAY_WIDTH - 1) & 0xFF);
    SPI_TRANSFER(DISPLAY_SET_CURSOR_Y, 0, 0, (DISPLAY_HEIGHT - 1) >> 8, (DISPLAY_HEIGHT - 1) & 0xFF);
#endif
#ifdef USE_SPLASH_SCREEN
    delete[] fbufferRow;
    fbufferRow = NULL;
    // splash_delay timeout
    if(init && !displayOff && !displayOffForced && !backlightOff && (firstpass | splashonclear)) {
        displayContentsLastChanged = tick();
        splashTimeout = true;
    }
#endif
}

/**
 * @brief Checks if timeout is passed
 *
 * @author SK
*/
void ClearScreenTick() {
#ifdef USE_SPLASH_SCREEN
    if(!displayOff && splashTimeout) {
        if(displayContentsLastChanged + splashdelay < tick()) {
            ClearScreen(false);
            splashTimeout = false;
        }
    }
#endif
}