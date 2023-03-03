#ifndef AST_OBJ

#include <vector>

#ifndef PICO_DISPLAY
#define WHITE 0
#define BLACK 1
#endif

#define SOLID 1

struct coordinate {
    coordinate() {};
    coordinate(int _x, int _y) {
        x = _x;
        y = _y;
    }
    coordinate(const struct coordinate &src) {
        x = src.x;
        y = src.y;
    }
    struct coordinate & operator=(const struct coordinate &src) {
        x = src.x;
        y = src.y;
        return *this;
    }
    struct coordinate & operator+=(const struct coordinate &src) {
        x += src.x;
        y += src.y;
        return *this;
    }
    struct coordinate operator+(const struct coordinate &src) {
        struct coordinate tmp;
        tmp.x = x + src.x;
        tmp.y = y + src.y;
        return tmp;
    }
    friend bool operator== (const struct coordinate &p0, const struct coordinate &p1);
    int x;
    int y;
};

struct line_segment {
    line_segment() {};
    line_segment(const struct line_segment &src) {
        p0 = src.p0;
        p1 = src.p1;
    } 
    line_segment(struct coordinate _p0, struct coordinate _p1) {
        p0 = _p0;
        p1 = _p1;
    }
    struct line_segment & operator=(const struct line_segment &src) {
        p0 = src.p0;
        p1 = src.p1;
        return *this;
    }
    struct coordinate p0;
    struct coordinate p1;
};

#define WINDOW_HEIGHT 320
#define WINDOW_WIDTH  480

// need screen size, background color defines!!!

class astObj {
    public:
    astObj() { Init(); };
    astObj(std::vector<struct coordinate> *outline, int x = 0, int y = 0) { 
        Init(); 
        AddOutline(outline);
        SetOrigin(x, y);
    };
    ~astObj() {};

    void Init() {
        outline = NULL;
        window_ulx = 0;
        window_uly = 0;
        window_lrx = WINDOW_WIDTH; 
        window_lry = WINDOW_HEIGHT; 
        scale = 1.0; 
        rotation = 0.0; 
        color = WHITE;
        style = SOLID;
        origin.x = 0;
        origin.y = 0;
        at_origin = true;
        move_count = 0;
        move_initial_count = 1;
    };

    void LocateSizeWindow(unsigned _window_ulx, unsigned _window_uly, unsigned _window_lrx, unsigned _window_lry) {
        window_ulx = _window_ulx;
        window_uly = _window_uly;
        window_lrx = _window_lrx;
        window_lry = _window_lry;
    };

    void ScaleWindow(float _scale, float _rotation = 0.0) {
        scale = _scale;
        rotation = _rotation;
    };

    // use these to see if some x,y coordinate exceeds window boundaries:

    int window_bounds_ULX() { return window_ulx; };    // upper left 
    int window_bounds_ULY() { return window_uly; };    //   x,y
    int window_bounds_LRX() { return window_lrx; };    // lower right
    int window_bounds_LRY() { return window_lry; };    //   x,y

    // use these when clipping line segments to legal x,y window coordinates:

    int window_ULX() { return window_ulx;     };       // upper left
    int window_ULY() { return window_uly;     };       //   x,y
    int window_LRX() { return window_lrx - 1; };       // lower right
    int window_LRY() { return window_lry - 1; };       //   x,y

    void AddOutline(std::vector<struct coordinate> *_outline) { outline = _outline; }; 

    unsigned Color() { return color; };
    unsigned LineStyle() { return style; };

    void SetStyle(unsigned int _color, unsigned int _style) {
        color = _color;
        style = _style;
    }

    void SetOrigin(int x, int y) {
        origin.x = x;
        origin.y = y;
    };

    void SetTrajectory(int x, int y) {
        trajectory_coord.x = x;
        trajectory_coord.y = y;
    };

    void SetTarget(struct coordinate _target_ul, struct coordinate _target_lr) {
        target_ul = _target_ul;
        target_lr = _target_lr;
    }

    void Advance(); // advance and (possibly redraw) object based on its speed

    // call this method ONLY AFTER calling Advance:
    bool Visible() { return line_segments_screen.size() > 0; }; 

    bool AHit(struct coordinate projectile); // has the object been hit?

    void DumpLineSegments();

    private:
    void Translate(); // translate each raw line segment into screen coordinates

    void Draw(bool erase=false);  // draw using processed line segments

    void Erase() { 
        Draw(true); 
        line_segments_screen.erase(line_segments_screen.begin(),line_segments_screen.end());
    };

    bool ClipLineSegment(struct coordinate &line_segment_start,struct coordinate &line_segment_end);

    unsigned clip_code(struct coordinate &p0);
    int grid_index(unsigned int clip_code);
    int grid_square(struct coordinate p);

    bool flip_line_segment(unsigned int code_p0, unsigned int code_p1);

    bool solve_for_x(unsigned &xt, struct coordinate &p0,struct coordinate &p1, unsigned yt);
    bool solve_for_y(unsigned &yt, struct coordinate &p0,struct coordinate &p1, unsigned xt);

    // the shape (outline) of the object is defined by a set of 'raw' line segments:

    std::vector<struct coordinate> *outline; // 'raw' coordinates

    // optimization: each time object is drawn, store processed line coordinates
    // to make it easier to erase the object prior to its redraw
    std::vector<struct line_segment> line_segments_screen; // 'processed' coordinates

    int window_ulx, window_uly; // upper left hand corner coordinates
    int window_lrx, window_lry; // lower right   "     "      ""

    float scale;   // scale each line segment component;
    float rotation; // rotation element for each coordinate

    unsigned int color; // uniform object color
    unsigned int style; // solid, dashed, dotted...

    struct coordinate target_ul; // missiles must hit within
    struct coordinate target_lr; //   this 'target' (for now just a box)

    struct coordinate origin;           // objects outline is placed at this origin
    bool at_origin;                     // true when object is first placed

    struct coordinate trajectory_coord; //
    float trajectory_scale;             // object moves based on its
    float trajectory_rotation;          //   trajectory

    int move_initial_count; // count down based on speed. 
    int move_count;         //   draw/reset count at zero
};
#endif
#define AST_OBJ