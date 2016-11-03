#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define USER_ROOT 			0
#define SERVER_MODE 		1
#define CLIENT_MODE 		2
#define BUFFER_SIZE			4	/*32 bits*/
#define BUFFER_RECV			128
#define SLEEP_TIME			50000

/*Error Codes*/
#define ERROR_NOTROOT 	 	-5
#define ERROR_SUCCESS		0
#define ERROR_OPTIONS		-2
#define ERROR_FILEIO		-6
#define ERROR_SOCKET		-7
#define ERROR_SEND			-9
#define ERROR_RECV          -8
#define HELP				-3


#define TRUE				1
#define FALSE				0

struct connection_info {
	int 			mode_server;
	char 			*destination_ip;
	unsigned int 	destination_port;
	unsigned int	th_win;
	char 			*file;

} connection_info;

struct send_tcp {
	struct ip ip_header;
	struct tcphdr tcp_header;
} send_tcp;

struct rec_tcp {
	struct ip ip_header;
    struct tcphdr tcp_header;
    char buffer[BUFFER_RECV];
} rec_pkt;

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
    struct tcphdr tcp;
} pseudo_header;

unsigned short in_cksum(unsigned short *addr, int len);
void forge_packet(unsigned int spoofedSrcIP);
int read_data(char *rec_buffer);
int send_data();
char *printIP(struct in_addr address);
void display_settings();
void decode_forged_packet();
int parse_cli(int argc, char *argv[]);
unsigned short in_cksum_tcp(int src, int dst, unsigned short *addr, int len);

