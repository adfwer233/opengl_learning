#include "common/math/interval.hxx"
#include <algorithm>

float Interval::intersection_length(Interval int_a, Interval int_b) {
    float min = std::max(int_a.start, int_b.start);
    float max = std::min(int_a.end, int_b.end);

    return max - min;
}