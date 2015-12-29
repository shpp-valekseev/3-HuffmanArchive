/* File: pqueueshpp.h
 * -----------------------------------------------------
 * This file exports a simple version of the PriorityQueue class
 * based on the binary heap.
 */

#ifndef PQUEUESHPP_H
#define PQUEUESHPP_H

#include <iostream>
#include <stdlib.h>
#include "vectorshpp.h"

/* Class: PQueueSHPP<ValueType> pqueue;
 * ---------------------------------------------------
 * This class implements priority queue of a specified ValueType
 * elements
 */
template <typename ValueType>
class PQueueSHPP {
    /* Public methods prototypes*/
public:
    /* Constructor: PQueueSHPP
   * Usage: PQueueSHPP<ValueType> pqueue;
   * -----------------------------------------------
   * Initializes a new empty pqueue
   */
    PQueueSHPP();

    /* Destructor: ~PQueueSHPP
   * ----------------------------------------------
   * Frees all allocated memory for the priority queue elements
   */
    virtual ~PQueueSHPP();

    /* Method: enqueue
   * Usage: pqueue.enqueue(value, priority);
   * -----------------------------------------------
   * Adds new value in the priority queue with the appropriate priority
   */
    void enqueue(ValueType value, double priority);

    /* Method: dequeue
   * Usage: value = pqueue.dequeue();
   * -----------------------------------------------
   * Removes and returns the value with the highest priority
   */
    ValueType dequeue();

    /* Method: peek
   * Usage: value = pqueue.peek();
   * ---------------------------------------------
   * Returns the value of the highest priority, without removing it
   */
    ValueType peek();

    /* Method: peekPriority
   * Usage: double priority = pqueue.peekPriority();
   * ---------------------------------------------
   * Returns the priority of the first element in the queue, without removing
   * it.
   */
    double peekPriority();

    /* Method: clear
   * Usage: pqueue.clear();
   * ---------------------------------------------
   * Removes all elements of the pqueue
   */
    void clear();

    /* Method: isEmpty
   * Usage: if(pqueue.isEmpty)...
   * --------------------------------------------
   * Returns true if vector is empty
   */
    bool isEmpty() const;

    /* Method: size
   * Usage: int size = pqueue.size();
   * --------------------------------------------
   * Return current number of the elements of the pqueue
   */
    int size() const;

    /* Operator: =
    * pqueueNew = pqueueOld;
    * -----------------------------------------------------
    * Overloads assign operator
    */
    PQueueSHPP<ValueType> & operator=(const PQueueSHPP<ValueType>& src);

    /* Copy constructor */
    PQueueSHPP(const PQueueSHPP<ValueType>& src);

    /* Private methods prototypes and instase variables*/
private:

    /* Structure for saving elements of the pqueue*/
    struct heapNode {
        ValueType value;
        double priority;
        heapNode *left;
        heapNode *right;
        heapNode *top;
    };

    /* Vector in wish stored node */
    VectorSHPP<heapNode*> array;

    /* Variable to store the number of elements in the structure*/
    int heapSize;

  /* Method: swap
   * Usage: swap(heapNode *firstNode, heapNode *secondNode);
   * --------------------------------------------
   * swaps the value and priority of the two nodes
   */
    void swap(heapNode *firstNode, heapNode *secondNode);

    /* Method: shiftUp
     * Usage: shiftUp(heapNode *node);
     * --------------------------------------------
     * After adding a new element regulates the values
     * in the array. "If priority current element is less than the
     * parent element, then change their positions"
     */
    void shiftUp(heapNode *node);

    /* Method: shiftDown
     * Usage: shiftDown(heapNode *node);
     * --------------------------------------------
     * After removing element regulates values in the array.
     * "If the priority of the current element is less than the
     * priority of the lesser of his sons, it changes their places"
     */
    void shiftDown(heapNode *node);

    /* Method: deepCoping;
     * Usage: deepCoping(PQueueSHPP src);
     * ------------------------------------------------
     * Coping received  PQueueSHPP to "this" PQueueSHPP
     */
    void deepCopy(const PQueueSHPP<ValueType> & src);

};

/* Implementation of all methods of PQueueSHPP class*/
template <typename ValueType>
PQueueSHPP<ValueType>::PQueueSHPP() {
    heapSize = 0;
}

template <typename ValueType>
PQueueSHPP<ValueType>::~PQueueSHPP() {

}

template <typename ValueType>
void PQueueSHPP<ValueType>::enqueue(ValueType value, double priority) {
    heapNode* tmp = new heapNode;
    tmp->value = value;
    tmp->priority = priority;
    tmp->left = tmp->right = tmp->top = 0;
    array.add(tmp);
    heapSize++;
    if(heapSize > 1){
        tmp->top = array[(heapSize - 2) / 2];
        if (heapSize % 2 == 0){
            tmp->top->left = tmp;
        } else {
            tmp->top->right = tmp;
        }
        shiftUp(tmp);
    }
}

template <typename ValueType>
ValueType PQueueSHPP<ValueType>::dequeue() {
    if(!array.isEmpty()){
        ValueType resTmp;
        if(heapSize > 1){

            heapNode* startNode = array[0];
            heapNode* endNode = array[heapSize - 1];
            resTmp = startNode->value;
            startNode->priority = endNode->priority;
            startNode->value = endNode->value;
            if (heapSize % 2 == 0){
                endNode->top->left = 0;
            } else {
                endNode->top->right = 0;
            }
            array.remove(heapSize - 1);
            delete endNode;
            shiftDown(startNode);
        } else if (heapSize == 1){
            resTmp = array[0]->value;

            delete array[0];
            array.remove(0);
        }
        heapSize--;
        return resTmp;
    }
    return 0;
}

template <typename ValueType>
ValueType PQueueSHPP<ValueType>::peek() {
    return array[0]->value;
}

template <typename ValueType>
double PQueueSHPP<ValueType>::peekPriority() {
    return array[0]->priority;
}

template <typename ValueType>
void PQueueSHPP<ValueType>::clear() {
    delete array;
    heapSize = 0;
}

template <typename ValueType>
bool PQueueSHPP<ValueType>::isEmpty() const {
    return heapSize == 0;
}

template <typename ValueType>
int PQueueSHPP<ValueType>::size() const {
    return heapSize;
}

template<typename ValueType>
void PQueueSHPP<ValueType>::deepCopy(const PQueueSHPP<ValueType> & src){
    for (int i = 0; i < src.heapSize; i++){
        heapNode* newNode = new heapNode;
        newNode->priority = src.array[i]->priority;
        newNode->value = src.array[i]->value;
        this->heapSize = src.heapSize;
        this->array.add(newNode);
    }

    for(int i = 0; i < heapSize; i++){

        if (2 * i + 1 < array.size()){
            array[i]->left = array[2 * i + 1];
        } else {
            array[i]->left = 0;
        }

        if (2 * i + 2 < heapSize){
            array[i]->right = array[2 * i +2];
        } else {
            array[i]->right = 0;
        }

        if( i > 0){
            array[i]->top = array[(i - 1) / 2];
        } else {
            array[i]->top = 0;
        }
    }
}

template<typename ValueType>
PQueueSHPP<ValueType>::PQueueSHPP(const PQueueSHPP<ValueType>& src){
    deepCopy(src);
}

template<typename ValueType>
PQueueSHPP<ValueType> & PQueueSHPP<ValueType>::operator =(const PQueueSHPP<ValueType>& src){
    deepCopy(src);
    return *this;
}

template <typename ValueType>
void PQueueSHPP<ValueType>::shiftUp(heapNode *node){
    if (node->top != 0){
        if(node->priority <= node->top->priority){
            swap(node, node->top);
            shiftUp(node->top);
        }
    }
}

template <typename ValueType>
void PQueueSHPP<ValueType>::shiftDown(heapNode *node){
    if(node->left != 0 && node->right != 0){
        if (node->left->priority < node->right->priority && node->left->priority < node->priority){
            swap(node, node->left);
            shiftDown(node->left);
        } else if (node->right->priority < node->priority){
            swap(node, node->right);
            shiftDown(node->right);
        }
    } else if(node->left != 0 && node->left->priority < node->priority){
        swap(node, node->left);
        shiftDown(node->left);
    } else if (node->right != 0 && node->right->priority < node->priority){
        swap(node, node->right);
        shiftDown(node->right);
    }
}

template <typename ValueType>
void PQueueSHPP<ValueType>::swap(heapNode *firstNode, heapNode *secondNode){
    double tmpPriority = firstNode->priority;
    ValueType tmpValue = firstNode->value;
    firstNode->priority = secondNode->priority;
    firstNode->value = secondNode->value;
    secondNode->value = tmpValue;
    secondNode->priority = tmpPriority;
}


#endif
