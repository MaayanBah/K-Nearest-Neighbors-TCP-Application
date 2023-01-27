# K-Nearest-Neighbors-TCP-Application

## Description
The project consists of two different programs: a server and a client.

The client simply connects to a given server and supports interfacing with its various features. 
The server handles multiple different clients and implements the KNN Algorithm for each client.
K-Nearest Neighbors is one of the most basic yet essential classification algorithms in Machine Learning. It belongs to
the supervised learning domain and is usually applied in pattern recognition.

## Input
### Server
After we compile the server (using `make server`), we run it with the following command line arguments:
1. An available port.
* Note that if the port is unavailable the server will exit and print a corresponding message (error code 98).
In this case try again with a different port.

The server supports multiple clients at once by allocating a separate thread for each client with its own data

### Client
After we compile the client (using `make client`), we run it with the following command line arguments:
1. Server IP address
2. Server port

Afterwards, the client app offers 5 different features:

`1` is for uploading a Classified csv file, and an Unclassified csv file.

`2` is for changing the number of nearest neighbors for the KNN algorithm, or the distance method by which to calculate vector distances.

`3` is for calculating the results.

`4` is for printing the results.

`5` is for saving the results to a file (on a seperate thread).

If invalid input is given to the server, it will simply respond with 'invalid input'.

## How the algorithm works  
Given the path to a Classified.csv file, which contains labeled vectors, it categorizes the label of a given vector by the user.  
In order to categorize the given input vector, we use the k nearest neighbours method based on a calculation of the 
distance between the vector to categorize, and all prior labeled vectors already.  
Basically, the algorithm consists of 3 steps:

1. Calculates all the distances between the categorized vector (input vector), to all prior data of labeled vectors already.  
2. Sort the labeled distances by distance.   
3. Within these k, find the most common label by counting occurrences for each label (in a map) and getting the label 
   with max occurrences.

### Optimisations
* We optimized the code by using std::unordered_map instead of std::map (to utilize hashing instead of tree algorithms).
* We made sure to pass everything by reference and avoiding unnecessary copies whenever possible.
* In order to count the number of occurrences of each label, 
we used a hash map instead of doing the straight-forward approach of nested loops.
* In order to be completely thread-safe, we open a separate socket in the client's 5th feature which supports
  downloading a file in parallel to the client app. Working with the same socket would otherwise cause risk of 
  potential races between data received for the downloading process and data received for general menu traversal.

## Methods of calculating distances between vectors  
In order to calculate the distance between 2 vectors, the user choose between 5 different algorithms:  
1. AUC - Euclidean Distance  
2. MAN - Manhattan Distance  
3. CHB - Chebyshev Distance  
4. CAN - Canberra Distance  
5. MIN - Minkowski Distance  

## Example of a running program  

**In order to compile and run server and client**, open 2 terminals, just where the Makefile is located.  
The first one represents the Server, the second represents the Client.  
Start with compiling the Server (make server), then compile the Client (make client).    
Right after, run the Server, and then run the Client.
You can also choose to run more clients from the same computer or from other computers.





