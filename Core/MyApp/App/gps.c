/**
* @file gps.c
* @brief Behandelt de gps input-strings (NMEA-protocol) van UART1.<br>
* <b>Demonstreert: xMessageBufferRead() </b><br>
* Aan UART1 is een interrupt gekoppeld (zie main.c: HAL_UART_RxCpltCallback(),
* die de inkomende string op een messagebuffer zet, die we hier uitlezen en verwerken.<br>
* @author MSC
*
* @date 5/5/2023
*/
#include <admin.h>
#include "main.h"
#include "cmsis_os.h"
#include "gps.h"


GNRMC gnrmc; // global struct for GNRMC-messages
SemaphoreHandle_t gps_access_semaphore;

void gps_get_GNRMC(GNRMC *latestgnrmc)
{
	xSemaphoreTake(gps_access_semaphore, portMAX_DELAY);

	memcpy(latestgnrmc, &gnrmc, sizeof(GNRMC));

	xSemaphoreGive(gps_access_semaphore);
}

/**
* @brief De chars van de binnengekomen GNRMC-string worden in data omgezet, dwz in een
* GNRMC-struct, mbv strtok(); De struct bevat nu alleen chars - je kunt er ook voor kiezen
* om gelijk met doubles te werken, die je dan met atof(); omzet.
* @return void
*/
void fill_GNRMC(char *message)
{
	xSemaphoreTake(gps_access_semaphore, portMAX_DELAY);
	// example: $GNRMC,164435.000,A,5205.9505,N,00507.0873,E,0.49,21.70,140423,,,A
	//          id    , time     ,s,
	char *tok = ",";
	char *s;

	memset(&gnrmc, 0, sizeof(GNRMC)); // clear the struct

	s = strtok(message, tok); // 0. header;
	strcpy(gnrmc.head, s);

	s = strtok(NULL, tok);    // 1. time; not used

	s = strtok(NULL, tok);    // 2. valid;
	gnrmc.status = s[0];

	s = strtok(NULL, tok);    // 3. latitude;
	strcpy(gnrmc.latitude, s);

	s = strtok(NULL, tok);    // 4. N/S; not used

	s = strtok(NULL, tok);    // 5. longitude;
	strcpy(gnrmc.longitude, s);

	s = strtok(NULL, tok);    // 6. E/W; not used

	s = strtok(NULL, tok);    // 7. speed;
	strcpy(gnrmc.speed, s);

	s = strtok(NULL, tok);    // 8. course;
	strcpy(gnrmc.course, s);

	if (Uart_debug_out & GPS_DEBUG_OUT)
	{
		UART_puts("\r\n\t GPS type: \t");  UART_puts(gnrmc.head);
		UART_puts("\r\n\t status: \t\t");  UART_putchar(gnrmc.status);
		UART_puts("\r\n\t latitude:\t\t"); UART_puts(gnrmc.latitude);
		UART_puts("\r\n\t longitude:\t");  UART_puts(gnrmc.longitude);
		UART_puts("\r\n\t speed:    \t");  UART_puts(gnrmc.speed);
		UART_puts("\r\n\t course:   \t");  UART_puts(gnrmc.course);
	}

	xSemaphoreGive(gps_access_semaphore);
}


/**
* @brief Leest de GPS-NMEA-strings die via de UART via interrupt-handler (HAL_UART_RxCpltCallback)
* binnenkomen. * De handler zet elk inkomende character gelijk op een queue, die hier uitgelezen wordt.
* Vervolgens wordt hiervan een GPS-message opgebouwd en verwerkt.
* @return void
*/
void GPS_getNMEA (void *argument)
{
	gps_access_semaphore=xSemaphoreCreateMutex();

    char  Q_buff[GPS_MAXLEN];   // buffer for chars in queue
	char  MSG_buff[GPS_MAXLEN]; // buffer for GPS-string
	int   pos = 0;
	int   cs;                   // checksum-flag
	int   new_msg = FALSE;      // do we encounter a '$'-char?
	int   msg_type = 0;         // do we want this message to be interpreted?

	UART_puts((char *)__func__); UART_puts("started\n\r");

	while (TRUE)
	{
		xQueueReceive(hGPS_Queue, Q_buff, portMAX_DELAY); // get one char from the q

		//UART_putchar(*Q_buff);  // echo, for testing

		if (*Q_buff == '$') // gotcha, new datastring started
		{
			memset(MSG_buff, 0, sizeof(MSG_buff)); // clear buff
			pos = 0;
			new_msg = TRUE;
		}

		if (new_msg == FALSE) // char only valid if started by $
			continue;

		MSG_buff[pos] = *Q_buff; // copy char read from Q into the msg-buf

		// if pos==5, the message type (f.i. "$GPGSA) is complete, so we now we can determine
		// if we want the rest of the message... else we skip the rest characters
		if (pos == 5)
		{
			msg_type = 0; // reset

			// next, we decide which message types we want to interpret
			// and we set the message-type for later use...
			if      (!strncmp(&MSG_buff[1], "GNRMC", 5)) msg_type = eGNRMC;
			else if (!strncmp(&MSG_buff[1], "GPGSA", 5)) msg_type = eGPGSA;
			else if (!strncmp(&MSG_buff[1], "GNGGA", 5)) msg_type = eGNGGA;

			if (!msg_type) // not an interesting message type
			{
				new_msg = FALSE;
				continue;
			}
		}

		// if we are here, we are reading the rest of the message into the msg_buff
		////////////////////////////////////////////////////////////////////////////
		if (pos >= GPS_MAXLEN - 1) // avoid overflow (should not happen, but still...)
		{
			new_msg = FALSE; // ignore it
			continue;
		}

		if (MSG_buff[pos] == '\r') // end of message encountered - all messages end with <CR-13><LF-10>
		{
			MSG_buff[pos] = '\0';          // close string
			cs = checksum_valid(MSG_buff); // note, checksumchars (eg "*43") are removed from string

			if (Uart_debug_out & GPS_DEBUG_OUT) // output to uart if wanted
			{
				UART_puts("\r\nGPS (UART4): "); UART_puts(MSG_buff);
				UART_puts( cs ? " [cs:OK]\r\n" : " [cs:ERR]\r\n");
			}

			if (cs) // checksum okay, so interpret the message
			{
				switch(msg_type) // extract data from msg into right struct
				{
				case eGNRMC: fill_GNRMC(MSG_buff);
						     // use the data...
						     break;
				case eGPGSA:
				case eGNGGA: break;
				default:     break;
				}
			}

			new_msg = FALSE; // new message possible
			continue;
		}
		pos++; // proceed reading next char from the queue
	}
}


// source: file:///C:/craigpeacock/NMEA-GPS
int hex2int(char *c)
{
	int value;

	value = hexchar2int(c[0]);
	value = value << 4;
	value += hexchar2int(c[1]);

	return value;
}


int hexchar2int(char c)
{
    if (c >= '0' && c <= '9')
        return (c - '0');
    if (c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return (-1);
}


// source: file:///C:/craigpeacock/NMEA-GPS
int checksum_valid(char *string)
{
	char *checksum_str;
	int checksum, i;
	unsigned char calculated_checksum = 0;

	// Checksum is postcede by *
	if ((checksum_str = strchr(string, '*')))
	{
		*checksum_str = '\0'; // Remove checksum from string
		// Calculate checksum, starting after $ (i = 1)
		for (i = 1; i < strlen(string); i++)
			calculated_checksum = calculated_checksum ^ string[i];

		checksum = hex2int((char *)checksum_str+1);
		//printf("Checksum Str [%s], Checksum %02X, Calculated Checksum %02X\r\n",(char *)checksum_str+1, checksum, calculated_checksum);
		if (checksum == calculated_checksum)
			return (1);
	}

	return (0);
}
