#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <arpa/inet.h>
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;
using namespace std;

class Network {
private:
    int sock;
    string server_ip;
    int port;

public:
public:
    Network() {
        loadConfig("../clientDir/config.txt");

        struct sockaddr_in server_addr;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            cerr << "Socket creation failed!" << endl;
            exit(1);
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
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

    ~Network() {
        close(sock);
    }
};

void mainLoop() {
    Network n;
    int player = stoi(n.getP());
    cout << "You are player " << player << endl;

    while (true) {
        this_thread::sleep_for(chrono::milliseconds(1000 / 60)); // ~60 FPS

        string game_data = n.sendData(json("get").dump());
        if (game_data.empty()) {
            cerr << "Couldn't get game" << endl;
            break;
        }

        json game = json::parse(game_data);

        if (game["bothConnected"]) {
            string turn = game["turn"];
            string board = game["board"];
            int time_left = game["time"];

            if ((player == 0 && turn == "r") || (player == 1 && turn == "b")) {
                cout << "New Board: " << board << endl;
                cout << "New Time: " << time_left << endl;

                string move;
                cout << "Enter move (format: E7-F7-1): ";
                cin >> move;

                json move_obj = { {"move", move} };
                n.sendData(move_obj.dump());
            }
        }
    }
}

int main() {
    while (true) {
        mainLoop();
    }
    return 0;
}
