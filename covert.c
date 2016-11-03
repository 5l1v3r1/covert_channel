/*--------------------------------------------------------------------------------------
* covert.c - Sends data over a covert channel
* Copywright (c) 2016 Vishav Singh <vishav27@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* gcc -o covert covert.c 
* 
*-------------------------------------------------------------------------------------*/
#include "covert.h"

/*-----------------------------------------------------------------------------
 * FUNCTION:    main 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   int main(int argc, char **argv)
 *                  argc - argument count
 *                  argv - array of arguments
 * 
 * RETURNS:     Result on success or failure.
 *
 * NOTES: Main entry point into the program. Initializes command-line argument
 *		  Enter client or server mode
 *     
 *----------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	if (getuid() != USER_ROOT) 
	{
		fprintf(stderr, "You must be root to run covert channel.\n");
		exit(ERROR_NOTROOT);
	}

	if(parse_cli(argc, argv) == ERROR_OPTIONS)
	{
		perror("Options");
		exit(ERROR_OPTIONS);
	}

	display_settings(argv[0]);

	if(connection_info.mode_server == TRUE)
	{
		decode_forged_packet();
	}else if(connection_info.mode_server == FALSE)
	{
		send_data();
	}
	return (ERROR_SUCCESS);
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    usage 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   void usage(char *command, int err)
 * 
 * RETURNS:     
 *
 * NOTES: Prints the program usage
 *
 *----------------------------------------------------------------------------*/
void usage(char *command, int err)
{
    if (err == HELP) {
        printf("usage: %s [arguments]\n\n", command);
        printf("Arguments:\n");
        printf("  -s  or  --server       Run as server, otherwise as client\n");
        printf("  -d  or  --dest-ip      Destination IP address\n");
        printf("  -p  or  --dest-port    Destination port\n");
        printf("  -w  or  --window-size  Window size (server/client must agree)\n");
        printf("  -f  or  --file         File to read from or write to\n");
		printf("  -h  or  --help         Prints out this screen\n");
		exit(HELP);
    }
	else if (err == ERROR_OPTIONS)
        printf("Try `%s --help` for more information.\n", command);
	else {
        printf("%s: unknown error\n", command);
        printf("Try `%s --help` for more information.\n", command);
    }	
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    parse_cli 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   int parse_cli(int argc, char *argv[])
 				argc - number of arguments
 				argv - array of arguments
 * 
 * RETURNS:     
 *
 * NOTES: Parses command line arguments also sets defaults
 *----------------------------------------------------------------------------*/
int parse_cli(int argc, char *argv[])
{
	int c, option_index = 0;
	static struct option long_options[] = 
	{
		{"server"			, no_argument		,0,	's'},
		{"destination-ip"	, required_argument	,0,	'd'},
		{"destination-port"	, required_argument	,0,	'p'},
		{"window-size"		, required_argument	,0,	'w'},
		{"file"				, required_argument	,0,	'f'},
		{"help"				, no_argument		,0,	'h'},
		{0 , 0, 0, 0}
	};

	/*We need to set the defaults*/
	connection_info.mode_server = FALSE;
	connection_info.destination_ip = "127.0.0.1";
	connection_info.destination_port = 3141;
	connection_info.th_win = 23456;
	connection_info.file = "/Users/vishavpreetsingh/Desktop/hello.txt";

	while(TRUE)
	{
		c = getopt_long(argc,argv, "sd:p:w:f:h", long_options, &option_index);

		if(c == -1)
		{
			break;
		}	

		switch (c)
		{
			case 0:
				if(long_options[option_index].flag != 0)
				{
					break;
				}
				break;

			case 's':
				connection_info.mode_server = TRUE;
				break;

			case 'd':
				connection_info.destination_ip = optarg;
				break;
			case 'p':
				connection_info.destination_port = atoi(optarg);
				break;
			case 'w':
				connection_info.th_win = atoi(optarg);
				break;
			case 'f':
				connection_info.file = optarg;
				break;
			case 'h':
				usage(argv[0], HELP);
				break;
			default:
				usage(argv[0], ERROR_OPTIONS);
				return ERROR_OPTIONS;
				break;
		}
	}
	return ERROR_SUCCESS;
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    display_settings 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   void display_settings()
 *				
 * 
 * RETURNS:     
 *
 * NOTES: displays the settings used by the user
 *----------------------------------------------------------------------------*/
void display_settings()
{
	if (connection_info.mode_server == TRUE)
		printf("  Running as server:       TRUE\n");
	else
		printf("  Running as client:       TRUE\n");
	printf("  Destination IP address:  %s\n", connection_info.destination_ip);
	printf("  Destination port:        %d\n", connection_info.destination_port);
	printf("  Window size:             %d\n", connection_info.th_win);

	if (connection_info.mode_server == TRUE)
		printf("  Output to File:          %s\n\n", connection_info.file);
	else
		printf("  Input from File:         %s\n\n", connection_info.file);
	
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    send_data 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   int send_data()
 *				
 * 
 * RETURNS:     ERROR_SUCCESS on success 
 *
 * NOTES: sends the crafted packet through covert channel
 *				
 *----------------------------------------------------------------------------*/
int send_data()
{
	FILE *file;
	char buffer[BUFFER_SIZE];
	char ip_address[16];
	struct in_addr address;
	int file_size, read_bytes, file_position = 0;

	if((file = fopen(connection_info.file, "rb")) == NULL)
	{
		return ERROR_FILEIO;
	}else
	{
		fseek(file,0,SEEK_END);
		file_size = ftell(file);
		rewind(file);

		/*Start reading file 32 bits at a time.*/
		while(file_position < file_size)
		{
			read_bytes = fread(buffer, sizeof(char), BUFFER_SIZE, file);
			file_position = file_position + read_bytes;
			sprintf(ip_address,"%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]);
			inet_aton(ip_address, &address);
			/*Start forging the packet with the provided
			Spoofed IP and Send the Packet*/
			forge_packet(address.s_addr);
			usleep(SLEEP_TIME);
			/*Reset the buffer*/
			printf("Forging complete.\n");
			memset(&buffer, '\0', sizeof(buffer));
		}
	}

	fclose(file);
	return ERROR_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    read_data 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   int read_data(char *rec_buffer)
 *				
 * 
 * RETURNS:     ERROR_SUCCESS on success 
 *
 * NOTES: Reads the data from the rec_buffer
 *				
 *----------------------------------------------------------------------------*/
int read_data(char *rec_buffer)
{
	FILE *file;
	if((file = fopen(connection_info.file, "a+")) == NULL)
	{
		return ERROR_FILEIO;
	}

	fprintf(file, "%s", rec_buffer);

	fclose(file);

	return ERROR_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    decode_forged_packet 
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   void decode_forged_packet()
 *				
 * 
 * RETURNS:     
 *
 * NOTES: Decodes the forged packet
 *				
 *----------------------------------------------------------------------------*/
void decode_forged_packet()
{
	int socket_fd, rec_length;

	while(TRUE)
	{
		if((socket_fd = socket(AF_INET, SOCK_RAW, 6)) < ERROR_SUCCESS)
		{
			perror("socket");
			exit(ERROR_SOCKET);
		}

		if((rec_length = read(socket_fd, (struct rec_tcp *)&rec_pkt, BUFFER_RECV)) < ERROR_SUCCESS)
		{
			perror("read");
			exit(ERROR_RECV);
		}

		if((rec_pkt.tcp_header.th_flags == TH_SYN) && (ntohs(rec_pkt.tcp_header.th_win) == connection_info.th_win))
		{
			printf("Recieving Data: %s\n", printIP(rec_pkt.ip_header.ip_src));

			if(read_data(printIP(rec_pkt.ip_header.ip_src)) == ERROR_FILEIO)
			{
				perror("file IO");
				exit(ERROR_FILEIO);
			}	
		}
		close(socket_fd);
	}
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    forge_packet
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   void forge_packet(unsigned int spoofedSrcIP)
 *				
 * 
 * RETURNS:     
 *
 * NOTES: Endoes the packet
 *				
 *----------------------------------------------------------------------------*/
void forge_packet(unsigned int spoofedSrcIP)
{
	struct sockaddr_in sock_address;
	struct timeval time;
	unsigned char *forged_packet;

	int sock_fd;
	int data_len;
	const int on = 1;

	gettimeofday(&time, NULL);
	srand(time.tv_usec);

	forged_packet = (unsigned char *)malloc(60);

	/*IP HEADER*/
	send_tcp.ip_header.ip_tos				=	0x0;
	send_tcp.ip_header.ip_hl 				=	0x5;
	send_tcp.ip_header.ip_v 				=	0x4;
	send_tcp.ip_header.ip_len				= 	sizeof(struct ip) + sizeof(struct tcphdr);
	send_tcp.ip_header.ip_id				=	htonl((int)(255.0 * rand() / (RAND_MAX + 1.0)));
	send_tcp.ip_header.ip_off				=	0x0;
	send_tcp.ip_header.ip_ttl				=	0x64;
	send_tcp.ip_header.ip_sum				=	0;
	send_tcp.ip_header.ip_p					=	IPPROTO_TCP;
	send_tcp.ip_header.ip_src.s_addr 		= 	spoofedSrcIP;
	send_tcp.ip_header.ip_dst.s_addr 		=	inet_addr(connection_info.destination_ip);

	/*TCP HEADER*/
	send_tcp.tcp_header.th_sport 	= 	htons(1 + (int)(10000.0) * rand() / (RAND_MAX + 1.0));
	send_tcp.tcp_header.th_dport 	= 	htons(connection_info.destination_port);
	send_tcp.tcp_header.th_sum 		= 	0;
	send_tcp.tcp_header.th_flags 	= 	TH_SYN;
	send_tcp.tcp_header.th_seq 		= 	htonl(1 + (int)(10000.0) * rand() / (RAND_MAX + 1.0));
	send_tcp.tcp_header.th_off 		= 	sizeof(struct tcphdr) / 4;
	send_tcp.tcp_header.th_win 		= 	htons(connection_info.th_win);
	

	/*Combine  Foged Packet into SOCKET*/

	memset(&sock_address, 0, sizeof(sock_address));
	sock_address.sin_family 		= 	AF_INET;
	sock_address.sin_addr.s_addr 	= 	send_tcp.ip_header.ip_dst.s_addr;

	/*Open RAW sockets for sending*/

	if((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < ERROR_SUCCESS)
	{
		perror("Error While opening RAW Socket");
		exit(ERROR_SOCKET);
	}

	if(setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < ERROR_SUCCESS)
	{
		perror("While setting socket option");
		exit(ERROR_SOCKET);
	}

	send_tcp.ip_header.ip_sum 		= 	in_cksum((unsigned short*)&send_tcp.ip_header, sizeof(send_tcp.ip_header));
	send_tcp.tcp_header.th_sum 		=	in_cksum_tcp(send_tcp.ip_header.ip_src.s_addr, send_tcp.ip_header.ip_dst.s_addr,
												  (unsigned short *)&send_tcp.tcp_header, sizeof(send_tcp.tcp_header));

	memcpy(forged_packet, &send_tcp.ip_header, sizeof(send_tcp.ip_header));
	memcpy(forged_packet + sizeof(send_tcp.ip_header), &send_tcp.tcp_header, sizeof(send_tcp.tcp_header));

	if((data_len = sendto(sock_fd, forged_packet, 60, 0,(struct sockaddr *)&sock_address,
				   sizeof(struct sockaddr))) <ERROR_SUCCESS)
	{
		perror("sendto");
		exit(ERROR_SEND);
	}

	printf("Sending Data: %s\t = %s\n", inet_ntoa(send_tcp.ip_header.ip_src),
			printIP(send_tcp.ip_header.ip_src));

	close(sock_fd);
}


/*-----------------------------------------------------------------------------
 * FUNCTION:    printIP
 * 
 * DATE:        September 23, 2016
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Vishav Singh <vishav27@gmail.com>
 * 
 * PROGRAMMER:  Vishav Singh <vishav27@gmail.com>
 * 
 * INTERFACE:   char *printIP(struct in_addr address)
 *				
 * 
 * RETURNS:     Decoded string
 *
 * NOTES: Splits the IP into 4 parts each 1 characted per section
 *				
 *----------------------------------------------------------------------------*/
char *printIP(struct in_addr address)
{
	int i=0;
	char *ip_str = inet_ntoa(address);
	char *data = malloc((BUFFER_SIZE+1) * sizeof(char));

	data[0] = atoi(strtok(i+ip_str, "."));

	for(i=1; i < BUFFER_SIZE; i++)
	{
		data[i] = atoi(strtok(NULL, "."));
	}
	return data;
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    in_cksum
 * 
 * DATE:        May 17, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    W. Richard Stevens (From UNIX Network Programming)
 * 
 * PROGRAMMER:  W. Richard Stevens (From UNIX Network Programming)
 * 
 * INTERFACE:   in_cksum(unsigned short *addr, int len)
 *                     addr - the IP to be checksummed
 *                     len - lengh of the header
 * 
 * RETURNS:     A checksum on the IP header.
 * 
 * NOTES: Creates a valid checksum for an IP header.
 *
 *----------------------------------------------------------------------------*/
unsigned short in_cksum(unsigned short *addr, int len)
{
	int nleft = len;
	int sum = 0;
	unsigned short *w = addr;
	unsigned short answer = 0;

	/* Our algorithm is simple, using a 32-bit accumulator (sum), we add
	 * sequential 16-bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft ==1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff); /* add high 16 to low 16 */
	sum += (sum >> 16);					/* add carry */
	answer = ~sum;						/* truncate to 16 bits */

	return (answer);
}

/*-----------------------------------------------------------------------------
 * FUNCTION:    in_cksum_tcp
 * 
 * DATE:        May 17, 2010
 * 
 * REVISIONS:   
 * 
 * DESIGNER:    Murat Balaban <murat@enderunix.org>
 * 
 * PROGRAMMER:  Murat Balaban <murat@enderunix.org>
 * 
 * INTERFACE:   in_cksum_tcp(int src, int dst, unsigned short *addr, int len)
 *                   src - source IP address
 *                   dst - destination IP address
 *                   addr - TCP header
 *                   len - length of TCP header
 * 
 * RETURNS:     Valid checksum on TCP header
 * 
 * NOTES: Creates a valid TCP checksum.
 *
 *----------------------------------------------------------------------------*/
unsigned short in_cksum_tcp(int src, int dst, unsigned short *addr, int len)
{
	struct pseudo_hdr {
		struct in_addr src;
		struct in_addr dst;
		unsigned char pad;
		unsigned char proto;
		unsigned short tcp_len;
		struct tcphdr tcp;
	} pseudo_hdr;

	unsigned short answer;

	memset(&pseudo_hdr, 0, sizeof(pseudo_hdr));

	pseudo_hdr.src.s_addr = src;
	pseudo_hdr.dst.s_addr = dst;
	pseudo_hdr.pad = 0;
	pseudo_hdr.proto = IPPROTO_TCP;
	pseudo_hdr.tcp_len = htons(len);

	memcpy(&(pseudo_hdr.tcp), addr, len);

	answer = in_cksum((unsigned short *)&pseudo_hdr, 12 + len);

	return (answer);
}












