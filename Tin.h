/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Tin.h
 * Author: elias
 *
 * Created on October 22, 2016, 3:45 PM
 */

#ifndef TIN_H
#define TIN_H
#include <vector>
#include <map>
#include <array>
#include "sorted_vector.h"
#include "geom.h"

struct Tin {
public:
    std::vector<Vertex*> vertices;
    std::vector<Triangle*> triangles;

    Tin();
    Tin(const Tin& orig);
    virtual ~Tin();
    void addVertex(double x, double y, double z);
    void addTriangle(Vertex* v1, Vertex* v2, Vertex* v3);
    void addTriangle(unsigned int i1, unsigned int i2, unsigned int i3);
    void swapTriangle(Triangle* t);
    void delineateStreams(SortedVector<std::pair<Vertex*, Vertex*>>* streams);
    void travelUpstream(Vertex* v, std::vector<Vertex*>* stack, SortedVector<std::pair<Vertex*, Vertex*>>* streams);
    void locateSinks(std::vector<Vertex*>* stack);
    std::vector<Vertex*> getFaceAscentDirections();
    std::vector<std::vector<Vertex*>> steepestPathFromStream(Vertex* s, std::vector<Vertex*> splitters);
    Vertex* travelUp(Vertex* s, Vertex* cl, Vertex* cr);
    int getVertexIndex(Triangle* t, Vertex* s);
    int getTriangleIndex(Triangle* t, Vertex* s);
    std::pair<Vertex*,Triangle*> locateOpposite(Vertex* l, Vertex* r, Vertex* s);
    Vertex* fixTriangles(Triangle* t, Vertex* s, Vertex* l, Vertex* r,std::array<double,3> n);
    void adjustTriangle(Triangle* t, Vertex* o, Vertex* n);
    void newTriangle(Vertex* v, Vertex* n, Vertex* o,Triangle* ot, int orient);
};

#endif /* TIN_H */

