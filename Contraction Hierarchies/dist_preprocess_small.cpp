#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <iostream>
#include <memory>
#include <cassert>

class Graph
{
    typedef int Distance;
    typedef int Vertex;

    int N;
    int s, t;
    int estimate = INFINITY;
    std::vector<std::vector<std::pair<int, int>>> outgoing_edges;
    std::vector<std::vector<std::pair<int, int>>> incoming_edges;

    static constexpr int INFINITY = std::numeric_limits<int>::max() / 2;
    std::vector<int> level;
    std::vector<int> rank;

    std::vector<std::vector<Distance>> bidistance;
    std::vector<bool> contracted;
    
    class StlHeap
    {
    public:
        using T = std::pair<Distance, Vertex>;
        using Queue = std::priority_queue<T, std::vector<T>, std::greater<T>>;

        StlHeap() {
            queue.reset(new Queue());
        }

        bool empty() const {
            return queue->empty();
        }

        void update(Vertex v, Distance d) {
            queue->push(std::make_pair(d, v));
        }

        void clear() {
            queue.reset(new Queue());
        }

        std::pair<Distance, Vertex> pop() {
            std::pair<Distance, Vertex> top = queue->top();
            queue->pop();
            return top;
        }

    private:
        std::unique_ptr<Queue> queue;
    };

    StlHeap diqueue[2];

public:
    Graph() {
        read_stdin();
        bidistance.resize(2, std::vector<int>(N, INFINITY));
        contracted.resize(N, false);
    }

    int get_n() { return N; }

    std::vector<std::pair<int, int>>& get_adjacent(int v, bool forward = true) {
        if (forward) {
            return outgoing_edges[v];
        } else {
            return incoming_edges[v];
        }
    }

    void preprocess() {
        rank.resize(N, -1);
        level.resize(N, 0);

        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> queue;

        for (int i = 0; i < N; ++i) {
            queue.push({estimate_importance(i), i});
        }

        int current_rank = 0;
        while (!queue.empty()) {
            int v = queue.top().second;
            queue.pop();
            if (contracted[v]) continue;

            std::vector<Shortcut> shortcuts;
            int importance = do_shortcut(v, shortcuts, level[v]);
            
            contracted[v] = true;
            rank[v] = current_rank++;
            
            for (auto& shortcut : shortcuts) {
                add_directed_edge(shortcut.from, shortcut.to, shortcut.cost);
            }

            for (auto& neighbor : outgoing_edges[v]) {
                int w = neighbor.first;
                if (!contracted[w]) {
                    queue.push({estimate_importance(w), w});
                }
            }

            for (auto& neighbor : incoming_edges[v]) {
                int u = neighbor.first;
                if (!contracted[u]) {
                    queue.push({estimate_importance(u), u});
                }
            }
        }
    }

    int query(int u, int w) {
        s = u;
        t = w;

        bidistance[0][s] = 0;
        bidistance[1][t] = 0;
        diqueue[0].update(s, 0);
        diqueue[1].update(t, 0);

        int estimate = INFINITY;

        while (!diqueue[0].empty() || !diqueue[1].empty()) {
            if (!diqueue[0].empty()) {
                auto [dist, v] = diqueue[0].pop();
                if (dist > estimate) break;
                update(v, dist, true);
            }
            if (!diqueue[1].empty()) {
                auto [dist, v] = diqueue[1].pop();
                if (dist > estimate) break;
                update(v, dist, false);
            }
        }

        return (estimate == INFINITY) ? -1 : estimate;
    }

private:
    void update(int v, int d, bool forward) {
        if (d > bidistance[forward][v]) return;

        for (const auto& edge : get_adjacent(v, forward)) {
            int to = edge.first;
            int cost = edge.second;
            if (bidistance[forward][to] > d + cost) {
                bidistance[forward][to] = d + cost;
                diqueue[forward].update(to, d + cost);

                if (bidistance[0][to] + bidistance[1][to] < estimate) {
                    estimate = bidistance[0][to] + bidistance[1][to];
                }
            }
        }
    }

    int estimate_importance(int v) {
        int shortcut_count = outgoing_edges[v].size() + incoming_edges[v].size();
        return shortcut_count + level[v];
    }

    struct Shortcut {
        int from;
        int to;
        int cost;
    };

    int do_shortcut(int v, std::vector<Shortcut>& shortcuts, int& mylevel) {
        for (auto& u : incoming_edges[v]) {
            for (auto& w : outgoing_edges[v]) {
                int shortcut_cost = u.second + w.second;
                add_shortcut(u.first, w.first, shortcut_cost, shortcuts);
            }
        }
        return shortcuts.size() - outgoing_edges[v].size() - incoming_edges[v].size() + mylevel;
    }

    void add_shortcut(int from, int to, int cost, std::vector<Shortcut>& shortcuts) {
        shortcuts.push_back({from, to, cost});
    }

    void set_n(int n) {
        N = n;
        outgoing_edges.resize(n);
        incoming_edges.resize(n);
    }

    void add_edge_to_list(std::vector<std::pair<int, int>>& list, int w, int c) {
        for (auto& p : list) {
            if (p.first == w) {
                if (p.second > c) {
                    p.second = c;
                }
                return;
            }
        }
        list.push_back({w, c});
    }

    void add_directed_edge(int u, int v, int c) {
        add_edge_to_list(outgoing_edges[u], v, c);
        add_edge_to_list(incoming_edges[v], u, c);
    }

    void add_edge(int u, int v, int c) {
        add_directed_edge(u, v, c);
    }

    void finalize() {
        // Optional: You can remove unnecessary edges after preprocessing.
    }

    bool read_stdin() {
        int u, v, c, n, m;
        assert(scanf("%d %d", &n, &m) == 2);
        set_n(n);
        for (int i = 0; i < m; ++i) {
            assert(scanf("%d %d %d", &u, &v, &c) == 3);
            add_edge(u - 1, v - 1, c);
        }
        finalize();
        return true;
    }
};

int main() {
    Graph g;
    g.preprocess();
    std::cout << "Ready" << std::endl;

    int t;
    assert(scanf("%d", &t) == 1);
    for (int i = 0; i < t; ++i) {
        int u, v;
        assert(scanf("%d %d", &u, &v) == 2);
        printf("%d\n", g.query(u - 1, v - 1));
    }
}
