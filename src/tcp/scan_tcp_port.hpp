#ifndef SCAN_TCP_PORT_HPP
#define SCAN_TCP_PORT_HPP
#define u_c unsigned char
#include <send_tcp_packet.hpp>
#include <check_recv.hpp>
#include <my_error.hpp>
#include <thread>
#include <iostream>
#include <string>
#include <port_state.hpp>
#include <get_my_ip.hpp>
#include <timeout_retransmission.hpp>
#include <map>
#include <vector>
namespace protocol
{
    namespace tcp
    {
        unsigned long const hardware_threads =
            std::thread::hardware_concurrency();
        unsigned long const min_per_thread = 20; // 一个线程处理的最小任务数量
        int get_thread_number(int beginPort, int endPort)
        {
            unsigned long max_threads =
                (endPort - beginPort + 1) / min_per_thread;
            if(max_threads == 0)
                max_threads = 1;
            return std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
        }
        void scan_tcp_port(time_t * start_time)
        {
            struct sockaddr_in myaddr, target;
            std::map<int, port_state> port;
            std::vector<std::thread> retransmission_thread;
            std::vector<std::thread> check_thread;
            std::vector<std::thread> send_thread;

            target.sin_family = AF_INET;
            myaddr.sin_family = AF_INET;
            myaddr.sin_port = htons(12345);

            int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
            if (sockfd == -1)
                protocol::error::handle_void("socket error");
            std::cout << "Below is the output of all the IP of your computer" << std::endl;
            ip::my_ip *__my_ip;
            __my_ip = ip::my_ip::initialize_my_ip(sockfd);
            __my_ip->print_ip_message();
            int id;
            std::cout << "please choose a ip which is not '127.0.0.1' : ";
            std::cin >> id;
            myaddr = __my_ip->choose_ip(id);

            std::string temp;
            std::cout << "Please enter the IP you want to scan: ";
            std::cin >> temp;

            int beginPort, endPort;

            std::cout << "Please enter the port range you want to scan and port's range is <1-65535>" << std::endl;
            std::cout << "begin port: ";
            std::cin >> beginPort;
            std::cout << "end port: ";
            std::cin >> endPort;
            std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"  << std::endl;
            std::cout << "begin scanning, please waite a while" << std::endl;
            time(start_time);
            int nums = get_thread_number(beginPort, endPort);
            int block_size = (endPort - beginPort + 1) / nums;
            if (inet_pton(AF_INET, temp.data(), &target.sin_addr) < 0)
                protocol::error::handle_void("inet_pton target error");

            for (int i = beginPort; i <= endPort; i++)
            {
                port.emplace(std::make_pair(i, port_state()));
            }
            int block_start = beginPort;
            int block_end = block_size;
            for (int i = 0; i < nums; i++)
            {
                check_thread.push_back(std::thread(recvpackage, sockfd, myaddr, target, block_start, block_end, std::ref(port)));
                retransmission_thread.push_back(std::thread(to_re, std::ref(port), sockfd, myaddr, target, block_start, block_end));
                send_thread.push_back(std::thread(send_packet, sockfd, myaddr, target, block_start, block_end));
                block_start = block_end + 1;
                if(i == nums - 2)
                {
                    block_end = endPort;
                }
                else
                {
                    block_end = block_end + block_size;
                }
            }  
            for (int i = 0; i < nums; ++i)
            {
                if (check_thread[i].joinable())
                    check_thread[i].join();
            }
            for (int i = 0; i < nums; ++i)
            {
                if (send_thread[i].joinable())
                    send_thread[i].join();
                if (retransmission_thread[i].joinable())
                    retransmission_thread[i].join();
            }
        }      
    }
}

#endif