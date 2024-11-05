#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Server {
  string name;
  int weight;
};

class LoadBanlancer {
private:
  vector<string> virtualNodes; // 虚拟节点列表
  vector<Server> servers;
  int current;

public:
  LoadBanlancer(vector<Server> server) : current(-1) {
    this->servers = server;

    for (const auto &server : servers) {
      for (int i = 0; i < server.weight; i++) {
        virtualNodes.push_back(server.name);
      }
    }
    random_shuffle(virtualNodes.begin(), virtualNodes.end());
  }

  string getServer() {
    if (virtualNodes.empty())
      return "";
    current = (current + 1) % virtualNodes.size();
    return virtualNodes[current];
  }
};

int main() {

  vector<Server> server = {{"A", 5}, {"B", 3}, {"C", 2}};

  LoadBanlancer lb(server);

  //测试20次

  for (int i = 0; i < 20; i++) {
    cout << "Request " << i + 1 << " goes to: " << lb.getServer() << endl;
  }
}
