#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>

// ---------------- TASK TYPES ----------------
enum TaskType {
    DOCUMENT,
    PPT,
    DATA_ANALYSIS,
    CALCULATION
};

// ---------------- TASK CLASS ----------------
class Task {
private:
    int taskId;
    TaskType type;

public:
    Task(int id, TaskType t) : taskId(id), type(t) {}

    int getId() const { return taskId; }

    std::string getTaskName() const {
        switch (type) {
            case DOCUMENT: return "Document Creation";
            case PPT: return "PPT Creation";
            case DATA_ANALYSIS: return "Data Analysis";
            case CALCULATION: return "Heavy Calculation";
        }
        return "Unknown";
    }

    void execute() const {
        volatile long long dummy = 0;
        for (int i = 0; i < 2000000; i++)
            dummy += i % 3;
    }
};

// ---------------- COMPUTE NODE ----------------
class ComputeNode {
private:
    int nodeId;
    bool alive;
    std::queue<Task> taskQueue;
    int processedCount;

public:
    ComputeNode(int id)
        : nodeId(id), alive(true), processedCount(0) {}

    bool isAlive() const { return alive; }

    void crashNode() {
        alive = false;
        std::cout << "\n❌ Node " << nodeId << " CRASHED!\n";
    }

    void assignTask(Task task) {
        if (alive)
            taskQueue.push(task);
    }

    std::queue<Task> stealTasks() {
        std::queue<Task> temp;
        std::swap(temp, taskQueue);
        return temp;
    }

    void processTasks() {
        if (!alive) return;

        while (!taskQueue.empty()) {
            Task task = taskQueue.front();
            taskQueue.pop();

            std::cout << "Node " << nodeId
                      << " executing Task " << task.getId()
                      << " [" << task.getTaskName() << "]\n";

            task.execute();
            processedCount++;
        }
    }

    int getQueueSize() const { return taskQueue.size(); }
    int getProcessedCount() const { return processedCount; }
    int getId() const { return nodeId; }
};

// ---------------- LOAD BALANCER ----------------
class LoadBalancer {
private:
    std::vector<ComputeNode> nodes;

public:
    LoadBalancer(int n) {
        for (int i = 0; i < n; i++)
            nodes.push_back(ComputeNode(i + 1));
    }

    void distributeTask(Task task) {
        int minLoad = 1e9;
        int selected = -1;

        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i].isAlive() &&
                nodes[i].getQueueSize() < minLoad) {
                minLoad = nodes[i].getQueueSize();
                selected = i;
            }
        }

        if (selected != -1)
            nodes[selected].assignTask(task);
    }

    void simulateCrash() {
        int crashIndex = rand() % nodes.size();
        nodes[crashIndex].crashNode();

        // Reassign tasks of crashed node
        std::queue<Task> lostTasks = nodes[crashIndex].stealTasks();
        while (!lostTasks.empty()) {
            distributeTask(lostTasks.front());
            lostTasks.pop();
        }
    }

    void run() {
        for (auto &node : nodes)
            node.processTasks();
    }

    void status() {
        std::cout << "\n========= FINAL STATUS =========\n";
        for (auto &n : nodes) {
            std::cout << "Node " << n.getId()
                      << " | Alive: " << (n.isAlive() ? "YES" : "NO")
                      << " | Tasks Done: " << n.getProcessedCount()
                      << "\n";
        }
        std::cout << "================================\n";
    }
};

// ---------------- MAIN ----------------
int main() {
    srand(time(0));

    std::cout << "DISTRIBUTED SYSTEM WITH FAULT TOLERANCE\n";

    LoadBalancer lb(4);

    std::vector<TaskType> tasks = {
        DOCUMENT, PPT, DATA_ANALYSIS, CALCULATION,
        DOCUMENT, PPT, DATA_ANALYSIS, CALCULATION
    };

    for (int i = 0; i < tasks.size(); i++)
        lb.distributeTask(Task(i + 1, tasks[i]));

    // 🔥 Simulate node failure
    lb.simulateCrash();

    lb.run();
    lb.status();

    std::cout << "\nSimulation Completed Successfully\n";
    return 0;
}