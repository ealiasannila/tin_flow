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

bool Tin::isSink(Vertex* v) {
    for (Triangle* t : v->triangles) {
        for (Vertex* s : t->vertices) {
            if (s->z < v->z) {
                return false;
            }
        }
    }
    return true;
}

bool Tin::isPeak(Vertex* v) {
    for (Triangle* t : v->triangles) {
        for (Vertex* s : t->vertices) {
            if (s->z > v->z) {
                return false;
            }
        }
    }
    return true;
}

void Tin::exhaustive(std::set<Vertex*>* streams) {
    for (Triangle* t : this->triangles) {
        array<double, 3> sd = steepestDescent(t);
        for (int i = 0; i < 3; i++) {
            Vertex* v1 = t->vertices[i];
            Vertex* v2 = t->vertices[(i + 1) % 3];
            Vertex* v3 = t->vertices[(i + 2) % 3];
            Vertex sdv = Vertex{v2->x + sd[0], v2->y + sd[1], v2->z + sd[2], 0};
            if (find(v1->downstream.begin(), v1->downstream.end(), v2) != v1->downstream.end() or
                    find(v2->downstream.begin(), v2->downstream.end(), v1) != v2->downstream.end()) {
                continue;
            }

            if (orient(v1, v2, &sdv) == 1) {

                Triangle* ot = this->locateOpposite(v2, v1, v3).second;
                if (ot != 0) {
                    array<double, 3> sdo = steepestDescent(ot);
                    Vertex sdvo = Vertex{v2->x + sdo[0], v2->y + sdo[1], v2->z + sdo[2], 0};
                    if (orient(v1, v2, &sdvo) == -1) {
                        if (v1->z > v2->z) {
                            v1->downstream.push_back(v2);
                            v2->upstream.push_back(v1);
                        } else if (v2->z > v1->z) {
                            v2->downstream.push_back(v1);
                            v1->upstream.push_back(v2);
                        }
                        streams->insert(v1);
                        streams->insert(v2);
                    }
                }
            }
        }
    }
}

void Tin::delineateStreams(set<Vertex*>*streams) {
    vector<Vertex*> stack;
    this->locateSinks(&stack);
    for (Vertex* v : stack) {
        cout << "iserting" << v->index << endl;
        cout << streams->insert(v).second;
    }
    cout << stack.size() << " sink points\n";
    while (!stack.empty()) {
        Vertex* v = stack.back();
        stack.pop_back();
        travelUpstream(v, &stack, streams);
    }
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

void Tin::streamEnd(Vertex* s, Vertex* cl, Vertex* cr, vector<vector < Vertex*>>*steepest) {
    Vertex* next = 0;
    steepest->push_back(vector<Vertex*>{s});


    while (true) {
        if (next == 0) {
            next = travelUp(s, cl, cr);
        } else {
            next = travelUp(next, 0, 0);
        }
        if (next == 0) {
            break;
        }
        steepest->back().push_back(next);
    }
}

vector<vector<Vertex*>> Tin::steepestPathFromStream(Vertex* s, vector<Vertex*> splitters) {
    vector<vector < Vertex*>> steepest;

    if (splitters.size() == 1) {
        streamEnd(s, s, splitters[0], &steepest);
        streamEnd(s, splitters[0], s, &steepest);
        return steepest;
    }
    for (unsigned int i = 0; i < splitters.size(); i++) {
        steepest.push_back(vector<Vertex*>{s});
        Vertex* next = 0;
        while (true) {
            if (next == 0) {
                next = travelUp(s, splitters[i], splitters[(i + 1) % splitters.size()]);
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
    if (orient(t->vertices[0], t->vertices[1], t->vertices[2]) != -1) {
        cout << "bad adjust" << endl;
        exit(1);
    }
    n->triangles.push_back(t);

}

void Tin::newTriangle(Vertex* v, Vertex* n, Vertex* o, Triangle* ot, int orientation) {
    Triangle* nt;
    if (orientation == -1) {
        nt = new Triangle{v, o, n};
    } else if (orientation == 1) {
        nt = new Triangle{v, n, o};
    }
    if (orient(nt->vertices[0], nt->vertices[1], nt->vertices[2]) != -1) {
        cout << "bad new triangle" << endl;
        exit(1);
    }
    this->triangles.push_back(nt);
    v->triangles.push_back(nt);
    unsigned int ti = this->getTriangleIndex(ot, o);
    o->triangles[ti] = nt;
    n->triangles.push_back(nt);

}

Vertex* Tin::fixTriangles(Triangle* t, Vertex* s, Vertex* l, Vertex* r, array<double, 3> n) {
    Vertex temp{n[0],n[1],n[2],0};
    if(orient(s,l,&temp)==0){
        cout<<"colienar detected at l\n";
        return l;
    }if(orient(s,r,&temp)==0){
        cout<<"colienar detected at r\n";
        return r;
    }
    
    this->addVertex(n[0], n[1], n[2]);
    Vertex* nv = this->vertices.back();

    //Triangle 1
    this->adjustTriangle(t, r, nv);
    //Triangle 2
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
    if (l->z > r->z) {
        auto lit = find(l->downstream.begin(), l->downstream.end(), r);
        if (lit != l->downstream.end()) {
            auto rit = find(r->upstream.begin(), r->upstream.end(), l);
            *rit = nv;
            *lit = nv;
            nv->downstream.push_back(r);
            nv->upstream.push_back(l);
        }
    } else if (r->z > l->z) {
        auto rit = find(r->downstream.begin(), r->downstream.end(), l);
        if (rit != r->downstream.end()) {
            auto lit = find(l->upstream.begin(), l->upstream.end(), r);
            *rit = nv;
            *lit = nv;
            nv->downstream.push_back(l);
            nv->upstream.push_back(r);
        }
    }

    return this->vertices.back();

}

void Tin::connectDisjointStreams(std::set<Vertex*>* streams) {
    cout << "CONNECT DISJOINT\n";
    vector<Vertex*> toAdd;
    for (Vertex* v : *streams) {
        //Possibly take inertia into account?
        Vertex* old = v;
        cout << "starting from " << v->index << endl;
        while (old != 0) {
            cout << endl;
            cout << old->triangles.size() << " tri\n";
            cout << "sinktest\n";
            cout << old << endl;
            cout << old->downstream.size() << endl;
            cout << old->upstream.size() << endl;
            cout << isSink(old) << endl;
            cout << old->index << ": " << old->x << "," << old->y << endl;



            if (!old->downstream.empty() or this->isSink(old)) {
                break;
            }

            Triangle* steepest = 0;
            double rateOfDescent = 0;
            array<double, 3> sdm;

            int i = -1;
            Vertex* l;
            Vertex* r;
            int lo;
            int ro;


            for (Triangle* t : old->triangles) {
                // NOT ENOUGH TO CHECK FOR TRIANGLE WITH STEEPEST DESCENT... MUST CHECK IF DESCENT IS WITHIN TRIANGLE -> think steep slope to mild slope but still no stream
                array<double, 3> sd = steepestDescent(t);
                double rd = sd[2] / sqrt(sd[0] * sd[0] + sd[1] * sd[2]);
                cout << "RD: " << rd << " " << rateOfDescent << endl;

                Vertex sdv{old->x + sd[0], old->y + sd[1], old->z + sd[2], 0};
                i = getVertexIndex(t, old);
                l = t->vertices[(i + 2) % 3];
                r = t->vertices[(i + 1) % 3];

                lo = orient(old, &sdv, l);
                ro = orient(old, &sdv, r);

                cout << "lo: " << lo << " ro: " << ro << endl;

                if ((lo != 1 and ro != -1) and rd < rateOfDescent) {
                    rateOfDescent = rd;
                    steepest = t;
                    sdm = sd;
                }

                //cout << "np.array([[" << old->x << "," << old->y << "],[" << sdv.x << "," << sdv.y << "]])\n";
            }

            if (steepest == 0) {
                int i = 0;
                for (Triangle* t : old->triangles) {
                    cout << "t" << i << " = np.array([";
                    i++;
                    cout << "[" << t->vertices[0]->x << "," << t->vertices[0]->y << "]";
                    cout << ",[" << t->vertices[1]->x << "," << t->vertices[1]->y << "]";
                    cout << ",[" << t->vertices[2]->x << "," << t->vertices[2]->y << "]";
                    cout << ",[" << t->vertices[0]->x << "," << t->vertices[0]->y << "]";
                    cout << "])\n";
                }

                cout << "no steepest..." << rateOfDescent << endl;
                cout << isPeak(old);
                exit(1);
            }

            i = getVertexIndex(steepest, old);
            l = steepest->vertices[(i + 2) % 3];
            r = steepest->vertices[(i + 1) % 3];


            //HOW TO CHECK FOR STREAMS ONCE TRIANGLES GET FIXED? Is it even any probelm?? maybe not... draw some shit...
            Vertex sdvm{old->x + sdm[0], old->y + sdm[1], old->z + sdm[2], 0};
            lo = orient(old, &sdvm, l);
            ro = orient(old, &sdvm, r);
            Vertex* nv;
            //case 1 directely to l or r
            if (lo == 0) {
                nv = l;
            } else if (ro == 0) {
                nv = r;
            } else if (ro != 1 or lo != -1) {
                cout << "SOMETHING VERY WIERD SHOULD BE OVERLAND FLOW BUT DOESN'T APPEAR SO" << lo << ", " << ro << endl;
                exit(1);
            } else {
                //case 2 need new vertex
                array<double, 3>intersect = intersectRayPlane(old, sdm[0], sdm[1], sdm[2], l, r);
                //ADD NEW VERTEX TO INTERSECT AND FIX TRIANGLES
                nv = fixTriangles(steepest, old, l, r, intersect);
            }
            old->downstream.push_back(nv);
            nv->upstream.push_back(old);
            old = nv;
            toAdd.push_back(nv);
        }
    }
    for(Vertex* v : toAdd){
        streams->insert(v);
    }
}

Vertex* Tin::travelUp(Vertex* s, Vertex* cl, Vertex* cr) {
    //UNNECESSARILY FIXES ALL TRIANGLES! SHOULD ONLY DO THE STEEPEST!
    double maxAscent = 0;
    Vertex* next = 0;

    for (Triangle* t : s->triangles) {
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

        if (lo == -1 and ro == 1) {
            rateOfAscent = steepDir[2] / sqrt(steepDir[0] * steepDir[0] + steepDir[1] * steepDir[1]);
            if (maxAscent < rateOfAscent) {
                array<double, 3> nextCoords = intersectRayPlane(s, steepDir[0], steepDir[1], steepDir[2], l, r);
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
    return next;
}

void Tin::travelUpstream(Vertex* s, vector<Vertex*>* stack, set<Vertex*>*streams) {
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
                l->downstream.push_back(s);
                s->upstream.push_back(l);
                if (streams->insert(l).second) { //if upstream of left vertex is not already handled by other stream
                    stack->push_back(l);
                }
            }
        }
        if (ro == 1) {
            if (!candidates.insert(r).second) {
                l->downstream.push_back(s);
                s->upstream.push_back(r);
                if (streams->insert(r).second) {
                    stack->push_back(r);
                }
            }
        }
    }
}

Vertex* Tin::travelDown(array<double, 3> v, Triangle* t) {

    array<double, 3> sd = steepestDescent(t);
    array<double, 3> sdv{v[0] + sd[0], v[1] + sd[1], v[2] + sd[2]};

    for (int i = 0; i < t->vertices.size(); i++) {
        Vertex* l = t->vertices[(i + 1) % 3];
        Vertex* r = t->vertices[i];


        int ro = orient(v, sdv,{r->x, r->y, r->z});
        int lo = orient(v, sdv,{l->x, l->y, l->z});

        if (ro == 0) {
            return r;
        }
        if (lo == 0) {

            return l;
        }

        if (ro == 1 and lo == -1) {
            if (find(l->downstream.begin(), l->downstream.end(), r) != l->downstream.end()) {
                return r;
            } else if (find(r->downstream.begin(), r->downstream.end(), l) != r->downstream.end()) {
                return l;
            }
            std::array<double, 3> intersect = intersectRayPlane(v, sd[0], sd[1], sd[2], l, r);
            pair<Vertex*, Triangle*> ot = locateOpposite(l, r, t->vertices[(i + 2) % 3]);
            if (ot.second != 0) {
                return travelDown(intersect, ot.second);
            }
        }
    }
}

void Tin::colorTriangles() {
    for (Triangle* t : this->triangles) {
        t->drainsTo = travelDown(t->centroid(), t);
    }
}
