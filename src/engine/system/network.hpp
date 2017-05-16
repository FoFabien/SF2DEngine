#define _USE_NETWORK_
#ifdef _USE_NETWORK_
#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network.hpp>
#include <SFML/System/Mutex.hpp>

class NetworkClient
{
    public:
        NetworkClient();
        ~NetworkClient();
        bool isConnected();
        char connect(const sf::IpAddress& remoteIp, const uint16_t &remotePort, const uint16_t &udpPort);
        void disconnect();

        bool send(sf::Packet& packet);
        bool receive(sf::Packet& packet);

        uint16_t getUDPport() const;
        uint8_t getID() const;
    protected:
        sf::TcpSocket tcp;
        sf::UdpSocket udp;
        bool type; // true = tcp, false = tcp+udp
        bool wasConnected;
        uint16_t remoteUDPport;
        uint8_t id;
};

class NetworkServer
{
    public:
        NetworkServer();
        ~NetworkServer();
        bool isRunning() const;
        char start(const uint16_t &localPort, const uint16_t &udpPort, const bool &comType = false);
        void stop();
        void listen();
        void update();

        char selfConnection(const uint16_t &udpPort);
        NetworkClient& getSelfClient();

        void setMaxClient(const uint8_t& max);
        uint8_t getMaxClient() const;
        std::vector<uint8_t> getConnectedID() const;

        void addDataToPacket(const std::string& str, int32_t target_client = -1);
        void addDataToPacket(const int32_t& i, int32_t target_client = -1);
        bool send(int32_t target_client = -1);
        bool send(sf::Packet& packet, int32_t target_client);
        bool receive();
        bool hasDataFromAllClients() const;
        bool hasDataFromClient(uint8_t id) const;
        sf::Packet getPacketFromClient(uint8_t id);
    protected:
        sf::TcpListener listener;
        bool type; // true = tcp, false = tcp+udp

        struct Client
        {
            sf::TcpSocket *tcp;
            sf::UdpSocket *udp;
            std::vector<sf::Packet> received;
            uint16_t remoteUDPport;
            uint8_t id;
            bool self;
        };

        std::vector<Client> clients;
        sf::Mutex netMutex;
        uint8_t maxClient;
        NetworkClient self;
};

#endif // NETWORK_HPP
#endif
