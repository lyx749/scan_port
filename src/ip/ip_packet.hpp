#ifndef IP_PACKET_HPP
#define IP_PACKET_HPP   

namespace protocol
{
    namespace ip
    {
        struct ip_header
        {
            unsigned char h_lenver;        
            unsigned char tos;             
            unsigned short total_len;      
            unsigned short ident;          
            unsigned short frag_and_flags; 
            unsigned char ttl;             
            unsigned char proto;           
            unsigned short checksum;       
            unsigned int sourceIP;         
            unsigned int destIP;
        };
        
    }
}

#endif 