/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Triangle.cpp
 * Author: elias
 * 
 * Created on October 22, 2016, 8:34 PM
 */

#include <vector>
#include <iostream>
#include "geom.h"
using namespace std;


Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
    int o = orient(v1, v2, v3);
    if (o == 0) {
        cout << "COLINEAR TRIANGLE!\n";
        exit(1);
    } else if (o == -1) {
        this->vertices[0] = v1;
        this->vertices[1] = v2;
        this->vertices[2] = v3;

    } else {
        this->vertices[0] = v1;
        this->vertices[2] = v2;
        this->vertices[1] = v3;

    }
}

Triangle::Triangle(const Triangle& orig) {

}

Triangle::~Triangle() {
}

void Triangle::addNeighbour(Triangle* t) {
    this->neighbours.push_back(t);
}

Vertex::Vertex(double x, double y, double z, unsigned int i) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->index = i;
}

Vertex::Vertex(const Vertex& orig) {
}

Vertex::~Vertex() {
}


