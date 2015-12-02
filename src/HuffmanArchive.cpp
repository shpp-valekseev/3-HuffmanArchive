/**
 * This program is a text file archiver. Algorithm is based on algortm Huffman.
 * For correct operation of the program need the following: after starting to choose
 * the action A - archive or D - decompress then need to enter the name of the
 * desired file. When you archive a file, you create a new file with the prefix Huff,
 * and when decoding, a new file with the addition of the prefix Decode.
 * After the complete processing of the file will be three files:
 * 1. Source File 2. Compressed file (with the prefix Huff) 3. Decompressed file (with the prefix DecodeHaff)
 *
 * For a convenient test program already has files with names:
 * Hamlet.txt | JaneEyre.txt | Middlemarch.txt | MLK-Dream.txt | TomSawyer.txt
 */

#include <fstream>
#include <iostream>
#include <string>
#include "string.h"
#include "console.h"
#include "bitstream.h"
#include "strlib.h"
#include "filelib.h"
#include "pqueueshpp.h"
#include "mapshpp.h"

using namespace std;

// The structure for storing symbols and references to other structures of this type
struct BSTNode {
    char ch;
    BSTNode *left, *right, *top;
};

// function prototypes
void archiveFile(ifstream &infile, string fileName);
MapSHPP<char, int> getAlphabet(ifstream &infile, int &length);
PQueueSHPP<BSTNode*> getQueue (MapSHPP<char, int> &alphabet);
BSTNode* getTree(PQueueSHPP<BSTNode*> queue);
void getTable(BSTNode* tree, string way, MapSHPP <char, string> &cipher);
void writeArchiveFile(string fileName, MapSHPP<char, int> &alphabet, int &length, MapSHPP <char, string> &cipher);
string toString(MapSHPP<char, int> &alphabet);
void decodeFile(string fileName);
string readFileDecode (string nameDecodeFile, int &colSimbol, MapSHPP<char, int> &decodeAlphabet);
void writeDecodeFile(BSTNode* decodeTree, string &textForDecode, string decodeFileName, int colSimbol);

/**
 * The main method of the program, that prompts
 * user for an action (A - archive or D - decompress)
 * and asks the name of the file, then calls the
 * appropriate method for the further processing of the file.
 */
int main() {
    cout << "Welcome to the Huffman archiver" << endl;
    while(true){
        string action;

        cout << "Enter A if you want to archive file, or D if you want to decompress file:" << endl;
        cin >> action;
        string fileName;
        if(toLowerCase(action) == "a"){
            ifstream infile;
            cout << "Enter the source file: " << endl;
            cin >> fileName;
            infile.open(fileName);

            // Verify the existence of the file
            if (!infile.is_open()){
                cout << "An error has occurred, most likely you have entered a non-existent file name." << endl;
                cout << "Try again." << endl;
                continue;
            }
            cout << "Processing... " << endl;

            archiveFile(infile, fileName);

            cout << "Finish!" << endl;
        } else if (toLowerCase(action) == "d"){
            cout << "Enter the source file with prefix Haff:  " << endl;
            cin >> fileName;
            ifstream infile;
            infile.open(fileName);
            if (!infile.is_open()){
                cout << "An error has occurred, most likely you have entered a non-existent file name." << endl;
                cout << "Try again." << endl;
                continue;
            }
            infile.close();
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
 * Method: archiveFile
 * Usage: archiveFile(ifstream &infile, string fileName)
 * ____________________________________________________
 *
 * This method takes the stream read from file and
 * name of source file. Calls in the correct
 * sequence, all methods for compressing file.
 *
 * @param infile - file stream
 * @param fileName - name file
 */
void archiveFile(ifstream &infile, string fileName){
    MapSHPP <char, string> cipher;
    BSTNode* tree;
    int length;
    MapSHPP<char, int> alphabet = getAlphabet(infile, length);
    infile.close();
    PQueueSHPP<BSTNode*> queue = getQueue(alphabet);
    tree = getTree(queue);
    string way = "";
    getTable(tree, way, cipher);

    writeArchiveFile(fileName, alphabet, length, cipher);
}

/**
 * Method: getAlphabet
 * Using: MapSHPP<char, int> alphabet = getAlphabet(ifstream &infile, int &length)
 * _______________________________________________________________________________
 *
 * Count the total number of characters in the file and considers how often use
 * each character in the text.
 *
 * @param infile - file stream
 * @param length - variable which must be placed in the number of characters in the file
 * @return - MapSHPP, in which the key is a symbol, and the value - number of uses it in a file
 */
MapSHPP<char, int> getAlphabet(ifstream &infile, int &length){
    MapSHPP<char, int> alphabet;
    char ch;
    length = 0;

    while (infile.get(ch)) {
        if (ch != 13){ // When reading the character "Enter" does not need to read the symbol "end of line"
            if (alphabet.containsKey(ch)){
                int tmp =  alphabet.get(ch);
                alphabet.put(ch, tmp+1);
            } else {
                alphabet.put(ch, 1);
            }
            length ++;
        }
    }
    return alphabet;
}

/**
 * Method: getQueue
 * Usage: PQueueSHPP<BSTNode*> queue = getQueue(MapSHPP<char, int> &alphabet)
 * _________________________________________________________________________
 *
 * For each symbol creates BSTNode and places this structure in a priority queue PQueueSPPP.
 *
 * @param alphabet - MapSHPP, where the key is the character and value - its frequency of use
 * @return priority queue that stores the structure BSTNode and the priority is the frequency of use of the symbol.
 */
PQueueSHPP<BSTNode*> getQueue(MapSHPP<char, int> &alphabet){
    PQueueSHPP<BSTNode*> queue;
    for(int i = 0; i < alphabet.size(); i++){
        char ch = alphabet.getKey(i);
        BSTNode *node = new BSTNode;
        node->left = node->right = 0;
        node->ch = ch;
        queue.enqueue(node, alphabet.get(ch));
    }
    return queue;
}

/**
 * Method: getTree
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
 * Method: getTable
 * Using: getTable(BSTNode *tree, string way, MapSHPP <char, string> &cipher)
 * __________________________________________________________________________
 *
 * Passing the tree gets to each character.
 * Each character assigns a unique binary value. The longer path to
 * the character from the beginning of the tree will be longer than its value.
 * Saves values in the MapSHPP<char, string>, where the symbol - the letter
 * and the string - the unique value of this symbol
 *
 * @param tree - binary tree nodes
 * @param way - way on the binary tree
 * @param cipher - it will be stored symbol and its unique cipher
 */
void getTable(BSTNode *tree, string way, MapSHPP <char, string> &cipher){
    if (tree != 0){
        getTable(tree->left, way + "0", cipher);
        if (tree->ch != 0) cipher.put(tree->ch, way);
        getTable(tree->right, way + "1", cipher);
    } else {
        return;
    }
}

/**
 * Method: writeArchiveFile
 * Usage: writeArchiveFile(string fileName, MapSHPP<char, int> &alphabet, int &length, MapSHPP <char, string> &cipher)
 * ___________________________________________________________________________________________________________________
 *
 * Recording of the compressed text file. Writes a number of characters in the source file,
 * then records each symbol that was used in the text and its frequency of use, and finally read from
 * the source file every character, and a new file is written to the binary value of the character in binary mode.
 *
 * @param fileName - file name
 * @param alphabet - MapSHPP in which stored all characters and the frequency of their use
 * @param length - number of symbols in the source file
 * @param cipher - characters and their unique ciphers
 */
void writeArchiveFile(string fileName, MapSHPP<char, int> &alphabet, int &length, MapSHPP <char, string> &cipher){
    string shifrString = integerToString(length) + toString(alphabet);
    ofbitstream stream("Haff" + fileName);

    // Writes a file number of characters and symbols with their frequency of use
    istringbitstream readString(shifrString);
    int tmpBin ;
    while(tmpBin != -1){
        tmpBin = readString.readBit();
        if (tmpBin == 0){
            stream.writeBit(0);
        } else if (tmpBin == 1){
            stream.writeBit(1);
        }
    }

    // Open source file
    ifstream infile;
    infile.open(fileName);

    // Writes a binary unique code corresponding to the character of the source file in binary mode.
    char ch;
    while (infile.get(ch)) {
        string tmp = cipher.get(ch);
        for(int i = 0; i < tmp.length(); i++){
            char a = tmp[i];
            if (a == '0'){
                stream.writeBit(0);
            } else if (a == '1'){
                stream.writeBit(1);
            }
        }
    }

    stream.close();
}

/**
 * Method: toString
 * Using: string str = toString(MapSHPP<char, int> &alphabet)
 * _________________________________________________________
 *
 * transfer values of MapSHPP in a string
 *
 * @param alphabet - MapSHPP in which stored all characters and the frequency of their use
 * @return - string
 */
string toString(MapSHPP<char, int> &alphabet){
    string res ="{";
    for(int i = 0; i < alphabet.size(); i++){
        char ch = alphabet.getKey(i);
        res += ch;
        res += ":";
        res += integerToString(alphabet.get(ch));
        res += ';';
    }
    res += "}}"; //mark end of the coding table in archive file
    return res;
}

/**
 * Method: decodeFile
 * Usage: decodeFile(string fileName)
 * ___________________________________
 *
 * Calls in the correct sequence, all methods for decompressing the file.
 *
 * @param fileName - file name
 */
void decodeFile(string fileName){
    MapSHPP<char, int> decodeAlphabet;
    int colSimbol;
    string textForDecode = readFileDecode (fileName, colSimbol, decodeAlphabet);
    PQueueSHPP<BSTNode*> decodeQueue = getQueue(decodeAlphabet);
    BSTNode* decodeTree = getTree(decodeQueue);
    writeDecodeFile(decodeTree, textForDecode, "DECODE"+fileName, colSimbol);
}

/**
 * Method: readFileDecode
 * Using: string str = readFileDecode (string nameDecodeFile, int &colSimbol, MapSHPP<char, int> &decodeAlphabet)
 * ______________________________________________________________________________________________________________
 *
 * Reading the compressed file. At first read the information on the number of characters
 * that will be the result. Then reads the information for the construction of the MapSHPP
 * which will be stored symbols and their frequency, and then reads the entire encoded text.
 *
 * @param nameDecodeFile - file name
 * @param colSimbol - number of symbols in the source file
 * @param decodeAlphabet - MapSHPP in which stored all characters and the frequency of their use
 * @return - returns the encoded text in a row
 */
string readFileDecode (string nameDecodeFile, int &colSimbol, MapSHPP<char, int> &decodeAlphabet){
    string textForDecode;
    string codeFile;
    // reads the entire file in binary mode into a string
    ifstream inCodeFile(nameDecodeFile, ifstream::binary);
    if(inCodeFile){
        inCodeFile.seekg(0, inCodeFile.end);
        int length = inCodeFile.tellg();
        inCodeFile.seekg(0, inCodeFile.beg);
        char * buffer = new char[length];
        inCodeFile.read(buffer, length);

        for (int i = 0; i < length; i++){
            codeFile += buffer[i];
        }

        string toIntRes;
        char ch;
        string colSimbolStr;

        int k = 0;
        char tmp;

        // reads the number of characters of text
        while(codeFile[k] != '{'){
            tmp = codeFile[k];
            colSimbolStr += tmp;
            k++;
        }
        colSimbol = stringToInteger(colSimbolStr);

        // reads cipher by which one can decrypt the text
        for(int i = k; i < codeFile.size(); i++){
            char tmp = codeFile[i];
            if ((codeFile[i-1] == ';' || codeFile[i-1] == '{')  && codeFile[i+1] == ':'){
                ch = tmp;
            } else if (tmp >= '0' && tmp <= '9'){
                toIntRes = toIntRes + charToString(tmp);
                if (codeFile[i+1] == ';' ){
                    decodeAlphabet.put(ch, stringToInteger(toIntRes));
                    toIntRes = "";
                }
                if (codeFile[i+2] == '}' && codeFile[i+3] == '}'){

                    // reads into a string all the encoded text
                    for(int j = i+4; j < codeFile.length(); j++){
                        textForDecode += codeFile[j];
                    }
                    return textForDecode;

                    break;
                }
            }
        }
    }
    return textForDecode;
}

/**
 * Method: writeDecodeFile
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
    istringbitstream readString(textForDecode);
    ofstream decodeFile(decodeFileName);
    int tmp = 5;
    int col = 0;
    BSTNode* top = decodeTree;
    BSTNode* tmpNode = decodeTree;
    while(tmp != -1){

        tmp = readString.readBit();
        if(tmp == 0){
            tmpNode = tmpNode->left;
        } else{
            tmpNode = tmpNode->right;
        }
        if(tmpNode->ch != 0){
            decodeFile << tmpNode->ch;
            col++;
            if(col == colSimbol)break;
            tmpNode = top;
        }
    }
    decodeFile.close();
}
