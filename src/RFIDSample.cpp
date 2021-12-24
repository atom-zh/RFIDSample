// RFIDAPI3_Console.cpp : Defines the entry point for the console application.
//

#include "General.h"
#include "sa_rfid.h"

static wchar_t hostName[260];
static int readerPort = 0;
static SINGULATION_CONTROL singulationControl;
extern ANTENNA_INFO g_antennaInfo;

void InventoryFilterOption(RFID_HANDLE32 readerHandle);
void Createmenu(RFID_HANDLE32 readerHandle);
void ConfigurationMenu(RFID_HANDLE32 readerHandle);
void InventoryMenu(RFID_HANDLE32 readerHandle);
void AccessMenu(RFID_HANDLE32 readerHandle);

struct sa_rfid_tag_data {
	int num;
	char id[64];
	char rssi;
	sa_rfid_evt event_type;
	unsigned int tagIDLength; /**< Tag ID Length (Number of Bytes).*/	
	unsigned int tagIDAllocated; /**<  Memory allocated for Tag ID  (Number of Bytes) .*/
	unsigned int PC; /**< PC BITS.*/
	unsigned int XPC; /** XPC BITS - XPC_W1 = LOWORD(XPC) and XPC_W2 = HIWORD(XPC) .*/
	unsigned int CRC;  /**< CRC.*/
	unsigned int ntennaID;/**< Antenna ID .*/
};

RFID_HANDLE32 readerHandle;
//RFID_STATUS ConnectReader(RFID_HANDLE32 *readerHandle,wchar_t *hostName,int readerPort);
int sa_device_rfid_open(char *host, int port)
{

	RFID_STATUS rfidStatus;
	mbstowcs(hostName, host, 32);
	wprintf(L"host: %S, port: %d\n", hostName, port);
	rfidStatus = ConnectReader(&readerHandle, hostName, port);
	if (RFID_API_SUCCESS == rfidStatus) {
		TAG_STORAGE_SETTINGS tagStorageSettings;

		RFID_GetTagStorageSettings(readerHandle, &tagStorageSettings);
		tagStorageSettings.discardTagsOnInventoryStop = TRUE;
		RFID_SetTagStorageSettings(readerHandle, &tagStorageSettings);

		//CreateEventThread(readerHandle);
	} else {
		wprintf(L"\nFailed to connect RFID\n");
		return -1;
	}

	rfidStatus = RFID_PerformInventory(readerHandle, NULL, NULL, NULL, NULL);
	if(RFID_API_SUCCESS != rfidStatus)
	{
		HandleResult(readerHandle, rfidStatus);
		return rfidStatus;
	}

	return 0;
}

int sa_device_rfid_close(void)
{
	RFID_STATUS rfidStatus;
	rfidStatus = RFID_StopInventory(readerHandle);
	HandleResult(readerHandle, rfidStatus);
	return 0;
}

int sa_device_rfid_read(struct sa_rfid_tag_data *tag_data)
{
	TAG_DATA* pTagData = NULL;
	pTagData = RFID_AllocateTag(readerHandle);
	if(NULL == pTagData)
	{
		// Handle memory allocation failure
		// Optimally, Tag Allocation can be done once and pointer reused till disconnection.
		wprintf(L"RFID_AllocateTag Failed.");
		return -1;
	}

	if (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {

#if 1
		wchar_t tagBuffer[260] = {0,};
		wchar_t* pTagReportData = tagBuffer;
		unsigned int   index = 0;

		for(index = 0; index < pTagData->tagIDLength; index++) {
			if(0 < index && index%2 == 0)
			{
				//*pTagReportData++ = L'-';
			}
			rfid_swprintf(pTagReportData, (260-index), L"%02X", pTagData->pTagID[index]);
			while(*pTagReportData) pTagReportData++;
		}
		tag_data->num = pTagData->tagIDLength;
		tag_data->rssi = pTagData->peakRSSI;
		wcstombs(tag_data->id, tagBuffer, sizeof(tag_data->id));
		wprintf(L"ID:%S, RSSI:%04d\n", tagBuffer, pTagData->peakRSSI);
#else
		printTagDataWithResults(pTagData);
#endif
	}

	if(pTagData)
		RFID_DeallocateTag(readerHandle, pTagData);

	return 0;
}

#define MAX_EVENTS 12
// The code will use one or the other of these following : Event Handles for win32 events or Event types for the callbacks
sa_rfid_evt rfid_event_types[MAX_EVENTS] = 
{
	GPI_EVENT, TAG_READ_EVENT, BUFFER_FULL_EVENT, BUFFER_FULL_WARNING_EVENT, 
	ANTENNA_EVENT, DISCONNECTION_EVENT, 
	INVENTORY_START_EVENT, INVENTORY_STOP_EVENT, ACCESS_START_EVENT, ACCESS_STOP_EVENT, NXP_EAS_ALARM_EVENT, READER_EXCEPTION_EVENT
};

typedef int (*sa_rfid_evt_hdlr)(sa_rfid_evt, struct sa_rfid_tag_data *tag_data);

struct sa_rfid_event_cb {
	unsigned int event_num;
	sa_rfid_evt_hdlr read_cb;
	sa_rfid_evt_hdlr start_cb;
	sa_rfid_evt_hdlr stop_cb;
};

struct sa_rfid_event_cb event_cb = {0};

int sa_rfid_read_event_handler(sa_rfid_evt, struct sa_rfid_tag_data *tag_data)
{
	printf("num:%d\tID:%s\tRSSI:%04d\n", tag_data->num, tag_data->id, tag_data->rssi);
	return 0;
}

int sa_rfid_start_event_handler(sa_rfid_evt, struct sa_rfid_tag_data *tag_data)
{
	printf("START type:%d\n", tag_data->event_type);
	return 0;
}

int sa_rfid_stop_event_handler(sa_rfid_evt, struct sa_rfid_tag_data *tag_data)
{
	printf("STOP type:%d\n", tag_data->event_type);
	return 0;
}

static void rfid_event_cb(RFID_HANDLE32 readerHandle, RFID_EVENT_TYPE eventType)
{
	struct sa_rfid_tag_data tag_data;

	switch(eventType) {
		case TAG_READ_EVENT:
			sa_device_rfid_read(&tag_data);
			tag_data.event_type = TAG_READ_EVENT;
			event_cb.read_cb(TAG_READ_EVENT, &tag_data);
			break;
		case INVENTORY_START_EVENT:
			tag_data.event_type = INVENTORY_START_EVENT;
			event_cb.start_cb(INVENTORY_START_EVENT, &tag_data);
			break;
		case INVENTORY_STOP_EVENT:
			tag_data.event_type = INVENTORY_STOP_EVENT;
			event_cb.stop_cb(INVENTORY_STOP_EVENT, &tag_data);
			break;
		default:
			break;
	}
}

int sa_device_register_rfid_cb(sa_rfid_evt event_type, sa_rfid_evt_hdlr cb)
{

	switch(event_type) {
		case TAG_READ_EVENT:
			if (event_cb.read_cb) event_cb.read_cb = cb;
			break;
		case INVENTORY_START_EVENT:
			if (event_cb.start_cb) event_cb.start_cb = cb;
			break;
		case INVENTORY_STOP_EVENT:
			if (event_cb.stop_cb) event_cb.stop_cb = cb;
			break;
		default:
			break;
	}
	
	if (event_cb.event_num <= 0) {
		RFID_RegisterEventNotificationCallback(readerHandle, (RFID_EVENT_TYPE *)rfid_event_types, \
			MAX_EVENTS, (RfidEventCallbackFunction) rfid_event_cb, NULL, NULL);
	}
	event_cb.event_num++;
	return 0;
}

int main(int argc, char* argv[])
{
	int option = 0;
	char host[32] = "169.254.78.149";
	int port = 5084;
	while(1) {
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. rfid_open");
		wprintf(L"\n2. rfid_close");
		wprintf(L"\n3. rfid_read");
		wprintf(L"\n4. rfid_regist");
		wprintf(L"\n5. exit\n");

		while(1 != scanf("%d", &option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}
		switch(option) {
			case 1:
				sa_device_rfid_open(host, port);
				break;
			case 2:
				sa_device_rfid_close();
				break;
			case 3:
				struct sa_rfid_tag_data tag_data;
				sa_device_rfid_read(&tag_data);
				wprintf(L"num:%d\t", tag_data.num);
				wprintf(L"Test ID:%s, RSSI:%04d\n", tag_data.id, tag_data.rssi);
				break;
			case 4:
				sa_device_register_rfid_cb(TAG_READ_EVENT, sa_rfid_read_event_handler);
				sa_device_register_rfid_cb(INVENTORY_START_EVENT, sa_rfid_start_event_handler);
				sa_device_register_rfid_cb(INVENTORY_STOP_EVENT, sa_rfid_stop_event_handler);
				break;
			case 5:
				exit(1);
			default:
				wprintf(L"\nInvalid case:");
			break;
		}
	}
	return 0;
}


int b_main(int argc, char* argv[])
{
	if(argc == 1 || argc == 3)
	{
		if(argc == 1)
		{
			wcscpy(hostName, L"localhost");
			readerPort = 0;
		}
		else
		{
			char *stopChar;
			mbstowcs((wchar_t *)hostName, argv[1], MAX_PATH);
			readerPort = strtol(argv[2], &stopChar, 10);
		}	
	}
	else
	{
		wprintf(L"\nEnter either 0 or 2 arguments\nPress any key to exit");
		getchar();
		exit(0);
	}

	RFID_HANDLE32 readerHandle;
	wprintf(L"host: %S, port: %d\n", (wchar_t *)hostName, readerPort);
	RFID_STATUS rfidStatus = ConnectReader(&readerHandle, hostName, readerPort);
	if(RFID_API_SUCCESS == rfidStatus)
	{
		TAG_STORAGE_SETTINGS tagStorageSettings;	

		RFID_GetTagStorageSettings(readerHandle, &tagStorageSettings);
		tagStorageSettings.discardTagsOnInventoryStop = TRUE;
		RFID_SetTagStorageSettings(readerHandle, &tagStorageSettings);

		CreateEventThread(readerHandle);
		Createmenu(readerHandle);
	}
	return 0;
}


void Createmenu(RFID_HANDLE32 readerHandle)
{
	int option = 0;
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	while(1)
	{
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. Capability -- Displays the device capabilities");
		wprintf(L"\n2. Configuration");
		wprintf(L"\n3. Inventory");
		wprintf(L"\n4. Access  - Select Mode of Access");
		wprintf(L"\n5. Access  - Select Mode of Access");
		wprintf(L"\n6. Exit\n");
		while(1 != scanf("%d",&option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}
		switch(option)
		{
		case 1:
			rfidStatus = ReaderCapability(readerHandle);
			break;
		case 2:
			ConfigurationMenu(readerHandle);
			break;
		case 3:
			InventoryMenu(readerHandle);
			break;
		case 4:
			AccessMenu(readerHandle);
			break;
		case 5:
			KillEventThread();
			if(g_antennaInfo.pAntennaList)
			{
				delete [] g_antennaInfo.pAntennaList;
				g_antennaInfo.pAntennaList = NULL;
			}
			RFID_Disconnect(readerHandle);
			return;
		}
	}
}


void ConfigurationMenu(RFID_HANDLE32 readerHandle)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;

	while(1)
	{
		int option = 0;
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. Get Singulation Control");
		wprintf(L"\n2. GPO");
		wprintf(L"\n3. GP1");
		wprintf(L"\n4. Antenna Config");
		wprintf(L"\n5. RF Mode ");
		wprintf(L"\n6. Back  to main menu\n");
		while(1 != scanf("%d",&option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}
		switch(option)
		{
		case 1:
			rfidStatus = SingulationControl(readerHandle,&singulationControl);
			break;
		case 2:
			rfidStatus = ConfigureGPO(readerHandle);
			break;
		case 3:
			rfidStatus = ConfigureGPI(readerHandle);
			break;
		case 4:
			rfidStatus = ConfigureAntenna(readerHandle);
			break;
		case 5:
			rfidStatus = ConfigureRFMode(readerHandle);
			break;
		case 6:
			return;
		}
		if(option > 0 && option < 6)
		{
			if(rfidStatus != RFID_API_SUCCESS)
			{
				ERROR_INFO ErrorInfo;
				RFID_GetLastErrorInfo(readerHandle,&ErrorInfo);
				wprintf(L"\nOperation Failed. Reason : %ls ",ErrorInfo.statusDesc);
			}
		}
	}
}

void InventoryMenu(RFID_HANDLE32 readerHandle)
{
	while(1)
	{
		int option = 0;
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. Simple");
		wprintf(L"\n2. Periodic Inventory");
		wprintf(L"\n3. Pre- filter");
		wprintf(L"\n4. Back  to main menu\n");
		while(1 != scanf("%d",&option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}

		switch(option)
		{
		case 1:
			SimpleInventory(readerHandle);
			break;
		case 2:
			PeriodicInventory(readerHandle);
			break;
		case 3:
			InventoryFilterOption(readerHandle);
			break;
		case 4:
			return;
		}
	}
}
void AccessMenu(RFID_HANDLE32 readerHandle)
{
	int option = 0;
	while(1)
	{
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. Access Operation with Specific EPC-ID ");
		wprintf(L"\n2. Access Operation with Access-Filters");
		wprintf(L"\n3. Back  to main menu\n");
	    while(1 != scanf("%d",&option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}
		switch(option)
		{
		case 1:
			PerformSingleTagAccess(readerHandle);
			break;	
		case 2:
			MultipleTagAccess(readerHandle);
			break;	
		case 3:
			return;
		}
	}
}

void InventoryFilterOption(RFID_HANDLE32 readerHandle)
{   
	while(1)
	{

		RFID_STATUS rfidStatus = RFID_API_SUCCESS;
		int option = 0;
		wprintf(L"\n");
		wprintf(L"\n----Command Menu----");
		wprintf(L"\n1. Add Pre-Filter [only 2 filters are allowed]");
		wprintf(L"\n2. Remove PreFilter");
		wprintf(L"\n3. Exit to Inventory-Menu\n");
		while(1 != scanf("%d",&option))
		{
			wprintf(L"\nEnter a Valid Input:");
			clean_stdin();
		}
		switch(option)
		{
		case 1:
			rfidStatus = AddPreFilter(readerHandle);	
			break;
		case 2:
			rfidStatus = RemovePrefilter(readerHandle);
			break;
		case 3:
			return;
		}
		if(rfidStatus != RFID_API_SUCCESS)
		{
			ERROR_INFO ErrorInfo;
			RFID_GetLastErrorInfo(readerHandle,&ErrorInfo);
			wprintf(L"Operation failed . Reason : %ls",ErrorInfo.statusDesc);
		}
	}
}



