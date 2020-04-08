using namespace std;

//Class containing the Node and its functions
class Node {
    public:
    Node* parent = NULL;
    Node* child = NULL;
    Node* left = NULL;
    Node* right = NULL;
    int degree = 0;
    bool childCut = NULL;
    int frequency = 0;
    int nodeId;//Used for identifying nodes while printing the heap

    Node(int nodeId){
        this -> nodeId = nodeId;
    }

    Node(){
        
    }

    Node(int nodeId, int frequency){
        this -> nodeId = nodeId;
        this-> frequency = frequency;
    }

    ~Node(){
        //cout << "Node deleted: " << nodeId << endl;
    }

    void setFrequency(int freq){
        this -> frequency = freq;
    }

    int getFrequency(){
        return frequency;
    }

    void setNodeId(int nodeId){
        this -> nodeId = nodeId;
    }

    int getNodeId(){
        return nodeId;
    }

    void setDegree(int degree){
        this -> degree = degree;
    }

    int getDegree(){
        return degree;
    }
};