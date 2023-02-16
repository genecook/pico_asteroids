#include <ast_obj.h>
#include <iostream>

// advance and (possibly redraw) object based on its speed
// (caller responsible for determining when to advance (move) an object)

void astObj::Advance() {
    move_count -= 1;
    if (move_count <= 0) {
        Erase();       // erase the object from the screen at its current position
        Translate();   // place the object at its new position
        Draw();        // draw the object at its new position
        move_count = move_initial_count; // reset move count
    }
}

// return true if projectile coordinates are on or within
// objects coordinates as a target...

bool astObj::AHit(struct coordinate projectile) {
    return projectile.x >= target_ul.x && projectile.x <= target_lr.x &&
            projectile.y >= target_ul.y && projectile.y <= target_lr.y;
}

void astObj::DumpLineSegments() {
    for(auto ix = line_segments_screen.begin(); ix != line_segments_screen.end(); ix++) {
        std::cout << "p0 x,y: " << (*ix).p0.x << "," << (*ix).p0.y
            << " --> p1 x,y: " << (*ix).p1.x << "," << (*ix).p1.y << std::endl;
    }
}

// translate object outline into set of on-screen line segments...

void astObj::Translate() {
    struct coordinate os0,os1;

    for(auto ix = (*outline).begin(); ix != (*outline).end(); ix++) {
        if (ix == (*outline).begin()) {
            os0 = *ix;
            os0 += at_origin ? origin : origin + trajectory_coord;
            continue;
        }
        os1 = *ix;
        os1 += at_origin ? origin : origin + trajectory_coord;

        struct coordinate p0, p1;
        if ( (os0.x > os1.x) || (os0.y > os1.y) ) {
            p0 = os1;
            p1 = os0;
        } else {
            p0 = os0;
            p1 = os1;
        }
        if (ClipLineSegment(p0,p1))
            line_segments_screen.push_back( line_segment(p0,p1) );

        os0 = os1;
    }

    at_origin = false;
    origin += trajectory_coord;   // we've moved on...
}

// clip either end or both ends of line segment (if possible) to display screen,
// return true if there is a solution...
//
// My version of Cohen-Sutherland line clipping.
// See https://www.cs.montana.edu/courses/spring2009/425/dslectures/clipping.pdf
//
bool astObj::ClipLineSegment(struct coordinate &p0,struct coordinate &p1) {

    // calculations assume line segment is ascending from p0 to p1...

    // NOTE: for our screen, origin is at upper left hand corner...

    unsigned code_p0 =  clip_code(p0);
    unsigned code_p1 =  clip_code(p1);

    if ( (code_p0 == code_p1) && (code_p0 == 0) ) return true; // both endpoints are within the (screen) window...
    if ( (code_p0 & code_p1) != 0 ) return false; // the line is completely outside the window...

    // the line may or may not cross the window...

    // the display screen boundaries are 'mapped' to 'tic-tac-to' squares as so:
    //
    //     0 | 1 | 2
    //     --+---+--
    //     3 | 4 | 5     4 - on-screen, all other squares off-screen
    //     --+---+--
    //     6 | 7 | 8
    //     
    // given the above square numbering:
    //
    //   square    p0, p1 on-screen?
    //   p0   p1       p0    p1         process
    //   --   --       ---   ---     --------------------------------------------------------------------------------------
    //    0    4       off   on      solve for p0.x using p0.y == 0 or p0.y using p0.x == 0, p1 is good
    //    3    1       off   off     solve for p0.y using p0.x == 0, solve for p1.x using p1.y == 0
    //    1    5       off   off     solve for p0.y using p0.x = screen-width, solve for p1.x using p1.y == 0
    //    4    2       on    off     p0 is good, solve for p1.x using p1.y == 0 or p1.y using p1.x == screen-width
    //    3    7       off   off     solve for p0.y using p0.x==0, solve for p1.x using p1.y == screen-height
    //    6    4       off   on      solve for p0.y using p0.x==0 or p0.x using p0.y==screen-height, p1 is good
    //    4    8       on    off     p0 is good, solve for p1.y using p1.x==screen-width or p1.x using p1.y == screen-height
    //    7    5       off   off     solve for p0.x using p0.y == screen-height, solve for p1.y using p1.x == screen-width

    unsigned xt, yt;
    bool have_solution = true; // assume line can be clipped to screen

    // NOTE: comments reflect original implementation wherein:
    //      x,y == 0,0 - upper left hand coordinates of (lcd) screen
    //      x,y == 480,320 - lower right hand coordinates of (lcd) screen
    // A coordinate is 'good' if it is within the windows (screens) boundary
    switch( ( (code_p0<<4) | code_p1) ) {
        case 0b01010000: // solve for p0.x using p0.y == 0 or p0.y using p0.x == 0, p1 is good
            if (solve_for_x(xt,p0,p1,window_ULY())) {
                p0.x = xt;
            } else if (solve_for_y(yt,p0,p1,window_ULX())) {
                p0.y = yt;
            } else
                have_solution = false;
            break; 
        case 0b00010100: // solve for p0.y using p0.x == 0, solve for p1.x using p1.y == 0
            have_solution = solve_for_y(yt,p0,p1,window_ULX()) && solve_for_x(xt,p0,p1,window_ULY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0b01000010: // solve for p0.y using p0.x == screen-width, solve for p1.x using p1.y == 0
            have_solution = solve_for_y(yt,p0,p1,window_LRX()) && solve_for_x(xt,p0,p1,window_ULY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0b00000110: // p0 is good, solve for p1.x using p1.y == 0 or p1.y using p1.x == 480
            if (solve_for_x(xt,p0,p1,window_ULY())) {
                p1.x = xt;
            } else if (solve_for_y(yt,p0,p1,window_LRX())) {
                p1.y = yt;
            } else
                have_solution = false;
            break;
        case 0b00011000: // solve for p0.y using p0.x == 0, solve for p1.x using p1.y == screen-height
            have_solution = solve_for_y(yt,p0,p1,window_ULX()) && solve_for_x(xt,p0,p1,window_LRY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0b10010000: // solve for p0.y using p0.x == 0 or p0.x using p0.y == screen-height, p1 is good
            if (solve_for_y(yt,p0,p1,window_ULX())) {
                p0.y = yt;
            } else if (solve_for_x(xt,p0,p1,window_LRY())) {
                p0.x = xt;
            } else
                have_solution = false;  
            break;
        case 0b00000010: // p0 is good, solve for p1.y using p1.x == screen-width or p1.x using p1.y == screen-height
            if (solve_for_y(yt,p0,p1,window_LRX())) {
                p1.y = yt;
            } else if (solve_for_x(xt,p0,p1,window_LRY())) {
                p1.x = xt;
            } else
                have_solution = false;  
            break;
        case 0b10001010: // solve for p0.x using p0.y == 320, solve for p1.y using p1.x == 480
            have_solution = solve_for_x(xt,p0,p1,window_LRY()) && solve_for_y(yt,p0,p1,window_LRX());
            if (have_solution) {
                p0.x = xt;
                p1.y = yt;
            }
            break;
        default: break;
    }
    // if there is a solution, make sure resulting clipped line is within the screen window...
    if (have_solution) {
        code_p0 =  clip_code(p0);
        code_p1 =  clip_code(p1);
        have_solution = (code_p0 == code_p1) && (code_p0 == 0);
    }
    return have_solution;
}

unsigned astObj::clip_code(struct coordinate &p0) {
    return ((p0.y > window_LRY()) << 3) | ((p0.y < window_ULY()) << 2) | ((p0.x > window_LRX()) << 1) | (p0.x < window_ULX());
}

// using parametric form of line using two line segment endpoints, 
//   solve for (point on line) x when y is known...
bool astObj::solve_for_x(unsigned &xt, struct coordinate &p0,struct coordinate &p1, unsigned yt) {
    float t = float(yt - p0.y) / float(p1.y - p0.y);
    if ( (t < 0.0) || (t > 1.0) )
        return false; // no solution?
    xt = (unsigned) (p0.x + (p1.x - p0.x) * t);
    return true;
}
// using parametric form of line using two line segment endpoints, 
//    solve for (point on line) y when x is known...
bool astObj::solve_for_y(unsigned &yt, struct coordinate &p0,struct coordinate &p1, unsigned xt) {
   float t = float(xt - p0.x) / float(p1.x - p0.x);
    if ( (t < 0.0) || (t > 1.0) )
        return false; // no solution?
    yt = (unsigned) (p0.y + (p1.y - p0.y) * t);
    return true;
}

// draw using processed line segments

void astObj::Draw(bool erase) {
    for(auto ix = line_segments_screen.begin(); ix != line_segments_screen.end(); ix++) {
        int draw_color = erase ? BLACK : color;
        int draw_style = erase ? SOLID : style;
        // DrawLine((*ix).p0.x,(*ix).p0.y, (*ix).p1.x,(*ix).p1.y, draw_color,draw_style); // via lcd graphics api!
    }
}

    
