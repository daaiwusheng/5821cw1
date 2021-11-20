//
// Created by 王宇 on 2021/10/24.
//

#include "face2faceindex.h"
#include <iostream>
#include <fstream>
#include "Cartesian3.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sstream>
#include <stdlib.h>

using namespace std;

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

//it's better for defining a const var for a reusable value.
const int notFound = -1;  // in an array, if an element is not found, then we can set the index == nofFound

/*!
 * the constructor of this class
 * @param filename that needed to be read
 */
face2faceindex::face2faceindex(char *filename) {
    this->filename = filename;
    this->newFileDir = strdup("./faces/");
    this->newFileSuffix = strdup(".face");
    ReadFileTriangleSoup(); //here, call this function to read the soup
}
/*!
 * read the triangle soup
 * @return fals if failed to read the soup
 */
bool face2faceindex::ReadFileTriangleSoup()
{
    if (access(this->filename, 0) == -1)	{
        // if the .tri file does not exist in the directory then exit.
        cout << "the file does not exist, please check" << endl;
        exit(0);
    }

    ifstream inFile(this->filename); // create the read stream
    if (inFile.bad()){
        return false; // if can not read the file, then return false.
    }

    long nTriangles = 0; // store the number of triangles,
    long nVertices = 0; // store the number of vertices, not uniques, every face has three vertices

    inFile >> nTriangles; // read the first token, the number of triangles, according to the form of the file
    this->nFaces = nTriangles;
    nVertices = nTriangles * 3; // every face has three vertices
    Cartesian3 currentVertex = Cartesian3();  // store the current coordinate of the current vertex
    int existIndex = notFound; // as initializing the index as -1, but using a const value is a good habit
    int vertexIndex = 0;  //for calculating the unique ID of vertex
    for(int vertex=0; vertex<nVertices; vertex++){
        // traverse all of the vertices and calculate the unique ID of them.
        inFile >> currentVertex.x >> currentVertex.y >> currentVertex.z; // read the coordinate in
        existIndex = notFound; // reset the no found value when the new loop starts
        for (int i = 0; i < vertices.size(); ++i) {
            //from the already existing vertices, look for if the coordinate is in the vector.
            //if so, record the index i to the var existIndex
            if (currentVertex == vertices[i]){
                existIndex = i;
                break;
            }
        }

        if (existIndex == notFound ) {
            //if this vertex is not in the vector, then store it in
            vertices.push_back(currentVertex); //store the unique coordinate
            vertexIDs.push_back(vertexIndex); //store the ID of current vertex
            vertexIndex++; //this is a counter for counting the next ID of next vertex
        } else{
            vertexIDs.push_back(existIndex);
            //here is important, even we have found the vertex is already existing in the vertexIDs, we store it.
            //Now, the existIndex is equal to the already existing vertex ID.
            //so the vertexIDs stores all of the vertex's ID in the file, not unique.
        }
    }//for(int vertex=0; vertex<nVertices; vertex++), the loop ends here
    inFile.close();  //close the stream
    return true;
}
/*!
 * producing the .face file
 * @return true, if the .face file can be writen successfully.
 */
bool face2faceindex::generateFaceIndexFile()
{
    this->newFileName = genetrateNewFileName();  //store the new .face file name
    cout<< this->newFileName <<endl;  // print the new file name, then the user can know where it is.
    ofstream outPutFile(this->newFileName.c_str()); //create the output stream

    if (outPutFile.bad()){
        return false; //if the stream can not be used, return false
    }
    string headerInfo = writeHeaderInfo(); // store the head info string
    outPutFile << headerInfo;
    string vertexInfo = writeVertices(); // store the vertices info string
    outPutFile << vertexInfo;
    string facesInfo = writeFaces(); // store the face info string
    outPutFile << facesInfo;

    outPutFile.close(); //the stream is ended, then close it
    return true;
}
/*!
 * generate the face info string
 * @return face info string
 */
string face2faceindex::writeFaces() {
    string facesInfo = string();
    for (int i = 0; i < this->nFaces; ++i) {
        // the index of face i, times 3 is the first vertex in the face, but in the vertexIDs,
        // we can find it by treating 3 * i as the index. then we can get the vertex ID of the vertex.
        // Because we store all of the IDs according the order of the original order corresponding to the
        // triangle faces.
        facesInfo.append("Face  ").append(to_string(i)).append("    ");
        facesInfo.append(to_string(this->vertexIDs[3 * i])).append("  ");
        facesInfo.append(to_string(this->vertexIDs[3 * i + 1])).append("  ");
        facesInfo.append(to_string(this->vertexIDs[3 * i + 2])).append("  ");
        facesInfo.append("\n");
    }

    return facesInfo;
}
/*!
 * generate the head info string
 * @return head info string
 */
string face2faceindex::writeHeaderInfo(){
    string headInfo = string();
    headInfo.append("# University of Leeds 2021-2022\n");
    headInfo.append("# COMP 5812M Assignment 1\n");
    headInfo.append("# Yu Wang\n");
    headInfo.append("# 201292704\n");
    headInfo.append("#\n");
    headInfo.append("# Object Name: ").append(this->objName).append("\n");
    headInfo.append("# Vertices=").append(to_string(this->vertices.size())).append("  Faces=").append(to_string(this->nFaces)).append("\n");
    headInfo.append("#\n");
    return headInfo;
}
/*!
 * generate the unique vertices string
 * @return unique vertices string
 */
string face2faceindex::writeVertices(){
    string vertexInfo = string();
    int lengthVertices = this->vertices.size();
    Cartesian3 currentVer;
    for (int i = 0; i < lengthVertices; ++i) {
        currentVer = this->vertices[i];
        vertexInfo.append("Vertex  ");
        vertexInfo.append(to_string(i));
        vertexInfo.append("  ");
        vertexInfo.append(to_string(currentVer.x));
        vertexInfo.append("  ");
        vertexInfo.append(to_string(currentVer.y));
        vertexInfo.append("  ");
        vertexInfo.append(to_string(currentVer.z));
        vertexInfo.append("\n");
    }

    return vertexInfo;
}
/*!
 * generate the new file name for .face file
 * @return the new file name
 */
string face2faceindex::genetrateNewFileName()
{
    newFileName = string();
    string fileFullPath = this->filename;
    string::size_type targetPosition = fileFullPath.find_last_of("/") + 1; //find the position of the last '/'
    string file = fileFullPath.substr(targetPosition, fileFullPath.length() - targetPosition); //get the ***.tri string

    this->objName = file.substr(0, file.rfind(".")); //get the file name excluding the suffix.

    if (access(this->newFileDir.c_str(), 0) == -1)	{
        mkdir(this->newFileDir.c_str(),0777); //give all authority to the file.
    }

    newFileName = this->newFileDir + this->objName + this->newFileSuffix;

    return newFileName;
}

