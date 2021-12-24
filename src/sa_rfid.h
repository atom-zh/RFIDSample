#ifndef __SA_RFID__
#define __SA_RFID__

/**
* RFID_EVENT_TYPE is a parameter for RFID_RegisterEventNotification.
*/
#if 0
enum sa_rfid_evt {

	GPI_EVENT = 0, 				/**<  A GPI event (state change from high to low, or low to high) has occurred on a GPI port.
								When this event is signaled, RFID_GetEventData can be called to know 
								the GPI port and the event that has occurred. 
								The Dll can store a maximum of
								2000 GPI_EVENT_DATA, which if not retrieved using RFID_GetEventData,
								results in dropping of events in FIFO manner.
								*/
	TAG_DATA_EVENT = 1,			
	TAG_READ_EVENT = 1,			/**<  Applications can register for this event to get Tag Reports from the 
								Reader.
								When the event corresponding to this is signaled, it implies that 
								Tag(s) are available for the Application to read and the application can call
								RFID_GetReadTag till all Tags are fetched from the Dll's Queue.
								The Dll can store a maximum of
								4096 TAG_DATA by default, which if not retrieved using RFID_GetEventData,
								results in dropping of further events. The maximum Tag Storage count can be altered
								using the function RFID_SetTagStorageSettings. Inorder to get Tag reports indicating 
								result of Access-operations, applications can set the value in 
								RFID_SetTagStorageSettings.
								*/
	BUFFER_FULL_WARNING_EVENT = 2,/**<  When the internal buffers are 90% full, this event will be signaled.*/

	ANTENNA_EVENT = 3,			/**<  A particular Antenna has been Connected/Disconnected.
								When this event is signaled, RFID_GetEventData can be called to know 
								the Antenna and the Connection Status that has occurred.
								The Dll can store a maximum of
								2000 ANTENNA_EVENT_DATA, which if not retrieved using RFID_GetEventData,
								results in dropping of events in FIFO manner.
								*/
	INVENTORY_START_EVENT = 4,	/**<  Inventory Operation has started. In case of periodic trigger
									this event will be triggered for each period.*/
	INVENTORY_STOP_EVENT = 5,	/**<  Inventory Operation has stopped. In case of periodic trigger
									this event will be triggered for each period.*/
	ACCESS_START_EVENT = 6,		/**<  Access Operation has started.*/
	ACCESS_STOP_EVENT = 7,		/**<  Access Operation has stopped.*/
	DISCONNECTION_EVENT = 8,	/**< Event notifying disconnection from the Reader. 
								When this event is signaled, RFID_GetEventData can be called to know 
								the reason for the disconnection. The Application can call RFID_Reconnect 
								periodically to attempt reconnection or call RFID_Disconnect to cleanup and exit.*/
	BUFFER_FULL_EVENT = 9,		/**<  When the internal buffers are 100% full, this event will be signaled and 
								  tags are discarded in FIFO manner.*/
    NXP_EAS_ALARM_EVENT = 10,		/**<This Event is generated when Reader finds a(NXP)tag with it's EAS System bit	
									still set to true.>*/

	READER_EXCEPTION_EVENT = 11, /**< Event notifying that an exception has occurred in the Reader. 
								When this event is signaled, RFID_GetEventData can be called to know 
								the reason for the exception. The Application can continue to use the connection if
								the reader renders is usable.*/		
	HANDHELD_TRIGGER_EVENT = 12, 	/**<  A Handheld Gun/Button event Pull/Release has occurred.
								When this event is signaled, RFID_GetEventData can be called to know 
								which event that has occurred. 
								The Dll can store a maximum of
								2000 HANDHELD_TRIGGER_EVENT_DATA, which if not retrieved using RFID_GetEventData,
								results in dropping of events in FIFO manner.
								*/						  
    DEBUG_INFO_EVENT = 13,		/**<This Event is generated when Reader sends a debug information>*/

	TEMPERATURE_ALARM_EVENT = 14, /** <When the reader�s operating temperature reaches Threshold level, 
								  this event will be generated. RFID_GetEventData can be called to get the event 
								  details like source name (PA/Ambient), current Temperature 
								  and alarm Level (Low, High or Critical).>*/
	RF_SURVEY_DATA_READ_EVENT = 15, /**<  Applications can register for this event to get RF Survey Reports from the 
								Reader.
								When the event corresponding to this is signaled, it implies that 
								RF Survey data are available for the Application to read and the application can call
								RFID_GetRFSurvey or rfid_GetRfSurveyData till all RF Survey data are fetched from the Dll's Queue.
								The Dll can store a maximum of
								4096 RF_SURVEY_DATA by default, which if not retrieved using RFID_GetEventData,
								results in dropping of further events. The maximum RF SURVEY Storage count can be altered
								using the function RFID_SetRFSurveyStorageSettings. Inorder to get RFSurvey reports indicating 
								result of Access-operations, applications can set the value in 
								RFID_SetRFSurveyStorageSettings.
								*/

	RF_SURVEY_START_EVENT = 16,/**<  RF Survey Operation has started. In case of periodic trigger
									this event will be triggered for each period.*/

	RF_SURVEY_STOP_EVENT = 17,/**<  RF Survey Operation has stopped. In case of periodic trigger
									this event will be triggered for each period.*/

	AUTONOMOUS_EVENT = 18,/**<  Autonomous Events, RFID_GetEventData() to ask LLRP reader to dump unreported event.
									if enable moving tag, RFID_GetEventData() to ask LLRP reader to dump stationary tag */
};
#endif

#endif