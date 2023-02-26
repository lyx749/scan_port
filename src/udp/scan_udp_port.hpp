#ifndef SCAN_UDP_PORT_HPP
#define SCAN_UDP_PORT_HPP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string>
#include <scan_tcp_port.hpp>
#include <vector>
#include <thread>
#define CHKADDRESS(_saddr_)                                                                            \
    {                                                                                                  \
        u_char *p = (char *)&(_saddr_);                                                                \
        if ((p[0] == 10) || (p[0] == 168 && 16 <= p[1] && p[1] <= 31) || (p[0] == 192 && p[1] == 168)) \
        {                                                                                              \
        }                                                                                              \
        else                                                                                           \
        {                                                                                              \
            fprintf(stderr, " IP address error.\n");                                                   \
            exit(EXIT_FAILURE);                                                                        \
        }                                                                                              \
    }
namespace protocol
{
    namespace udp
    {
#define MAXBUFF 4192

        class scan_port_by_udp
        {
        private:
            static void scan_udp_port(std::string dst_ip, int startport, int endport);
            std::vector<std::thread> scan_port_thread;
        public:
            void creat_thread(time_t &start_time)
            {
                std::string temp;
                std::cout << "Please enter the IP you want to scan: ";
                std::cin >> temp;

                int beginPort, endPort;
                std::cout << "Please enter the port range you want to scan and port's range is <1-65535>" << std::endl;
                std::cout << "begin port: ";
                std::cin >> beginPort;
                std::cout << "end port: ";
                std::cin >> endPort;
                std::cout << "begin scanning" << std::endl;
                std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
                time(&start_time);
                int thread_nums = tcp::get_thread_number(beginPort, endPort);

                int block_size = (endPort - beginPort + 1) / thread_nums;

                int block_start = beginPort;
                int block_end = block_size;
                //scan_port_thread.push_back(std::thread(scan_udp_port, temp, block_start, block_end));
                for (int i = 0; i < thread_nums; i++)
                {

                    scan_port_thread.push_back(std::thread(scan_udp_port, temp, block_start, block_end));
                    block_start = block_end + 1;
                    if(i == thread_nums - 2)
                    {
                        block_end = endPort;
                    }
                    else
                    {
                        block_end = block_end + block_size;
                    }
                }
            }
            ~scan_port_by_udp()
            {
                for(auto &e : scan_port_thread)
                {
                    if(e.joinable())
                        e.join();
                }
            }
        };
        void scan_port_by_udp::scan_udp_port(std::string dst_ip, int startport, int endport)
        {

            struct icmp *icmp;          /*icmp报头*/
            fd_set select_fd;           /*select用位图*/
            struct sockaddr_in send_sa; /*发送端的地址信息*/
            int recv_sd;                /*接收用的描述符*/
            int send_sd;                /*发送用的描述符*/
            char buff[MAXBUFF];         /*接收包用的缓冲区*/
            int dstport;                /*端口扫描*/
            struct timeval tv;          /*select的定时*/
            /*设定发送端地址*/
            send_sa.sin_family = AF_INET;
            send_sa.sin_addr.s_addr = inet_addr(dst_ip.data());

            // CHKADDRESS(send_sa.sin_addr.s_addr);

            /*打开一个发送用UDP套接字*/
            if ((send_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                perror(" socket(SOCK_DGRAM)");
                exit(EXIT_FAILURE);
            }

            /*打开一个接收用RAW套接字*/
            if ((recv_sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
            {
                perror(" socket(SOCK_RAW)");
                exit(EXIT_FAILURE);
            }

            for (dstport = startport; dstport <= endport; dstport++)
            {
                // fprintf(stdout,"scan port %d \n",dstport);

                /*发送UDP包(只发送报头，数据为0Byte)*/
                send_sa.sin_port = htons(dstport);
                sendto(send_sd, NULL, 0, 0, (sockaddr *)&send_sa, sizeof(send_sa));
                /*设定select的超时值*/
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                while (1)
                {
                    /*设定使用select 来检查描述符*/
                    FD_ZERO(&select_fd);
                    FD_SET(recv_sd, &select_fd);
                    int select_res = select(recv_sd + 1, &select_fd, NULL, NULL, &tv);
                    if (select_res > 0)
                    {
                        struct ip *ip; /*IP报头的结构体*/
                        int hlen;      /*IP报头的长度*/
                        int port;      /*端口号*/

                        /*接收ICMP包(56=20(IP)+8(ICMP)+20(IP)+8(UDP))*/
                        if (recvfrom(recv_sd, buff, MAXBUFF, 0, NULL, NULL) != 56)
                            continue;
                        ip = (struct ip *)buff;
                        hlen = ip->ip_hl << 2;
                        icmp = (struct icmp *)(buff + hlen);
                        /*取出所接收到的ICMP的端口号 */
                        port = ntohs(*(u_short *)(buff + 20 + 8 + 20 + 2));
                        // fprintf(stdout,"got port %d\n",port);

                        /* 检查所接收到的包 */
                        if ((ip->ip_src.s_addr == send_sa.sin_addr.s_addr) && (icmp->icmp_type == ICMP_UNREACH) && (icmp->icmp_code == ICMP_UNREACH_PORT) && (port == dstport))
                        {
                            fprintf(stdout, "%d - is close\n", port);
                            //  fprintf(stdout,"unreachable");
                            break;
                        }
                        else
                        {

                            continue;
                        }
                        /*select 超时*/
                    }
                    else if (select_res == 0)
                    {
                        struct servent *se; /*服务信息*/
                        se = getservbyport(dstport, "udp");
                        fprintf(stdout, "%d - is open ", dstport);
                        fprintf(stdout, " & service is %s\n", se == NULL ? "unknown" : se->s_name);
                        break;
                    }
                }
            }
        }
    }
}



#endif