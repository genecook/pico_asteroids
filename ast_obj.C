#include <display.h>
#include <ast_obj.h>
#include <iostream>
#include <bitset>
#include <cassert>
#include <exception>

//#define ASTOBJ_DEBUG

extern void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style);

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
#ifdef ASTOBJ_DEBUG
    for(auto ix = line_segments_screen.begin(); ix != line_segments_screen.end(); ix++) {
        std::cout << "p0 x,y: " << (*ix).p0.x << "," << (*ix).p0.y
            << " --> p1 x,y: " << (*ix).p1.x << "," << (*ix).p1.y << std::endl;
    }
#endif
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

        struct coordinate p0 = os0, p1 = os1;

        if (ClipLineSegment(p0,p1)) {
            line_segments_screen.push_back( line_segment(p0,p1) );
#ifdef ASTOBJ_DEBUG
            std::cout << "\tp0 x,y: " << p0.x << "," << p0.y << " --> p1 x,y: " << p1.x << "," << p1.y << "\n" << std::endl;
#endif
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
// the display screen boundaries are 'mapped' to 'tic-tac-to' squares as so:
//
//     0 | 1 | 2
//     --+---+--
//     3 | 4 | 5     4 - on-screen, all other squares off-screen
//     --+---+--
//     6 | 7 | 8

/*
                                                     actions...
                                                     
                    case                                             snap p0 or p1 to window x or y boundary...                       solve for...         
       ----------------------------------    ----------------------------------------------------------------------------------   ----------------------
        p0  p1   p0.x==p1.x?  p0.y==p1.y?    p0.x->ULX p0.y->ULY  p0.x->LRX p0.y->LRY  p1.x->LRX p1.y->LRY  p1.x->ULX p1.y->ULY   p0.x  p0.y  p1.x  p1.y         notes
       ---  ---  -----------  -----------    --------- ---------  --------- ---------  --------- ---------  --------- ---------   ----  ----  ----  ----      -------------------
        3    4       0             0            1           0         0         0          0         0          0        0         0     1     0     0    
                     0             1            1           0         0         0          0         0          0        0         0     0     0     0         horizontal line

        3    5       0             0            1           0         0         0          1         0          0        0         0     1     0     1    
                     0             1            1           0         0         0          1         0          0        0         0     0     0     0         horizontal line

        4    5       0             0            0           0         0         0          1         0          0        0         0     0     0     1    
                     0             1            0           0         0         0          1         0          0        0         0     0     0     0         horizontal line


        1    4       0             0            0           1         0         0          0         0          0        0         1     0     0     0    
                     1             0            0           1         0         0          0         0          0        0         0     0     0     0         vertical line

        1    7       0             0            0           1         0         0          0         1          0        0         1     0     1     0      
                     1             0            0           1         0         0          0         1          0        0         0     0     0     0         vertical line

        4    7       0             0            0           0         0         0          0         1          0        0         0     0     1     0     
                     1             0            0           0         0         0          0         1          0        0         0     0     0     0         vertical line


        0    4       0             0            1           0         0         0          0         0          0        0         0     1     0     0     

        0    8       0             0            1           0         0         0          0         0          0        0         0     1     0     0         solve for p0...     
                                                0           0         0         0          1         0          0        0         0     0     0     1            ...then p1   

        4    8       0             0            0           0         0         0          1         0          0        0         0     0     0     1      


        1    5       0             0            0           1         0         0          0         0          0        0         1     0     0     0         solve p0...
                                                0           0         0         0          1         0          0        0         0     0     0     1            ...then p1

        3    7       0             0            1           0         0         0          0         0          0        0         0     1     0     0         solve p0...
                                                0           0         0         0          0         1          0        0         0     0     1     0            ...then p1


        2    4       0             0            0           0         1         0          0         0          0        0         0     1     0     0      

        2    6       0             0            0           0         1         0          0         0          0        0         0     1     0     0         solve p0...
                                                0           0         0         0          0         0          1        0         0     0     0     1            ...then p1

        4    6       0             0            0           0         0         0          0         0          1        0         0     0     0     1     


        1    3       0             0            0           1         0         0          0         0          0        0         1     0     0     0         solve p0...
                                                0           0         0         0          0         0          1        0         0     0     0     1            ...then p1 

        5    7       0             0            0           0         1         0          0         0          0        0         0     1     0     0         solve p0...
                                                0           0         0         0          1         0          0        0         0     0     0     1            ...then p1
*/

bool astObj::ClipLineSegment(struct coordinate &p0,struct coordinate &p1) {
#ifdef ASTOBJ_DEBUG
    std::cout << "\t(ClipLineSegment)" << "p0 x,y: " << p0.x << "," << p0.y
            << " --> p1 x,y: " << p1.x << "," << p1.y << "???" << std::endl;
#endif

    // NOTE: for our screen, origin is at upper left hand corner...
    // calculations assume line segment is ascending from p0 to p1...
   
    unsigned code_p0 =  clip_code(p0);
    unsigned code_p1 =  clip_code(p1);
    
    if (flip_line_segment(code_p0,code_p1)) {
        struct coordinate px = p0;
        p0 = p1;
        p1 = px;
        code_p0 =  clip_code(p0);
        code_p1 =  clip_code(p1);
    }

    unsigned clip_case = (code_p0<<4) | code_p1;

    std::bitset<4> code_p0_bits(code_p0);
    std::bitset<4> code_p1_bits(code_p1);
    std::bitset<8> clip_case_bits(clip_case);

#ifdef ASTOBJ_DEBUG
    std::cout << "\tclip-case: 0x" <<std::hex << clip_case << std::dec  
            << "(p0: 0b" << code_p0_bits << " p1: 0b" << code_p1_bits << ")" << std::endl;
#endif
    if ( (code_p0 == code_p1) && (code_p0 == 0) ) {
#ifdef ASTOBJ_DEBUG
        std::cout << "\tboth endpoints are within the (screen) window...\n" << std::endl;
#endif
        return true; // both endpoints are within the (screen) window...
    }

    if ( (code_p0 & code_p1) != 0 ) {
#ifdef ASTOBJ_DEBUG
        std::cout << "\tthe line segment is completely outside the window...\n" << std::endl;
#endif
        return false; // the line segment is completely outside the window...
    }

    // the line may or may not cross the window...

    unsigned xt, yt;
    bool have_solution = true; // assume line can be clipped to screen

    // NOTE: comments reflect original implementation wherein:
    //      x,y == 0,0 - upper left hand coordinates of (lcd) screen
    //      x,y == 480,320 - lower right hand coordinates of (lcd) screen
    // A coordinate is 'good' if it is within the windows (screens) boundary

    unsigned int clip_action = (grid_index(code_p0)<<12) | (grid_index(code_p1)<<8) | ((p0.x==p1.x)<<4) | (p0.y==p1.y);
#ifdef ASTOBJ_DEBUG
    std::cout << "\tclip action: 0x" << std::hex << clip_action << std::dec << std::endl;
#endif
    switch( (int) clip_action) {
        case 0x3400: have_solution = solve_for_y(yt,p0,p1,window_ULX());
                     if (have_solution) {
                        p0.x = window_ULX(); 
                        p0.y = yt; 
                     }
                     break;  

        case 0x3401: p0.x = window_ULX(); 
                     break;

        case 0x3500: have_solution = solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p0.x = window_ULX(); 
                        p0.y = yt;
                     } 
                     have_solution &= solve_for_y(yt,p0,p1,window_LRX()); 
                     if (have_solution) {
                        p1.x = window_LRX();
                        p1.y = yt; 
                     }
                     break; 

        case 0x3501: p0.x = window_ULX();
                     p1.x = window_LRX();
                     break;

        case 0x4500: have_solution = solve_for_y(yt,p0,p1,window_LRX()); 
                     if (have_solution) {
                        p1.x = window_LRX();
                        p1.y = yt;
                     }
                     break;

        case 0x4501: p1.x = window_LRX();
                     break;

        case 0x1400: have_solution = solve_for_x(xt,p0,p1,window_ULY());
                     if (have_solution) {
                        p0.x = xt;
                        p0.y = window_ULY();
                     }
                     break;

        case 0x1410: p0.y = window_ULY();
                     break;

        case 0x1700: have_solution = solve_for_x(xt,p0,p1,window_ULY());
                     if (have_solution) {
                        p0.x = xt;
                        p0.y = window_ULY();
                     }
                     have_solution &= solve_for_x(xt,p0,p1,window_LRY());
                     if (have_solution) {
                        p1.x = xt;
                        p1.y = window_LRY();
                     }
                     break;

        case 0x1710: p0.y = window_ULY();
                     p1.y = window_LRY();
                     break;
 
        case 0x4700: have_solution &= solve_for_x(xt,p0,p1,window_LRY());
                     if (have_solution) {
                        p1.x = xt;
                        p1.y = window_LRY();
                     }
                     break;

        case 0x4710: p1.y = window_LRY();
                     break;

        case 0x0400: have_solution = solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p0.x = window_ULX();
                        p0.y = yt;
                     }
                     break;

        case 0x0800: have_solution = solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p0.x = window_ULX();
                        p0.y = yt;
                     }
                     have_solution &= solve_for_y(yt,p0,p1,window_LRX());
                     if (have_solution) {
                        p1.x = window_LRX();
                        p1.y = yt;
                     }
                     break;

        case 0x4800: have_solution = solve_for_y(yt,p0,p1,window_LRX());
                     if (have_solution) {
                        p1.x = window_LRX();
                        p1.y = yt;
                     }
                     break;

        case 0x1500: have_solution = solve_for_x(xt,p0,p1,window_ULY());
                     if (have_solution) {
                        p0.x = xt;
                        p0.y = window_ULY();
                     }
                     have_solution &= solve_for_y(yt,p0,p1,window_LRX());
                     if (have_solution) {
                        p1.x = window_LRX();
                        p1.y = yt;
                     }
                     break;

        case 0x3700: have_solution = solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p0.x = window_ULX();
                        p0.y = yt;
                     }
                     have_solution &= solve_for_x(xt,p0,p1,window_LRY());
                     if (have_solution) {
                        p1.x = xt;
                        p1.y = window_LRY();
                     }
                     break;

        case 0x2400: have_solution = solve_for_y(yt,p0,p1,window_LRX()); 
                     if (have_solution) {
                        p0.x = window_LRX();
                        p0.y = yt;
                     }
                     break;

        case 0x2600: have_solution = solve_for_y(yt,p0,p1,window_LRX()); 
                     if (have_solution) {
                        p0.x = window_LRX();
                        p0.y = yt;
                     }
                     have_solution &= solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p1.x = window_ULX();
                        p1.y = yt;
                     }
                     break;

        case 0x4600: have_solution = solve_for_y(yt,p0,p1,window_ULX()); 
                     if (have_solution) {
                        p1.x = window_ULX();
                        p1.y = yt;
                     }
                     break;

        case 0x1300: have_solution = solve_for_x(xt,p0,p1,window_ULY());
                     if (have_solution) {
                        p0.x = xt;
                        p0.y = window_ULY();
                     }
                     have_solution &= solve_for_y(yt,p0,p1,window_ULX());
                     if (have_solution) {
                        p1.x = window_ULX();
                        p1.y = yt;
                     }
                     break;

        case 0x5700: have_solution = solve_for_y(yt,p0,p1,window_LRX()); 
                     if (have_solution) {
                        p0.x = window_LRX();
                        p0.y = yt;
                     }
                     have_solution &= solve_for_x(xt,p0,p1,window_LRY()); 
                     if (have_solution) {
                        p1.x = xt;
                        p1.y = window_LRY();
                     }
                     break;

        default: 
#ifdef ASTOBJ_DEBUG
            std::cerr << "Bad clip action: 0x" << std::hex << (int) clip_action << std::dec << "!!!" << std::endl;
            throw std::exception();
#endif
            break;
    }

    // clipping one end or the other of a line segment may result in a zero length segment...

    if (have_solution && (p0 == p1)) {
#ifdef ASTOBJ_DEBUG
        std::cout << "\tline clipping results in zero-length line..." << std::endl;
#endif
        have_solution = false;
    }
    
    // if there is a solution, make sure resulting clipped line is within the screen window...
    if (have_solution) {
        code_p0 =  clip_code(p0);
        code_p1 =  clip_code(p1);
        have_solution = (code_p0 == code_p1) && (code_p0 == 0);
        if (!have_solution) {
#ifdef ASTOBJ_DEBUG
            std::cout << "\tafter clipping, no solution. p0? " << ((code_p0 == 0) ? "yes" : "no")
                << ", p1? " << ((code_p1 == 0) ? "yes" : "no") << std::endl;
#endif
        }
    }
#ifdef ASTOBJ_DEBUG
    if (have_solution) {
        std::cout << "\thave solution!" << std::endl;
    } else {
        std::cout << "\tno solution!\n" << std::endl;
    }
#endif

    return have_solution;
}

unsigned astObj::clip_code(struct coordinate &p0) {
    //std::cout << "\t[clip_code] x,y: " << p0.x << "," << p0.y 
    //<< " window LRX,LRY: " << window_ULX() << "," << window_ULY() 
    //<< " LRX,LRY: " << window_LRX() << "," << window_LRY() 
    //<< std::endl;
    return ((p0.y >= window_bounds_LRY()) << 3) | ((p0.y < window_bounds_ULY()) << 2) | ((p0.x >= window_bounds_LRX()) << 1) | (p0.x < window_bounds_ULX());
}

/*
                     pX                          
    ------------------------------------    grid square
    y >= 320    y < 0   x >= 480   x < 0    -----------
       0          0         0        0          4      
       0          0         0        1          3
       0          0         1        0          5
       0          0         1        1       invalid
       0          1         0        0          1
       0          1         0        1          0  
       0          1         1        0          2   
       0          1         1        1       invalid     
       1          0         0        0          7
       1          0         0        1          6 
       1          0         1        0          8  
       1          0         1        1       invalid 
       1          1         0        0       invalid     
       1          1         0        1       invalid     
       1          1         1        0       invalid     
       1          1         1        1       invalid
*/

int astObj::grid_index(unsigned int clip_code) {
    int ci = -1;
    switch((int) clip_code) {
        case 0b0000: ci = 4; break;
        case 0b0001: ci = 3; break;
        case 0b0010: ci = 5; break;
        case 0b0100: ci = 1; break;
        case 0b0101: ci = 0; break;
        case 0b0110: ci = 2; break;
        case 0b1000: ci = 7; break;
        case 0b1001: ci = 6; break;
        case 0b1010: ci = 8; break;
        default: 
#ifdef ASTOBJ_DEBUG
            std::cerr << "Bad grid case!!!" << std::endl;
            throw std::exception();
#endif
            break;
    } 
    return ci;
}

int astObj::grid_square(struct coordinate p) {
    return grid_index(clip_code(p));
}

bool astObj::flip_line_segment(unsigned int code_p0, unsigned int code_p1) {
    switch( (grid_index(code_p0) << 4) | grid_index(code_p1) ) {
    case 0x54:
    case 0x74: 
    case 0x84:
    case 0x75:
    case 0x43:
    case 0x41:
    case 0x31:
    case 0x40:
    case 0x42:
    case 0x73:
    case 0x64:
    case 0x51:
        return true; break;
    
    default: break;
    }
    return false;
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
        DrawLine((*ix).p0.x,(*ix).p0.y, (*ix).p1.x,(*ix).p1.y, draw_color,draw_style); // via lcd graphics api!
    }
}

    
