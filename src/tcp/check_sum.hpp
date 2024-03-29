#ifndef CHECK_SUM_HPP
#define CHECK_SUM_HPP
namespace protocol
{
    namespace tcp
    {
        unsigned short checksum(unsigned short *addr, int len)
        {
            int nleft = len;
            int sum = 0;
            unsigned short *w = addr;
            unsigned short answer = 0;

            while (nleft > 1)
            {
                sum += *w++;
                nleft -= 2;
            }

            if (nleft == 1)
            {
                *(unsigned char *)(&answer) = *(unsigned char *)w;
                sum += answer;
            }

            sum = (sum >> 16) + (sum & 0xffff);
            sum += (sum >> 16);
            answer = ~sum;
            return (answer);
        }
    }
}
#endif