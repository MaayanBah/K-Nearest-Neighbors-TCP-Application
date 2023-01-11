server:
	g++ -std=c++11 KNearsestNeightborsServer/main.cpp KNearsestNeightborsServer/ClientSocket.h KNearsestNeightborsServer/ClientSocket.cpp KNearsestNeightborsServer/Server.h KNearsestNeightborsServer/server.cpp KNearsestNeightborsServer/LabeledDataSet.h KNearsestNeightborsServer/LabeledDataSet.cpp KNearsestNeightborsServer/Distances.h KNearsestNeightborsServer/Distances.cpp -o server.out;

client: client.cpp
	g++ -std=c++11 client.cpp -o client.out

clean:
	rm client.out server.out
