#ifndef CONCURRENT_LINKED_LIST
#define CONCURRENT_LINKED_LIST
#include <bits/stdc++.h>
#include <mutex>
#include <shared_mutex>
#include <memory>

class CausalDot {
        std::string clientUUID;
        long long clientTimestamp;
    public:
        CausalDot(std::string clientUUID, long long clientTimestamp);
        CausalDot(const CausalDot& a);
        bool operator<(const CausalDot& other) const;
        bool operator==(const CausalDot& other) const;
        std::string print();
    friend class Node;
    friend class ConcurrentLinkedList;
};

class Node {

        CausalDot causalDot;
        bool isTombstone;
        std::shared_ptr<Node> nextNode;
        char data;
    public:
        Node(CausalDot causalDot, char data, bool isTombstone = false, std::shared_ptr<Node> nextNode = nullptr);
        Node(const Node& a);
        char getData();
        std::string print();
        friend class ConcurrentLinkedList;
};

class ConcurrentLinkedList {
        std::shared_ptr<Node> head;
        mutable std::shared_mutex mutex; 
        //Will serve as an RW lock so that multiple threads, if needed, can read stuff. Read throughput++
        std::shared_ptr<Node> traverse(int index, bool skipTombstone = true);
        std::shared_ptr<Node> traverseAndDelete(int index);
    public:
        ConcurrentLinkedList();
        std::vector<Node> getCopy() const;
        std::string serialize() const;
        std::string getInorderTraversal() const;
        void markDeleted(int indexToDelete);
        void insertNode(int indexToInsert, CausalDot causalDot, char charToInsert);
        void merge(std::vector<Node> newList);
};

#endif
