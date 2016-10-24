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
    void connectTriangles(unsigned int t1, unsigned int t2);
    void addTriangle(unsigned int i1, unsigned int i2, unsigned int i3);
    void delineateStreams(SortedVector<std::pair<Vertex*, Vertex*>>* streams);
    void travelUpstream(Vertex* v, std::vector<Vertex*>* stack, SortedVector<std::pair<Vertex*, Vertex*>>* streams);
    void locateSinks(std::vector<Vertex*>* stack);
    std::vector<Vertex*> getFaceAscentDirections();
    void steepestPath(Vertex* s, Vertex* c1, Vertex* c2);
    int getCurrentIndex(Triangle* t, Vertex* s);
    
};

#endif /* TIN_H */

