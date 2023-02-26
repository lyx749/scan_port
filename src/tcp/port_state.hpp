#ifndef PORT_STATE_HPP
#define PORT_STATE_HPP
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
namespace protocol
{
    namespace tcp
    {
        struct port_state
        {
            int state = -1; // 开启为1 关闭为0 超时为2 默认为-1
            int T_R = 0; //超时重传次数，超过3次，则认为端口不可达
        };
        int check_port_state(std::map<int, port_state> port, int begin_port, int end_port)
        {
            for (int i = begin_port; i <= end_port; ++i)
            {
                if (port[i].state == 1 || port[i].state == 0)
                {
                    continue;
                }
                else
                {
                    //std::cout << i << std::endl;
                    return i;
                }
            }
            return 0;
        }


    }
}



#endif