#include "Snake.h"

/// Snake

using namespace snake_game;

std::map<SnakeDirs, Point> snake_game::dir_to_point{ {SnakeDirs::left, {-1,0}}, {SnakeDirs::right, {1,0}}, {SnakeDirs::up, {0,-1}}, {SnakeDirs::down, {0,1}} };

Point& Point::operator+= (const Point& p) {
    x += p.x;
    y += p.y;
    return *this;
}
Point snake_game::operator+ (const Point& p, const Point& q) { return Point{ p.x + q.x, p.y + q.y }; }

//% operators give only nonnegative points and are intended to be used with positive argument
Point& Point::operator%= (const Point& p) {
    x = (x < 0) ? (x + p.x) : (x % p.x);
    y = (y < 0) ? (y + p.y) : (y % p.y);
    return *this;
}
Point snake_game::operator% (const Point& p, const Point& q) { return Point{ (p.x < 0) ? (p.x + q.x) : (p.x % q.x), (p.y < 0) ? (p.y + q.y) : (p.y % q.y) }; }

Snake::Snake(Point head_pos, int start_size = 3) : size{ start_size } {
    for (int i = start_size - 1; i >= 0; --i) {
        body.push_front(Point{ head_pos.x - i, head_pos.y });
        body_set.insert(Point{ head_pos.x - i, head_pos.y });
    }
}

bool Snake::setDir(SnakeDirs new_dir) {
    //If dirs are "adjacent", new dir can be set,
    if (abs(static_cast<int>(last_move_dir) - static_cast<int>(new_dir)) % 2 == 1) {
        dir = new_dir;
        return true;
    }
    //otherwise (if they are the same or opposite), they cannot be set this step but may be saved for the next one
    else
        return false;
}

std::set<Point> Snake::eatItself(Point cutoff) {
    std::set<Point> dead_parts;
    while (body.back() != cutoff) {
        dead_parts.insert(body.back());
        body_set.erase(body.back());
        body.pop_back();
        --size;
    }
    return dead_parts;
}

void Snake::move(Point new_head, bool ate_food) {

    last_move_dir = dir;

    if (ate_food)
        ++size;
    else {
        body_set.erase(body.back());
        body.pop_back();
    }

    body.push_front(new_head);
    body_set.insert(new_head);

}

bool Snake::isBody(Point p) const {
    return (body_set.find(p) != body_set.end());
}
