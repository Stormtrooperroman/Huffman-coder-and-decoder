#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <unordered_map>
#include <sys/stat.h>

using namespace std;

class Node{ //A tree node
    public:
    string key;
    unsigned long long size;
    Node *R; // Right node
    Node *L; // Left node

    bool operator() (const Node& x, const Node& y){ // Comparison function to be used to order the heap
        return x.size >= y.size;
    }

    Node (const string& value = "", unsigned long long amount = 0, Node * left = NULL, Node * right = NULL) { // Node constructor 
        key = value; 
        size = amount;
        L = left; 
        R = right; 
    }


    Node * join (Node x) { // Node pooling function
        return new Node( x.key+key , x.size + size, new Node(x), this);
    }
    
};


Node * builder(priority_queue<Node, vector<Node>, Node> leafs) { // Builds Huffman tree function
    while (leafs.size() > 1) {
            Node *n = new Node(leafs.top());
           
            leafs.pop();
            cout<<"Build: "<<n->key<<" "<<leafs.top().key<<endl;
            leafs.push(*n->join(*new Node(leafs.top())));
            leafs.pop();
        }
        return new Node(leafs.top());
}


void huffmanCodes(Node* root, string  code, unordered_map<string, string > &huffmanCode) // Generate Huffman codes function
{
	if (root == nullptr)
		return;
	if (!root->L && !root->R) {
		huffmanCode[root->key] = code;
	}
    
	huffmanCodes(root->L, code+ "0", huffmanCode);
	huffmanCodes(root->R, code+ "1", huffmanCode);
}

double coder(const char* input_name="input.txt", const char* output_name="encoded.txt"){ // Coding function
    unsigned long long * alfabet = new unsigned long long [256];
    for(int i=0; i<256; i++){
        alfabet[i] = 0;
    }
    FILE* input_file = fopen(input_name, "r"); 
    if (input_file == nullptr) {
       throw invalid_argument("File not found.");
    }

    unsigned char character = 0;
    while (!feof(input_file)) { // Read from input file
       character = fgetc(input_file);
       if(!feof(input_file)){
           alfabet[character]++;
       }
    }
    
    fclose(input_file);

    priority_queue<Node, vector<Node>, Node> leafs;
    for(int i=0; i<256; i++){ // Create nodes
        if(alfabet[i] != 0){
            string s(1, (char)i);
            
            Node new_leaf(s, alfabet[i]);
            cout<<s<<" : "<<alfabet[i]<<" : "<<new_leaf.size<<endl;
            leafs.push(new_leaf);
        }
    }

    Node *tree = builder(leafs); // Create tree

    unordered_map<string, string> huffmanCode;
	huffmanCodes(tree, "", huffmanCode); // Generate Huffman codes 

	cout << "Huffman Codes are :\n" << '\n'; // Print Huffman codes
	for (auto pair: huffmanCode) {
		cout << pair.first << " " << pair.second << '\n';
	}
    


    FILE* output_file = fopen(output_name, "w +");
    input_file = fopen(input_name, "r");
    
    
    character = 0;
    unsigned char k = 0;
    unsigned int len = 0;

    unsigned int bit_len = 0;
    unsigned char letter=0;
    char col_letters = leafs.size();
    fputc(col_letters, output_file);

    for(int i=0; i<256; i++){ // Writing the letters used and their number
        if(alfabet[i] != 0){
            fputc((char)i, output_file);
            fwrite(reinterpret_cast<const char*>(&alfabet[i]), sizeof(unsigned long long), 1, output_file);
        }
    }


    while (!feof(input_file)) { // Compressing the file
       character = fgetc(input_file);
       if(!feof(input_file)){
            string s(1, character);
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
                if(huffmanCode[s].length()-8+bit_len >= 8){
                    int i= 8-bit_len;
                    while( i + 7< huffmanCode[s].length()){
                        k=0;
                        
                        for(int j = 0; j<8; j++){
                            k = k<<1 | (huffmanCode[s][i+j] - '0');                            
                        }

                        i+=8;
                        fputc(letter, output_file);
                        letter = k;
                        
                    }

                    k = 0;
                    len=0;
                    
                    for(int j = i; j<huffmanCode[s].length(); j++){
                        k = k<<1 | (huffmanCode[s][j] - '0');
                        len++;
                    }
                }

                else{
                    len=0;
                    for(int i = 8-bit_len; i<huffmanCode[s].length(); i++){
                        k = k<<1 | (huffmanCode[s][i] - '0');
                        len++;
                    }
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
       }

       else if(bit_len<8){
           letter= letter<<8-bit_len;
           fputc(letter, output_file);
       }
    }
    
    fclose(input_file);
    fclose(output_file);

    unsigned long long file_full_size = 0;
    unsigned long long commpres_size = 0;
    struct stat sb{};
    struct stat se{};
    // Finding compression ratio
    if (!stat(input_name, &sb)) {
        file_full_size = sb.st_size;
    } else {
        perror("stat");
    }
    if (!stat(output_name, &se)) {
        commpres_size = se.st_size;
    } else {
        perror("stat");
    }
    return (file_full_size + 0.0) / commpres_size;
}

int main(){
    cout<<coder()<<endl; // Print compression ratio
}
