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
struct Vertex;
const int EPSILON = 0.01;

struct Triangle {
public:
    std::array<Vertex*, 3> vertices;
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3);
    Triangle(const Triangle& orig);
    virtual ~Triangle();
private:
};

struct Vertex {
public:
    double x;
    double y;
    double z;
    unsigned int index;
    std::vector<Triangle*> triangles;
    Vertex(double x, double y, double z, unsigned int i);
    Vertex(const Vertex& orig);
    virtual ~Vertex();

};

inline int orient(Vertex* v1, Vertex* v2, Vertex* v3) {
    double d = (v2->y - v1->y) * (v3->x - v2->x) - (v2->x - v1->x) * (v3->y - v2->y);
    if(fabs(d)<0.0001){
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

inline double dotprod(double ux, double uy, double uz, double vx, double vy, double vz) {
    return (ux * vx + uy * vy + uz * vz);
}

inline std::array<double, 3> intersectAscentAndTri(Vertex* s, double ui, double uj, double uk, Vertex* l, Vertex* r) {

    //vector w
    double wi = s->x - l->x;
    double wj = s->y - l->y;
    double wk = s->z - l->z;
    //nromal vector n
    double ni = l->y - r->y;
    double nj = r->x - l->x;
    double nk = 0;

    double D = dotprod(ni, nj, nk, ui, uj, uk);
    double N = -dotprod(ni, nj, nk, wi, wj, wk);

    double sI = N / D;
    return {s->x + sI*ui, s->y + sI*uj, s->z + sI*uk};
}
/*
inline std::array<double, 3> steepestAscent(Triangle* tri) {
    
    Vertex* v1 = tri->vertices[0];
    Vertex* v2 = tri->vertices[1];
    Vertex* v3 = tri->vertices[2];
    double a = v1->y * (v2->z - v3->z) + v2->y * (v3->z - v1->z) + v3->y * (v1->z - v2->z);
    double b = v1->z * (v2->x - v3->x) + v2->z * (v3->x - v1->x) + v3->z * (v1->x - v2->x);
    double c = v1->x * (v2->y - v3->y) + v2->x * (v3->y - v1->y) + v3->x * (v1->y - v2->y);
    //double d = -a*v1->x-b*v1->y-c*v1->z;
    return
    {
        -a/c, -b/c, 0
    };
     
    
}*/

#endif /* TRIANGLE_H */

