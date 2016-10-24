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
#include "sorted_vector.h"
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

bool write_tin(std::string path, Tin* tin) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        for (int i = 0; i < tin->vertices.size(); i++) {
            output << "v " << tin->vertices[i]->x << " " << tin->vertices[i]->y << " " << tin->vertices[i]->z << endl;
        }
        output << "g ground\nusemtl green\n";
        for (int i = 0; i < tin->triangles.size(); i++) {
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

bool write_streams(std::string path, SortedVector<std::pair<Vertex*, Vertex*>> streams) {
    ofstream output(path.c_str());
    output << fixed << setprecision(2);
    if (output.is_open()) {
        output << "mtllib tin.mtl\n";
        map<Vertex*, int> indexes;
        int i = 1;
        for (pair<Vertex*, Vertex*>p : streams) {
            output << "v " << p.first->x << " " << p.first->y << " " << p.first->z << endl;
            indexes.insert(make_pair(p.first, i));
            i++;
        }
        for (pair<Vertex*, Vertex*>p : streams) {
            if (p.second != 0) {
                output << "l " << indexes[p.first] << " " << indexes[p.second] << endl;
            }
        }
        output.close();
    } else cout << "Unable to open file";
    return true;
}

int main(int argc, char** argv) {
    
 
    cout << "Hello\n";
    Tin tin;
    parse_obj("/home/elias/Documents/tin/small.obj", &tin, 0, 2, 1);
    write_tin("/home/elias/Documents/tin/small2.obj", &tin);
    write_ascents("/home/elias/Documents/tin/ascents.obj", tin.getFaceAscentDirections());

    vector<Vertex*> sinks;
    tin.locateSinks(&sinks);
    write_sinks("/home/elias/Documents/tin/sinks.obj", sinks);
    SortedVector<std::pair < Vertex*, Vertex*>> streams;
    tin.delineateStreams(&streams);
    for (pair<Vertex*, Vertex*> p : streams) {
        cout << p.first->index << "->";
        if (p.second != 0) {
            cout << p.second->index << endl;
        }
        cout << 0 << endl;
    }
    write_streams("/home/elias/Documents/tin/streams.obj", streams);

    return 0;
}

