#include <cstdio>
#include <cassert>
#include <vector>
#include <queue>
#include <limits>
#include <utility>

using namespace std;

// External vector of size 2 - for forward and backward search.
typedef vector<vector<vector<int>>> Adj;
typedef long long Len;
typedef vector<priority_queue<pair<Len, int>, vector<pair<Len, int>>, greater<pair<Len, int>>>> Queue;

const Len INFINITY = numeric_limits<Len>::max() / 4;

class Bidijkstra {
    int n_;
    Adj adj_;
    Adj cost_;
    vector<vector<Len>> distance_;
    vector<int> workset_;
    vector<bool> visited_;

public:
    Bidijkstra(int n, Adj adj, Adj cost)
        : n_(n), adj_(adj), cost_(cost), distance_(2, vector<Len>(n, INFINITY)), visited_(n)
    { workset_.reserve(n); }

    void clear() {
        for (int i = 0; i < workset_.size(); ++i) {
            int v = workset_[i];
            distance_[0][v] = distance_[1][v] = INFINITY;
            visited_[v] = false;
        }
        workset_.clear();
    }

    void visit(Queue& q, int side, int v, Len dist) {
        if (distance_[side][v] > dist) {
            distance_[side][v] = dist;
            q[side].push({dist, v});
            workset_.push_back(v);
        }
    }

    Len query(int s, int t) {
        clear();
        Queue q(2);
        visit(q, 0, s, 0);
        visit(q, 1, t, 0);
        
        Len best_dist = INFINITY;

        while (!q[0].empty() || !q[1].empty()) {
            // Forward search
            if (!q[0].empty()) {
                auto [dist, u] = q[0].top();
                q[0].pop();
                
                if (visited_[u]) continue;
                visited_[u] = true;
                
                if (visited_[u]) {
                    best_dist = min(best_dist, distance_[0][u] + distance_[1][u]);
                }
                
                for (int i = 0; i < adj_[0][u].size(); ++i) {
                    int v = adj_[0][u][i];
                    Len cost = cost_[0][u][i];
                    visit(q, 0, v, distance_[0][u] + cost);
                }
            }

            // Backward search
            if (!q[1].empty()) {
                auto [dist, u] = q[1].top();
                q[1].pop();
                
                if (visited_[u]) continue;
                visited_[u] = true;
                
                if (visited_[u]) {
                    best_dist = min(best_dist, distance_[0][u] + distance_[1][u]);
                }
                
                for (int i = 0; i < adj_[1][u].size(); ++i) {
                    int v = adj_[1][u][i];
                    Len cost = cost_[1][u][i];
                    visit(q, 1, v, distance_[1][u] + cost);
                }
            }
        }

        return best_dist == INFINITY ? -1 : best_dist;
    }
};

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
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

    Bidijkstra bidij(n, adj, cost);

    int t;
    scanf("%d", &t);
    for (int i = 0; i < t; ++i) {
        int u, v;
        scanf("%d%d", &u, &v);
        printf("%lld\n", bidij.query(u - 1, v - 1));
    }
}




