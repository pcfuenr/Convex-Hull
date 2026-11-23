#ifndef DNC_CPP
#define DNC_CPP

#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

namespace dnc {

const double EPS = 1e-9;

// returns the cross product of the vectors a -> b and a -> c,
// positive if c is on the left of a -> b, negative if on the right
double cross(const vector<double>& a, const vector<double>& b, const vector<double>& c) {
    return (b[0] - a[0]) * (c[1] - a[1]) -
           (b[1] - a[1]) * (c[0] - a[0]);
}

// returns true if point a comes before point b, comparing x first and then y
bool isSmaller(const vector<double>& a, const vector<double>& b) {
    return a[0] < b[0] || (a[0] == b[0] && a[1] < b[1]);
}

// merges the hulls of two groups of points, where every point of left comes before
// every point of right, and both hulls are in counter-clockwise order
vector<vector<double>> mergeHulls(const vector<vector<double>>& left, const vector<vector<double>>& right) {
    int nL = left.size(), nR = right.size();

    // two small hulls can lie on one line, so their hull is just the two end points
    if (nL <= 2 && nR <= 2) {
        vector<vector<double>> all = left;
        all.insert(all.end(), right.begin(), right.end());
        vector<double> lo = all[0], hi = all[0];
        for (auto& p : all) {
            if (isSmaller(p, lo))
                lo = p;
            if (isSmaller(hi, p))
                hi = p;
        }
        bool collinear = true;
        for (auto& p : all) {
            if (fabs(cross(lo, hi, p)) > EPS)
                collinear = false;
        }
        if (collinear)
            return {lo, hi};
    }

    // start from the rightmost point of the left hull and the leftmost point of the right hull
    int ia = 0, ib = 0;
    for (int i = 1; i < nL; i++) {
        if (isSmaller(left[ia], left[i]))
            ia = i;
    }
    for (int i = 1; i < nR; i++) {
        if (isSmaller(right[i], right[ib]))
            ib = i;
    }

    // upper tangent: move a counter-clockwise and b clockwise while the next point is above the line a -> b
    int a = ia, b = ib;
    bool moved = true;
    while (moved) {
        moved = false;
        while (cross(left[a], right[b], left[(a + 1) % nL]) > EPS) {
            a = (a + 1) % nL;
            moved = true;
        }
        while (cross(left[a], right[b], right[(b - 1 + nR) % nR]) > EPS) {
            b = (b - 1 + nR) % nR;
            moved = true;
        }
    }
    int upperA = a, upperB = b;

    // lower tangent: move a clockwise and b counter-clockwise while the next point is below the line a -> b
    a = ia;
    b = ib;
    moved = true;
    while (moved) {
        moved = false;
        while (cross(left[a], right[b], left[(a - 1 + nL) % nL]) < -EPS) {
            a = (a - 1 + nL) % nL;
            moved = true;
        }
        while (cross(left[a], right[b], right[(b + 1) % nR]) < -EPS) {
            b = (b + 1) % nR;
            moved = true;
        }
    }
    int lowerA = a, lowerB = b;

    // the left hull from the upper tangent point to the lower tangent point,
    // followed by the right hull from the lower tangent point to the upper tangent point
    vector<vector<double>> merged;
    int li = upperA;
    merged.push_back(left[li]);
    while (li != lowerA) {
        li = (li + 1) % nL;
        merged.push_back(left[li]);
    }
    int ri = lowerB;
    merged.push_back(right[ri]);
    while (ri != upperB) {
        ri = (ri + 1) % nR;
        merged.push_back(right[ri]);
    }

    // remove the points that are collinear with their neighbours
    int m = merged.size();
    vector<vector<double>> hull;
    for (int k = 0; k < m; k++) {
        if (fabs(cross(merged[(k - 1 + m) % m], merged[k], merged[(k + 1) % m])) > EPS)
            hull.push_back(merged[k]);
    }

    return hull;
}

// returns the hull of pts[lo..hi) in counter-clockwise order, where pts is sorted by x and then by y
vector<vector<double>> buildHull(const vector<vector<double>>& pts, int lo, int hi) {

    // a single point is its own hull
    if (hi - lo == 1)
        return {pts[lo]};

    // divide the points into two halves, and merge the hulls of the halves
    int mid = (lo + hi) / 2;
    return mergeHulls(buildHull(pts, lo, mid), buildHull(pts, mid, hi));
}

// Time Complexity: O(n log n), Auxiliary Space: O(n)
vector<vector<double>> findConvexHull(vector<vector<double>>& points) {
    int n = points.size();

    // a polygon needs at least 3 points
    if (n < 3)
        return {};

    // sort the points by x, and by y when x is equal, removing duplicates
    vector<vector<double>> pts = points;
    sort(pts.begin(), pts.end());
    pts.erase(unique(pts.begin(), pts.end()), pts.end());

    if (pts.size() < 3)
        return {};

    vector<vector<double>> hull = buildHull(pts, 0, pts.size());

    // fewer than 3 corners means all the points are collinear
    if (hull.size() < 3)
        return {};

    return hull;
}

} 

#endif