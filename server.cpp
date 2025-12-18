

//ma use ka it -lws2_32 pag link. and make sure na it windows.h is included first before winsock header

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

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
int main(){
    startup();
   
    
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;
    //wry ko kadayon an first pram na 0 kay gusto ko mag localhost. no need naman to automatically pick a local interface
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
    
    Iresult=bind(ConnectSocket, result->ai_addr,(int)result->ai_addrlen);
    if (Iresult == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);
    Iresult=listen(ConnectSocket,SOMAXCONN);
    if (Iresult == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //pag listen hin mga clients 
    SOCKET client;
    printf("Server listening on port %s...\n", DEFAULT_PORT);
    client = accept(ConnectSocket,NULL,NULL);
    closesocket(ConnectSocket);



    char recvbuf[512];
    const char *msg = "Hi from the server";
    int iResult, iSendResult;
    int recvbuflen = 512;

    do {

        iResult = recv(client, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            recvbuf[iResult] = '\0';
            printf("message from client: %s\n",recvbuf);
            iSendResult = send(client, msg, (int)strlen(msg), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed:\n");
                closesocket(client);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed:\n");
            closesocket(client);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);
    closesocket(ConnectSocket);
    WSACleanup();
}