#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <AI.h>
#include <Utils.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    typedef SOCKET SocketType;
#else
    #include <unistd.h>
    #include <arpa/inet.h>
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
        loadConfig("../clientInfo/config.txt");

#ifdef _WIN32
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed" << endl;
            exit(1);
        }
#endif

        sockaddr_in server_addr{};
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            cerr << "Cannot create socket" << endl;
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

        if (connect(sock, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            cerr << "Connection failed!" << endl;
            exit(1);
        }
    }

    void loadConfig(const string& filename) {
        ifstream infile(filename);
        if (!infile) {
            cerr << "Could not open config file." << endl;
            exit(1);
        }
        getline(infile, server_ip);
        infile >> port;
        infile.close();
    }

    string getP() {
        char buffer[2048] = {0};
#ifdef _WIN32
        recv(sock, buffer, sizeof(buffer), 0);
#else
        read(sock, buffer, sizeof(buffer));
#endif
        return string(buffer);
    }

    string sendData(const string& data) {
        send(sock, data.c_str(), data.size(), 0);
        char buffer[4096] = {0};
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0) return "";
        return string(buffer, len);
    }

    ~Network() {
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
    }
};

void mainLoop() {
    Network n; // start connection in constructor

    Game game;

    int player = stoi(n.getP());
    cout << "You are player " << player << endl;

    while (true) {
        string game_data = n.sendData(json("get").dump());
        if (game_data.empty()) {
            cerr << "Couldn't get game" << endl;
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

                // TODO maybe make AI an Class of Functions not Object
                AI AI{game};

                // TODO DEPTH SET TO 5 and real AI
                string ki_result = Utils::convert::moveToString(AI.alphaBetaTimed());

                cout << ki_result << endl;

                std::pair<uint64_t, uint64_t> move = Game::moveStringToBitboard(ki_result);
                game.makeMove(move.first, move.second, ki_result[6]-'0');
                game.printGame();

                n.sendData(json(ki_result).dump());
            }
        }
    }
}

int main() {
    mainLoop();
    return 0;
}
