#pragma once
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>

namespace snake_game {

    enum class SnakeDirs { right = 0, down = 1, left = 2, up = 3,};

    struct Point {
        int x, y;
        bool operator< (const Point& p) const { return x + y * 1000 < p.x + p.y * 1000; } // We assume all points in this game have coordinates under 1000
        bool operator== (const Point& p) const { return ((x == p.x) && (y == p.y)); }
        bool operator!= (const Point& p) const { return !(*this == p); }
        Point& operator+= (const Point& p);
        Point& operator%= (const Point& p);
    };
    Point operator+ (const Point& p, const Point& q);
    Point operator% (const Point& p, const Point& q);

    extern std::map<SnakeDirs, Point> dir_to_point; //used in SnakeGame as well

    class Snake {
    public:
        Snake(Point, int);

        bool setDir(SnakeDirs new_dir);
        void move(Point, bool);
        std::set<Point> eatItself(Point);

        int getSize() const { return size; }
        bool isBody(Point) const;
        SnakeDirs getDir() const { return dir; }

        Point getHead() const { return body.front(); }
        Point getTail() const { return body.back(); }

    private:

        std::deque<Point> body;
        std::set<Point> body_set;

        int size;
        SnakeDirs dir{ SnakeDirs::right };
        SnakeDirs last_move_dir{ SnakeDirs::right };
    };

}