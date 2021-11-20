//
// Created by 王宇 on 2021/10/27.
//

#include "faceindex2directededge.h"
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

/*!
 * the constructor
 * @param filename , the .face file name
 */
faceindex2directededge::faceindex2directededge(char *filename) {
    this->filename = filename;
    this->newFileDir = strdup("./edges/"); //the dir that stores the .diredge files
    this->newFileSuffix = strdup(".diredge");
    this->isManifold = true; // set true as default
    ReadFaceIndexFile(); // call this function to read data from .face.
}
/*!
 *
 * @return bool, ture means read the .face file successfully
 */
bool faceindex2directededge::ReadFaceIndexFile(){

    if (access(this->filename, 0) == -1)	{
        // if the file does not exist in the directory then exit.
        cout << "the .face file does not exist, please check" << endl;
        exit(0);
    }

    ifstream inFile(this->filename); //create the stream
    if (inFile.bad()){
        return false;
    }

    string currentToken = string(); // store the current token, then we can use it

    string currentLine; // read file by line
    while (getline(inFile, currentLine)){//read a line from the stream
        stringstream lineStream(currentLine);//create stringstream by currentLine
        lineStream >> currentToken; // then read the line by token
        if ("Vertex" == currentToken){
            //in this while loop, we want to read the header, so if we reach the Vertex section, we need to bread the loop
            break;
        } else{
            this->headInfo.append(currentLine).append("\n"); //store the current line in the header info
        }
        if (inFile.eof()){
            // here if we are reading the end of the file, it means the data structure is wrong.
            cout<<"invalid face file, please check."<<endl;
            exit(0);
        }
    }

    float currentX, currentY, currentZ; // for storing the x,y,z of a coordinate

    do {
        stringstream lineStream(currentLine);// as the currentLine has read the first line of the vertex section, so use do while here.
        lineStream >> currentToken;
        if ("Face" == currentToken){
            //in this loop, we want to read the vertex section, so if we reach the face section, we need to bread the loop
            break;
        }
        lineStream >> currentToken; //the first token is "vertex", wo do not need it
        lineStream >> currentX >> currentY >> currentZ;
        this->vertices.push_back(Cartesian3(currentX, currentY, currentZ));
        if (inFile.eof()){
            // here if we are reading the end of the file, it means the data structure is wrong.
            cout<<"invalid face file, please check."<<endl;
            exit(0);
        }

    } while (getline(inFile, currentLine)); //get the next line

    do {//in this loop, we are reading the face section
        this->facesInfo.append(currentLine).append("\n"); // the first line of face info has been read in the loop above. so here we can use it.
        stringstream lineStream(currentLine);// create a stream to read tokens of a line
        lineStream >> currentToken; //read "Face"
        if ("Face" != currentToken){
            //here, we have to confirm the first token is "Face", if not the data structure is wrong
            break;
        }
        lineStream >> currentToken; //read face index, but we do not need it
        for (int i = 0; i < 3; ++i) {
            //then the next three tokens is the vertices IDs of a face, so read three times
            long currentVerId = 0;
            lineStream >> currentVerId;
            this->vertexIDs.push_back(currentVerId);
        }
        if (inFile.eof()){
            //here, if we have reach the end of the file, then break the loop, it means we have already read all of the data. so break
            break;
        }

    } while (getline(inFile, currentLine)); //get next line

    inFile.close(); // close the stream
    return true;
}
/*!
 * generate the .diredge file
 * @return true if the .diredge file is successfully generated
 */
bool faceindex2directededge::generateDirectedEdgeFile(){
    //from the first  directed edges, we can find all directed edges
    generateFirstDirectedEdges(); //call this function, then we can get the first directed edges array
    generateOtherHalfEdges(); //generate the other half edges, based on the first directed edges
    lookForPinchPoint(); //after we have got the first directed edges, we can go through all the directed edges,
                         // then we can check if at a vertex, there is a pinch point

    if (!this->isManifold){ //if it's not a manifold then we do not need to do anything next.
        cout << "Not a manifold, so did not save the directed-edge file" << endl;
        return false;
    } else{
        long numGenus = this->calculateGenus(); // calculate the number of Genus
        cout << "--------------------------------------------------" << endl;
        cout << "The number of Genus is : " << numGenus << endl;
        cout << "--------------------------------------------------" << endl;
    }

    this->newFileName = generateNewFileName(); //store the new .diredge file name
    cout << this->newFileName << endl; //print the new file name for the user
    ofstream outPutFile(this->newFileName.c_str()); //create the stream for writing the file

    if (outPutFile.bad()){
        return false;
    }

    string headerInfo = writeHeaderInfo(); //get header info
    outPutFile << headerInfo; //write header
    string vertexInfo = writeVertices(); //get vertices info string
    outPutFile << vertexInfo; //write vertices info
    string firstDirEdgesInfo = writeFirstDirectedEdges(); //get first directed edges info string
    outPutFile << firstDirEdgesInfo; //write first directed edges info
    string facesInfo = writeFaces(); //get face info string
    outPutFile << facesInfo; // write face info string
    string otherHalfEdgesInfo = writeOtherHalfEdges(); //get other half edges info string
    outPutFile << otherHalfEdgesInfo; //write other half info string
    outPutFile.close(); //close the stream
    return true;
}
/*!
 * calculating the number of Genus
 * @return the number
 */
long faceindex2directededge::calculateGenus(){
    long v = this->vertices.size();
    long e = this->vertexIDs.size() / 2;
    long f = this->vertexIDs.size() / 3;

    long g = (e + 2 - f - v) / 2; //the formula of calculating the number of Genus
    return g;
}
/*!
 *  find where are the pinch points
 */
void faceindex2directededge::lookForPinchPoint(){
    vector<long> verDegrees; //store the degrees of every vertex
    verDegrees.resize(this->vertices.size()); //here we need to resize the array, as we do no use push_back to add new element here

    long lengthVerIDs = this->vertexIDs.size(); //store the length, then in the loop, we do not need to call size() every time.
    for (int i = 0; i < lengthVerIDs; ++i) {
        int currentVer = this->vertexIDs[i]; //find every vertex
        verDegrees[currentVer] ++; //calculate how many times a vertex appears, as here wo do not care the direction of an edge,
                                   //we just care how many edges connected to the vertex. One time a vertex appears,
                                   //it means an edge connected to it appears.
    }
    long lengthFirstDirEdges = this->firstDirEdges.size(); //store the size for the loop
    long startFirstEdge; // store a started first directed edge, then we can go through the next edges from it
    long firstEdge, secondEdge, thirdEdge; // in a face, the 1st edge, 2nd edge, and the 3rd edge
    long nextFirstEdge;
    long currentVerDegree; // store the curren vertex degree

    for (long i = 0; i < lengthFirstDirEdges; ++i) {
        startFirstEdge = this->firstDirEdges[i]; //get a first directed edge
        firstEdge = startFirstEdge; //set the first edge of a face
        nextFirstEdge = -1; //for a new loop, set it as -1
        currentVerDegree = 0; //set the curren vertex degree as 0
        while (nextFirstEdge != startFirstEdge){ //if they are equal, then we have gone through all the edges from the point
            if (currentVerDegree > 0){ // judge this, because the first time we come here, we do not need to reset the first edge
                firstEdge = nextFirstEdge; //after the first time, we go to the next face, so we need to reset the firstedge as nextFirstEdge
            }
            secondEdge = (firstEdge / 3) * 3 + ((firstEdge + 1) % 3); // this is the formula to calculate the second edge in a face,
                                                                      // but the result is the ID of a directed edge.
            thirdEdge = (secondEdge / 3) * 3 + ((secondEdge + 1) % 3); // same as the second
            nextFirstEdge = this->otherHalfEdges[thirdEdge]; //from the other half deges, we can get the pair of the third edge
                                                             // which is the next face's first directed edge.
            currentVerDegree ++; //here we have found a directed edge starts from the current vertex, so we can increase the degree.

            if (currentVerDegree > verDegrees[i]){
                break; //here it's important, we have to prevent that the degree of the vertex is infinite, if so we will not end this loop.
                //if the current degrees are bigger than we have calculated, we can know that this is not a manifold, so just break.
            }

        }
        if (currentVerDegree != verDegrees[i]){
            cout << "this graph is not a manifold, because vertex : " << i << " has a pinch point." << endl;
            this->isManifold = false;
        }
    }//for (long i = 0; i < lengthFirstDirEdges; ++i)

}

/*!
 * generate the other half edges, store them is an array
 * index is an edge ID, the the value is the other half edge ID
 */
void faceindex2directededge::generateOtherHalfEdges(){
    long startVerID, endVerId; //one directed edge's start vertex ID, and its end vertex ID
    long otherHalfStartID, otherHalfEndId; //the other half directed edge's start vertex ID, and its end vertex ID

    int numPairs; // store the number of a pair of edges
    long lengthVertexIDs = this->vertexIDs.size();
    this->otherHalfEdges.resize(this->vertexIDs.size()); // we need resize, as we won't use push_back to add element

    for (int d = 0; d < lengthVertexIDs; ++d) { // d is the DI of a directed edge
        numPairs = 0; //for every loop, we need to reset the numPairs as 0
        int startVerIDIndex = ((d / 3) * 3) + (d % 3); // a formula to calculate the start vertex ID, divided by 3 is important,
                                                       // as we can restrict the index won't out of the face. especially for the endVerIDIndex.
        int endVerIDIndex = ((d / 3) * 3) + ((d + 1 )% 3);
        startVerID = this->vertexIDs[startVerIDIndex]; //the start vertex ID of a directed edge
        endVerId = this->vertexIDs[endVerIDIndex];

        for (int halfEdgeD = 0; halfEdgeD < lengthVertexIDs; halfEdgeD ++) {
            // here we need to go through all of the vertex IDs, which also can define the vertex edges
            int otherHalfStartIDIndex = ((halfEdgeD / 3) * 3) + (halfEdgeD % 3); // same as startVerIDIndex
            int otherHalfEndIdIndex = ((halfEdgeD / 3) * 3) + ((halfEdgeD + 1 )% 3);
            otherHalfStartID = this->vertexIDs[otherHalfStartIDIndex]; //the start vertex ID of the other half directed edge
            otherHalfEndId = this->vertexIDs[otherHalfEndIdIndex]; //the end vertex ID of the other half directed edge

            if (startVerID == otherHalfEndId && endVerId == otherHalfStartID){
                //if the start vertex ID is equal to the other half edge's end vertex ID,
                //and the end vertex ID is equal to the other half edge's start ID, then they are a pair
                this->otherHalfEdges[d] = halfEdgeD;
                this->otherHalfEdges[halfEdgeD] = d;
                numPairs ++;
            }

        }//for (int halfEdgeD = 0; halfEdgeD < lengthVertexIDs; halfEdgeD ++)

        if (numPairs != 1){
            //if the pair is less than one, it does satisfy the condition that every edge is shared by two triangles.
            //if the pair is bigger than one, it means the edge is shared by more than two edges.
            this->isManifold = false;
            cout << "This is not a manifold, because the edge : " << d << " has " << numPairs << " other half edges." << endl;
        }
    }//for (int d = 0; d < lengthVertexIDs; ++d)

}
/*!
 * assemble the other half edges info
 * @return other half edge info string
 */
string faceindex2directededge::writeOtherHalfEdges(){
    string otherHalfEdgesInfo = string();
    int lengthOtherHalfDeges = this->otherHalfEdges.size();

    for (int i = 0; i < lengthOtherHalfDeges; ++i) {
        otherHalfEdgesInfo.append("OtherHalf   ");
        otherHalfEdgesInfo.append(to_string(i));
        otherHalfEdgesInfo.append("  ");
        otherHalfEdgesInfo.append(to_string(this->otherHalfEdges[i]));
        otherHalfEdgesInfo.append("\n");
    }
    return otherHalfEdgesInfo;
}
/*!
 * generate the first directed edges
 */
void faceindex2directededge::generateFirstDirectedEdges(){
    long d; // the ID of directed edges
    long fastIndex = 0; // the fastIndex for counting the next new vertex ID
    long length = this->vertexIDs.size();
    for (int vertexIndex = 0; vertexIndex < length; ++vertexIndex) {
        d = vertexIndex;
        if (fastIndex == this->vertexIDs[vertexIndex]){
            // if the condition is true, then we can confirm that the new vertex ID is a new vertex,
            // it is the first time that the vertex ID appears, so define it as the first directed edge.
            this->firstDirEdges.push_back(d); //store the ID of the directed edge as the first directed edge.
            fastIndex ++; //increase 1 as refer to the next vertex ID, then we can ignore the vertices that has appeared
        }
    }

}
/*!
 * define this function for assembling the face info,
 * it's easy but if we need to modify the info we do not need to care about other codes.
 * @return face info string
 */
string faceindex2directededge::writeFaces(){
    return this->facesInfo;
}

/*!
 * assemble the first directed edges info
 * @return first directed edges info string
 */
string faceindex2directededge::writeFirstDirectedEdges(){
    string firstDirEdgeInfo = string();
    int lengthFirstDirDeges = this->firstDirEdges.size();

    for (int i = 0; i < lengthFirstDirDeges; ++i) {
        long currentFirstEdgeIndex = this->firstDirEdges[i];
        firstDirEdgeInfo.append("FirstDirectedEdge  ");
        firstDirEdgeInfo.append(to_string(i));
        firstDirEdgeInfo.append("  ");
        firstDirEdgeInfo.append(to_string(lengthFirstDirDeges));
        firstDirEdgeInfo.append("\n");
    }

    return firstDirEdgeInfo;
}
/*!
 * assemble the unique vertices info
 * @return vertices info string
 */
string faceindex2directededge::writeVertices(){
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
 * define this function for assembling the head info,
 * it's easy but if we need to modify the info we do not need to care about other codes.
 * @return head info string
 */
string faceindex2directededge::writeHeaderInfo() {
    return this->headInfo;
}
/*!
 * generate the new file name for .diredge file
 * @return the new file name
 */
string faceindex2directededge::generateNewFileName()
{
    newFileName = string();
    string fileFullPath = this->filename;
    string::size_type targetPosition = fileFullPath.find_last_of("/") + 1;
    string file = fileFullPath.substr(targetPosition, fileFullPath.length() - targetPosition);

    this->objName = file.substr(0, file.rfind("."));

    if (access(this->newFileDir.c_str(), 0) == -1)	{
        mkdir(this->newFileDir.c_str(),0777);
    }

    newFileName = this->newFileDir + this->objName + this->newFileSuffix;

    return newFileName;
}












