#ifndef TCP_PACKET_HPP
#define TCP_PACKET_HPP
#include <ip_packet.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
namespace protocol
{
    namespace tcp
    {
        struct tcp_header
        {

            unsigned short src_port;
            unsigned short dst_port;
            unsigned int seq_no;
            unsigned int ack_no;
#if LITTLE_ENDIAN
            unsigned char reserved_1 : 4;
            unsigned char thl : 4;
            unsigned char flag : 6;
            unsigned char reseverd_2 : 2;
#else
            unsigned char thl : 4;
            unsigned char reserved_1 : 4;
            unsigned char reseverd_2 : 2;
            unsigned char flag : 6;
#endif
            unsigned short wnd_size;
            unsigned short chk_sum;
            unsigned short urgt_p;
        };
        struct psd_hdr
        {
            unsigned long saddr;
            unsigned long daddr;
            char mbz;
            char ptcl;
            unsigned short tcpl;
        };
    }
}

#endif // TCP_PACKET_HPP