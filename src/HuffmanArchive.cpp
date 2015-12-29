/**
 * This program is a text file archiver. Algorithm is based on algortm Huffman.
 * For correct operation of the program need the following: after starting to choose
 * the action A - archive or D - decompress then need to enter the name of the
 * desired file. When you archive a file, you create a new file with the prefix Huff,
 * and when decoding, a new file with the addition of the prefix Decode.
 * After the complete processing of the file will be three files:
 * 1. Source File 2. Compressed file (with the prefix Huff) 3. Decompressed file (with the prefix DecodeHuff)
 *
 * For a convenient test program already has files with names:
 * 22.png | Martin.mp3 |Hamlet.txt | JaneEyre.txt | Middlemarch.txt | MLK-Dream.txt | TomSawyer.txt
 */

#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "pqueueshpp.h"


using namespace std;

// The structure for storing symbols and references to other structures of this type
struct BSTNode {
    unsigned char ch;
    bool busy = false;
    BSTNode *left, *right, *top;
};

const int NUMBER_OF_BYTES = 256;
const int DATABITS = 8;

// function prototypes
void archiveFile(string fileName);
vector<int> getAlphabet(string fileName, int &length);
PQueueSHPP<BSTNode*> getQueue (vector<int> &alphabet);
BSTNode* getTree(PQueueSHPP<BSTNode*> queue);
void getTable(BSTNode* tree, string way, vector <string> &cipher);
void writeArchiveFile(string fileName, vector<int> &alphabet, int &length, vector <string> &cipher);
string toString(vector<int> &alphabet);
void decodeFile(string fileName);
string readFileDecode (string nameDecodeFile, int &colSimbol, vector<int> &decodeAlphabet);
void writeDecodeFile(BSTNode* decodeTree, string &textForDecode, string decodeFileName, int colSimbol);
char makeByteFromString(string byte);
string bitsOfByteInString(char ch);

/**
 * The main function of the program, that prompts
 * user for an action (A - archive or D - decompress)
 * and asks the name of the file, then calls the
 * appropriate function for the further processing of the file.
 */
int main() {
    cout << "Welcome to the Huffman archiver" << endl;
    while(true){
        string action;

        cout << "Enter A if you want to archive file, or D if you want to decompress file:" << endl;
        cin >> action;
        string fileName;
        if(toLowerCase(action) == "a"){
            cout << "Enter the source file: " << endl;
            cin >> fileName;
            cout << "Processing... " << endl;
            archiveFile(fileName);
            cout << "Finish!" << endl;
        } else if (toLowerCase(action) == "d"){
            cout << "Enter the source file with prefix Huff:  " << endl;
            cin >> fileName;
            cout << "Processing... " << endl;
            decodeFile(fileName);
            cout << "Finish!" << endl;
        } else {
            cout << "You made a mistake. Try again." << endl;
        }
    }
    return 0;
}

/**
 * Function: archiveFile
 * Usage: archiveFile(string fileName)
 * ____________________________________________________
 *
 * This function takes name of source file.
 * Calls in the correct sequence, all functions
 * for compressing file.
 *
 * @param fileName - file name
 */
void archiveFile(string fileName){
    vector <string> cipher(NUMBER_OF_BYTES, "");
    BSTNode* tree;
    int length;
    vector<int> alphabet = getAlphabet(fileName, length);
    PQueueSHPP<BSTNode*> queue = getQueue(alphabet);
    tree = getTree(queue);
    string way = "";
    getTable(tree, way, cipher);

    writeArchiveFile(fileName, alphabet, length, cipher);
}

/**
 * Function: getAlphabet
 * Using: vector<int> alphabet = getAlphabet(string fileName, int &length)
 * _______________________________________________________________________________
 *
 * Count the total number of characters in the file and considers how often use
 * each character in the text.
 *
 * @param fileName - file name
 * @param length - variable which must be placed in the number of characters in the file
 * @return - vector, in which the index is a symbol of the ascii table, and the value - number of uses it in a file
 */
vector<int> getAlphabet(string fileName, int &length){
    vector<int> alphabet(NUMBER_OF_BYTES, 0);

    ifstream stream(fileName, ios::binary);
    if(stream){
        stream.seekg(0, stream.end);
        length = stream.tellg();
        stream.seekg(0, stream.beg);
        cout << length << endl;
        char * buffer = new char[length];
        stream.read(buffer, length);
        for(int i = 0; i < length; i++){
            unsigned char ch = buffer[i];
            alphabet[ch]++;
        }
    } else {
        // Verify the existence of the file
        cout << "An error has occurred, most likely you have entered a non-existent file name." << endl;
        exit(1);

    }
    return alphabet;
}

/**
 * Function: getQueue
 * Usage: PQueueSHPP<BSTNode*> queue = getQueue(vector<int> &alphabet)
 * _________________________________________________________________________
 *
 * For each symbol creates BSTNode and places this structure in a priority queue PQueueSHPPP.
 *
 * @param alphabet - vector, in which the index is a symbol of the ascii table and value - its frequency of use
 * @return priority queue that stores the structure BSTNode and the priority is the frequency of use of the symbol.
 */
PQueueSHPP<BSTNode*> getQueue(vector<int> &alphabet){
    PQueueSHPP<BSTNode*> queue;
    for(int i = 0; i < alphabet.size(); i++){
		if(alphabet[i] != 0){
        unsigned char ch = i;
        BSTNode *node = new BSTNode;
        node->left = node->right = 0;
        node->busy = true;
        node->ch = ch;
        queue.enqueue(node, alphabet[i]);
		}
    }
    return queue;
}

/**
 * Function: getTree
 * Using: BSTNode* node = getTree(PQueueSHPP<BSTNode*> queue)
 * _________________________________________________________
 *
 * The principle of constructing the next.
 * Take out the two nodes with the lowest priority, it calculates
 * the sum of the priorities and creates a new node in place of
 * previous. This is repeated until the queue will not remain a
 * single node with the sum of all priorities.
 *
 * @param queue - priority queue that stores the structure BSTNode
 * @return node which are references to all the nodes
 */
BSTNode* getTree(PQueueSHPP<BSTNode*> queue){
    while(queue.size() != 1){
        int newPriority = queue.peekPriority();
        BSTNode* newNode = new BSTNode;
        newNode->ch = 0;
        newNode->left = queue.dequeue();
        newPriority += queue.peekPriority();

        newNode->right = queue.dequeue();
        newNode->right->top = newNode->left->top = newNode;
        queue.enqueue(newNode, newPriority);
    }
    return queue.dequeue();
}

/**
 * Function: getTable
 * Using: getTable(BSTNode *tree, string way, vector<string> &cipher)
 * __________________________________________________________________________
 *
 * Passing the tree gets to each character.
 * Each character assigns a unique binary value. The longer path to
 * the character from the beginning of the tree will be longer than its value.
 * Saves values in the vector<string>, where index - symbol of the table ascii
 * and the string - the unique value of this symbol
 *
 * @param tree - binary tree nodes
 * @param way - way on the binary tree
 * @param cipher - it will be stored symbol and its unique cipher
 */
void getTable(BSTNode *tree, string way, vector<string> &cipher){
    if (tree != 0){
        getTable(tree->left, way + "0", cipher);
        if (tree->busy){
            cipher[tree->ch] = way;
        }
        getTable(tree->right, way + "1", cipher);
    } else {
        return;
    }
}

/**
 * Function: writeArchiveFile
 * Usage: writeArchiveFile(string fileName, vector<int> &alphabet, int &length, vector <string> &cipher)
 * ___________________________________________________________________________________________________________________
 *
 * Recording of the compressed text file. Writes a number of characters in the source file,
 * then records each symbol that was used in the text and its frequency of use, and finally read from
 * the source file every character, and a new file is written to the binary value of the character in binary mode.
 *
 * @param fileName - file name
 * @param alphabet - vector in which stored all characters and the frequency of their use
 * @param length - number of symbols in the source file
 * @param cipher - characters and their unique ciphers
 */
void writeArchiveFile(string fileName, vector<int> &alphabet, int &length, vector<string> &cipher){
    string shifrString = toString(alphabet);
    ofstream outputStream("Huff" + fileName, ios::binary);
    outputStream << length << shifrString;

    // Open source file
    ifstream sourceStream(fileName.c_str(), ios::binary);
    if(sourceStream){
        sourceStream.seekg(0, sourceStream.end);
        length = sourceStream.tellg();
        sourceStream.seekg(0, sourceStream.beg);
        char * buffer = new char[length];
        sourceStream.read(buffer, length);

        string encodedText;
        for(int k = 0; k < length; k++){
            unsigned char fromBuf;
            fromBuf = buffer[k];
            encodedText += cipher[fromBuf];
        }

        // Transformation string of zeros and ones in binary view
        string result = "";
        string str = "";

        int count = encodedText.size() / DATABITS;
        if(encodedText.size() % DATABITS != 0) {
            count++;
        }
        for(int i = 0; i < count; i++){
            str = encodedText.substr(i * DATABITS, DATABITS);
            char ch = makeByteFromString(str);
            result += ch;
        }
        outputStream << result;
    }
    outputStream.close();
}

/**
 * Function: makeByteFromString
 * Using: char byte = makeByteFromString(string str)
 * __________________________________________________
 *
 * Function takes a string of 8 characters "0" and "1"
 * and converts them into a single byte
 *
 * @param str - string of 8 "0" and "1"
 * @return one byte
 */
char makeByteFromString(string str){
    int byte = 0;
    int actualValue = NUMBER_OF_BYTES / 2;
    for (int i = 0; i < DATABITS; i++){
        if (str[i] == '1'){
            byte += actualValue;
        }
        actualValue = actualValue / 2;
    }
    return (unsigned char) byte;
}

/**
 * Function: toString
 * Using: string str = toString(vector<int> &alphabet)
 * _________________________________________________________
 *
 * transfer values of vector in a string
 *
 * @param alphabet - vector in which stored all characters and the frequency of their use
 * @return - string
 */
string toString(vector<int> &alphabet){
    string res ="{";
    for(int i = 0; i < alphabet.size(); i++){
        if(alphabet[i] != 0){
            unsigned char ch = i;
            res += ch;
            res += ":";
            res += integerToString(alphabet[i]);
            res += ';';
        }
    }
    res += "}}"; //mark end of the coding table in archive file
    return res;
}

/**
 * Function: decodeFile
 * Usage: decodeFile(string fileName)
 * ___________________________________
 *
 * Calls in the correct sequence, all functions for decompressing the file.
 *
 * @param fileName - file name
 */
void decodeFile(string fileName){
    vector<int> decodeAlphabet(NUMBER_OF_BYTES, 0);
    int colSimbol;
    string textForDecode = readFileDecode (fileName, colSimbol, decodeAlphabet);
    PQueueSHPP<BSTNode*> decodeQueue = getQueue(decodeAlphabet);
    BSTNode* decodeTree = getTree(decodeQueue);
    writeDecodeFile(decodeTree, textForDecode, "DECODE"+fileName, colSimbol);
}

/**
 * Function: readFileDecode
 * Using: string str = readFileDecode (string nameDecodeFile, int &colSimbol, vector<int> &decodeAlphabet)
 * ______________________________________________________________________________________________________________
 *
 * Reading the compressed file. At first read the information of characters number
 * that will be at result. Then reads the information for the construction of the vector
 * which will be stored symbols and their frequency, and then reads the entire encoded text.
 *
 * @param nameDecodeFile - file name
 * @param colSimbol - number of symbols in the source file
 * @param decodeAlphabet - vector in which stored all characters and the frequency of their use
 * @return - returns the encoded text in a row
 */
string readFileDecode(string nameDecodeFile, int &colSimbol, vector<int> &decodeAlphabet){
    string textForDecode;
    // reads the entire file in binary mode into a string
    ifstream inCodeFile(nameDecodeFile, ifstream::binary);
    if(inCodeFile){
        inCodeFile.seekg(0, inCodeFile.end);
        int length = inCodeFile.tellg();
        inCodeFile.seekg(0, inCodeFile.beg);
        char * buffer = new char[length];
        inCodeFile.read(buffer, length);

        string toIntRes;
        unsigned char ch;
        string colSimbolStr;

        int k = 0;
        char tmp;

        // reads the number of characters of text
        while(buffer[k] != '{'){
            tmp = buffer[k];
            colSimbolStr += tmp;
            k++;
        }
        colSimbol = stringToInteger(colSimbolStr);

        // reads cipher by which one can decrypt the text
        for(int i = k; i < length; i++){
            char tmp = buffer[i];
            if ((buffer[i-1] == ';' || buffer[i-1] == '{')  && buffer[i+1] == ':'){
                ch = tmp;
            } else if (tmp >= '0' && tmp <= '9'){
                toIntRes = toIntRes + charToString(tmp);
                if (buffer[i+1] == ';' ){
                    decodeAlphabet[ch] = stringToInteger(toIntRes);
                    toIntRes = "";
                }
                if (buffer[i+2] == '}' && buffer[i+3] == '}'){

                    // reads into a string all the encoded text
                    for(int j = i+4; j < length; j++){
                        textForDecode += buffer[j];
                    }
                    return textForDecode;

                    break;
                }
            }
        }
    } else {
        cout << "An error has occurred, most likely you have entered a non-existent file name." << endl;
        exit(1);
    }
    return textForDecode;
}

/**
 * Function: writeDecodeFile
 * Using: writeDecodeFile(BSTNode* decodeTree, string &textForDecode, string decodeFileName, int colSimbol)
 * _______________________________________________________________________________________________________
 *
 * Writes in file encoded text. Reads text encoded in binary mode, passes through the binary
 * tree, and when it finds a symbol, writes it to the output file.
 *
 * @param decodeTree - binary tree nodes
 * @param textForDecode - text to decode
 * @param decodeFileName - file name
 * @param colSimbol - number of characters in the source file
 */
void writeDecodeFile(BSTNode* decodeTree, string &textForDecode, string decodeFileName, int colSimbol){
    string strBool;
    for(int i = 0; i < colSimbol; i++){
        strBool += bitsOfByteInString(textForDecode[i]);
    }
    ofstream decodeFile(decodeFileName, ios::out | ios::binary);
    int tmp = 5;
    int col = 0;
    string tmpRes;
    BSTNode* top = decodeTree;
    BSTNode* tmpNode = decodeTree;
    for(int j = 0; j < strBool.size(); j++){

        tmp = (strBool[j]);
        if(tmp == '0'){
            tmpNode = tmpNode->left;
        } else {
            tmpNode = tmpNode->right;
        }
        if(tmpNode->busy){
            tmpRes += tmpNode->ch;
            col++;
            tmpNode = top;
            if(col == colSimbol){
                decodeFile << tmpRes;
                break;
            }
        }
    }
    decodeFile.close();
}

/**
 * Function: bitsOfByteInString
 * Using: bitsOfByteInString(char ch)
 * ___________________________________
 *
 * Function adds a bit of char to a string,
 * and then returns it
 *
 * @param ch - character from which to remove bits
 * @return a string of eight characters
 */
string bitsOfByteInString(char ch){
    string result;
    for(int i = DATABITS - 1; i >= 0; i--){
        int bit = (ch >> i) & 1;
        result += integerToString(bit);
    }
    return result;
}
