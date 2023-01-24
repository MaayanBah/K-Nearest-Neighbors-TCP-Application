server:
	g++ -std=c++11 KNearsestNeighborsServer/main.cpp KNearsestNeighborsServer/Network/ClientSocket.h KNearsestNeighborsServer/Network/ClientSocket.cpp KNearsestNeighborsServer/Network/Server.h KNearsestNeighborsServer/Network/Server.cpp KNearsestNeighborsServer/Dataset/LabeledDataSet.h KNearsestNeighborsServer/Dataset/LabeledDataSet.cpp KNearsestNeighborsServer/Dataset/Distances.h KNearsestNeighborsServer/Dataset/Distances.cpp KNearsestNeighborsServer/Utils/ParseMethods.h KNearsestNeighborsServer/Utils/ParseMethods.cpp KNearsestNeighborsServer/ClientApp/AlgorithmSettingsCommand.h KNearsestNeighborsServer/ClientApp/AlgorithmSettingsCommand.cpp KNearsestNeighborsServer/ClientApp/AppData.h KNearsestNeighborsServer/ClientApp/ClassifyDataCommand.h KNearsestNeighborsServer/ClientApp/ClassifyDataCommand.cpp KNearsestNeighborsServer/ClientApp/ClientThread.h KNearsestNeighborsServer/ClientApp/ClientThread.cpp KNearsestNeighborsServer/ClientApp/Command.h KNearsestNeighborsServer/ClientApp/DownloadCommand.cpp KNearsestNeighborsServer/ClientApp/DownloadCommand.h KNearsestNeighborsServer/ClientApp/IO.h KNearsestNeighborsServer/ClientApp/UploadCommand.h KNearsestNeighborsServer/ClientApp/UploadCommand.cpp -IKNearsestNeighborsServer -o server.out;

client: client.cpp
	g++ -std=c++11 client.cpp -o client.out

clean:
	rm client.out server.out
