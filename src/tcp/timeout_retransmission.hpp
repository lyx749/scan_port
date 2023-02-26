#ifndef TIMEOUT_RETRANSMISSION_HPP
#define TIMEOUT_RETRANSMISSION_HPP
#include <port_state.hpp>
#include <send_tcp_packet.hpp>
#include <mutex>
#include <chrono>
#include <thread>
namespace protocol
{
    namespace tcp
    {
        std::mutex change_port_state_mutex;
        std::chrono::microseconds retransmission_time(3000000);
        void to_re(std::map<int, port_state> &port, int sockfd, struct sockaddr_in myaddr, struct sockaddr_in target, int begin_port, int end_port)
        {
            int count = 0;
            while (1)
            {
                std::this_thread::sleep_for(retransmission_time);
                // std::cout << "begin_port = " << begin_port << "  "
                //           << "endPort = " << end_port << "to_re" << " count = " << count << std::endl;
                for (int i = begin_port; i <= end_port; ++i)
                {
                    //std::lock_guard<std::mutex> lock(change_port_state_mutex);
                    if (port[i].T_R == 3)
                    {
                        port[i].state = 0;
                    }
                    else if (port[i].state == -1 || port[i].state == 2)
                    {
                        port[i].T_R++;
                        port[i].state = 2;
                        TCP_Send(sockfd, i, 2, myaddr, target);
                    }
                }
                // for (int i = begin_port; i <= end_port; ++i)
                // {
                //     std::cout << "port_id = " << i << " port_state = " << port[i].state << " port_TR = " << port[i].T_R << std::endl;
                // }
                ++count;
                if (count == 4)
                {
                    break;
                }               
            }
            // std::cout << "finish to_re" << std::endl;
        }
        void change_port_state(std::map<int, port_state> &port, int id, int state)
        {
            //std::lock_guard<std::mutex> lock(change_port_state_mutex);
            port[id].state = state;
        }
    }
}


#endif