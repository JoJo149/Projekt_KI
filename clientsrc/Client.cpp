#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#include "transposition_table.h"
#include "AI.h"
#include "Utils.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    typedef SOCKET SocketType;
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    typedef int SocketType;
#endif

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

class Network {
private:
    SocketType sock;
    string server_ip;
    int port{};
    #ifdef _WIN32
        WSADATA wsaData;
    #endif

public:
    Network() {
        #ifdef _WIN32
                if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                    cerr << "WSAStartup failed" << endl;
                    exit(1);
                }
        #endif

        loadConfig( "../clientInfo/config.txt");

        struct addrinfo hints{}, *res;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        std::cout << "Connecting to " << server_ip << ":" << port << std::endl;
        int status = getaddrinfo(server_ip.c_str(), std::to_string(port).c_str(), &hints, &res);
        if (status != 0) {
            cerr << "getaddrinfo failed: " << gai_strerror(status) << endl;
            exit(1);
        }
        sock = socket(res->ai_family, res->ai_socktype, 0);
        if (sock < 0) {
            cerr << "Cannot create socket" << endl;
        }

        if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
            cerr << "Connection failed!" << endl;
            freeaddrinfo(res);
            exit(1);
        }

        freeaddrinfo(res);

    }

    void loadConfig(const string& filename) {
        ifstream infile(filename);
        if (!infile) {
            cerr << "Could not open config file." << endl;
            exit(1);
        }

        if (std::getline(infile, server_ip)) {
            // Remove leading and trailing whitespace
            server_ip.erase(0, server_ip.find_first_not_of(" \t\r\n"));
            server_ip.erase(server_ip.find_last_not_of(" \t\r\n") + 1);
        }

        infile >> port;
        infile.close();
    }

    string getP() {
        char buffer[2048] = {0};
        recv(sock, buffer, sizeof(buffer), 0);
        return string(buffer);
    }

    string sendData(const string& data) {
        send(sock, data.c_str(), data.size(), 0);
        char buffer[4096] = {0};
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) return "";
        return string(buffer, len);
    }

    void close() const {
        #ifdef _WIN32
            closesocket(sock);
            WSACleanup();
        #else
            ::close(sock);
        #endif
    }

    ~Network() {
        #ifdef _WIN32
            closesocket(sock);
            WSACleanup();
        #else
            ::close(sock);
        #endif
    }
};

void mainLoop() {
    try {
        Network n; // start connection in constructor

        Game game{};



        int player = stoi(n.getP());
        cout << "You are player " << player << endl;
        int moves = 0;
        while (true) {
            string game_data = n.sendData(json("get").dump());
            if (game_data.empty()) {
                cout << "Game Over" << endl;
                cout << "Moves played:  "<< moves << endl;
                n.close();
                break;
            }

            json input_json = json::parse(game_data);

            if (input_json["bothConnected"]) {
                string turn = input_json["turn"];
                string board = input_json["board"];
                int time_left = input_json["time"];

                if ((player == 0 && turn == "r") || (player == 1 && turn == "b")) {
                    cout << "New Board: " << board << endl;
                    cout << "New Time: " << time_left << endl;

                    cout << "[KI] Thinking..." << endl;
                    game.stringToGame(board.c_str());

                    game.printGame();

                    AI AI{game};

                    string ki_result = AI.alphaBetaTimed().toString();

                    cout << "KI makes Move: " << ki_result << endl;
                    cout << endl;

                    n.sendData(json(ki_result).dump());
                    moves++;
                }
            }
        }
    } catch (const std::exception& ex) {
        cerr << "Exception caught: " << ex.what() << endl;
    }
}

int main() {
    TT::loadFromFile();
    mainLoop();
    TT::saveToFile();
    return 0;
}
