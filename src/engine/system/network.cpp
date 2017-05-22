#include "network.hpp"
#ifdef _USE_NETWORK_
#include "../mlib/mlib.hpp"

// ==========================================================
// Client
// ==========================================================

NetworkClient::NetworkClient()
{
    type = false;
    wasConnected = false;
    remoteUDPport = sf::Socket::AnyPort;
}

NetworkClient::~NetworkClient()
{
    tcp.disconnect();
    udp.unbind();
}

bool NetworkClient::isConnected()
{
    bool result = (tcp.getRemoteAddress() != sf::IpAddress::None);
    if(wasConnected && !result) disconnect();
    return result;
}

char NetworkClient::connect(const sf::IpAddress& remoteIp, const uint16_t &remotePort, const uint16_t &udpPort)
{
    if(isConnected()) return 1;

    if(tcp.connect(remoteIp, remotePort, sf::seconds(10)) != sf::Socket::Done)
        return 2;

    sf::Packet p;
    std::string s;
    if(tcp.receive(p) != sf::Socket::Done)
    {
        tcp.disconnect();
        return 3;
    }
    p >> s;
    if(s == "TCP_MODE") type = true;
    else if(s == "UDP_MODE") type = false;
    else
    {
        tcp.disconnect();
        return 4;
    }

    if(!type)
    {
        if(udp.bind(udpPort) != sf::Socket::Done)
        {
            tcp.disconnect();
            return 5;
        }
        p.clear();
        p << "CLIENT_PORT" << udp.getLocalPort();
        if(tcp.send(p) != sf::Socket::Done)
        {
            tcp.disconnect();
            return 6;
        }
        p.clear();
        if(tcp.receive(p) != sf::Socket::Done)
        {
            tcp.disconnect();
            return 7;
        }
        p >> s;
        if(s == "SERVER_PORT") p >> remoteUDPport;
        else
        {
            tcp.disconnect();
            return 8;
        }
    }
    p.clear();
    if(tcp.receive(p) != sf::Socket::Done)
    {
        tcp.disconnect();
        return 9;
    }
    p >> id;
    Out = "NetworkClient: Connected with ID #" + mlib::int2str(id) + "\n";

    wasConnected = true;
    return 0;
}

void NetworkClient::disconnect()
{
    tcp.disconnect();
    udp.unbind();
    wasConnected = false;
}

bool NetworkClient::send(sf::Packet& packet)
{
    if(!isConnected()) return false;

    if(type) return (tcp.send(packet) == sf::Socket::Done);
    else return (udp.send(packet, tcp.getRemoteAddress(), remoteUDPport) == sf::Socket::Done);
}

bool NetworkClient::receive(sf::Packet& packet)
{
    if(!isConnected()) return false;

    if(type) return (tcp.receive(packet) == sf::Socket::Done);
    else
    {
        sf::IpAddress ip;
        uint16_t p;
        if(udp.receive(packet, ip, p) == sf::Socket::Done)
        {
            return (ip == tcp.getRemoteAddress());
        }
        else return false;
    }
}

uint16_t NetworkClient::getUDPport() const
{
    return udp.getLocalPort();
}

uint8_t NetworkClient::getID() const
{
    return id;
}

// ==========================================================
// Server
// ==========================================================

NetworkServer::NetworkServer()
{
    maxClient = 4;
    type = false;
}

NetworkServer::~NetworkServer()
{
    netMutex.lock();
    netMutex.unlock();
    stop();
}

bool NetworkServer::isRunning() const
{
    return (listener.getLocalPort() != 0);
}

char NetworkServer::start(const uint16_t &localPort, const uint16_t &udpPort, const bool &comType)
{
    if(isRunning()) return 1;

    netMutex.lock();
    if(listener.listen(localPort) != sf::Socket::Done)
    {
        netMutex.unlock();
        return 2;
    }

    type = comType;
    netMutex.unlock();
    Out = std::string("NetworkServer: Server started in ") + (type ? "TCP mode\n" : "UDP mode\n");
    return 0;
}

void NetworkServer::stop()
{
    netMutex.lock();
    listener.close();
    self.disconnect();

    for(Client& i : clients)
    {
        i.tcp->disconnect();
        delete i.tcp;
        delete i.udp;
    }
    clients.clear();
    netMutex.unlock();
}

void NetworkServer::listen()
{
    if(isRunning())
    {
        netMutex.lock();
        bool newConnection = false;
        sf::SocketSelector selector;
        selector.add(listener);
        if(selector.wait(sf::seconds(1)))
        {
            if(clients.size() < maxClient)
            {
                clients.push_back(Client());
                clients.back().tcp = new sf::TcpSocket();
                if(!type)
                {
                    clients.back().udp = new sf::UdpSocket();
                    clients.back().udp->setBlocking(false);
                    clients.back().udp->bind(sf::Socket::AnyPort);
                }
                else clients.back().udp = nullptr;
                if(listener.accept(*(clients.back().tcp)) == sf::Socket::Done)
                {
                    sf::Packet p;
                    std::string s;
                    if(type) p << "TCP_MODE";
                    else p << "UDP_MODE";

                    if(clients.back().tcp->send(p) == sf::Socket::Done)
                    {
                        if(!type)
                        {
                            p.clear();
                            if(clients.back().tcp->receive(p) == sf::Socket::Done)
                            {
                                p >> s;
                                if(s == "CLIENT_PORT") p >> clients.back().remoteUDPport;
                                p.clear();
                                p << "SERVER_PORT" << clients.back().udp->getLocalPort();
                                if(clients.back().tcp->send(p) == sf::Socket::Done) newConnection = true;
                            }
                        }
                        else newConnection = true;
                    }
                }

                if(newConnection)
                {
                    clients.back().self;
                    uint8_t &ref = clients.back().id;
                    ref = 0;
                    for(size_t i = 0; i < clients.size()-1; ++i)
                    {
                        if(ref == clients[i].id) ++ref;
                        //else if(ref < clients[i].id) break;
                    }
                    sf::Packet p;
                    p << clients.back().id;
                    if(clients.back().tcp->send(p) == sf::Socket::Done)
                        Out = "NetworkServer: Client connected from " + clients.back().tcp->getRemoteAddress().toString() + " [ID:" + mlib::int2str(clients.back().id) + "][" + mlib::int2str(clients.size()) + "/" + mlib::int2str(maxClient) + "]\n";
                    else
                    {
                        delete clients.back().tcp;
                        delete clients.back().udp;
                        clients.pop_back();
                    }
                }
                else
                {
                    delete clients.back().tcp;
                    delete clients.back().udp;
                    clients.pop_back();
                }
            }
            else
            {
                Out = "NetworkServer: Max client count reached. Can't accept connection\n";
                sf::TcpSocket tmp;
                if(listener.accept(tmp) == sf::Socket::Done) tmp.disconnect();
            }
        }
        netMutex.unlock();
    }
}

void NetworkServer::update()
{
    if(isRunning())
    {
        netMutex.lock();
        for(size_t i = 0; i < clients.size(); ++i)
        {
            if(clients[i].tcp->getRemoteAddress() == sf::IpAddress::None)
            {
                Out = "NetworkServer: A Client is disconnected\n";
                delete clients[i].tcp;
                delete clients[i].udp;
                clients.erase(clients.begin()+i);
                i--;
            }
        }
        netMutex.unlock();
    }
}

char NetworkServer::selfConnection(const uint16_t &udpPort)
{
    char c;
    if(!isRunning()) c = -1;
    else if(self.isConnected()) c = -2;
    else
    {
        c = self.connect(sf::IpAddress::LocalHost, listener.getLocalPort(), udpPort);
        netMutex.lock();
        for(Client &i : clients)
        {
            if((udpPort == 0 || self.getUDPport() == i.remoteUDPport) && (i.tcp->getRemoteAddress() == sf::IpAddress::LocalHost))
            {
                Out = "NetworkServer: Self connection detected\n";
                i.self = true;
            }
        }
        netMutex.unlock();
    }
    return c;
}

NetworkClient& NetworkServer::getSelfClient()
{
    return self;
}

void NetworkServer::setMaxClient(const uint8_t& max)
{
    netMutex.lock();
    maxClient = max;
    netMutex.unlock();
}

uint8_t NetworkServer::getMaxClient() const
{
    return maxClient;
}

std::vector<uint8_t> NetworkServer::getConnectedID() const
{
    std::vector<uint8_t> id_list;
    if(!isRunning()) return id_list;

    for(const Client &i : clients) id_list.push_back(i.id);

    return id_list;
}

bool NetworkServer::send(sf::Packet& packet, int32_t target_client)
{
    if(!isRunning() || target_client < 0 || target_client >= (int32_t)clients.size()) return false;

    netMutex.lock();
    if(type) // add error and disconnected check
    {
        if(clients[target_client].tcp->send(packet) != sf::Socket::Done)
        {
            clients[target_client].tcp->disconnect();
        }
    }
    else
    {
        if(clients[target_client].tcp->send(packet) != sf::Socket::Done)
        {
            clients[target_client].udp->send(packet, clients[target_client].tcp->getRemoteAddress(), clients[target_client].remoteUDPport);
        }
    }
    netMutex.unlock();
    return true;
}

bool NetworkServer::receive()
{
    if(!isRunning()) return false;

    netMutex.lock();
    if(type)
    {
        sf::SocketSelector sel;
        for(Client &i : clients) sel.add(*(i.tcp));
        if(sel.wait(sf::Time::Zero))
        {
            for(Client &i : clients)
            {
                if(sel.isReady(*(i.tcp)))
                {
                    i.received.push_back(sf::Packet());
                    i.tcp->receive(i.received.back());
                }
            }
        }
    }
    else
    {
        for(Client &i : clients)
        {
            sf::Packet p;
            sf::IpAddress clientIp = i.tcp->getRemoteAddress();
            if(i.udp->receive(p, clientIp, i.remoteUDPport) == sf::Socket::Done)
            {
                i.received.push_back(p);
            }
        }
    }
    netMutex.unlock();
    return true;
}

bool NetworkServer::hasDataFromAllClients() const
{
    bool r = true;
    for(const Client &i : clients) if(i.received.empty()) r = false;
    return r;
}

bool NetworkServer::hasDataFromClient(uint8_t id) const
{
    for(const Client &i : clients) if(i.id == id) return (!i.received.empty());
    return false;
}

sf::Packet NetworkServer::getPacketFromClient(uint8_t id)
{
    sf::Packet p;
    for(Client &i : clients)
        if(i.id == id)
        {
            if(!i.received.empty())
            {
                p = i.received[0];
                i.received.erase(i.received.begin());
            }
            return p;
        }
    return p;
}

#endif
