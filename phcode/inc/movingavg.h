/* 
 * Header file for MovingAvg class
 * Keeps a moving average of the N last elements of a data stream in O(1) time
 */

#ifndef MOVINGAVG_H
#define MOVINGAVG_H

#include <iostream>

template <class T>
class MovingAvg {
	private:
		int numElementsMax;
		int numElements;
		T *vals;
		T  avg;
		T  sum;
		int head;

	public:
		MovingAvg(int n) {
			numElementsMax = n;
			vals = new T[n];
			numElements = 0;
			avg = 0;
			sum = 0;
			head = 0;
		}

		void add(float num) {
			
			// Subtract out oldest element
			if (numElements == numElementsMax) {
				sum -= vals[head];
				numElements--;
			}

			// Store the new element in the head position
			vals[head] = num;

			// Update head index
			head = (head+1)%numElementsMax;

			sum += num;
			numElements++;

			if (numElements != 0)
				avg = sum / numElements;
			else
				avg = 0;
		}

		void printElements() {
			std::cout << "Element of MovingAvg(" << numElements << "/" << numElementsMax << "): ";

			for (int i = 0; i < numElements; i++) {
				std::cout << vals[i] << " ";
			}
			std::cout << std::endl;

		}

		T getAvg() {
			return avg;
		}


};


#endif
