#include <iostream>
#include <vector>
#include <cassert>
#include <functional>
#include <queue>
#include <set>

struct IGraph {
    virtual ~IGraph() {};

    virtual void AddEdge(int from, int to) = 0;

    virtual int VerticesCount() const  = 0;

    virtual std::vector<int> GetNextVertices(int vertex) const = 0;
    virtual std::vector<int> GetPrevVertices(int vertex) const = 0;
};



class ListGraph: public IGraph {
public:
    ListGraph(int size): adjacencyLists(size) {};

    ListGraph(const IGraph &graph) : adjacencyLists(graph.VerticesCount()) {
        for (int i = 0; i < graph.VerticesCount(); i++) {
            adjacencyLists[i] = graph.GetNextVertices(i);
        }
    };

    ~ListGraph() {};

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < adjacencyLists.size());
        assert(0 <= to && to < adjacencyLists.size());
        adjacencyLists[from].push_back(to);
    }

    int VerticesCount() const override {
        return (int)adjacencyLists.size();
    }

    std::vector<int> GetNextVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencyLists.size());
        return adjacencyLists[vertex];
    }

    std::vector<int> GetPrevVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencyLists.size());
        std::vector<int> prevVertices;

        for (int from = 0; from < adjacencyLists.size(); from++) {
            for (int to: adjacencyLists[from]) {
                if (to == vertex) {
                    prevVertices.push_back(from);
                }
            }
        }

        return prevVertices;
    }

private:
    std::vector<std::vector<int>> adjacencyLists;
};



class MatrixGraph: public IGraph {
public:
    MatrixGraph(size_t size): adjacencyMatrix(size) {};

    ~MatrixGraph() {};

    MatrixGraph(const IGraph& graph): adjacencyMatrix(graph.VerticesCount()) {
        for (auto i = 0; i < graph.VerticesCount(); i++) {
            adjacencyMatrix[i].resize(graph.VerticesCount());
            for (auto j = 0; j < graph.GetNextVertices(i).size(); j++) {
                adjacencyMatrix[i][graph.GetNextVertices(i)[j]] = 1;
            }
        }
    };

    int VerticesCount() const override {
        return adjacencyMatrix.size();
    };

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < adjacencyMatrix.size());
        assert(0 <= to && to < adjacencyMatrix.size());
        adjacencyMatrix[from][to] = 1;
    };

    std::vector<int> GetNextVertices(int vertex) const override {
        std::vector<int> vector(adjacencyMatrix[vertex].size());
        for (int i = 0; i < adjacencyMatrix[vertex].size(); i++) {
            if (adjacencyMatrix[vertex][i]) {
                vector.push_back(i);
            }
        }
        return vector;
    };

    std::vector<int> GetPrevVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencyMatrix.size());
        std::vector<int> prevVertices;

        for (int from = 0; from < adjacencyMatrix.size(); from++) {
            for (int to: adjacencyMatrix[from]) {
                if (to == vertex) {
                    prevVertices.push_back(from);
                }
            }
        }

        return prevVertices;
    };

private:
    std::vector<std::vector<int> > adjacencyMatrix;
};



class SetGraph: public IGraph {
public:
    SetGraph(size_t size): adjacencySet(size) {};

    SetGraph(const IGraph& graph): adjacencySet(graph.VerticesCount()) {
        for (auto i = 0; i < graph.VerticesCount(); i++) {
            for (auto j = 0; j < graph.GetNextVertices(i).size(); j++) {
                adjacencySet[i].insert(graph.GetNextVertices(i)[j]);
            }
        }
    }

    ~SetGraph() {};

    void AddEdge(int from, int to) override {
        assert(0 <= from && from < adjacencySet.size());
        assert(0 <= to && to < adjacencySet.size());
        adjacencySet[from].insert(to);
    }

    int VerticesCount() const override {
        return (int)adjacencySet.size();
    }

    std::vector<int> GetNextVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencySet.size());
        std::vector<int> to_return;

        for (int i : adjacencySet[vertex]) {
            to_return.push_back(i);
        }

        return to_return;
    }

    std::vector<int> GetPrevVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencySet.size());
        std::vector<int> prevVertices;

        for (int from = 0; from < adjacencySet.size(); from++) {
            for (int to: adjacencySet[from]) {
                if (to == vertex) {
                    prevVertices.push_back(from);
                }
            }
        }

        return prevVertices;
    }

private:
    std::vector<std::set<int> > adjacencySet;
};



class ArcGraph: public IGraph {
public:
    ArcGraph(size_t size): adjacencyPair(size) {};

    ArcGraph(const IGraph& graph): adjacencyPair(graph.VerticesCount()) {
        for (auto i = 0; i < graph.VerticesCount(); i++) {
            for (int j = 0; j < graph.GetNextVertices(i).size(); j++) {
                adjacencyPair[i] = std::make_pair(i, graph.GetNextVertices(i)[j]);
            }
        }
    };

    ~ArcGraph() {};

    void AddEdge(int from, int to) override {
        adjacencyPair[from] = std::make_pair(from, to);
    }

    int VerticesCount() const override {
        return adjacencyPair.size();
    }

    std::vector<int> GetNextVertices(int vertex) const override {
        std::vector<int> vector;
        for (int i = 0; i < adjacencyPair.size(); i++) {
            if (adjacencyPair[i].first == vertex) {
                vector.push_back(adjacencyPair[i].second);
            }
        }
        return vector;
    };

    std::vector<int> GetPrevVertices(int vertex) const override {
        assert(0 <= vertex && vertex < adjacencyPair.size());
        std::vector<int> prevVertices;

        for (int from = 0; from < adjacencyPair.size(); from++) {
            if (from == vertex) {
                prevVertices.push_back(adjacencyPair[from].second);
            }
        }

        return prevVertices;
    };

private:
    std::vector<std::pair<int, int> > adjacencyPair;
};



void BFS(const IGraph &graph, int vertex, std::vector<bool> &visited, std::function<void(int)> &func) {
    std::queue<int> qu;
    qu.push(vertex);
    visited[vertex] = true;

    while (!qu.empty()) {
        int currentVertex = qu.front();
        qu.pop();

        func(currentVertex);

        for (int nextVertex: graph.GetNextVertices(currentVertex)) {
            if (!visited[nextVertex]) {
                visited[nextVertex] = true;
                qu.push(nextVertex);
            }
        }
    }
}



void mainBFS(const IGraph &graph, std::function<void(int)> func) {
    std::vector<bool> visited(graph.VerticesCount(), false);

    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i])
            BFS(graph, i, visited, func);
    }
}

void DFS(const IGraph &graph, int vertex, std::vector<bool> &visited, std::function<void(int)> &func) {
    visited[vertex] = true;
    func(vertex);

    for (int nextVertex: graph.GetNextVertices(vertex)) {
        if (!visited[nextVertex])
            DFS(graph, nextVertex, visited, func);
    }

}

void mainDFS(const IGraph &graph, std::function<void(int)> func) {
    std::vector<bool> visited(graph.VerticesCount(), false);

    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i]) {
            DFS(graph, i, visited, func);
        }
    }
}

void topologicalSortInternal(const IGraph &graph, int vertex, std::vector<bool> &visited, std::deque<int> &sorted) {
    visited[vertex] = true;

    for (int nextVertex: graph.GetNextVertices(vertex)) {
        if (!visited[nextVertex]) {
            topologicalSortInternal(graph, nextVertex, visited, sorted);
        }
    }

    sorted.push_front(vertex);
}

std::deque<int> topologicalSort(const IGraph &graph) {
    std::deque<int> sorted;
    std::vector<bool> visited(graph.VerticesCount(), false);

    for (int i = 0; i < graph.VerticesCount(); i++) {
        if (!visited[i])
            topologicalSortInternal(graph, i, visited, sorted);
    }

    return sorted;
}

int main(int argc, const char * argv[]) { // Test to check all methods
    ListGraph listGraph(7); // Creating test graph
    listGraph.AddEdge(0, 1);
    listGraph.AddEdge(0, 5);
    listGraph.AddEdge(1, 2);
    listGraph.AddEdge(1, 3);
    listGraph.AddEdge(1, 5);
    listGraph.AddEdge(1, 6);
    listGraph.AddEdge(3, 2);
    listGraph.AddEdge(3, 4);
    listGraph.AddEdge(3, 6);
    listGraph.AddEdge(5, 4);
    listGraph.AddEdge(5, 6);
    listGraph.AddEdge(6, 4);

    std::cout << "List Graph:" << std::endl;
    mainBFS(listGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    mainDFS(listGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    for (int vertex: topologicalSort(listGraph)) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    std::cout << "Matrix Graph:" << std::endl;
    MatrixGraph matrixGraph(listGraph);

    mainBFS(matrixGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    mainDFS(matrixGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    for (int vertex: topologicalSort(matrixGraph)) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    std::cout << "Set Graph:" << std::endl;
    SetGraph setGraph(matrixGraph);

    mainBFS(setGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    mainDFS(setGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    for (int vertex: topologicalSort(setGraph)) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    std::cout << "Arc Graph:" << std::endl;
    MatrixGraph arcGraph(setGraph);

    mainBFS(arcGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    mainDFS(arcGraph, [](int vertex){ std::cout << vertex << " ";});
    std::cout << std::endl;

    for (int vertex: topologicalSort(arcGraph)) {
        std::cout << vertex << " ";
    }
    std::cout << std::endl;

    return 0;
}
