#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define STUDENT_ID_2 15
#define STUDENT_ID_3 115

void PrintError(const string& message)
{
    cout << "[ERROR] " << message
        << " | Error code: "
        << WSAGetLastError()
        << endl;
}


void PrintWSAInfo(const WSADATA& wsaData)
{
    cout << "==========================================" << endl;
    cout << "WINSOCK SYSTEM INFORMATION" << endl;
    cout << "==========================================" << endl;

    cout << "Requested WinSock version: "
        << (int)LOBYTE(wsaData.wVersion)
        << "."
        << (int)HIBYTE(wsaData.wVersion)
        << endl;

    cout << "Highest supported version: "
        << (int)LOBYTE(wsaData.wHighVersion)
        << "."
        << (int)HIBYTE(wsaData.wHighVersion)
        << endl;

    cout << "Description: "
        << wsaData.szDescription
        << endl;

    cout << "System status: "
        << wsaData.szSystemStatus
        << endl;

    cout << endl;
}


bool BindSocket(SOCKET sock, sockaddr_in& addr, const string& socketName)
{
    int result = bind(sock, (SOCKADDR*)&addr, sizeof(addr));

    if (result == SOCKET_ERROR)
    {
        cout << "[FAIL] bind() for "
            << socketName
            << " failed. ";

        PrintError("bind() error");
        return false;
    }

    cout << "[OK] bind() successful for "
        << socketName
        << endl;

    return true;
}

void SimulateBindErrors()
{
    cout << endl;
    cout << "==========================================" << endl;
    cout << "SIMULATING bind() ERRORS" << endl;
    cout << "==========================================" << endl;


    {
        cout << endl;
        cout << "[1] Simulating WSANOTINITIALISED" << endl;

        SOCKET tempSock = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5000);
        addr.sin_addr.s_addr = INADDR_ANY;

        WSACleanup();

        bind(tempSock, (SOCKADDR*)&addr, sizeof(addr));

        PrintError("Expected WSANOTINITIALISED");

        WSADATA tempData;
        WSAStartup(MAKEWORD(2, 2), &tempData);

        closesocket(tempSock);
    }


    {
        cout << endl;
        cout << "[2] Simulating WSAENOTSOCK" << endl;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5001);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind((SOCKET)12345, (SOCKADDR*)&addr, sizeof(addr));

        PrintError("Expected WSAENOTSOCK");
    }


    {
        cout << endl;
        cout << "[3] Simulating WSAEFAULT" << endl;

        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

        bind(s, nullptr, sizeof(sockaddr_in));

        PrintError("Expected WSAEFAULT");

        closesocket(s);
    }


    {
        cout << endl;
        cout << "[4] Simulating WSAEAFNOSUPPORT" << endl;

        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_UNSPEC;
        addr.sin_port = htons(5002);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(s, (SOCKADDR*)&addr, sizeof(addr));

        PrintError("Expected WSAEAFNOSUPPORT");

        closesocket(s);
    }


    {
        cout << endl;
        cout << "[5] Simulating WSAEADDRINUSE" << endl;

        SOCKET s1 = socket(AF_INET, SOCK_STREAM, 0);
        SOCKET s2 = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5050);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(s1, (SOCKADDR*)&addr, sizeof(addr));

        bind(s2, (SOCKADDR*)&addr, sizeof(addr));

        PrintError("Expected WSAEADDRINUSE");

        closesocket(s1);
        closesocket(s2);
    }


    {
        cout << endl;
        cout << "[6] Simulating WSAEINVAL" << endl;

        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5060);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(s, (SOCKADDR*)&addr, sizeof(addr));

        // Repeated bind() of the same socket
        bind(s, (SOCKADDR*)&addr, sizeof(addr));

        PrintError("Expected WSAEINVAL");

        closesocket(s);
    }

    cout << endl;
}


// start
int main()
{
    WSADATA wsaData;

    cout << endl;
    cout << "[1] Initializing WinSock..." << endl;

    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (wsaResult != 0)
    {
        cout << "WSAStartup() finished with error: "
            << wsaResult
            << endl;

        return 1;
    }

    cout << "[OK] WinSock successfully initialized." << endl;

    PrintWSAInfo(wsaData);


    cout << "==========================================" << endl;
    cout << "SOCKET CREATION" << endl;
    cout << "==========================================" << endl;

    SOCKET broadcastSocket;      
    SOCKET fixedIPSocket;        
    SOCKET incrementedIPSocket;  
    SOCKET anyLocalSocket;       

    broadcastSocket = socket(AF_INET, SOCK_DGRAM, 0);

    fixedIPSocket = socket(AF_INET, SOCK_STREAM, 0);

    incrementedIPSocket = socket(AF_INET, SOCK_STREAM, 0);

    anyLocalSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (broadcastSocket == INVALID_SOCKET ||
        fixedIPSocket == INVALID_SOCKET ||
        incrementedIPSocket == INVALID_SOCKET ||
        anyLocalSocket == INVALID_SOCKET)
    {
        PrintError("Failed to create sockets");

        if (broadcastSocket != INVALID_SOCKET) closesocket(broadcastSocket);
        if (fixedIPSocket != INVALID_SOCKET) closesocket(fixedIPSocket);
        if (incrementedIPSocket != INVALID_SOCKET) closesocket(incrementedIPSocket);
        if (anyLocalSocket != INVALID_SOCKET) closesocket(anyLocalSocket);

        WSACleanup();
        return 1;
    }

    cout << "[OK] All 4 sockets created successfully." << endl;
    cout << "  Socket 1 (Broadcast):    " << broadcastSocket << endl;
    cout << "  Socket 2 (Fixed IP XX):   " << fixedIPSocket << endl;
    cout << "  Socket 3 (Fixed IP XX+1): " << incrementedIPSocket << endl;
    cout << "  Socket 4 (Any Local):     " << anyLocalSocket << endl;

    int broadcastEnable = 1;

    setsockopt(
        broadcastSocket,
        SOL_SOCKET,
        SO_BROADCAST,
        (char*)&broadcastEnable,
        sizeof(broadcastEnable)
    );

    cout << "[OK] Socket 1 enabled for broadcast mode." << endl;


    sockaddr_in addr2{};
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(4002);

    string ip2 = "10.1.2.1" + to_string(STUDENT_ID_2);
    cout << "Socket 2 IP address: " << ip2 << endl;

    inet_pton(AF_INET, ip2.c_str(), &addr2.sin_addr);

    sockaddr_in addr3{};
    addr3.sin_family = AF_INET;
    addr3.sin_port = htons(4003);

    string ip3 = "10.1.2.1" + to_string(STUDENT_ID_2 + 1);
    cout << "Socket 3 IP address: " << ip3 << endl;

    inet_pton(AF_INET, ip3.c_str(), &addr3.sin_addr);


    sockaddr_in addr4{};
    addr4.sin_family = AF_INET;
    addr4.sin_port = htons(4004);
    addr4.sin_addr.s_addr = INADDR_ANY;
    cout << "Socket 4 using INADDR_ANY for any local address." << endl;


    cout << endl;
    cout << "==========================================" << endl;
    cout << "BINDING SOCKETS TO ADDRESSES" << endl;
    cout << "==========================================" << endl;

    cout << endl << "[Socket 1] Binding broadcast socket..." << endl;
    int result1 = bind(broadcastSocket, (SOCKADDR*)&addr2, sizeof(addr2));
    if (result1 == SOCKET_ERROR)
    {
        PrintError("bind() for broadcast socket failed");
    }
    else
    {
        cout << "[OK] Socket 1 bind() successful." << endl;
    }

    cout << endl << "[Socket 2] Binding fixed IP socket (10.1.2.1" << STUDENT_ID_2 << ")..." << endl;
    int result2 = bind(fixedIPSocket, (SOCKADDR*)&addr2, sizeof(addr2));
    if (result2 == SOCKET_ERROR)
    {
        PrintError("bind() for fixed IP socket failed");
    }
    else
    {
        cout << "[OK] Socket 2 bind() successful." << endl;
    }

    cout << endl << "[Socket 3] Binding incremented IP socket (10.1.2.1" << (STUDENT_ID_2 + 1) << ")..." << endl;
    int result3 = bind(incrementedIPSocket, (SOCKADDR*)&addr3, sizeof(addr3));
    if (result3 == SOCKET_ERROR)
    {
        PrintError("bind() for incremented IP socket failed");
    }
    else
    {
        cout << "[OK] Socket 3 bind() successful." << endl;
    }

    cout << endl << "[Socket 4] Binding any local address socket..." << endl;
    int result4 = bind(anyLocalSocket, (SOCKADDR*)&addr4, sizeof(addr4));
    if (result4 == SOCKET_ERROR)
    {
        PrintError("bind() for any local address socket failed");
    }
    else
    {
        cout << "[OK] Socket 4 bind() successful." << endl;
    }

    cout << endl;


    SimulateBindErrors();


    cout << "==========================================" << endl;
    cout << "CLEANUP" << endl;
    cout << "==========================================" << endl;

    cout << endl << "[Socket 1] Closing broadcast socket..." << endl;
    if (broadcastSocket != INVALID_SOCKET)
    {
        closesocket(broadcastSocket);
        cout << "[OK] Socket 1 closed." << endl;
    }

    cout << endl << "[Socket 2] Closing fixed IP socket..." << endl;
    if (fixedIPSocket != INVALID_SOCKET)
    {
        closesocket(fixedIPSocket);
        cout << "[OK] Socket 2 closed." << endl;
    }

    cout << endl << "[Socket 3] Closing incremented IP socket..." << endl;
    if (incrementedIPSocket != INVALID_SOCKET)
    {
        closesocket(incrementedIPSocket);
        cout << "[OK] Socket 3 closed." << endl;
    }

    cout << endl << "[Socket 4] Closing any local address socket..." << endl;
    if (anyLocalSocket != INVALID_SOCKET)
    {
        closesocket(anyLocalSocket);
        cout << "[OK] Socket 4 closed." << endl;
    }

    cout << endl << "[OK] All 4 sockets closed successfully." << endl;

    WSACleanup();

    cout << endl << "Press any key to exit..." << endl;
    char key;
    cin.get(key);

    return 0;
}
