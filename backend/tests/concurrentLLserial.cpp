#include "../headers/concurrentLL.hpp"

int main()
{
    ConcurrentLinkedList cll1 = ConcurrentLinkedList();
    for(int i = 0; i < 5; i++)
    {cll1.insertNode(i, CausalDot("kavya", i), 'k' + i);}
    cll1.insertNode(5, CausalDot("kavya", 5), '\n');
    cll1.markDeleted(1);
    for(auto x: cll1.getCopy())  
    {
        std::cout<<x.getData()<<" ";
    }
    std::cout<<std::endl;
    ConcurrentLinkedList cll2 = ConcurrentLinkedList();
    for(int i = 0; i < 5; i++)
    {cll2.insertNode(i, CausalDot("amish", i), 'a' + i);}
    cll2.insertNode(5, CausalDot("amish", 5), '\n');
    for(auto x: cll2.getCopy())  
    {
        std::cout<<x.getData()<<" ";
    }
    std::cout<<std::endl;
    cll1.merge(cll2.getCopy());
    std::cout<<"cll1 merged is "<<std::endl;
    for(auto x: cll1.getCopy())  
    {
        std::cout<<x.getData()<<" ";
    }
    std::cout<<std::endl;
    cll2.merge(cll1.getCopy());
    for(auto x: cll2.getCopy())  
    {
        std::cout<<x.getData()<<" ";
    }
    std::cout<<std::endl;
    // cll1.merge(cll2.getCopy());
    // for(auto x: cll1.getCopy())  
    // {
    //     std::cout<<x.getData()<<" ";
    // }
    // std::cout<<"cll1 inorder "<<cll1.getInorderTraversal()<<std::endl;
    // std::cout<<std::endl;
    // for(int i = 0; i < 5; i++)
    // {cll1.insertNode(i, CausalDot("kavya", i), '0' + i);
    // }
    // cll1.markDeleted(7);
    // for(auto x: cll1.getCopy())  
    // {
    //     std::cout<<x.getData()<<" ";
    // }
    // std::cout<<"cll1 inorder "<<cll1.getInorderTraversal()<<std::endl;
}