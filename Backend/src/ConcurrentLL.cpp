#include "../headers/ConcurrentLL.hpp"



//All Causal Dot Functions: Do we port to another file?
CausalDot::CausalDot(std::string clientUUID, long long clientTimestamp) : clientUUID(clientUUID), clientTimestamp(clientTimestamp) {}

CausalDot::CausalDot(const CausalDot& a) 
    : clientUUID(a.clientUUID), clientTimestamp(a.clientTimestamp) {}

bool CausalDot::operator<(const CausalDot& other) const 
{
    if (clientTimestamp != other.clientTimestamp)
    {
        return clientTimestamp < other.clientTimestamp;
    }
    else
    {
        return clientUUID < other.clientUUID;
    } 
}

bool CausalDot::operator==(const CausalDot& other) const 
{
    return clientTimestamp == other.clientTimestamp && clientUUID == other.clientUUID;
}
std::string CausalDot::print()
{
    std::string representation = "";
    representation += clientUUID;
    representation += std::to_string(clientTimestamp);
    return representation;
}

//All Node Functions: Do we port to another file?

Node::Node(CausalDot causalDot, char data, bool isTombstone, std::shared_ptr<Node> nextNode): 
    causalDot(causalDot), 
    data(data), 
    isTombstone(isTombstone), 
    nextNode(nextNode) 
    {}

Node::Node(const Node& a): 
    causalDot(a.causalDot), 
    data(a.data), 
    isTombstone(a.isTombstone), 
    nextNode(nullptr) 
    {}

char Node::getData()
{
    return data;
}

std::string Node::print()
{
    std::string representation = "";
    representation += causalDot.print();
    representation += ", data:";
    representation += data;
    representation += ", tombstone : ";
    representation += isTombstone ? "true" : "false";
    representation += " | ";
    return representation;
}
//All ConcurrentLinkedList Functions
std::shared_ptr<Node> ConcurrentLinkedList::traverse(int index, bool skipTombstone) {
    std::shared_ptr<Node> current = head;
    int counter = 0;
    //print nodes for debugging
    while(current != nullptr && counter < index)
    {
        //std::cout<<"counter: "<<counter<<current->print();
        if (skipTombstone && (!current->isTombstone))
        {
            counter++;
        }
        current = current->nextNode;
    }
    
    return current;
}

std::shared_ptr<Node> ConcurrentLinkedList::traverseAndDelete(int index) {
    std::shared_ptr<Node> current = head;
    int counter = 0;
    //print nodes for debugging
    while(current != nullptr && counter <= index)
    {
        //std::cout<<"counter: "<<counter<<current->print();
        if ((!current->isTombstone))
        {
            counter++;
        }
        if(counter == index) break;
        current = current->nextNode;
    }
    
    return current;
}

ConcurrentLinkedList::ConcurrentLinkedList() : head(nullptr) {}

std::vector<Node> ConcurrentLinkedList::getCopy() const 
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::vector<Node> copy;
    std::shared_ptr<Node> current = head;
    while (current != nullptr) 
    {
        copy.push_back(Node(*current));
        current = current->nextNode;
    }
    return copy;
}

std::string ConcurrentLinkedList::getInorderTraversal() const 
{
    std::shared_lock<std::shared_mutex> lock(mutex);
    std::string representation = "";
    std::shared_ptr<Node> current = head;
    
    while (current != nullptr) 
    {
        if(!current->isTombstone)
        {
            if(current->data == '\n'){
            representation.push_back('\\');
            representation.push_back('n');
            }
            else{
            representation.push_back(current->data);
        }
        }
        current = current->nextNode;
    }
    return representation;
}

void ConcurrentLinkedList::markDeleted(int indexToDelete) 
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    std::shared_ptr<Node> nodeToDelete = traverseAndDelete(indexToDelete + 1);  
      
    if (nodeToDelete != nullptr) 
    {
        //std::cout<<"deleting "<<nodeToDelete->getData()<<std::endl;
        nodeToDelete->isTombstone = true;
    }
    else
    {
        std::cout<<"Node not found"<<std::endl;
    }
}

void ConcurrentLinkedList::insertNode(int indexToInsert, CausalDot causalDot, char charToInsert) 
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    auto newNode = std::make_shared<Node>(causalDot, charToInsert);
    
    if (indexToInsert == 0) 
    {
        newNode->nextNode = head;
        head = newNode;
        return;
    }
    std::shared_ptr<Node> prev = traverse(indexToInsert - 1);
    if (prev != nullptr) 
    {
        newNode->nextNode = prev->nextNode;
        prev->nextNode = newNode;
    }
}

void ConcurrentLinkedList::merge(std::vector<Node> newList) 
{
    std::unique_lock<std::shared_mutex> lock(mutex);
    std::shared_ptr<Node> newHead = nullptr;
    std::shared_ptr<Node> current = nullptr;
    
    for (const auto& node : newList) {
        auto newNode = std::make_shared<Node>(node);
        // std::cout<<newNode->causalDot.print()<<" ";
        if (newHead == nullptr) {
            newHead = newNode;
            current = newNode;
        } else {
            current->nextNode = newNode;
            current = newNode;
        }
    }
    // std::cout<<std::endl;
    std::shared_ptr<Node> result = nullptr;
    std::shared_ptr<Node> resultTail = nullptr;
    std::shared_ptr<Node> list1 = head;
    std::shared_ptr<Node> list2 = newHead;
    //print old list
    //std::cout<<"Old list is ";
    auto temp = list1;
    while(temp != nullptr){
        //std::cout<<temp->causalDot.print()<<": "<<temp->getData()<<" ";
        temp = temp->nextNode;
    }
    //std::cout<<std::endl;
    //print new list
    temp = list2;
    // std::cout<<"New list is ";
    // while(temp != nullptr){
    //     std::cout<<temp->causalDot.print()<<": "<<temp->getData()<<" ";
    //     temp = temp->nextNode;
    // }
    std::cout<<std::endl;

    while (list1 != nullptr || list2 != nullptr) {
        std::shared_ptr<Node> nodeToAdd;

        if (list1 == nullptr) 
        {
            nodeToAdd = std::make_shared<Node>(*list2);
            list2 = list2->nextNode;
        } 
        else if (list2 == nullptr) 
        {
            nodeToAdd = std::make_shared<Node>(*list1);
            list1 = list1->nextNode;
        } 
        else if (list1->causalDot == list2->causalDot) 
        {
            // std::cout<<list2->causalDot.print()<<" "<<list2->causalDot.print()<<std::endl;
            nodeToAdd = std::make_shared<Node>(*list1);
            if(list1->isTombstone || list2->isTombstone){
                nodeToAdd->isTombstone = true;
            }
            list1 = list1->nextNode;
            list2 = list2->nextNode;
        } 
        else if (list1->causalDot < list2->causalDot) 
        {
            // std::cout<<list1->causalDot.print()<<" "<<list2->causalDot.print()<<std::endl;
            nodeToAdd = std::make_shared<Node>(*list2);
            list2 = list2->nextNode;
        } 
        else 
        {
            // std::cout<<list1->causalDot.print()<<" "<<list2->causalDot.print()<<std::endl;
            nodeToAdd = std::make_shared<Node>(*list1);
            list1 = list1->nextNode;
        }
        if (result == nullptr) 
        {
            result = nodeToAdd;
            resultTail = nodeToAdd;
        } 
        else 
        {
            resultTail->nextNode = nodeToAdd;
            resultTail = nodeToAdd;
        }
    }
    head = result;
    //print merged list
    temp = head;
    //std::cout<<"Merged list is ";
    while(temp != nullptr){
        //std::cout<<temp->causalDot.print()<<": "<<temp->getData()<<" ";
        temp = temp->nextNode;
    }
    //std::cout<<std::endl;
}

std::string ConcurrentLinkedList::serialize() const{
   std::vector<Node> inp = getCopy();
  std::string outp = "";
  outp += std::to_string(inp.size());
  outp += " ";
  for(int i = 0;i < inp.size();i++){
   outp += std::to_string(int(inp[i].data));
   outp.push_back(':');
   if(inp[i].isTombstone){
    outp.push_back('1');
   }
   else{
    outp.push_back('0');
   }
   outp.push_back(':');
   outp += (inp[i].causalDot).clientUUID;
   outp.push_back(':');
   outp += std::to_string((inp[i].causalDot).clientTimestamp);
  outp += " ";   
  }
return outp;
}
