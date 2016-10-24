/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Tin.cpp
 * Author: elias
 * 
 * Created on October 22, 2016, 3:45 PM
 */

#include "Tin.h"
#include "geom.h"
#include "sorted_vector.h"

#include <iostream>
#include <map>
#include<set>

using namespace std;

Tin::Tin() {
}

Tin::Tin(const Tin& orig) {
}

Tin::~Tin() {
}

void Tin::addVertex(double x, double y, double z) {
    this->vertices.push_back(new Vertex{x, y, z, (unsigned int) this->vertices.size()});
}

void Tin::addTriangle(unsigned int i1, unsigned int i2, unsigned int i3) {
    Vertex* v1 = this->vertices[i1];
    Vertex* v2 = this->vertices[i2];
    Vertex* v3 = this->vertices[i3];

    this->triangles.push_back(new Triangle(v1, v2, v3));

    v1->triangles.push_back(this->triangles.back());
    v2->triangles.push_back(this->triangles.back());
    v3->triangles.push_back(this->triangles.back());


}

void Tin::connectTriangles(unsigned int i1, unsigned int i2) {
    Triangle* tri1 = this->triangles[i1];
    Triangle* tri2 = this->triangles[i2];
    tri1->addNeighbour(tri2);
    tri2->addNeighbour(tri1);

}

void Tin::locateSinks(vector<Vertex*>* vec) {
    for (Vertex* v : this->vertices) {
        bool sink = true;
        for (Triangle* t : v->triangles) {
            for (Vertex* n : t->vertices) {
                if (n->z < v->z) {
                    sink = false;
                    break;
                }
            }
            if (!sink) {
                break;
            }
        }
        if (sink) {
            vec->push_back(v);
            cout << "sink " << v->index << " x " << v->x << " y " << v->y << endl;
        }
    }
}

void Tin::delineateStreams(SortedVector<std::pair<Vertex*, Vertex*>>*streams) {
    vector<Vertex*> stack;
    this->locateSinks(&stack);
    for (Vertex* v : stack) {
        Vertex* nv = 0;
        streams->insert(make_pair(v, nv));
    }
    cout << stack.size() << " sink points\n";
    while (!stack.empty()) {
        Vertex* v = stack.back();
        stack.pop_back();
        travelUpstream(v, &stack, streams);
    }
}

vector<Vertex*> Tin::getFaceAscentDirections() {
    vector<Vertex*> v;
    unsigned int i = 1;
    for (Triangle* t : this->triangles) {
        Vertex* p = t->vertices[0];
        Vertex* q = t->vertices[1];
        Vertex* r = t->vertices[2];
        double x = (p->x + q->x + r->x) / 3;
        double y = (p->y + q->y + r->y) / 3;
        double z = (p->z + q->z + r->z) / 3;
        array<double, 3> d = steepestAscent(t);

        Vertex* v1 = new Vertex{x, y, z, i};
        Vertex* v2 = new Vertex{d[0]*7+x, d[1]*7+y, d[2]*7+z, i + 1};
        v.push_back(v1);
        v.push_back(v2);
        i += 2;


    }

    return v;
}

int Tin::getCurrentIndex(Triangle* t, Vertex* s) {
    int i;
    for (i = 0; i < t->vertices.size(); i++) {
        if (t->vertices[i] == s) {
            break;
        }
    }
    return i;
}

void Tin::steepestPath(Vertex* s, Vertex* c1, Vertex* c2) {
    for (Triangle* t : s->triangles) {
        int i = this->getCurrentIndex(t, s);
        std::array<double, 3> steepDir = steepestAscent(t);
        
        
        //Calculate how steep...
        //Calculate intersection of direction and triangle - prepare for rounding errors resulting not getting exactly to corners...
    }
}

void Tin::travelUpstream(Vertex* s, vector<Vertex*>* stack, SortedVector<std::pair<Vertex*, Vertex*>>*streams) {
    set<Vertex*> candidates;
    for (Triangle* t : s->triangles) {
        int i = this->getCurrentIndex(t, s);
        Vertex* l = t->vertices[(i + 2) % 3];
        Vertex* r = t->vertices[(i + 1) % 3];
        if (l->z - EPSILON < s->z or r->z - EPSILON < s->z) {
            continue;
        }
        std::array<double, 3> steepDir = steepestAscent(t);
        Vertex sv{steepDir[0]+s->x, steepDir[1]+s->y, steepDir[2]+s->z, 0};

        int lo = orient(s, &sv, l);
        int ro = orient(s, &sv, r);
        if (lo == -1) {
            if (!candidates.insert(l).second) { //If other side of left vertex also drains to stream
                if (streams->insert(make_pair(l, s)).second) { //if upstream of left vertex is not already handled by other stream
                    stack->push_back(l);
                }
            }
        }
        if (ro == 1) {
            if (!candidates.insert(r).second) {
                if (streams->insert(make_pair(r, s)).second) {
                    stack->push_back(r);
                }
            }
        }
    }
}
