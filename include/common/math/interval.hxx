#pragma once

struct Interval {
    float end, start;

    static float intersection_length(Interval int_a, Interval int_b);
};
