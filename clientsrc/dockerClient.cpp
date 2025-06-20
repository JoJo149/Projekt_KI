#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "AI.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

typedef int SocketType;

class Network {
private:
    SocketType sock;
    string server_ip;
    int port{};

public:
    Network() {
        server_ip = "gameserver";
        port = 5555;
        struct addrinfo hints{}, *res;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        cout << "Connecting to " << server_ip << ":" << port << endl;
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
            ::close(sock);
    }

    ~Network() {
            ::close(sock);
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

                    string ki_result = AI.alphaBetaTimed(100000).toString();

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
    // read-only docker
    // TT::saveToFile();
    return 0;
}
