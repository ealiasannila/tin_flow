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
struct Vertex;
const int EPSILON = 0.00001;

struct Triangle {
public:
    std::vector<Triangle*> neighbours;
    std::array<Vertex*, 3> vertices;
    Triangle(Vertex* v1, Vertex* v2, Vertex* v3);
    Triangle(const Triangle& orig);
    virtual ~Triangle();
    void addNeighbour(Triangle* t);
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
    if (d + EPSILON < 0) {
        return -1;
    }
    if (d - EPSILON > 0) {
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
    double d = -a*v1->x-b*v1->y-c*v1->z;
    
    double x =v1->x -a/c;
    double y =v1->y -b/c;
    double z = -a*x/c - b*y/c -d/c - v1->z;
    
    return
    {
        -a/c, -b/c, z
    };
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

