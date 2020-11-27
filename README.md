# CS6380-Asynchronous-BellmanFord-algorithm-in-Distributed-systems

This code is used to implement Asynchronous Bellman ford algorithm to identify the shortest path from all nodes to a given specific root node.
The connectivity.txt file contains the num of asynchronous processes, root process, and the adjacency matrix for the processes with edge weight.(-1 is used for unavailability of an edge and 0 is refered to the self weight).

Sample input connectivity.txt:
5 1
0 5 -1 -1 3
5 0 2 1 -1
-1 2 0 6 3
-1 1 6 0 2
3 -1 3 2 0

Sample output:

Vertex number: 2 Shortest path to root: 2 Parent: 1 Children: None
Vertex number: 3 Shortest path to root: 1 Parent: 1 Children: 4
Vertex number: 4 Shortest path to root: 3 Parent: 3 Children: None
Vertex number: 1 Shortest path to root: 0 I am root and  Children: 0 2 3
Vertex number: 0 Shortest path to root: 5 Parent: 1 Children: None
2 2 1 None
3 1 1 4
4 3 3 None
1 0 i am root 0 2 3
0 5 1 None

Total messages sent = 56

--------------------------------

