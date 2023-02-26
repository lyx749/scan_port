#include <scan_tcp_port.hpp>
#include <scan_udp_port.hpp>
#include <ctime>
int main()
{
    time_t start_time, end_time;
choose:
    std::cout << "please choose scan mode udp or tcp" << std::endl;
    std::cout << "<1> udp <2>tcp" << std::endl;
    int choose_id;
    std::cin >> choose_id;
    if (choose_id == 2)
    {
        protocol::tcp::scan_tcp_port(&start_time);
    }
    else if (choose_id == 1)
    {
        protocol::udp::scan_port_by_udp scan_udp_port;
        scan_udp_port.creat_thread(start_time);
    }
    else
    {
        std::cerr << "input error" << std::endl;
        goto choose;
    }
    // time(&end_time);
    // std::cout << "need " << (double)end_time - start_time << "s" << std::endl;
}
