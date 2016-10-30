/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: elias
 *
 * Created on October 22, 2016, 3:27 PM
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include<iomanip>
#include<map>
#include  <vector>
#include "Tin.h"
#include <set>
#include <ctime>
using namespace std;

/*
 * 
 */

bool parse_obj(std::string path, Tin* tin, int x, int y, int z) {
    ifstream input(path.c_str());
    string line;
    while (getline(input, line)) {
        istringstream iss(line);
        string header;
        std::array<double, 3> vals;
        std::array<string, 3> svals;
        if (!(iss >> header)) {
            cout << "Could not parse header on line: " << line << endl;
            exit(1);
        }

        if (header.compare("v") == 0) {
            if (!(iss >> vals[0] >> vals[1] >> vals[2])) {
                cout << "Header: " << header << " Could not parse vertex values in line: " << line << endl;
                exit(1);
            }
            tin->addVertex(vals[x], vals[y], vals[z]);
        } else if (header.compare("f") == 0) {
            if (!(iss >> svals[0] >> svals[1] >> svals[2])) {
                cout << "Header: " << header << " Could not parse face values in line: " << line << endl;
                exit(1);
            }
            vals[0] = stod(svals[0].substr(0, svals[0].find("/")));
            vals[1] = stod(svals[1].substr(0, svals[1].find("/")));
            vals[2] = stod(svals[2].substr(0, svals[2].find("/")));

            tin->addTriangle(vals[0] - 1, vals[1] - 1, vals[2] - 1);
        }
    }
    return true;

}

void write_materials(std::string path, int n) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "newmtl mnull" << endl;
        output << "Ka " << 1 << " " << 0 << " " << 0 << endl;
        output << "Kd " << 1 << " " << 0 << " " << 0 << endl;
        output << "Ks " << 1 << " " << 0 << " " << 0 << endl;
        output << "Tf " << 1 << " " << 0 << " " << 0 << endl;

        for (int i = 0; i < n; i++) {
            double r = ((double) rand() / (double) RAND_MAX);
            double g = ((double) rand() / (double) RAND_MAX);
            double b = ((double) rand() / (double) RAND_MAX);
            output << "newmtl m" << i << endl;
            output << "Ka " << r << " " << g << " " << b << endl;
            output << "Kd " << r << " " << g << " " << b << endl;
            output << "Ks " << r << " " << g << " " << b << endl;
            output << "Tf " << r << " " << g << " " << b << endl;
        }
        output.close();
    } else cout << "Unable to open file";
}

bool write_tin(std::string path, Tin* tin) {
    map<Vertex*, int> colors;
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        for (int i = 0; i < tin->vertices.size(); i++) {
            output << "v " << tin->vertices[i]->x << " " << tin->vertices[i]->y << " " << tin->vertices[i]->z << endl;
            colors.insert(make_pair(tin->vertices[i], i));
        }
        for (int i = 0; i < tin->triangles.size(); i++) {
            if (tin->triangles[i]->drainsTo == 0) {
                cout << "NO DRAIN" << endl;
                output << "usemtl mnull" << endl;
            } else {
                output << "usemtl m" << colors[tin->triangles[i]->drainsTo] << endl;

            }
            output << "f " << tin->triangles[i]->vertices[0]->index + 1 << " " << tin->triangles[i]->vertices[1]->index + 1 << " " << tin->triangles[i]->vertices[2]->index + 1 << endl;
        }


        output.close();
    } else cout << "Unable to open file";
    return true;

}

bool write_sinks(string path, vector<Vertex*> sinks) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        for (Vertex* v : sinks) {
            output << "v " << v->x << " " << v->y << " " << v->z << endl;
        }
        output.close();
    } else cout << "Unable to open file";
    return true;
}

bool write_ascents(string path, vector<Vertex*> ascents) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        for (int i = 0; i < ascents.size(); i++) {
            output << "v " << ascents[i]->x << " " << ascents[i]->y << " " << ascents[i]->z << endl;
        }
        for (int i = 0; i < ascents.size() - 1; i++) {
            if (i % 2 == 0) {
                output << "l " << i + 1 << " " << i + 2 << endl;
            }
        }
        output.close();
    } else cout << "Unable to open file";
    return true;
}

bool write_streams(std::string path, set<Vertex*> streams) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        map<Vertex*, int> indexes;
        int i = 1;
        for (Vertex* v : streams) {
            output << "v " << v->x << " " << v->y << " " << v->z << endl;
            indexes.insert(make_pair(v, i));
            i++;
        }
        for (Vertex* v : streams) {
            if (v->upstream.size() != 0) {
                for (Vertex* u : v->upstream) {
                    output << "l " << indexes[v] << " " << indexes[u] << endl;
                }
            }
        }
        output.close();
    } else cout << "Unable to open file";
    return true;
}

bool write_steepestPath(string path, vector<vector<Vertex*>> paths) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        map <Vertex*, unsigned int>indexes;
        unsigned int ind = 1;
        for (vector<Vertex*> p : paths) {
            for (int i = 0; i < p.size(); i++) {
                output << "v " << p[i]->x << " " << p[i]->y << " " << p[i]->z << endl;
                indexes.insert(make_pair(p[i], ind));
                ind++;
            }
        }
        for (vector<Vertex*> p : paths) {
            for (int i = 0; i < p.size() - 1; i++) {
                output << "l " << indexes[p[i]] << " " << indexes[p[i + 1]] << endl;
            }
        }

        output.close();
    } else cout << "Unable to open file";
    return true;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    cout << "Hello\n";
    Tin tin;
    parse_obj("/home/elias/Documents/tin/small.obj", &tin, 0, 2, 1);
    set<Vertex*> streams;
    //tin.delineateStreams(&streams);
    tin.exhaustive(&streams);
    cout << streams.size();
    cout << "Streams:" << endl;
    vector < vector < Vertex*>> paths;
    for (Vertex* v : streams) {
        cout << v->index << ": " << v->x << "," << v->y << endl;
        if (v->upstream.size() != 0) {
            vector<Vertex*> splitters;
            splitters.reserve(v->downstream.size() + v->upstream.size()); // preallocate memory
            splitters.insert(splitters.end(), v->downstream.begin(), v->downstream.end());
            splitters.insert(splitters.end(), v->upstream.begin(), v->upstream.end());
            sortSplitters(&splitters, v);
            vector<vector < Vertex*>> some_paths = tin.steepestPathFromStream(v, splitters);
            for (vector<Vertex*> p : some_paths) {
                paths.push_back(p);
            }
        }
    }

    tin.colorTriangles();


    write_materials("/home/elias/Documents/tin/tin.mtl", tin.vertices.size());
    write_streams("/home/elias/Documents/tin/streams.obj", streams);
    write_tin("/home/elias/Documents/tin/small2.obj", &tin);

    write_steepestPath("/home/elias/Documents/tin/steepest.obj", paths);

    int out = system("blender --python /home/elias/Documents/tin/obj_loader.py "
            "/home/elias/Documents/tin/streams.obj "
            "/home/elias/Documents/tin/small2.obj "
            "/home/elias/Documents/tin/steepest.obj ");





    //       system("g3dviewer /home/elias/Documents/tin/small2.obj");
    return 0;
}

