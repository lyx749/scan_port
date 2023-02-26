#ifndef SEND_TCP_PACKET_HPP 
#define SEND_TCP_PACKET_HPP
#include <tcp_packet.hpp>
#include <check_sum.hpp>
#include <my_error.hpp>
#include <iostream>
namespace protocol
{
    namespace tcp
    {
        void TCP_Send(int sockfd, int port, unsigned char flag,struct sockaddr_in myaddr, struct sockaddr_in target)
        {
            target.sin_port = htons(port);

            char buffer[256];
            memset(buffer, 0, 256);
            tcp_header tcpHeader;
            psd_hdr psdHeader;

            //构造tcp包
            tcpHeader.dst_port = htons(port);
            tcpHeader.src_port = htons(12345);
            tcpHeader.seq_no = htonl(0x12345678);
            tcpHeader.ack_no = 0;
            tcpHeader.thl = (sizeof(tcpHeader)/4);
            //printf("tcpHeader.thl = %d", tcpHeader.thl);
            tcpHeader.flag = flag;
            tcpHeader.wnd_size = htons(2048);
            tcpHeader.urgt_p = 0;
            tcpHeader.chk_sum = 0;

            psdHeader.saddr = myaddr.sin_addr.s_addr;
            psdHeader.daddr = target.sin_addr.s_addr;
            psdHeader.mbz = 0;
            psdHeader.ptcl = IPPROTO_TCP;
            psdHeader.tcpl = htons(sizeof(tcpHeader));

            memcpy(buffer, &psdHeader, sizeof(psdHeader));
            memcpy(buffer + sizeof(psdHeader), &tcpHeader, sizeof(tcpHeader));
            tcpHeader.chk_sum = checksum((unsigned short *)buffer, sizeof(psdHeader) + sizeof(tcpHeader));

            memset(buffer, 0, 256);

            memcpy(buffer, &tcpHeader, sizeof(tcpHeader));
            int ret;
            if ((ret = sendto(sockfd, buffer, sizeof(tcpHeader), 0, (struct sockaddr *)&target, sizeof(target))) < 0)
                protocol::error::handle_void("sendto error");
        }

        void send_packet(int sockfd, struct sockaddr_in myaddr, struct sockaddr_in target, int beginPort, int endPort)
        {
            for (int i = beginPort; i <= endPort; ++i)
            {
                TCP_Send(sockfd, i, 2, myaddr, target);
            }
        }
    }
}

#endif