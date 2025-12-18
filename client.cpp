


//ma use ka it -lws2_32 pag link. and make sure na it windows.h is included first before winsock header

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include <string>

#define DEFAULT_PORT "27015"

WSADATA wsadata;
int Iresult;

int startup(){
    
    Iresult = WSAStartup(MAKEWORD(2,2),&wsadata);
    if(Iresult !=0){
        std::cout << "Startup failed";
        return 1;
    }
    return 0;
}
int connection(SOCKET &s, addrinfo *add){
    int result = connect(s, add->ai_addr,(int)add->ai_addrlen);
    if (result == SOCKET_ERROR) {
        closesocket(s);
        s = INVALID_SOCKET;
    }
    return result;
}
int closeconnection(SOCKET &s){
    int iResult = shutdown(s, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return 1;
    }
    return 0;
}
int main(){
    startup();


    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  

    Iresult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
    if (Iresult != 0) {
        printf("getaddrinfo failed: %d\n", Iresult);
        WSACleanup();
        return 1;
    }
    SOCKET ConnectSocket = INVALID_SOCKET;
    ptr=result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
        ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    Iresult = connection(ConnectSocket,ptr);
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    const char *msg = "Hi this is me the client";
    Iresult = send(ConnectSocket, msg, (int)strlen(msg),0);
    if (Iresult == SOCKET_ERROR) {
        printf("send failed");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    int iResult;
    char buffer[512];
    do {
        iResult = recv(ConnectSocket,buffer, 512, 0);
        if (iResult > 0){
            printf("Bytes received: %d\n", iResult);
            buffer[iResult] = '\0';   // pag null-terminate
            printf("Received: %s\n", buffer);
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed:\n");
    } while (iResult > 0);

    closesocket(ConnectSocket);
    WSACleanup();
}
