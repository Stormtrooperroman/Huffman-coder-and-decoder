#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <unordered_map>
#include <cmath>

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


void huffmanCodes(Node* root, string  code, unordered_map<string, string > &huffmanCode)
{
	if (root == nullptr)
		return;
    cout<<root->key<<" : "<<code<<endl;
	if (!root->L && !root->R) {
		huffmanCode[root->key] = code;
	}
    
	huffmanCodes(root->L, code+ "0", huffmanCode);
	huffmanCodes(root->R, code+ "1", huffmanCode);
}

int coder(const char* input_name="input.txt", const char* output_name="encoded.txt"){
    unsigned long long* alfabet = new unsigned long long[256];

    for(int i=0; i<256; i++){
        alfabet[i] = 0;
    }
    FILE* input_file = fopen(input_name, "r");
    if (input_file == nullptr) {
       throw invalid_argument("File not found.");
    }

    unsigned char character = 0;
    while (!feof(input_file)) {
       character = fgetc(input_file);
       if(!feof(input_file)){
           alfabet[character]++;
       }
       
    }
    
    fclose(input_file);

    priority_queue<Node, vector<Node>, Node> tree;
    for(int i=0; i<256; i++){
        if(alfabet[i] != 0){
            string s(1, (char)i);
            
            Node new_leaf(s, alfabet[i]);
            cout<<s<<" : "<<alfabet[i]<<" : "<<new_leaf.size<<endl;
            tree.push(new_leaf);
        }
    }
    // Node test = tree.top();
    // cout<<test.key<<endl;

    Node *n = builder(tree);

    unordered_map<string, string> huffmanCode;
	huffmanCodes(n, "", huffmanCode);

	cout << "Huffman Codes are :\n" << '\n';
	for (auto pair: huffmanCode) {
		cout << pair.first << " " << pair.second << '\n';
	}
    // cout<<huffmanCode["a"]<<endl;


    FILE* output_file = fopen(output_name, "w +");
    input_file = fopen(input_name, "r");
    

    character = 0;
    unsigned char k = 0;
    unsigned int len = 0;

    unsigned int bit_len = 0;
    unsigned int x = 0;
    // for (auto pair: huffmanCode) {
	// 	if(x< ceil(log2(pair.second))){
    //         x = ceil(log2(pair.second));
    //     }
	// }
     
    // cout<<x<<endl;
    unsigned char letter=0;

    
    while (!feof(input_file)) {
       character = fgetc(input_file);
       if(!feof(input_file)){
            string s(1, character);
            // if(8-bit_len-x>=0){
            //     letter = letter | huffmanCode[s]<<(8-bit_len-x);
            //     bit_len+=x;
            // }
            // else
            // {
            //     letter = letter | huffmanCode[s] >> (8-bit_len);
            //     k = huffmanCode[s] &  (~(0<< (8 - (x - 8-bit_len))))>>(8 - (x - 8-bit_len));
            //     len = x - 8-bit_len;
            //     bit_len += 8-bit_len;
            // }
            
            if(bit_len+huffmanCode[s].length()<=8){
                
                for(int i = 0; i<huffmanCode[s].length(); i++){
                    letter = letter<<1 | (huffmanCode[s][i] - '0');
                }
                bit_len+=huffmanCode[s].length();
            }

            else{
                
                for(int i = 0; i<8-bit_len; i++){
                    letter = letter<<1 | (huffmanCode[s][i] - '0');
                } 

                for(int i = 8-bit_len; i<huffmanCode[s].length(); i++){
                    k = k<<1 | (huffmanCode[s][i] - '0');
                    len++;
                }

                bit_len = 8; 
            }

            if(bit_len == 8){
                fputc(letter, output_file);
                letter = k;
                bit_len = len;
                k=0;
                len=0;
            }
            else if(bit_len > 8){
                throw invalid_argument("Error man");
            }
            
       }
       else if(bit_len<8){
           letter= letter<<8-bit_len;
           fputc(letter, output_file);
       }
       
    }
    
    fclose(input_file);
    fclose(output_file);
    return 1;
}

int main(){
    
    coder();
}