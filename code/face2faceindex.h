//
// Created by 王宇 on 2021/10/24.
//

#ifndef INC_5821_FACE2FACEINDEX_H
#define INC_5821_FACE2FACEINDEX_H

#include <vector>
#include "Cartesian3.h"
using  namespace std;

class face2faceindex {
private:
    string newFileDir; //store the dir of the new .face file
    string newFileSuffix; //store the suffix (.face)
    string objName; //store the filename excluding the suffix
    string genetrateNewFileName(); //generate the ***.face file with a directory

    bool ReadFileTriangleSoup(); //read all info from the .tri file; returning true means it is successful for reading the soup
    string writeHeaderInfo();  //return the header string
    string writeVertices(); //return the vertices string
    string writeFaces(); // return the face string
public:
    vector<Cartesian3> vertices; //store all vertices but unique
    vector<long> vertexIDs;  //store the ID of all vertices, not unique.
    char *filename;  //store the input file name
    string newFileName;  //store the new .face file name including the directory
    long nFaces;  //store the number of faces
    face2faceindex(char* filename); //the constructor of this class
    bool generateFaceIndexFile(); //call this function, then generate the .face file.
};


#endif //INC_5821_FACE2FACEINDEX_H
