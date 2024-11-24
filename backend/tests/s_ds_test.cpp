#include "../headers/ConcurrentLL.hpp"
#include <vector>
#include <cstring>

std::vector<Node> deserialize(std::string sen){

    std::vector<Node> outp;
    std::stringstream ss(sen);
    std::string word;
    ss >> word;
    int size = stoi(word);
    for(int i = 0;i < size;i++){
        ss >> word;
        std::cout << word << "kfdjfk" << std :: endl;
        std::stringstream ns(word);
        std::string data;
        std::string tombstone;
        std::string id;
        std::string ts;
        getline(ns,data,':');
        getline(ns,tombstone,':');
        getline(ns,id,':');
        getline(ns,ts,':');
        std::cout << data << ";" << tombstone << ";" << id << ";" << ts << ";" << std::endl;
        char d = data[0];
        bool t = tombstone[0] == '1' ? true : false;
        long long time = std::stoll(ts);
        outp.push_back(Node(CausalDot(id,time),d,t));

    }
    return outp;
}


int main()
{
    ConcurrentLinkedList cll1 = ConcurrentLinkedList();
    for(int i = 0; i < 5; i++)
    {cll1.insertNode(i, CausalDot("kavya", i), 'k' + i);}
    cll1.insertNode(5, CausalDot("kavya", 5), '\n');
    cll1.markDeleted(1);
    std::string s_tree= cll1.serialize();
    std::cout << s_tree << std::endl;
    std::vector<Node> cll_cp = deserialize(s_tree);
    cll1.merge(cll_cp);
    std::cout<< cll1.getInorderTraversal();
    std::cout << "habibi" << std::endl;
}