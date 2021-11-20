//
// Created by 王宇 on 2021/10/27.
//

#ifndef INC_5821_FACEINDEX2DIRECTEDEDGE_H
#define INC_5821_FACEINDEX2DIRECTEDEDGE_H

#include <vector>
#include "Cartesian3.h"
using  namespace std;

class faceindex2directededge {

private:
    string newFileDir; // store the directory of .diredge files
    string newFileSuffix;  // .diredge
    string objName;  // the pure file name without suffix
    string headInfo; // the file's header info
    string facesInfo; // the faces info string
    vector<long> firstDirEdges; // the array of first directed edges, store the starting vertex IDs
    vector<long> otherHalfEdges; // the array of the other half edges, store the other half edge IDs
    string generateNewFileName(); // generate the new .diredge file name
    bool ReadFaceIndexFile(); // read data from the .face file

    void generateFirstDirectedEdges(); //calculate the directed edges array : firstDirEdges
    void generateOtherHalfEdges(); // calculate other half edges array : otherHalfEdges
    void lookForPinchPoint();  //find out the pinch points and print the vertices
    long calculateGenus(); //if it's a manifold, then call this function, and calculate the number of Genus

    string writeHeaderInfo(); //return the header info string
    string writeVertices();  //return the vertices info string
    string writeFirstDirectedEdges(); //return the first directed edges info string
    string writeFaces(); //return the faces info string
    string writeOtherHalfEdges(); // return the other half edges info string
public:
    vector<Cartesian3> vertices; // store the vertices, unique coordinates
    vector<long> vertexIDs; // store the vertex IDs, not unique
    char *filename; // store the .face filename,
    string newFileName; // store the new .diredge file name
    bool isManifold; // true means this is a manifold
    faceindex2directededge(char* filename); // the constructor
    bool generateDirectedEdgeFile(); // call this function, then generate the .diredge file

};


#endif //INC_5821_FACEINDEX2DIRECTEDEDGE_H
