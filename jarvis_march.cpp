// Algoritmo extraído de GeeksForGeeks https://www.geeksforgeeks.org/dsa/convex-hull-using-jarvis-algorithm-or-wrapping/
#ifndef JARVIS_MARCH_CPP
#define JARVIS_MARCH_CPP

#include <vector>
using namespace std;

namespace jarvis {

// returns the cross product of the vectors a -> b and a -> c,
// positive if c is on the left of a -> b, negative if on the right
long long cross(const vector<int>& a, const vector<int>& b, const vector<int>& c) {
    return (long long)(b[0] - a[0]) * (c[1] - a[1]) -
           (long long)(b[1] - a[1]) * (c[0] - a[0]);
}

// returns the squared distance between the points a and b
long long distSq(const vector<int>& a, const vector<int>& b) {
    return (long long)(a[0] - b[0]) * (a[0] - b[0]) +
           (long long)(a[1] - b[1]) * (a[1] - b[1]);
}

// Time Complexity: O(n * h), Auxiliary Space: O(h), where h is the number of corners
vector<vector<int>> findConvexHull(vector<vector<int>>& points) {
    int n = points.size();

    // a polygon needs at least 3 points
    if (n < 3)
        return {};

    // the leftmost point (the lowest one when x is equal) is always a corner
    int start = 0;
    for (int i = 1; i < n; i++) {
        if (points[i][0] < points[start][0] ||
            (points[i][0] == points[start][0] && points[i][1] < points[start][1]))
            start = i;
    }

    vector<vector<int>> hull;
    int p = start;

    do {
        hull.push_back(points[p]);

        // take any other point as the candidate for the next corner
        int q = (p + 1) % n;

        for (int r = 0; r < n; r++) {
            long long c = cross(points[p], points[q], points[r]);

            // r is on the right of p -> q, so q cannot be the next corner
            if (c < 0)
                q = r;

            // r is collinear with p and q, so keep the farthest point from p
            else if (c == 0 && distSq(points[p], points[r]) > distSq(points[p], points[q]))
                q = r;
        }

        p = q;
    } while (p != start);

    // fewer than 3 corners means all the points are collinear
    if (hull.size() < 3)
        return {};

    return hull;
}

} 

#endif
