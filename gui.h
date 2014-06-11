
#ifndef ENGINE_GUI_H
#define ENGINE_GUI_H


#include "entity.h"
#include "render.h"
#include "character_controller.h"

typedef struct gui_text_line_s
{
    char                       *text;
    uint16_t                    buf_size;
    int8_t                      show_rect;
    int8_t                      show;

    GLint                       x;
    GLint                       y;

    GLfloat                     rect_color[4];
    GLfloat                     font_color[4];
    GLfloat                     rect_border;
    GLfloat                     rect[4];                                        //x0, yo, x1, y1

    struct gui_text_line_s     *next;
    struct gui_text_line_s     *prev;
} gui_text_line_t, *gui_text_line_p;


// Fader is a simple full-screen rectangle, which always sits above the scene,
// and, when activated, either shows or hides gradually - hence, creating illusion
// of fade in and fade out effects.
// TR1-3 had only one type of fader - black one, which was activated on level 
// transitions. Since TR4, additional colored fader was introduced to emulate
// various full-screen effects (flashes, flares, and so on).

// Immutable fader enumeration.
// These faders always exist in engine, and rarely you will need more than these.

enum Faders
{
    FADER_BLACK,        // Classic black fader for level transition
    FADER_COLORED,      // Effect fader (flashes, etc.)
    FADER_SUN,          // Sun fader (engages on looking at the sun)
    FADER_DEATH,        // Just for fun - death fader.
    FADER_LASTINDEX
};

#define TR_FADER_DIR_IN  0
#define TR_FADER_DIR_OUT 1

#define TR_FADER_CORNER_TOPLEFT     0
#define TR_FADER_CORNER_TOPRIGHT    1
#define TR_FADER_CORNER_BOTTOMLEFT  2
#define TR_FADER_CORNER_BOTTOMRIGHT 3

// Main fader class.

class gui_Fader
{
public:
    gui_Fader();                  // Fader constructor.

    void Show();                  // Shows and updates fader.
    void Engage(int fade_dir);    // Resets and starts fader.
    void Cut();                   // Immediately cuts fader.
    
    bool IsFading();              // Get current state of the fader.
    
    void SetColor(uint8_t R, uint8_t G, uint8_t B, int corner = -1);
    void SetBlendingMode(uint32_t mode = BM_OPAQUE);
    void SetAlpha(uint8_t alpha  = 255);
    void SetSpeed(uint16_t fade_speed = 200);
    void SetAutoReset(bool reset);
    
private:
    GLfloat         top_left_color[4];      // All colors are defined separately, for
    GLfloat         top_right_color[4];     // further possibility of advanced full
    GLfloat         bottom_left_color[4];   // screen effects with gradients.
    GLfloat         bottom_right_color[4];
    
    uint32_t        blending_mode;          // Fader's blending mode.
    
    GLfloat         current_alpha;          // Current alpha value.
    GLfloat         max_alpha;              // Maximum reachable alpha value.
    GLfloat         speed;                  // Fade speed.
    
    bool            autoreset;              // Specifies if fader auto-resets or not.
    bool            active;                 // Specifies if fader active or not.
    int8_t          direction;              // Specifies fade direction.
};


// Immutable bars enumeration.
// These are the bars that are always exist in GUI.
// Scripted bars could be created and drawn separately later.

enum Bars
{
    BAR_HEALTH,     // TR 1-5
    BAR_AIR,        // TR 1-5, alternate state - gas (TR5)
    BAR_SPRINT,     // TR 3-5
    BAR_FREEZE,     // TR 3 only
    BAR_LOADING,
    BAR_LASTINDEX
};

// Bar color types.
// Each bar part basically has two colours - main and fade.

enum BarColorType
{
    BASE_MAIN,
    BASE_FADE,
    ALT_MAIN,
    ALT_FADE,
    BACK_MAIN,
    BACK_FADE,
    BORDER_MAIN,
    BORDER_FADE
};

// Screen metering resolution specifies user-friendly relative dimensions of screen,
// which are not dependent on screen resolution. They're primarily used to parse
// bar dimensions.

#define TR_GUI_SCREEN_METERING_RESOLUTION 1000.0

// Main bar class.

class gui_ProgressBar
{
public:
    gui_ProgressBar();  // Bar constructor.

    void Show(float value);    // Main show bar procedure.

    void SetColor(BarColorType colType, uint8_t R, uint8_t G, uint8_t B, uint8_t alpha);
    void SetDimensions(float X, float Y, float width, float height, float borderSize);
    void SetValues(float maxValue, float warnValue);
    void SetBlink(int interval);
    void SetExtrude(bool enabled, uint8_t depth);
    void SetAutoshow(bool enabled, int delay, bool fade, int fadeDelay);

    bool          Visible;              // Is it visible or not.
    bool          Alternate;            // Alternate state, in which bar changes color to AltColor.

    bool          Invert;               // Invert decrease direction flag.
    bool          Vertical;             // Change bar style to vertical.

private:

    bool          UpdateResolution      // Try to update bar resolution.
                       (int ScrWidth,
                        int ScrHeight);

    void          RecalculateSize();    // Recalculate size and position.
    void          RecalculatePosition();

    float         mX;                   // Real X position.
    float         mY;                   // Real Y position.
    float         mWidth;               // Real width.
    float         mHeight;              // Real height.
    float         mBorderWidth;         // Real border size (horizontal).
    float         mBorderHeight;        // Real border size (vertical).

    float         mAbsX;                // Absolute (resolution-independent) X position.
    float         mAbsY;                // Absolute Y position.
    float         mAbsWidth;            // Absolute width.
    float         mAbsHeight;           // Absolute height.
    float         mAbsBorderSize;       // Absolute border size (horizontal).

    float         mBaseMainColor[5];    // Color at the min. of bar.
    float         mBaseFadeColor[5];    // Color at the max. of bar.
    float         mAltMainColor[5];     // Alternate main color.
    float         mAltFadeColor[5];     // Alternate fade color.
    float         mBackMainColor[5];    // Background main color.
    float         mBackFadeColor[5];    // Background fade color.
    float         mBorderMainColor[5];  // Border main color.
    float         mBorderFadeColor[5];  // Border fade color.

    int8_t        mBaseBlendingMode;    // Blending modes for all bar parts.
    int8_t        mBackBlendingMode;    // Note there is no alt. blending mode, cause
    int8_t        mBorderBlendingMode;  // base and alt are actually the same part.

    bool          mExtrude;             // Extrude effect.
    float         mExtrudeDepth[5];     // Extrude effect depth.

    float         mMaxValue;            // Maximum possible value.
    float         mWarnValue;           // Warning value, at which bar begins to blink.
    float         mLastValue;           // Last value back-up for autoshow on change event.

    bool          mBlink;               // Warning state (blink) flag.
    float         mBlinkInterval;       // Blink interval (speed).
    float         mBlinkCnt;            // Blink counter.

    bool          mAutoShow;            // Autoshow on change flag.
    float         mAutoShowDelay;       // How long bar will stay on-screen in AutoShow mode.
    float         mAutoShowCnt;         // Auto-show counter.
    bool          mAutoShowFade;        // Fade flag.
    float         mAutoShowFadeDelay;   // Fade length.
    float         mAutoShowFadeCnt;     // Fade progress counter.

    float         mRangeUnit;           // Range unit used to set base bar size.
    float         mBaseSize;            // Base bar size.
    float         mBaseRatio;           // Max. / actual value ratio.

    int           mLastScrWidth;        // Back-up to check resolution change.
    int           mLastScrHeight;
};

void Gui_Init();
void Gui_Destroy();

void Gui_InitBars();
void Gui_InitFaders();

void Gui_AddLine(gui_text_line_p line);
void Gui_DeleteLine(gui_text_line_p line);
void Gui_RenderStringLine(gui_text_line_p l);
void Gui_RenderStrings();

/**
 * Calculates rect coordinates around the text
 */
gui_text_line_p Gui_StringAutoRect(gui_text_line_p l);

/**
 * Draws text using a current console font.
 */
gui_text_line_p Gui_OutTextXY(int x, int y, const char *fmt, ...);

/**
 * Helper method to setup OpenGL state for console drawing.
 *
 * Either changes to 2D matrix state (is_gui = 1) or away from it (is_gui = 0). Does not do any drawing.
 */
 
void Gui_SwitchConGLMode(char is_gui);

/**
 * Draws wireframe of this frustum.
 *
 * Expected state:
 *  - Vertex array is enabled, color, tex coord, normal disabled
 *  - No vertex buffer object is bound
 *  - Texturing is disabled
 *  - Alpha test is disabled
 *  - Blending is enabled
 *  - Lighting is disabled
 * Ignored state:
 *  - Currently bound texture.
 *  - Currently bound element buffer.
 *  - Depth test enabled (disables it, then restores)
 *  - Vertex pointer (changes it)
 *  - Matrices (changes them, restores)
 *  - Line width (changes it, then restores)
 *  - Current color (changes it)
 * Changed state:
 *  - Current position will be arbitrary.
 *  - Vertex pointer will be arbitray.
 *  - Current color will be arbitray (set by console)
 *  - Blend mode will be SRC_ALPHA, ONE_MINUS_SRC_ALPHA (set by console)
 */
 
void Gui_Render();

/**
 *  Draw simple rectangle.
 *  Only state it changes is the blend mode, according to blendMode value.
 */

void Gui_DrawRect(const GLfloat &x, const GLfloat &y,
                  const GLfloat &width, const GLfloat &height,
                  const GLfloat colorUpperLeft[], const GLfloat colorUpperRight[],
                  const GLfloat colorLowerLeft[], const GLfloat colorLowerRight[],
                  const int &blendMode);
                  
/**
 *  Initiate fade or check if fade is active.
 *  When Gui_Fade function is called without second argument, it will act like
 *  state check, and when second argument is present, it will immediately engage
 *  corresponding fader.
 */

bool Gui_Fade(int fader, int fade_direction = -1);

                  
/**
 * General GUI drawing routines.
 */
 
void Gui_DrawCrosshair();
void Gui_DrawFaders();
void Gui_DrawBars();
void Gui_DrawLoadingBar(int value);

#endif
