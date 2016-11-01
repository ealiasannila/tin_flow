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
#include <array>
using namespace std;

Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3) {
    int o = orient(v1, v2, v3);
    if (o == 0) {
        cout << v1->index << " " << v2->index << " "<<v3->index << endl;
        cout << v1->x << " " << v3->x << " " << v3->x << endl;
        cout << v1->y << " " << v3->y << " " << v3->y << endl;
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

std::array<double, 3> Triangle::centroid() {
    double x = 0, y = 0, z = 0;
    for (int i = 0; i < 3; i++) {
        x += this->vertices[i]->x;
        y += this->vertices[i]->y;
        z += this->vertices[i]->z;
    }
    return
    {
        x / 3, y / 3, z / 3
    };

}

Triangle::Triangle(const Triangle& orig) {

}

Triangle::~Triangle() {
}

Vertex::Vertex(double x, double y, double z, unsigned int i) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->index = i;
}

StreamNode::StreamNode(std::vector<StreamNode*> up, std::vector<StreamNode*> down, Vertex* v) {
    this->upstream = up;
    this->downstream = down;
    this->v = v;
}

Vertex::Vertex(const Vertex& orig) {
}

Vertex::~Vertex() {
}


