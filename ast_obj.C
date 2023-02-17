#include <ast_obj.h>
#include <iostream>
#include <bitset>
#include <cassert>
#include <exception>

bool operator== (const struct coordinate &p0, const struct coordinate &p1) {
        return (p0.x == p1.x) && (p0.y == p1.y);
}

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
        if (ClipLineSegment(p0,p1)) {
            line_segments_screen.push_back( line_segment(p0,p1) );
            std::cout << "\tp0 x,y: " << p0.x << "," << p0.y << " --> p1 x,y: " << p1.x << "," << p1.y << std::endl;
        }

        os0 = os1;
    }

    if (!at_origin)
        origin += trajectory_coord;   // we've moved on...

    at_origin = false;
}

// clip either end or both ends of line segment (if possible) to display screen,
// return true if there is a solution...
//
// My version of Cohen-Sutherland line clipping.
// See https://www.cs.montana.edu/courses/spring2009/425/dslectures/clipping.pdf
//
bool astObj::ClipLineSegment(struct coordinate &p0,struct coordinate &p1) {
    std::cout << "\t(ClipLineSegment)" << "p0 x,y: " << p0.x << "," << p0.y
            << " --> p1 x,y: " << p1.x << "," << p1.y << "???" << std::endl;
    // calculations assume line segment is ascending from p0 to p1...

    // NOTE: for our screen, origin is at upper left hand corner...

    unsigned code_p0 =  clip_code(p0);
    unsigned code_p1 =  clip_code(p1);

    unsigned clip_case = (code_p0<<4) | code_p1;

    std::bitset<4> code_p0_bits(code_p0);
    std::bitset<4> code_p1_bits(code_p1);
    std::bitset<8> clip_case_bits(clip_case);

    std::cout << "\tclip-case: 0x" <<std::hex << clip_case << std::dec  
            << "(p0: 0b" << code_p0_bits << " p1: 0b" << code_p1_bits << ")" << std::endl;

    if ( (code_p0 == code_p1) && (code_p0 == 0) ) {
        std::cout << "\tboth endpoints are within the (screen) window...\n" << std::endl;
        return true; // both endpoints are within the (screen) window...
    }

    if ( (code_p0 & code_p1) != 0 ) {
        std::cout << "\tthe line segment is completely outside the window...\n" << std::endl;
        return false; // the line segment is completely outside the window...
    }

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
    //    3    4       off   on      set p0.x to 0, p1 is good
    //    4    5       on    off     set p1.x to screen-width, p0 is good
    //    1    4       off   on      set p0.y to 0, p1 is good
    //    4    7       on    off     set p1.y to 0, p0 is good

    unsigned xt, yt;
    bool have_solution = true; // assume line can be clipped to screen

    // NOTE: comments reflect original implementation wherein:
    //      x,y == 0,0 - upper left hand coordinates of (lcd) screen
    //      x,y == 480,320 - lower right hand coordinates of (lcd) screen
    // A coordinate is 'good' if it is within the windows (screens) boundary
    switch( ( (code_p0<<4) | code_p1) ) {
        case 0x10: // 0001 0000: set p0.x to 0, p1 is good
            p0.x = 0;
            break;
        case 0x01: // 0000 0001: set p1.x to screen-width, p0 is good 
            p1.x = window_LRX();
            break;
        case 0x40: // 0100 0000: set p0.y to 0, p1 is good
            p0.y = 0;
            break;
        case 0x04: // 0000 0100: set p1.y to screen-height, p0 is good
            p1.y = window_LRY();
            break;
        case 0x50: // 0101 0000: solve for p0.x using p0.y == 0 or p0.y using p0.x == 0, p1 is good
            if (solve_for_x(xt,p0,p1,window_ULY())) {
                p0.x = xt;
            } else if (solve_for_y(yt,p0,p1,window_ULX())) {
                p0.y = yt;
            } else
                have_solution = false;
            break; 
        case 0x14: // 0001 0100: solve for p0.y using p0.x == 0, solve for p1.x using p1.y == 0
            have_solution = solve_for_y(yt,p0,p1,window_ULX()) && solve_for_x(xt,p0,p1,window_ULY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0x42: // 0100 0010: solve for p0.y using p0.x == screen-width, solve for p1.x using p1.y == 0
            have_solution = solve_for_y(yt,p0,p1,window_LRX()) && solve_for_x(xt,p0,p1,window_ULY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0x06: // 0000 0110: p0 is good, solve for p1.x using p1.y == 0 or p1.y using p1.x == 480
            if (solve_for_x(xt,p0,p1,window_ULY())) {
                p1.x = xt;
            } else if (solve_for_y(yt,p0,p1,window_LRX())) {
                p1.y = yt;
            } else
                have_solution = false;
            break;
        case 0x18: // 0001 1000: solve for p0.y using p0.x == 0, solve for p1.x using p1.y == screen-height
            have_solution = solve_for_y(yt,p0,p1,window_ULX()) && solve_for_x(xt,p0,p1,window_LRY());
            if (have_solution) {
                p0.y = yt;
                p1.x = xt;
            }
            break;
        case 0x90: // 1001 0000: solve for p0.y using p0.x == 0 or p0.x using p0.y == screen-height, p1 is good
            if (solve_for_y(yt,p0,p1,window_ULX())) {
                p0.y = yt;
            } else if (solve_for_x(xt,p0,p1,window_LRY())) {
                p0.x = xt;
            } else
                have_solution = false;  
            break;
        case 0x02: // 0000 0010: p0 is good, solve for p1.y using p1.x == screen-width or p1.x using p1.y == screen-height
            if (solve_for_y(yt,p0,p1,window_LRX())) {
                p1.y = yt;
            } else if (solve_for_x(xt,p0,p1,window_LRY())) {
                p1.x = xt;
            } else
                have_solution = false;  
            break;
        case 0x8a: // 1000 1010: solve for p0.x using p0.y == 320, solve for p1.y using p1.x == 480
            have_solution = solve_for_x(xt,p0,p1,window_LRY()) && solve_for_y(yt,p0,p1,window_LRX());
            if (have_solution) {
                p0.x = xt;
                p1.y = yt;
            }
            break;
        default: std::cerr << "Bad clip case!!!" << std::endl;
            throw std::exception();
        break;
    }

    // clipping one end or the other of a line segment may result in a zero length segment...

    if (have_solution && (p0 == p1)) {
        std::cout << "\tline clipping results in zero-length line..." << std::endl;
        have_solution = false;
    }
    
    // if there is a solution, make sure resulting clipped line is within the screen window...
    if (have_solution) {
        code_p0 =  clip_code(p0);
        code_p1 =  clip_code(p1);
        have_solution = (code_p0 == code_p1) && (code_p0 == 0);
    }

    if (have_solution) {
        std::cout << "\thave solution!\n" << std::endl;
    } else {
        std::cout << "\tno solution!\n" << std::endl;
    }

    return have_solution;
}

unsigned astObj::clip_code(struct coordinate &p0) {
    //std::cout << "\t[clip_code] x,y: " << p0.x << "," << p0.y 
    //<< " window LRX,LRY: " << window_ULX() << "," << window_ULY() 
    //<< " LRX,LRY: " << window_LRX() << "," << window_LRY() 
    //<< std::endl;
    return ((p0.y >= window_LRY()) << 3) | ((p0.y < window_ULY()) << 2) | ((p0.x >= window_LRX()) << 1) | (p0.x < window_ULX());
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

    
