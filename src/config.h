#pragma once

// Build options: Uncomment any of these, or set at the command line to configure:

// Display driver
/// Target KeDei 3.5 inch SPI TFTLCD 480*320 16bit/186bit version 6.3 2018/4/9 display (MPI3501)
// #define KEDEI_V63_MPI3501
/// Target Adafruit's ILI9341-based PiTFT display
// #define ADAFRUIT_ILI9341_PITFT
/// Target WaveShare32B ILI9341 display on Freeplaytech's CM3/Zero devices)
// #define FREEPLAYTECH_WAVESHARE32B
/// Target Waveshare's ILI9486-based Waveshare Wavepear 3.5 inch (B) display
// #define WAVESHARE35B_ILI9486
/// Target Tontec's MZ61581-based 3.5 inch display
// #define TONTEC_MZ61581
/// Targeting Adafruit 3.5 inch PiTFT with HX8357D
// #define ADAFRUIT_HX8357D_PITFT
/// Targeting WaveShare 240x240 1.3inch IPS LCD Hat with ST7789VW controller
// #define WAVESHARE_ST7789VW_HAT
/// Targeting WaveShare 128x128 1.44inch LCD Hat with ST7735S controller
// #define WAVESHARE_ST7735S_HAT
/// Targeting Pirate Audio 240x240 1.3inch IPS LCD Hat with ST7789 controller
// #define PIRATE_AUDIO_ST7789_HAT
/// If you are running on any other generic ILI9341 display, or on Waveshare32b display 
/// that is standalone and not on the FreeplayTech CM3/Zero device, pass this flag.
// #define ILI9341
/// If you have a ILI9340 display, pass this directive. ILI9340 and ILI9341 chipsets are 
// very similar, but ILI9340 doesn't support all of the features on ILI9341 and they 
/// will be disabled or downgraded.
// #define ILI9340
/// If you have a ILI9486L display, pass this directive. Note that ILI9486 and ILI9486L 
/// are quite different, mutually incompatible controller chips, so be careful here identifying 
/// which one you have. (or just try both, should not break if you misidentified)
// #define ILI9486L
// #define ILI9488
// #define ILI9486
// #define HX8357D
#define ST7735R
// #define ST7735S
// #define ST7789
// #define ST7789VW
// #define SSD1351
// #define MZ61581
// #define MPI3501

// Specifies/overrides which GPIO pin to use for the Data/Control (DC) line 
// on the 4-wire SPI communication. This pin number is specified in BCM pin numbers. 
// If you have a 3-wire SPI display that does not have a Data/Control line, 
// **set this value to -1**, i.e. `-DGPIO_TFT_DATA_CONTROL=-1` to tell fbcp-ili9341 
// to target 3-wire ("9-bit") SPI communication.
// Using 4-wire SPI mode of communication, with GPIO pin ${GPIO_TFT_DATA_CONTROL} for Data/Control line
#define GPIO_TFT_DATA_CONTROL                               9
#if (GPIO_TFT_DATA_CONTROL < 0)
// Using 3-wire SPI mode of communication, i.e. a display that does not have a Data/Control line
#define SPI_3WIRE_PROTOCOL                                  1
#endif                                                  // GPIO_TFT_DATA_CONTROL

// Specifies/overrides which GPIO pin to use for the display Reset line. 
// This pin number is specified in BCM pin numbers. If omitted, it is assumed 
// that the display does not have a Reset pin, and is always on.
#define GPIO_TFT_RESET_PIN                                  25

// If enabled, build to utilize DMA transfers to communicate with the SPI peripheral. Otherwise polling
// writes will be performed (possibly with interrupts, if using kernel side driver module)
// defaunt: enabled
#ifndef KEDEI_V63_MPI3501
#define USE_DMA_TRANSFERS
#       ifdef USE_DMA_TRANSFERS
// Manually specifies the DMA send channel to use
// *DMA TX and RX channels cannot be the same channel!
// #define DMA_TX_CHANNEL                                   0
// Manually specifies the DMA receive channel to use
// #define DMA_RX_CHANNEL                                   1
#       endif                                              // USE_DMA_TRANSFERS
#endif                                              // KEDEI_V63_MPI3501

// Specifies/overrides which GPIO pin to use for the display backlight line. 
// This pin number is specified in BCM pin numbers. If omitted, it is assumed 
// that the display does not have a GPIO-controlled backlight pin, and is always on. 
#define GPIO_TFT_BACKLIGHT                                  24
#ifdef GPIO_TFT_BACKLIGHT
// If enabled, reads keyboard for input events to detect when the system has gone inactive and backlight
// can be turned off
// #define BACKLIGHT_CONTROL_FROM_KEYBOARD

// This device file is used to capture keyboard input. This may be "/dev/input/event0" or something else
// on some Pis
// #define KEYBOARD_INPUT_FILE "/dev/input/event1"

// If enabled, the display backlight will be turned off after this many usecs of no activity on screen.
// if defined, "turn_off_after" will be evaluated first, if "turn_off_after" is not set or 0 the followinng value will be used
// value in seconds
#define TURN_DISPLAY_OFF_AFTER_USECS_OF_INACTIVITY          60
#endif

// Sets the clock divisor number which along with the Pi 
// https://www.raspberrypi.org/documentation/configuration/config-txt/overclocking.md option in `/boot/config.txt` 
// specifies the overall speed that the display SPI communication bus is driven at. `SPI_frequency = core_freq/divisor`. 
// `SPI_BUS_CLOCK_DIVISOR` must be an even number. Default Pi 3B and Zero W `core_freq` is 400MHz, 
// and generally a value `-DSPI_BUS_CLOCK_DIVISOR=6` seems to be the best that a ILI9341 display can do. 
// Try a larger value if the display shows corrupt output, or a smaller value to get higher bandwidth. 
// See https://github.com/juj/fbcp-ili9341/blob/master/ili9341.h#L13 and https://github.com/juj/fbcp-ili9341/blob/master/waveshare35b.h#L10 
// for data points on tuning the maximum SPI performance. Safe initial value could be something like `-DSPI_BUS_CLOCK_DIVISOR=30`.
#define SPI_BUS_CLOCK_DIVISOR                               40

// Pass this option if you are running on a Pi that has only one hardware 
// thread (Pi Model A, Pi Model B, Compute Module 1, Pi Zero/Zero W). 
// If not present, autodetected.
#define SINGLE_CORE_BOARD                                   1
// Pass this option to specifically optimize for ARMv6Z instruction 
// set (Pi 1A, 1A+, 1B, 1B+, Zero, Zero W). If not present, autodetected.
#define ARMV6Z
// Pass this option to specifically optimize for ARMv7-A instruction 
// set (Pi 2B < rev 1.2). If not present, autodetected.
// #define ARMV7A
// Pass this option to specifically optimize for ARMv8-A instruction 
// set (Pi 2B >= rev. 1.2, 3B, 3B+, CM3, CM3 lite, 4B, CM4, Pi400). 
// If not present, autodetected.
// #define ARMV8A

// If defined, renders a performance overlay on top of the screen. This option is passed from CMake
// configuration script. If you are getting statistics printed on screen
// even when this is uncommented, pass -DSTATISTICS=0 to CMake invocation line. You can also try
// building with
//   'make VERBOSE=1'
// to see which config flags are coming from CMake to the build.
// Set to 0, 1 or 2 to configure the level of statistics to display. 
// 0=OFF, 1=regular statistics, 2=frame rate interval histogram
#define STATISTICS                                          0
#if (STATISTICS > 0)
// How often the on-screen statistics is refreshed (in usecs)
#define STATISTICS_REFRESH_INTERVAL                         200000

#if (STATISTICS > 1)
// If enabled, displays a visual graph of frame completion times
#define FRAME_COMPLETION_TIME_STATISTICS
#endif                                                      // (STATISTICS > 1)
#endif                                                  // (STATISTICS > 0)

// How many usecs worth of past frame rate data do we preserve in the history buffer. Higher values
// make the frame rate display counter smoother and respond to changes with a delay, whereas smaller
// values can make the display fluctuate a bit erratically.
#define FRAMERATE_HISTORY_LENGTH                            400000

// If defined, no sleeps are specified and the code runs as fast as possible. This should not improve
// performance, as the code has been developed with the mindset that sleeping should only occur at
// times when there is no work to do, rather than sleeping to reduce power usage. The only expected
// effect of this is that CPU usage shoots to 200%, while display update FPS is the same. Present
// here to allow toggling to debug this assumption.
// #define NO_THROTTLING

// If defined, display updates are synced to the vsync signal provided by the VideoCore GPU. That seems
// to occur quite precisely at 60 Hz. Testing on PAL NES games that run at 50Hz, this will not work well,
// since they produce new frames at every 20msecs, and the VideoCore functions for snapshotting also will
// output new frames at this vsync-detached interval, so there's a 50 Hz vs 60 Hz mismatch that results
// in visible microstuttering. Still, providing this as an option, this might be good for content that
// is known to run at native 60Hz.
// #define USE_GPU_VSYNC

// Always enable GPU VSync on the Pi Zero. Even though it is suboptimal and can cause stuttering, it saves battery.
#ifdef SINGLE_CORE_BOARD

#if !defined(USE_GPU_VSYNC)
#define USE_GPU_VSYNC
#endif

#else // Multicore Pi boards (Pi 2, 3)
// If defined, communication with the SPI bus is handled with a dedicated thread. On the Pi Zero, this does
// not gain much, since it only has one hardware thread.
#define USE_SPI_THREAD

// If USE_GPU_VSYNC is defined, then enabling this causes new frames to be snapshot more often than at
// TARGET_FRAME_RATE interval to try to keep up smoother 60fps instead of stuttering. Consumes more CPU.
#define SELF_SYNCHRONIZE_TO_GPU_VSYNC_PRODUCED_NEW_FRAMES
#endif                                  // SINGLE_CORE_BOARD

// If enabled, the source video frame is not scaled to fit to the screen, but instead if the source frame
// is bigger than the SPI display, then content is cropped away, i.e. the source is displayed "centered"
// on the SPI screen:
// #define DISPLAY_CROPPED_INSTEAD_OF_SCALING

// If enabled, the main thread and SPI thread are executed with realtime priority
// #define RUN_WITH_REALTIME_THREAD_PRIORITY

// If defined, progressive updating is always used (at the expense of slowing down refresh rate if it's
// too much for the display to handle)
// #define NO_INTERLACING

#if (defined(FREEPLAYTECH_WAVESHARE32B) || (defined(ILI9341) && SPI_BUS_CLOCK_DIVISOR <= 4)) && defined(USE_DMA_TRANSFERS) && !defined(NO_INTERLACING)
// The Freeplaytech CM3/Zero displays actually only have a visible display resolution of 302x202, instead of
// 320x240, and this is enough to give them full progressive 320x240x60fps without ever resorting to
// interlacing. Also, ILI9341 displays running with clock divisor of 4 have enough bandwidth to never need
// interlacing either.
#define NO_INTERLACING
#endif

// If defined, all frames are always rendered as interlaced, and never use progressive rendering.
// #define ALWAYS_INTERLACING

// By default, if the SPI bus is idle after rendering an interlaced frame, but the GPU has not yet produced
// a new application frame to be displayed, the same frame will be rendered again for its other field.
// Define this option to disable this behavior, in which case when an interlaced frame is rendered, the 
// remaining other field half of the image will never be uploaded.
// #define THROTTLE_INTERLACING

// The ILI9486 has to resort to interlacing as a rule rather than exception, and it works much smoother
// when applying throttling to interlacing, so enable it by default there.
#if defined(ILI9486) || defined(HX8357D)
#define THROTTLE_INTERLACING
#endif

// If defined, DMA usage is foremost used to save power consumption and CPU usage. If not defined,
// DMA usage is tailored towards maximum performance.
// #define ALL_TASKS_SHOULD_DMA

// If defined, screen updates are performed in strictly one update rectangle per frame.
// This reduces CPU consumption at the expense of sending more pixels. You can try enabling this
// if your SPI display runs at a good high SPI bus MHz speed with respect to the screen resolution.
// Useful on Pi Zero W and ILI9341 to conserve CPU power. If this is not defined, the default much
// more powerful diffing algorithm is used, which sends far fewer pixels each frame, (but that diffing
// costs more CPU time). Enabling this requires that ALL_TASKS_SHOULD_DMA is also enabled.
// #define UPDATE_FRAMES_IN_SINGLE_RECTANGULAR_DIFF

// If UPDATE_FRAMES_IN_SINGLE_RECTANGULAR_DIFF is used, controls whether the generated tasks are aligned for
// ARMv6 cache lines. This is good to be enabled for ARMv6 Pis, doesn't make much difference on ARMv7 and ARMv8 Pis.
#define ALIGN_DIFF_TASKS_FOR_32B_CACHE_LINES

// If defined, screen updates are performend without performing diffing at all, i.e. by doing
// full updates. This is very lightweight on CPU, but excessive on the SPI bus. Enabling this
// requires that ALL_TASKS_SHOULD_DMA is also enabled.
// #define UPDATE_FRAMES_WITHOUT_DIFFING

#if defined(SINGLE_CORE_BOARD) && defined(USE_DMA_TRANSFERS) && !defined(SPI_3WIRE_PROTOCOL) // TODO: 3-wire SPI displays are not yet compatible with ALL_TASKS_SHOULD_DMA option.
// These are prerequisites for good performance on Pi Zero
#ifndef ALL_TASKS_SHOULD_DMA
#define ALL_TASKS_SHOULD_DMA
#endif
#ifndef NO_INTERLACING
#define NO_INTERLACING
#endif
// This saves a lot of CPU, but if you don't care and your SPI display does not have much bandwidth, try uncommenting this for more performant
// screen updates
#ifndef UPDATE_FRAMES_IN_SINGLE_RECTANGULAR_DIFF
#define UPDATE_FRAMES_IN_SINGLE_RECTANGULAR_DIFF
#endif
#endif

// If per-pixel diffing is enabled (neither UPDATE_FRAMES_IN_SINGLE_RECTANGULAR_DIFF or UPDATE_FRAMES_WITHOUT_DIFFING
// are enabled), the following variable controls whether to lean towards more precise pixel diffing, or faster, but
// coarser pixel diffing. Coarse method is twice as fast than the precise method, but submits slightly more pixels.
// In most cases it is better to use the coarse method, since the increase in pixel counts is small (~5%-10%),
// so enabled by default. If your display is very constrained on SPI bus speed, and don't mind increased
// CPU consumption, comment this out to use the precise algorithm.
#if !defined(ALL_TASKS_SHOULD_DMA) // At the moment the coarse method is not good at producing long spans, so disable if all tasks should DMA
#define FAST_BUT_COARSE_PIXEL_DIFF
#endif

#if defined(ALL_TASKS_SHOULD_DMA)
// This makes all submitted tasks go through DMA, and not use a hybrid Polled SPI + DMA approach.
#define ALIGN_TASKS_FOR_DMA_TRANSFERS
#endif

// If defined, the GPU polling thread will be put to sleep for 1/TARGET_FRAMERATE seconds after receiving
// each new GPU frame, to wait for the earliest moment that the next frame could arrive.
#define SAVE_BATTERY_BY_SLEEPING_UNTIL_TARGET_FRAME

// Detects when the activity on the screen is mostly idle, and goes to low power mode, in which new
// frames will be polled first at 10fps, and ultimately at only 2fps.
#define SAVE_BATTERY_BY_SLEEPING_WHEN_IDLE

// Builds a histogram of observed frame intervals and uses that to sync to a known update rate. This aims
// to detect if an application uses a non-60Hz update rate, and synchronizes to that instead.
#define SAVE_BATTERY_BY_PREDICTING_FRAME_ARRIVAL_TIMES

// If defined, rotates the display 180 degrees. This might not rotate the panel scan order though,
// so adding this can cause up to one vsync worth of extra display latency. It is best to avoid this and
// install the display in its natural rotation order, if possible.
#define DISPLAY_ROTATE_180_DEGREES

// If defined, displays in landscape. Undefine to display in portrait.
#define DISPLAY_OUTPUT_LANDSCAPE

// If defined, the source video frame is scaled to fit the SPI display by stretching to fit, ignoring
// aspect ratio. Enabling this will cause e.g. 16:9 1080p source to be stretched to fully cover
// a 4:3 320x240 display. If disabled, scaling is performed preserving aspect ratio, so letterboxes or
// pillarboxes will be introduced if needed to retain proper width and height proportions.
// #define DISPLAY_BREAK_ASPECT_RATIO_WHEN_SCALING

// If defined, reverses RGB<->BGR color subpixel order. This is something that seems to be display panel
// specific, rather than display controller specific, and displays manufactured with the same controller
// can have different subpixel order (without the controller taking it automatically into account).
// If display colors come out reversed in blue vs red channels, define this to swap the two.
// #define DISPLAY_SWAP_BGR

// If defined, inverts display pixel colors (31=black, 0=white). Default is to have (0=black, 31=white)
// Pass this if the colors look like a photo negative of the actual image.
// #define DISPLAY_INVERT_COLORS

// If defined, flipping the display between portrait<->landscape is done in software, rather than
// asking the display controller to adjust its RAM write direction.
// Doing the flip in software reduces tearing, since neither the ILI9341 nor ILI9486 displays (and
// probably no other displays in existence?) allow one to adjust the direction that the scanline refresh
// cycle runs in, but the scanline refresh always runs in portrait mode in these displays. Not having
// this defined reduces CPU usage at the expense of more tearing, although it is debatable which
// effect is better - this can be subjective. Impact is around 0.5-1.0msec of extra CPU time.
// DISPLAY_FLIP_ORIENTATION_IN_SOFTWARE disabled: diagonal tearing
// DISPLAY_FLIP_ORIENTATION_IN_SOFTWARE enabled: traditional no-vsync tearing (tear line runs in portrait
// i.e. narrow direction)
#if !defined(SINGLE_CORE_BOARD)
#define DISPLAY_FLIP_ORIENTATION_IN_SOFTWARE
#endif

// If defined, enable a low battery icon triggered by a GPIO pin whose BCM number is given.
// #define LOW_BATTERY_PIN                                   19
#ifdef LOW_BATTERY_PIN
// Which state of the LOW_BATTERY_PIN is considered to be low battery. Note that the GPIO pin must be
// in the correct state (input with pull-up/pull-down resistor) before the program is started.
#define LOW_BATTERY_IS_ACTIVE_HIGH                          0

// Polling interval (in micro-second) for the low battery pin.
#define LOW_BATTERY_POLLING_INTERVAL                        1000000
#endif                                              // LOW_BATTERY_PIN

// If less than this much % of the screen changes per frame, the screen is considered to be inactive, and
// the display backlight can automatically turn off, if TURN_DISPLAY_OFF_AFTER_USECS_OF_INACTIVITY is 
// defined.
#define DISPLAY_CONSIDERED_INACTIVE_PERCENTAGE              (60.0 / 100.0)

// Compile as a kernel module
// #define KERNEL_MODULE
#ifdef KERNEL_MODULE
// Define this if building the client side program to run against the kernel driver module, rather than
// as a self-contained userland program.
#define KERNEL_MODULE_CLIENT

// Experimental/debugging: If defined, let the userland side program create and run the SPI peripheral
// driving thread. Otherwise, let the kernel drive SPI (e.g. via interrupts or its own thread)
// This should be unset, only available for debugging.
// #define KERNEL_MODULE_CLIENT_DRIVES
#endif

// Using splash screens while loading
#define USE_SPLASH_SCREEN 
#ifdef USE_SPLASH_SCREEN
// If enabled, splash screen data must be builtin the executable
// This setup increases the overal footprint
// #define USE_SPLASH_BUILTIN
#endif
