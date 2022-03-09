#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;


class Node{
    public:
    string key;
    unsigned long long size;
    Node *R;
    Node *L;

    bool operator() (const Node& x, const Node& y){
        return x.size >= y.size;
    }

    Node (const string& value = "", unsigned long long amount = 0, Node * left = NULL, Node * right = NULL) {
        key = value; 
        size = amount;
        L = left; 
        R = right; 
    }


    Node * join (Node x) {
        return new Node( x.key+key , x.size + size, new Node(x), this);
    }


};
Node * builder(priority_queue<Node, vector<Node>, Node> tree) {
    while (tree.size() > 1) {
            Node *n = new Node(tree.top());
           
            tree.pop();
            cout<<"Build: "<<n->key<<" "<<tree.top().key<<endl;
            tree.push(*n->join(*new Node(tree.top())));
            tree.pop();
        }
        return new Node(tree.top());
}

void decoder(const char* input_name="encoded.txt", const char* output_name="output.txt"){
    unsigned long long * alfabet = new unsigned long long [256];
    for(int i=0; i<256; i++){
        alfabet[i] = 0;
    }
    FILE* input_file = fopen(input_name, "r");
    if (input_file == nullptr) {
       throw invalid_argument("File not found.");
    }
    unsigned char col = 0;
    unsigned int col_letters = 0;
    col = fgetc(input_file);
    if(!feof(input_file)){
       col_letters =(int) col;
    }

    //cout<<col_letters<<endl;
    unsigned char character = 0;
    for(int i = 0; i< col_letters; i++){
        character = fgetc(input_file);
        if(!feof(input_file)){
            unsigned long long col_of;
            //fwrite(reinterpret_cast<const char*>(&alfabet[i]), sizeof(unsigned long long), 1, output_file);
            fread(reinterpret_cast<char*>(&alfabet[character]), sizeof(unsigned long long), 1, input_file);
            cout<<alfabet[character]<<endl;
        }
        else{
            throw invalid_argument("Can't decompress file.");
        }
    }

    priority_queue<Node, vector<Node>, Node> tree;
    for(int i=0; i<256; i++){
        if(alfabet[i] != 0){
            string s(1, (char)i);
            
            Node new_leaf(s, alfabet[i]);
            cout<<s<<" : "<<alfabet[i]<<" : "<<new_leaf.size<<endl;
            tree.push(new_leaf);
        }
    }
    character = 0;
    Node *n = builder(tree);

    FILE* output_file = fopen(output_name, "w +");
    Node *nodes = n;
    unsigned char letter = 0;
    while (!feof(input_file)) {
        character = fgetc(input_file);
        if(!feof(input_file)){
            for (int i = 7; i > -1; i--){
                if(((character >> i) & 1) == 1){
                    if(nodes->R == NULL){
                        
                        letter = nodes->key[0];
                        fputc(letter, output_file);
                        nodes = n->R;
                    }
                    else{
                        nodes = nodes->R;
                    }
                    
                }
                else if(((character >> i) & 1) == 0){
                    if(nodes->L == NULL){
                        
                        letter = nodes->key[0];
                        fputc(letter, output_file);
                        nodes = n->L;
                    }
                    else{
                        nodes = nodes->L;
                    }
                }
           }
       }
    }

    
    fclose(input_file);
    fclose(output_file);
}

int main(){
    decoder();
}