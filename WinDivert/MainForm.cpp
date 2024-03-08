#include <Windows.h>
#include <windivert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "MainForm.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "WinDivert.lib")
#pragma comment(lib, "User32.lib")

using namespace WinDivert;


#define NUM_WORKERS         4
#define MAX_PACKET          4096

struct conn
{
	uint16_t port;
	uint8_t state;
};


// Prototypes:
static DWORD redirect_worker(LPVOID arg);


// State:
static bool redirect_on = false;
static HANDLE handle = INVALID_HANDLE_VALUE;
static HANDLE handle_drop = INVALID_HANDLE_VALUE;
static HANDLE workers[NUM_WORKERS] = { NULL };    // Worker threads
static struct conn conns[UINT16_MAX + 1] = { { 0 } };


OVERLAPPED *poverlapped;
OVERLAPPED overlapped;

// Send a packet asynchronously
static void send_packet_inbound(HANDLE handle, void *packet, size_t packet_len, PWINDIVERT_ADDRESS addr)
{
	addr->Direction = WINDIVERT_DIRECTION_INBOUND;

	WinDivertHelperCalcChecksums(packet, packet_len, addr, 0);

	//poverlapped = (OVERLAPPED *)malloc(sizeof(OVERLAPPED));
	//memset(poverlapped, 0, sizeof(OVERLAPPED));
	//WinDivertSendEx(handle, packet, packet_len, 0, addr, NULL, poverlapped);

	WinDivertSend(handle, packet, packet_len, addr, 0);
}

static void send_packet_outbound(HANDLE handle, void *packet, size_t packet_len, PWINDIVERT_ADDRESS addr)
{
	addr->Direction = WINDIVERT_DIRECTION_OUTBOUND;

	WinDivertHelperCalcChecksums(packet, packet_len, addr, 0);

	//poverlapped = (OVERLAPPED *)malloc(sizeof(OVERLAPPED));
	//memset(poverlapped, 0, sizeof(OVERLAPPED));
	//WinDivertSendEx(handle, packet, packet_len, 0, addr, NULL, poverlapped);

	WinDivertSend(handle, packet, packet_len, addr, 0);
}


// Start traffic redirection
extern void redirect_start(void)
{
	std::string filter = "tcp.DstPort == 1234 or tcp.SrcPort == 80";
	handle = WinDivertOpen(filter.c_str(), WINDIVERT_LAYER_NETWORK, 0, 0);

	if (handle == INVALID_HANDLE_VALUE)
	{
		MessageBox::Show("Failed to load WinDivert Filter");
		exit(EXIT_FAILURE);
	}

	// Launch threads:
	redirect_on = true;
	memset(conns, 0, sizeof(conns));

	for (size_t i = 0; i < NUM_WORKERS; i++)
	{
		workers[i] = CreateThread(NULL, MAX_PACKET * 3, (LPTHREAD_START_ROUTINE)redirect_worker, (LPVOID)handle, 0, NULL);

		if (workers[i] == NULL)
		{
			MessageBox::Show("failed to create WinDivert worker thread");
			exit(EXIT_FAILURE);
		}
	}
}


// Stop traffic redirection
extern void redirect_stop(void)
{
	if (handle == INVALID_HANDLE_VALUE)
		return;

	// Close the WinDivert handle; will cause the workers to exit.
	redirect_on = false;

	if (!WinDivertClose(handle))
	{
		MessageBox::Show("failed to close WinDivert filter");
		exit(EXIT_FAILURE);
	}

	handle = INVALID_HANDLE_VALUE;

	for (size_t i = 0; i < NUM_WORKERS; i++)
	{
		WaitForSingleObject(workers[i], INFINITE);
		workers[i] = NULL;
	}
}


// Redirect worker thread:
static DWORD redirect_worker(LPVOID arg)
{
	HANDLE handle = (HANDLE)arg;

	// Packet processing loop:
	char packet[MAX_PACKET];
	UINT packet_len;
	WINDIVERT_ADDRESS addr;

	while (redirect_on)
	{
		if (!WinDivertRecv(handle, packet, sizeof(packet), &addr, &packet_len))
		{
			continue;
		}

		PWINDIVERT_IPHDR iphdr = NULL;
		PWINDIVERT_TCPHDR tcphdr = NULL;
		PWINDIVERT_UDPHDR udphdr = NULL;

		PVOID data;
		UINT data_len;

		WinDivertHelperParsePacket(packet, packet_len, &iphdr, NULL, NULL, NULL, &tcphdr, &udphdr, &data, &data_len);

		switch (addr.Direction)
		{

		case WINDIVERT_DIRECTION_OUTBOUND:

			if (tcphdr->DstPort == htons(1234))
			{
				tcphdr->DstPort = htons(80);
				send_packet_outbound(handle, packet, packet_len, &addr);
			}

			break;

		case WINDIVERT_DIRECTION_INBOUND:

			if (tcphdr->SrcPort == htons(80))
			{
				tcphdr->SrcPort = htons(1234);
				send_packet_inbound(handle, packet, packet_len, &addr);
			}

			break;
		}
	}

	return 0;
}


// Initialize program / Create Window
void Main()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew MainForm);
	Application::Exit();
}

void MainForm::bStart_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->bStart->Enabled = false;
	this->bStop->Enabled = true;
	redirect_start();
}

void MainForm::bStop_Click(System::Object^  sender, System::EventArgs^  e)
{
	redirect_stop();
	this->bStart->Enabled = true;
	this->bStop->Enabled = false;
}