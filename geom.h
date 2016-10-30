/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Triangle.h
 * Author: elias
 *
 * Created on October 22, 2016, 8:34 PM
 */

#ifndef GEOM_H
#define GEOM_H
#include <array>
#include <math.h>
#include <iostream>
#include <algorithm>
struct Vertex;
const double EPSILON = 0.01;

struct Triangle {
public:
    std::array<Vertex*, 3> vertices;
    Vertex* drainsTo = 0;
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3);
    Triangle(const Triangle& orig);
    virtual ~Triangle();
    std::array<double, 3> centroid();
private:
};

struct Vertex {
public:
    double x;
    double y;
    double z;
    std::vector<Vertex*> upstream;
    std::vector<Vertex*> downstream;
    unsigned int index;
    std::vector<Triangle*> triangles;
    Vertex(double x, double y, double z, unsigned int i);
    Vertex(const Vertex& orig);
    virtual ~Vertex();

};

struct StreamNode {
public:
    std::vector<StreamNode*> upstream;
    std::vector<StreamNode*> downstream;
    Vertex* v;

    friend bool operator<(const StreamNode& l, const StreamNode& r) {
        return l.v < r.v; // keep the same order
    }

    friend bool operator>(const StreamNode& l, const StreamNode& r) {
        return l.v > r.v; // keep the same order
    }

    friend bool operator==(const StreamNode& l, const StreamNode& r) {
        return l.v == r.v; // keep the same order
    }
    StreamNode(std::vector<StreamNode*>, std::vector<StreamNode*>, Vertex*);
};



inline int orient(std::array<double,3> v1, std::array<double,3> v2, std::array<double,3> v3) {
    double d = (v2[1] - v1[1]) * (v3[0] - v2[0]) - (v2[0] - v1[0]) * (v3[1] - v2[1]);
    if (fabs(d)<EPSILON) {
        return 0;
    }
    if (d < 0) {
        return -1;
    }
    if (d > 0) {
        return 1;
    }
    return 0;
}
inline int orient(Vertex* v1, Vertex* v2, Vertex* v3) {
    if(v1==0 or v2==0 or v3==0){
        std::cout<<"V==0\n";
        exit(1);
    }
    return orient({v1->x, v1->y, v1->z},{v2->x, v2->y, v2->z},{v3->x, v3->y, v3->z});
}
inline std::array<double, 3> steepestAscent(Triangle* tri) {

    Vertex* v1 = tri->vertices[0];
    Vertex* v2 = tri->vertices[1];
    Vertex* v3 = tri->vertices[2];
    double a = v1->y * (v2->z - v3->z) + v2->y * (v3->z - v1->z) + v3->y * (v1->z - v2->z);
    double b = v1->z * (v2->x - v3->x) + v2->z * (v3->x - v1->x) + v3->z * (v1->x - v2->x);
    double c = v1->x * (v2->y - v3->y) + v2->x * (v3->y - v1->y) + v3->x * (v1->y - v2->y);
    double d = -a * v1->x - b * v1->y - c * v1->z;

    double x = v1->x - a / c;
    double y = v1->y - b / c;
    double z = -a * x / c - b * y / c - d / c - v1->z;

    return
    {
        -a / c, -b / c, z
    };
}

inline void sortSplitters(std::vector<Vertex*>* splitters, Vertex* centre) {

    struct cmp {
        Vertex* c;
        cmp(Vertex* c){
            this->c = c;
        };
        bool operator()(Vertex* a, Vertex* b) {
            return orient(c, a, b) == 1;
        }
    };
    std::sort(splitters->begin(), splitters->end(),cmp(centre));
}

inline std::array<double, 3> steepestDescent(Triangle* tri) {
    std::array<double, 3> asc = steepestAscent(tri);
    return
    {
        -asc[0], -asc[1], -asc[2]
    };
}

inline double dotprod(double ux, double uy, double uz, double vx, double vy, double vz) {
    return (ux * vx + uy * vy + uz * vz);
}


inline std::array<double, 3> intersectRayPlane(std::array<double, 3> s, double ui, double uj, double uk, Vertex* l, Vertex* r) {

    //vector w
    double wi = s[0] - l->x;
    double wj = s[1] - l->y;
    double wk = s[2] - l->z;
    //nromal vector n
    double ni = l->y - r->y;
    double nj = r->x - l->x;
    double nk = 0;

    double D = dotprod(ni, nj, nk, ui, uj, uk);
    double N = -dotprod(ni, nj, nk, wi, wj, wk);

    double sI = N / D;
    return
    {
        s[0] + sI*ui, s[1] + sI*uj, s[2] + sI * uk
    };
}


inline std::array<double, 3> intersectRayPlane(Vertex* s, double ui, double uj, double uk, Vertex* l, Vertex* r) {
    return intersectRayPlane({s->x,s->y,s->z},ui,uj,uk,l,r);
}

#endif /* TRIANGLE_H */

