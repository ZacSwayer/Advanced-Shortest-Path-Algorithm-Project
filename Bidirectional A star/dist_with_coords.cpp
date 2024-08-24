#include <cstdio>
#include <cassert>
#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include <cmath>

using namespace std;

// Type aliases and constants
typedef vector<vector<vector<int>>> Adj;
typedef long long Len;
typedef vector<priority_queue<pair<Len, int>, vector<pair<Len, int>>, greater<pair<Len, int>>>> Queue;

const Len INF = numeric_limits<Len>::max() / 4;

class AStar {
    int n_;
    Adj adj_;
    Adj cost_;
    vector<vector<Len>> distance_;
    vector<int> workset_;
    vector<bool> visited_;
    std::vector<std::pair<Len, Len>> xy_;

public:
    AStar(int n, Adj adj, Adj cost, std::vector<std::pair<Len, Len>> xy)
        : n_(n), adj_(adj), cost_(cost), distance_(2, vector<Len>(n_, INF)), visited_(n), xy_(xy)
    { workset_.reserve(n); }

    // Clears the workset and resets distances
    void clear() {
        for (int i = 0; i < workset_.size(); ++i) {
            int v = workset_[i];
            distance_[0][v] = distance_[1][v] = INF;
            visited_[v] = false;
        }
        workset_.clear();
    }

    // Visits a node v from one side (0 for forward, 1 for backward)
    void visit(Queue& q, int side, int v, Len dist) {
        if (distance_[side][v] > dist) {
            distance_[side][v] = (dist);
            q[side].emplace(dist, v);
            workset_.push_back(v);
        }
    }

    // Heuristic function: Euclidean distance between two nodes
    Len heuristic(int u, int v) {
        Len dx = xy_[u].first - xy_[v].first;
        Len dy = xy_[u].second - xy_[v].second;
        return sqrt(dx * dx + dy * dy);
    }

    // Returns the shortest path estimate when both searches meet
    Len shortestPathEstimate() {
        Len result = INF;
        for (int v : workset_) {
            if (distance_[0][v] != INF && distance_[1][v] != INF) {
                result = min(result, distance_[0][v] + distance_[1][v]);
            }
        }
        return result;
    }

    // Returns the distance from s to t in the graph
    Len query(int s, int t) {
        clear();
        Queue q(2); // Priority queues for forward and backward search
        visit(q, 0, s, 0);
        visit(q, 1, t, 0);

        while (!q[0].empty() || !q[1].empty()) {
            // Forward search
            if (!q[0].empty()) {
                auto [dist, v] = q[0].top();
                q[0].pop();
                
                if (visited_[v]) continue;
                visited_[v] = true;

                for (int i = 0; i < adj_[0][v].size(); ++i) {
                    int u = adj_[0][v][i];
                    Len cost = cost_[0][v][i];
                    visit(q, 0, u, dist + cost);
                }

                if (visited_[v]) return shortestPathEstimate();
            }

            // Backward search
            if (!q[1].empty()) {
                auto [dist, v] = q[1].top();
                q[1].pop();
                
                if (visited_[v]) continue;
                visited_[v] = true;

                for (int i = 0; i < adj_[1][v].size(); ++i) {
                    int u = adj_[1][v][i];
                    Len cost = cost_[1][v][i];
                    visit(q, 1, u, dist + cost);
                }

                if (visited_[v]) return shortestPathEstimate();
            }
        }

        return -1;
    }
};

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    std::vector<std::pair<Len, Len>> xy(n);
    for (int i = 0; i < n; ++i) {
        int a, b;
        scanf("%d%d", &a, &b);
        xy[i] = make_pair(a, b);
    }
    Adj adj(2, vector<vector<int>>(n));
    Adj cost(2, vector<vector<int>>(n));
    for (int i = 0; i < m; ++i) {
        int u, v, c;
        scanf("%d%d%d", &u, &v, &c);
        adj[0][u - 1].push_back(v - 1);
        cost[0][u - 1].push_back(c);
        adj[1][v - 1].push_back(u - 1);
        cost[1][v - 1].push_back(c);
    }

    AStar astar(n, adj, cost, xy);

    int t;
    scanf("%d", &t);
    for (int i = 0; i < t; ++i) {
        int u, v;
        scanf("%d%d", &u, &v);
        printf("%lld\n", astar.query(u - 1, v - 1));
    }
}

