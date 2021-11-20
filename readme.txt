# University of Leeds
# COMP 5821M Coursework 1
# Yu Wang 201292704

============================
LINUX (FENG-LINUX) COMPILE:
============================

1.One way:

In the directory code, there is a shell script called compile_mesh.sh. Following the command below:

    [userid@machine code]$ sh compile_mesh
Then we can get a build directory at the same level with compile_mesh.sh. Then change to build directory, there
is an excutable file called mesher. Then we can use mesher to do complete the task required in the coursework.
Following the command below:

	[userid@machine build]$ ./mesher ../../triangles/many.tri

Then,in the same level directory with mesher, there are two directories edges and faces. In faces, we can find 
.face file. In edges we can find .diredge file.

In the terminal, we can see the result that if the mesh is a manifold and the number of Genus.

2.Second way:

In the directory code, following the commands below:
	[userid@machine code]$ mkdir build
	[userid@machine code]$ cd build
	[userid@machine code]$ cmake ..
	[userid@machine code]$ make
	[userid@machine code]$ ./mesher ../../triangles/many.tri
	
Then,in the same level directory with mesher, there are two directories edges and faces. In faces, we can find 
.face file. In edges we can find .diredge file.

In the terminal, we can see the result that if the mesh is a manifold and the number of Genus.

============================
Complexity analyse:
============================
Please check the Complexity_5821_assignment_1.pdf file.





