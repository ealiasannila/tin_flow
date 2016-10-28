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
#include <cmath>

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

void Tin::addTriangle(Vertex* v1, Vertex* v2, Vertex* v3) {
    if (v1 == v2) {
        cout << "V1 == V2\n";
        exit(1);
    }
    this->triangles.push_back(new Triangle(v1, v2, v3));

    v1->triangles.push_back(this->triangles.back());
    v2->triangles.push_back(this->triangles.back());
    v3->triangles.push_back(this->triangles.back());

}

void Tin::addTriangle(unsigned int i1, unsigned int i2, unsigned int i3) {
    Vertex* v1 = this->vertices[i1];
    Vertex* v2 = this->vertices[i2];
    Vertex* v3 = this->vertices[i3];
    this->addTriangle(v1, v2, v3);
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
    Vertex* nv = 0;
    for (Vertex* v : stack) {
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
        Vertex* v2 = new Vertex{d[0]*7 + x, d[1]*7 + y, d[2]*7 + z, i + 1};
        v.push_back(v1);
        v.push_back(v2);
        i += 2;


    }

    return v;
}

int Tin::getVertexIndex(Triangle* t, Vertex* s) {
    int i;
    for (i = 0; i < t->vertices.size(); i++) {
        if (t->vertices[i] == s) {
            break;
        }
    }
    return i;
}

int Tin::getTriangleIndex(Triangle* t, Vertex* s) {
    int i;
    for (i = 0; i < s->triangles.size(); i++) {
        if (s->triangles[i] == t) {
            break;
        }
    }
    return i;
}

vector<vector<Vertex*>> Tin::steepestPathFromStream(Vertex* s, vector<Vertex*> splitters) {
    vector<vector < Vertex*>> steepest;

    for (unsigned int i = 0; i < splitters.size(); i++) {
        steepest.push_back(vector<Vertex*>{s});
        Vertex* next = 0;
        while (true) {
            if (next == 0) {
                if (splitters.size() == 1) {
                    next = travelUp(s, s, splitters[i]);

                } else {
                    next = travelUp(s, splitters[i], splitters[(i + 1) % splitters.size()]);
                }
            } else {
                next = travelUp(next, 0, 0);
            }
            if (next == 0) {
                break;
            }
            steepest.back().push_back(next);
        }
    }
    return steepest;
}

pair<Vertex*, Triangle*> Tin::locateOpposite(Vertex* l, Vertex* r, Vertex* s) {
    for (Triangle* tl : l->triangles) {
        for (Triangle* tr : r->triangles) {
            if (tl == tr) {
                for (Vertex* v : tr->vertices) {
                    if (v != s and v != l and v != r) {
                        return make_pair(v, tl);
                    }
                }
            }
        }
    }
    return make_pair((Vertex*) 0, (Triangle*) 0);
}

void Tin::adjustTriangle(Triangle* t, Vertex* o, Vertex* n) {
    unsigned int oi = this->getVertexIndex(t, o);
    t->vertices[oi] = n;
    n->triangles.push_back(t);

}

void Tin::newTriangle(Vertex* v, Vertex* n, Vertex* o, Triangle* ot, int orient) {
    Triangle* nt;
    if (orient == -1) {
        nt = new Triangle{v, o, n};
    } else if (orient == 1) {
        nt = new Triangle{v, n, o};
    }
    this->triangles.push_back(nt);
    v->triangles.push_back(nt);
    unsigned int ti = this->getTriangleIndex(ot, o);
    o->triangles[ti] = nt;
    n->triangles.push_back(nt);
}

Vertex* Tin::fixTriangles(Triangle* t, Vertex* s, Vertex* l, Vertex* r, array<double, 3> n) {
    this->addVertex(n[0], n[1], n[2]);
    Vertex* nv = this->vertices.back();

    //Triangle 1
    this->adjustTriangle(t, r, nv);
    //Triangle 2
    cout << "sofar\n";
    this->newTriangle(s, nv, r, t, -1);

    pair<Vertex*, Triangle*> lo = this->locateOpposite(l, r, s);
    if (lo.first == 0) {
        return this->vertices.back();
    }
    Vertex* ov = lo.first;
    Triangle* ot = lo.second;

    //Triangle 3
    this->adjustTriangle(ot, r, nv);
    //Triangle 4
    this->newTriangle(ov, nv, r, ot, 1);

    return this->vertices.back();
}

Vertex* Tin::travelUp(Vertex* s, Vertex* cl, Vertex* cr) {
    double maxAscent = 0;
    Vertex* next = 0;
    cout << s << endl;
    cout << "triangles: " << s->triangles.size() << endl;
    for (Triangle* t : s->triangles) {
        cout << "t" << endl;
        int i = this->getVertexIndex(t, s);
        Vertex* l = t->vertices[(i + 2) % 3];
        Vertex* r = t->vertices[(i + 1) % 3];
        if ((cl != 0 and orient(s, l, cl) == 1) or (cr != 0 and orient(s, r, cr) == -1)) {
            continue;
        }

        std::array<double, 3> steepDir = steepestAscent(t);

        Vertex sv{s->x + steepDir[0], s->y + steepDir[1], s->z + steepDir[2], 0};
        double rateOfAscent;
        // CHECKS every edge twice! unnecessary!
        int lo = orient(s, &sv, l);
        int ro = orient(s, &sv, r);

        cout<<s->index<<" "<<r->index<<" "<<l->index<<endl;
        cout<<s->x<<" "<<s->y<<endl;
        cout<<l->x<<" "<<l->y<<endl;
        cout<<r->x<<" "<<r->y<<endl;
        cout<<sv.x<<" "<<sv.y<<endl;
        cout<<lo<<" "<<ro<<endl;
        cout<<endl;
        if (lo == -1 and ro == 1) {
            rateOfAscent = steepDir[2] / sqrt(steepDir[0] * steepDir[0] + steepDir[1] * steepDir[1]);
            if (maxAscent < rateOfAscent) {
                array<double, 3> nextCoords = intersectAscentAndTri(s, steepDir[0], steepDir[1], steepDir[2], l, r);
                cout<<r->x<<" "<<r->y<<" "<<r->z<<endl;
                cout<<nextCoords[0]<<" "<<nextCoords[1]<<" "<<nextCoords[2]<<endl;
                next = fixTriangles(t, s, l, r, nextCoords);
                maxAscent = rateOfAscent;
            }
        } else if (lo != -1) {
            rateOfAscent = (l->z - s->z) / sqrt((l->x - s->x)*(l->x - s->x) + (l->y - s->y)*(l->y - s->y));
            if (maxAscent < rateOfAscent) {
                next = l;
                maxAscent = rateOfAscent;
            }
        } else {
            rateOfAscent = (r->z - s->z) / sqrt((r->x - s->x)*(r->x - s->x) + (r->y - s->y)*(r->y - s->y));
            if (maxAscent < rateOfAscent) {
                next = r;
                maxAscent = rateOfAscent;
            }
        }
    }
    cout << "Returning\n";
    return next;
}

void Tin::travelUpstream(Vertex* s, vector<Vertex*>* stack, SortedVector<std::pair<Vertex*, Vertex*>>*streams) {
    set<Vertex*> candidates;
    for (Triangle* t : s->triangles) {
        int i = this->getVertexIndex(t, s);
        Vertex* l = t->vertices[(i + 2) % 3];
        Vertex* r = t->vertices[(i + 1) % 3];
        if (l->z - EPSILON < s->z or r->z - EPSILON < s->z) {
            continue;
        }
        std::array<double, 3> steepDir = steepestAscent(t);
        Vertex sv{steepDir[0] + s->x, steepDir[1] + s->y, steepDir[2] + s->z, 0};

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
