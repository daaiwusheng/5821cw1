#include <iostream>
#include "face2faceindex.h"
#include "faceindex2directededge.h"
#include <string.h>
using namespace std;


void testFaceIndex(){
    char* filename = strdup("./tetrahedron.tri");
    cout<<filename<<endl;
    face2faceindex test = face2faceindex(filename);
    cout << test.generateFaceIndexFile() << endl;
}

void testDirectedage(){
    char* filename = strdup("./faces/cube.face");
    cout<<filename<<endl;
    faceindex2directededge test = faceindex2directededge(filename);
    test.generateDirectedEdgeFile();

}

int main(int argc, char * argv[]) {

    if (argc != 2){
        cout << "the number of arguments is wrong" << endl;
        return 0;
    }

    char *fileName = argv[1];
    face2faceindex faceIndex = face2faceindex(fileName);
    bool isOkForFaces = faceIndex.generateFaceIndexFile();

    if (isOkForFaces)
    {
        cout << "success for generating .face file"<< endl;

    } else{
        cout << "failed for generating .face file"<< endl;
        return 0;
    }
    char * faceFileName = (char *)faceIndex.newFileName.c_str();
    faceindex2directededge directedEdge = faceindex2directededge(faceFileName);
    bool isOkForDirEdge = directedEdge.generateDirectedEdgeFile();
    if (isOkForDirEdge)
    {
        cout << "success for generating .diredge file"<< endl;

    } else{
        cout << "failed for generating .diredge file"<< endl;
        return 0;
    }


    return 0;
}
