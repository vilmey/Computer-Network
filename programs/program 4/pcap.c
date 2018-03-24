/*
 * Alberto Armienta and Vilmey Francisco Romano Filho
 * Program 4
 * Networks, Spring 2015
 */
#include <pcap/pcap.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <string.h>

/* Maximum time that the OS will buffer packets before giving them to your program. */
#define MAX_BUFFER_TIME_MS (300)

/* Maximum time the program will wait for a packet during live capture.
 * Measured in MAX_BUFFER_TIME_MS units. Program closes when it expires. */
#define MAX_IDLE_TIME 100 /* 100*MAX_BUFFER_TIME_MS idle time at most */

#define ETHER_HDR_SIZE 14

/* Function that create the structures necessary to perform a packet capture and
 * determines capture source depending on arguments. Function will terminate the
 * program on error, so return value always valid. */
pcap_t* setup_capture(int argc, char *argv[], char *use_file);

/* Cleanup the state of the capture. */
void cleanup_capture(pcap_t *handle);

/* Check for abnormal conditions during capture.
 * 1 returned if a packet is ready, 0 if a packet is not available.
 * Terminates program if an unrecoverable error occurs. */
char valid_capture(int return_value, pcap_t *pcap_handle, char use_file);

/********My functions*********/
void print_ipv4(const u_char *, uint8_t , uint8_t );
void print_ipv6(const u_char *, uint8_t , uint8_t );
void print_protocol(const u_char *, uint8_t);

int main(int argc, char *argv[]) {

	pcap_t *pcap_handle = NULL;             /* Handle for PCAP library */
	struct pcap_pkthdr *packet_hdr = NULL;  /* Packet header from PCAP */
	const u_char *packet_data = NULL;       /* Packet data from PCAP */
	int ret = 0;                            /* Return value from library calls */
	char use_file = 0;                      /* Flag to use file or live capture */
  struct ether_addr source_addrs;
  struct ether_addr dest_addrs;
  uint8_t version, length, protocol;

	/* Setup the capture and get the valid handle. */
  pcap_handle = setup_capture(argc, argv, &use_file);

  /* Loop through all the packets in the trace file.
   * ret will equal -2 when the trace file ends.
   * ret will never equal -2 for a live capture. */
  ret = pcap_next_ex(pcap_handle, &packet_hdr, &packet_data);
  //int i = 1;
  while( ret != -2 ) {
    if( valid_capture(ret, pcap_handle, use_file) ){
      version = *(packet_data + 14) >> 4;
      /*******Ethernet header stuff***********************/

      memcpy(source_addrs.ether_addr_octet, (packet_data + 6), ETHER_ADDR_LEN);
      memcpy(dest_addrs.ether_addr_octet, packet_data , ETHER_ADDR_LEN);
      printf("%s -> ",ether_ntoa(&source_addrs));
      printf("%s\n",ether_ntoa(&dest_addrs));
      /*************IP STUFF*****************************/
      if(version == 4 && ( 0x0800 == ntohs(*(uint16_t *)(packet_data + 12) ) ) )
      {
        length = (*(packet_data + 14) & 0x0f) << 2;
        protocol = *(uint8_t*)(packet_data + 23 );
        print_ipv4(packet_data + 14, length, protocol);
      }
      else if(version == 6 && ( 0x86DD == ntohs(*(uint16_t *)(packet_data + 12 ) ) ))
      {
        protocol = *(uint8_t*)(packet_data + 20);
        length = 32;
        print_ipv6(packet_data + 22, length, protocol);
      }
      else
        printf("    [%"PRIu8"]\n", ntohs(*(uint16_t *)(packet_data + 12)));
      //i++;
    }

    /* Get the next packet */
    ret = pcap_next_ex(pcap_handle, &packet_hdr, &packet_data);
  }

  cleanup_capture(pcap_handle);
  return 0;
}

void print_ipv4(const u_char *packet, uint8_t len, uint8_t prot)
{
  char ipSrc[INET_ADDRSTRLEN], ipDest[INET_ADDRSTRLEN];
  struct in_addr ipv4_src, ipv4_dest;

  ipv4_src.s_addr = *(unsigned int *)(packet + 12);
  ipv4_dest.s_addr = *(unsigned int *)(packet + 16);
  inet_ntop(AF_INET, &ipv4_src, ipSrc, INET_ADDRSTRLEN);
  inet_ntop(AF_INET, &ipv4_dest, ipDest, INET_ADDRSTRLEN);
  printf("    [IPv4] %s -> %s\n", ipSrc, ipDest);
  print_protocol(packet + len, prot);
}

void print_ipv6(const u_char *packet, uint8_t len, uint8_t prot)
{
  char ipSrc[INET6_ADDRSTRLEN], ipDest[INET6_ADDRSTRLEN];
  struct in6_addr ipv6_src, ipv6_dest;
  
  memcpy(ipv6_src.s6_addr, packet, sizeof(ipv6_src.s6_addr));
  memcpy(ipv6_dest.s6_addr, packet + 16, sizeof(ipv6_src.s6_addr));
  inet_ntop(AF_INET6, &ipv6_src, ipSrc, INET6_ADDRSTRLEN);
  inet_ntop(AF_INET6, &ipv6_dest, ipDest, INET6_ADDRSTRLEN);
  printf("    [IPv6] %s -> %s\n", ipSrc, ipDest);
  print_protocol(packet + len, prot);
}

void print_protocol(const u_char *packet, uint8_t protocol)
{
  unsigned short srcPort, dstPort;

  srcPort = *(unsigned short*)(packet);
  dstPort = *(unsigned short*)(packet + 2); 
  if(protocol == 6)
  {
    int syn_fin = 0;
    char str[3]; 
    if ((*(uint8_t *)(packet + 13) & 0x02) != 0)
    {
      syn_fin = 1;
      strcpy(str, "SYN");
    }
    else if ((*(uint8_t *)(packet + 13) & 0x01) != 0)
    {
      syn_fin = 1;
      strcpy(str, "FIN");
    }

    if( syn_fin == 1)
      printf("    [TCP] %hu -> %hu %s\n", ntohs(srcPort), ntohs(dstPort), str);
    else
      printf("    [TCP] %hu -> %hu\n", ntohs(srcPort), ntohs(dstPort));
  }
  else if (protocol == 17)
    printf("    [UDP] %hu -> %hu\n", ntohs(srcPort), ntohs(dstPort));
  else 
    printf("    [%"PRIu8"]\n",protocol);
}

pcap_t* setup_capture(int argc, char *argv[], char *use_file) {
  char *trace_file = NULL;                /* Trace file to process */
  pcap_t *pcap_handle = NULL;             /* Handle for PCAP library to return */
  char pcap_buff[PCAP_ERRBUF_SIZE];       /* Error buffer used by pcap functions */
  char *dev_name = NULL;                  /* Device name for live capture */

  /* Check command line arguments */
  if( argc > 2 ) {
    fprintf(stderr, "Usage: %s [trace_file]\n", argv[0]);
    exit(-1);
  }
  else if( argc > 1 ){
    *use_file = 1;
    trace_file = argv[1];
  }
  else {
    *use_file = 0;
  }

  /* Open the trace file, if appropriate */
  if( *use_file ){
    pcap_handle = pcap_open_offline(trace_file, pcap_buff);
    if( pcap_handle == NULL ){
      fprintf(stderr, "Error opening trace file \"%s\": %s\n", trace_file, pcap_buff);
      exit(-1);
    }
    printf("Processing file '%s'\n", trace_file);
  }
  /* Lookup and open the default device if trace file not used */
  else{
    dev_name = pcap_lookupdev(pcap_buff);
    if( dev_name == NULL ){
      fprintf(stderr, "Error finding default capture device: %s\n", pcap_buff);
      exit(-1);
    }

    /* Use buffer length as indication of warning, per pcap_open_live(3). */
    pcap_buff[0] = 0;

    pcap_handle = pcap_open_live(dev_name, BUFSIZ, 1, MAX_BUFFER_TIME_MS, pcap_buff);
    if( pcap_handle == NULL ){
      fprintf(stderr, "Error opening capture device %s: %s\n", dev_name, pcap_buff);
      exit(-1);
    }
    if( pcap_buff[0] != 0 ) {
      printf("Warning: %s\n", pcap_buff);
    }

    printf("Capturing on interface '%s'\n", dev_name);
  }

  return pcap_handle;

}

void cleanup_capture(pcap_t *handle) {
  /* Close the trace file or device */
  pcap_close(handle);
}

char valid_capture(int return_value, pcap_t *pcap_handle, char use_file) {
  static int idle_count = 0;  /* Count of idle periods with no packets */
  char ret = 0;               /* Return value, invalid by default */

  /* A general error occurred */
  if( return_value == -1 ) {
    pcap_perror(pcap_handle, "Error processing packet:");
    cleanup_capture(pcap_handle);
    exit(-1);
  }

  /* Timeout occured for a live packet capture */
  else if( (return_value == 0) && (use_file == 0) ){
    if( ++idle_count >= MAX_IDLE_TIME ){
      printf("Timeout waiting for additional packets on interface\n");
      cleanup_capture(pcap_handle);
      exit(0);
    }
  }

  /* Unexpected/unknown return value */
  else if( return_value != 1 ) {
    fprintf(stderr, "Unexpected return value (%i) from pcap_next_ex()\n", return_value);
    cleanup_capture(pcap_handle);
    exit(-1);
  }
  /* Normal operation, packet arrived */
  else{
    idle_count = 0;
    ret = 1;
  }

  return ret;
}
