#ifndef CHECK_RECV_HPP
#define CHECK_RECV_HPP
#include <send_tcp_packet.hpp>
#include <port_state.hpp>
#include <iostream>
#include <timeout_retransmission.hpp>
namespace protocol
{
    namespace tcp
    {
        void recvpackage(int sockfd, struct sockaddr_in myaddr, struct sockaddr_in target, int beginPort, int endPort, std::map<int, port_state> &port);
    }
}

void protocol::tcp::recvpackage(int sockfd, struct sockaddr_in myaddr, struct sockaddr_in target, int beginPort, int endPort, std::map<int, port_state> &port)
{
    // std::cout << "begin recvpackage" << std::endl;
    // for (int i = beginPort; i <= endPort; i++)
    // {
    //     TCP_Send(sockfd, i, 2, myaddr, target);
    // }
    // std::cout  << "thread_id = "<< std::this_thread::get_id()<< " beginport = " << beginPort << " "
    //           << "endPort = " << endPort << std::endl;
    tcp_header *testtcp;
    char msg[1024];
    socklen_t len = sizeof(struct sockaddr_in);
    int count, size;
    while (1)
    {
        if (!check_port_state(port, beginPort, endPort))
        {
            break;
        }
        memset(msg, 0, 1024);
        size = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr *)&myaddr, &len);
        if (size == -1)
        {
            break;
        }
        testtcp = (tcp_header *)(msg + sizeof(protocol::ip::ip_header)); // testtcp指向tcp报文头部
        
        //printf("testtcp flag = %d\n", testtcp->flag);
        if (size < sizeof(tcp_header) + sizeof(protocol::ip::ip_header)) //如果收到的包长度小于头部之和，不符合
        {
            printf("size = %d\n", size);
            continue;
        }
        if (ntohs(testtcp->dst_port) != 12345)
        {
            // printf("testtcp->dstport =  %d\n", testtcp->dst_port);
            continue;
        }
        if (testtcp->flag == 20) //收到ACK+RST,表示相应端口关闭
        {
            // printf("%d close\n", ntohs(testtcp->src_port));
            // if (ntohs(testtcp->src_port) == endPort) //如果受到的TCP包的源端口为截至端口，跳出循环
            // {
            //     break;
            // }
            change_port_state(port, ntohs(testtcp->src_port), 0);
            continue;
        }
        if (testtcp->flag == 18) //收到ACK+SYN，表示相应端口开放
        {
            // printf("%d open!ACK+SYN...\n", ntohs(testtcp->src_port));
            // if (ntohs(testtcp->src_port) == endPort)
            // {
            //     break;
            // }
            TCP_Send(sockfd, ntohs(testtcp->src_port), 4, myaddr, target); //回复RST,主动切断三次握手的连接，避免真正建立连接
            change_port_state(port, ntohs(testtcp->src_port), 1);
            continue;
        }
    }
    for (int i = beginPort; i <= endPort; ++i)
    {
        if(port[i].state == 1)
        {
            std::cout << i << "open!" << std::endl;
        }
        // else
        // {
        //     std::cout << i << "close!" << std::endl;
        // }
    }
    // std::cout << "finsh" << std::endl;
}
#endif