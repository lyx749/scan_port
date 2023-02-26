#ifndef GET_MY_IP_HPP
#define GET_MY_IP_HPP
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <vector>
#include <stdio.h>
#include <my_error.hpp>
namespace protocol
{
    namespace ip
    {
        class my_ip
        {
            private:
                static my_ip *this_computer_ip;
                struct ifreq *__ifreq = NULL;
                struct ifconf __ifconf;
                int id = 1;
                my_ip(int sockfd)
                {
                    char *buff = new char[512];
                    __ifconf.ifc_len = 512;
                    __ifconf.ifc_buf = buff;
                    ioctl(sockfd, SIOCGIFCONF, &__ifconf);
                    __ifreq = (struct ifreq *)(__ifconf.ifc_buf);
                }
                std::vector<struct sockaddr_in> all_ip;
            public:
                static my_ip *initialize_my_ip(int sockfd)
                {
                    if (!this_computer_ip)
                    {
                        this_computer_ip = new my_ip(sockfd);
                    }
                    return this_computer_ip;
                }
                void print_ip_message()
                {
                    struct ifreq *free_ifreq = __ifreq;
                    for (int i = (__ifconf.ifc_len / sizeof(struct ifreq)); i > 0; i--, id++)
                    {
                        if (__ifreq->ifr_flags == AF_INET)
                        { // for ipv4
                            printf("<%d> ", id);
                            printf("name =[%s] : ", __ifreq->ifr_name);
                            printf("local addr = [%s]\n", inet_ntoa(((struct sockaddr_in *)&(__ifreq->ifr_addr))->sin_addr));
                            all_ip.push_back(*(struct sockaddr_in *)&(__ifreq->ifr_addr));
                            __ifreq++;
                        }
                    }
                    delete free_ifreq;
                    free_ifreq = NULL;
                    __ifreq = NULL;
                }
                struct sockaddr_in choose_ip(int id)
                {
                    if (this->id < id)
                    {
                        error::handle_void("input error don't have this id\n");
                    }
                    return all_ip[id - 1];
                }
                class delete_class
                {
                    ~delete_class()
                    {
                        delete this_computer_ip;
                    }
                };
        };
        my_ip *my_ip::this_computer_ip = NULL;
    }
}


#endif