#include <iostream>
#include "vector.h"
#include "egorvector.h"

using namespace vct;

struct Point {
    int x = 0;
    int y = 0;

    Point() = default;

    Point(int x, int y): x(x), y(y) {}

    bool operator==(const Point& p) const {
        return x == p.x && y == p.y;
    }

    bool operator<(const Point& p) const {
        return get_length() < p.get_length();
    }

    Point(const Point& p) = default;

    Point(Point&& p) = default;

private:

    [[nodiscard]] int get_length() const noexcept {
        return x * x + y * y;
    }
};

int main() {

    vector<Point> vec2;
    vec2.push_back(Point(1, 2));
}
