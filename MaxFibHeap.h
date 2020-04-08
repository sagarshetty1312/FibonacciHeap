#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Node.h"

using namespace std;

//Class containing Max Fibonacci Heap and its functions
class MaxFibHeap{
    public:
    Node* headNodePtr = NULL;//For use during pairwise combine
    Node* maxNodePtr = NULL;
    int nodeIdCounter = 0;

    MaxFibHeap(Node node){
        maxNodePtr = &node;
    }
    
    MaxFibHeap(){
    }

    //Initializes a new node with given frequency and inserts into the Heap
    Node* insertNewNode(int frequency)
    {
        Node* nodePtr = (Node*)malloc(sizeof(Node));
        Node& node = *nodePtr;
        node.setNodeId(nodeIdCounter++);
        node.setFrequency(frequency);
        node.setDegree(0);
        insertIntoTopLevel(nodePtr);
        return nodePtr;
    }

    //Inserts a given node into the top level doubly linked list of the heap
    bool insertIntoTopLevel(Node* nodePtr)
    {
        Node& node = *nodePtr;
        node.parent = NULL;
        
        //Inserting first element into an empty heap
        if(maxNodePtr == NULL) 
        {   
            node.right = &node;
            node.left = &node;
            headNodePtr = &node;
            maxNodePtr = &node;
            return true;
        }

        Node& headNode = *headNodePtr;
        Node maxNode = *maxNodePtr;
        Node& rSibling = *headNode.right;

        //Changing pointers to insert into doubly linked list
        node.right = headNode.right;
        node.left = &headNode;
        headNode.right = nodePtr;
        rSibling.left = nodePtr;
        if(headNode.left == headNodePtr)
        {//For case when heap has one existing element
            headNode.left = nodePtr;
        }

        //Changing max pointer if necessary
        if (node.frequency > maxNode.frequency)
        {
            maxNodePtr = nodePtr;
        }
        return true;
    }

    //IncreaseKey function for the MaxFibonacci Heap
    //Increases frequency of node and balances heap if required
    bool increaseFrequency(Node* nodePtr, int newFreq)
    {
        Node& node = *nodePtr;
        Node& maxNode = *maxNodePtr;
        bool success = true;
        node.frequency = newFreq;
        if(node.parent == NULL){
            if(maxNode.frequency < node.frequency){
                maxNodePtr = nodePtr;
            }
            return success;
        }
        Node& parent = *node.parent;

        //Ensuring the integrity of the max Fibonacci heap
        if(parent.frequency < node.frequency){
            removeNode(nodePtr);
            insertIntoTopLevel(nodePtr);
        }
        return success;
    }

    //Changes pointers to remove the given node from its tree without modifying subtrees
    bool removeNode(Node* nodePtr)
    {
        Node& node = *nodePtr;
        if(node.parent == NULL)
        {//case when at the top level doubly linked list
            return removeFromDoublyLinkedList(nodePtr);
        }

        //change child pointer of parent
        Node& parent = *node.parent;
        if(parent.child == nodePtr){
            if(node.right != nodePtr)
                parent.child = node.right;
            else
                parent.child = NULL;
        }
        //change degree of parent
        parent.degree = parent.degree-1;

        //Removing from doubly linked list
        removeFromDoublyLinkedList(nodePtr);

        //Updating childCut value and ensuring cascading cut for its parent
        if((parent.childCut == true) && (parent.parent != NULL)){
            removeNode(&parent);
            insertIntoTopLevel(&parent);
        } else {
            parent.childCut = true;
        }

        return true;
    }

    //Changes pointers of the given node to remove it from its doubly linked list
    bool removeFromDoublyLinkedList(Node* nodePtr){
        Node& node = *nodePtr;
        if(node.right != nodePtr)//Checking if its a member of a linked list
        {
            Node& rSiblingNode = *node.right;
            Node& lSiblingNode = *node.left;

            rSiblingNode.left = &lSiblingNode;
            lSiblingNode.right = &rSiblingNode;

            node.right = nodePtr;
            node.left = nodePtr;
        }
        return true;
    }

    //Performs arbitrary delete of the given node
    bool deleteNode(Node* nodePtr){
        bool success = true;

        success = removeNode(nodePtr);
        insertChildrenToTopLevel(nodePtr);

        free(nodePtr);
        return success;
    }

    //Performs removeMax function of a Max Fibonacci Heap
    //Extracts node with highest frequency
    Node* removeMax()
    {
        Node* returnNode = NULL;
        if(maxNodePtr == NULL)
        {//Empty heap
            return returnNode;
        }
        
        returnNode = maxNodePtr;
        Node& maxNode = *maxNodePtr;
        
        //Checking if maxFibonacciHeap has only one node
        if((maxNode.right == maxNodePtr) && (maxNode.degree == 0))
        {   
            maxNodePtr = NULL;
            headNodePtr = NULL;
            return returnNode;
            
        }

        //Checking if top level has only one node or not
        if(maxNode.right != maxNodePtr)
        {
            headNodePtr = maxNode.right;
            maxNodePtr = maxNode.right;
            removeFromDoublyLinkedList(returnNode);
        } 
        else
        {
            headNodePtr = maxNode.child;
            maxNodePtr = maxNode.child;
        }

        insertChildrenToTopLevel(returnNode);
        pairwiseCombine();
        return returnNode;
    }

    //Inserts children of given node to the top level doubly linked list of the heap
    bool insertChildrenToTopLevel(Node* nodePtr)
    {
        Node& node = *nodePtr;
        if(node.degree == 0) return true;
        
        Node* nextNodePtr = node.child;

        while(node.degree > 0){
            Node& curNode = *nextNodePtr;
            nextNodePtr = curNode.right;
            removeFromDoublyLinkedList(&curNode);
            insertIntoTopLevel(&curNode);
            node.degree = node.degree-1;
        }
        node.child = NULL;
        return true;
    }

    //Performs pairwise combine/meld of all trees in the top level doubly linked list and updates max node pointer
    void pairwiseCombine()
    {
        //Table with degree as key, node pointer as value
        unordered_map<int, Node*> combineTable;
        Node& headNode = *headNodePtr;
        Node& lastNode = *headNode.left;
        maxNodePtr = headNodePtr;

        //Start with headNode and populate table or meld
        combineTable[headNode.degree] = headNodePtr;

        if(headNode.right == headNodePtr)
        {//If top level doubly linked list has only 1 node
            return;
        }

        //Loop through elements at the top
        Node* nextNodePtr = headNode.right;
        while(nextNodePtr != &lastNode)
        {
            Node& curNode =  *nextNodePtr;
            nextNodePtr = curNode.right;
            Node maxNode = *maxNodePtr;
            if(curNode.frequency > maxNode.frequency){
                maxNodePtr = &curNode;
            }

            if(combineTable.find(curNode.degree) == combineTable.end()){
                combineTable[curNode.degree] = &curNode;
            } else {
                combine(&combineTable, &curNode);
            }
        }

        //Same as above for the last node in the top level doubly linked list.
        Node maxNode = *maxNodePtr;
        if(lastNode.frequency > maxNode.frequency){
            maxNodePtr = &lastNode;
        }
        if(combineTable.find(lastNode.degree) != combineTable.end()){
            combine(&combineTable, &lastNode);
        }
    }

    //Melds one tree to another tree with the same degree and updates childCut value
    void combine(unordered_map<int, Node*>* tablePtr, Node* curNodePtr){
        unordered_map<int, Node*>& combineTable = *tablePtr;
        Node& curNode = *curNodePtr;
        Node& mapNode = *combineTable[curNode.degree];
        combineTable.erase(curNode.degree);
        
        if(mapNode.frequency >= curNode.frequency){
            if(headNodePtr == &curNode){
                headNodePtr = curNode.right;
            }
            removeFromDoublyLinkedList(&curNode);
            curNode.parent = &mapNode;
            insertChild(&mapNode, &curNode);
            curNode.childCut = false;

            if(combineTable.find(mapNode.degree) == combineTable.end()){
                combineTable[mapNode.degree] = &mapNode;
            } else {
                return combine(&combineTable, &mapNode);
            }

        } else {
            if(headNodePtr == &mapNode){
                headNodePtr = mapNode.right;
            }
            removeFromDoublyLinkedList(&mapNode);
            mapNode.parent = &curNode;
            insertChild(&curNode, &mapNode);
            mapNode.childCut = false;

            if(combineTable.find(curNode.degree) == combineTable.end()){
                combineTable[curNode.degree] = &curNode;
            } else {
                return combine(&combineTable, &curNode);
            }
        }
    }
    
    //Prints all nodes in the max fibonacci heap
    void printAll(){
        printAll("All nodes: ");
    }

    //Prints all nodes in the max fibonacci heap
    void printAll(string str){
        Node& maxNode = *maxNodePtr;
        Node curNode = *maxNode.right;
        cout << str << endl;
        cout << maxNode.frequency << " t\t";
        int totalCount = 1;
        printChildren(maxNodePtr, &totalCount);
        while(curNode.nodeId != maxNode.nodeId){
            cout << curNode.frequency << " t\t";
            totalCount++;
            printChildren(&curNode, &totalCount);
            curNode = *curNode.right;
        }cout <<"Total Count: "<<totalCount<< " END" << endl;
    }

    //Prints children of given node
    void printChildren(Node* nodePtr, int* totalCountPtr){
        int& totalCount = *totalCountPtr;
        Node& node = *nodePtr;
        Node* nextNodePtr = node.child;
        int deg = node.degree;
        while(deg > 0 ){
            Node& curNode = *nextNodePtr;
            cout << curNode.frequency << "\t";
            totalCount++;
            if(curNode.degree > 0){
                printChildren(&curNode, &totalCount);
            }
            nextNodePtr = curNode.right;
            deg--;
        }
    }

    //Updates pointers to insert child to parent
    void insertChild(Node* parentPtr, Node* childPtr){
        Node& parent = *parentPtr;
        Node& newChild = *childPtr;
        if(parent.degree == 0){
            newChild.right = childPtr;
            newChild.left = childPtr;
            parent.child = childPtr;
        }else{
            Node& lSiblingNode = *parent.child;
            Node& rSiblingNode = *lSiblingNode.right;
            newChild.right = lSiblingNode.right;
            newChild.left = &lSiblingNode;
            lSiblingNode.right = childPtr;
            rSiblingNode.left = childPtr;
        }
        newChild.parent = parentPtr;
        parent.degree = parent.degree + 1;
        return;
    }

};