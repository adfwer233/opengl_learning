#pragma once

struct Interval {
    float start, end;

    static float intersection_length(Interval int_a, Interval int_b);
};
