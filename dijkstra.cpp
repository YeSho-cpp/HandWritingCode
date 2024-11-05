
#include <bits/stdc++.h>
using namespace std;
constexpr int INF = numeric_limits<int>::max() / 2;  // 防止整数溢出的无穷大值
int n;                                  // 节点数量
int m;                                  // 边的数量
int b;                                  // 起始节点
int s;                                  // 终止节点
vector<vector<int>> G;                  // 邻接矩阵
vector<int> weight;                     // 节点权重
vector<int> num;                        // 最短路径数量
vector<int> d;                          // 到各点的最短距离
vector<int> w;                          // 到各点的总权重
vector<bool> vis;                       // 访问标记

void Dijkstra(const int start, const int end) {
    // 初始化各个vector
    w.assign(n, 0);
    w[start] = weight[start];
    num.assign(n, 0);
    num[start] = 1;
    d.assign(n, INF);
    d[start] = 0;
    vis.assign(n, false);

    for (int i = 0; i < n; i++) {
        // 找到当前未访问的最短距离节点
        int u = -1;
        int MIN = INF;
        for (int j = 0; j < n; j++) {
            if (!vis[j] && d[j] < MIN) {
                u = j;
                MIN = d[j];
            }
        }

        if (u == -1) return;
        vis[u] = true;

        // 更新相邻节点
        for (int v = 0; v < n; v++) {
            if (!vis[v] && G[u][v] != INF) {
                if (d[u] + G[u][v] < d[v]) {
                    // 找到更短路径
                    d[v] = d[u] + G[u][v];
                    num[v] = num[u];
                    w[v] = w[u] + weight[v];
                }
                else if (d[u] + G[u][v] == d[v]) {
                    // 找到相同长度的路径
                    if (w[u] + weight[v] > w[v]) {
                        w[v] = w[u] + weight[v];
                    }
                    num[v] += num[u];
                }
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);        // 优化输入输出
    cin.tie(nullptr);

    // 输入基本信息
    cin >> n >> m >> b >> s;

    // 初始化所有vector
    G.assign(n, vector<int>(n, INF));   // n x n的邻接矩阵
    weight.resize(n);                    // 节点权重数组

    // 输入节点权重
    for (int i = 0; i < n; i++) {
        cin >> weight[i];
    }

    // 输入边的信息
    for (int i = 0; i < m; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        G[u][v] = c;
        G[v][u] = c;
    }

    Dijkstra(b, s);
    cout << num[s] << ' ' << w[s];

    return 0;
}