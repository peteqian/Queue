#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//Prototype Functions
void siftUp(int i);
void siftDown (int i);

struct event {
	float time;

	// contents of event struct
	// server
	float busyDuration;

	//event Types
	int eventType;
	int passengerClass;
	float serviceDuration;
	float queueDuration;

};

event* heap;
int heapSize;

int main(){
	
	// Variables of main (4)
	const int QUEUE_MAX = 500;
	event bQueue[QUEUE_MAX];
	int bHead = 0;
	int bTail = 0;
	int bqSize = 0;
	int bqMax = 0;
	float bqTotal;
	event tQueue[QUEUE_MAX];
	int tHead = 0;
	int tTail = 0;
	int tqSize = 0; 
	int tqMax = 0;
	float tqTotal;
	int qMax = 0;
	event *bStack;
	event *tStack;

	char filename[20];
	ifstream fin;

	float now;
	float inArrival;
	int inClass;
	float inService;

	int bCount, tCount, bIdle, tIdle;
	
	float totalService[3];
	float pureService[3];
	int nServed[3];
	
	for(int pass = 1; pass <= 2; pass++){

		// Open and validate the input file (8)
		if(pass == 1){
			cerr << "Please enter the name of the input file: "; 
			cin >> filename;
		}

		fin.open(filename);

		if(!fin){
			cerr << "Error opening file " << filename << ". Program will exit." << endl;
			return 0;
		}
		
		// Initialize the simulation (9)
		fin >> bCount >> tCount;
		
		if (pass == 1){
			heap = new event[bCount + tCount + 1];
			bStack = new event[bCount];
			tStack = new event[tCount];
		}
		
		bIdle = bCount;
		tIdle = tCount;

		for(int i = 0; i < bCount; i++){
			bStack[i].busyDuration = 0.0;
		}
		for(int i = 0; i < tCount; i++){
			tStack[i].busyDuration = 0.0;
		}
		
		// Simulation starts of at time 0.0
		now = 0.0;
		heapSize = 1;
		heap[0].eventType = 0;
		fin >> inArrival >> inClass >> inService;
		heap[0].time = inArrival;
		heap[0].passengerClass = inClass;
		heap[0].serviceDuration = inService;
		heap[0].queueDuration = 0.0;

		// Arrays set to zero 28
		for (int i = 0; i < 3; i++){
			totalService[i] = 0.0;
			pureService[i] = 0.0;
			nServed[i] = 0;
		}
		
		// Do the main loop
		while(heapSize != 0){
			
			bqTotal += (heap[0].time - now) * bqSize;
			tqTotal += (heap[0].time - now) * tqSize;
			now = heap[0].time;
			

			if(heap[0].eventType == 0){
				// Process an arrival (18)

				// Serve or queue the current passenger (19)
				if (heap[0].passengerClass == 0){

					if(tIdle != 0){
						// Serving a tourist class server (2)
						heap[heapSize].time = now + heap[0].serviceDuration;
						heap[heapSize].eventType = 1;
						heap[heapSize].passengerClass = heap[0].passengerClass;
						heap[heapSize].serviceDuration = heap[0].serviceDuration;
						heap[heapSize].queueDuration = 0;
						tIdle--;
						heap[heapSize].busyDuration = tStack[tIdle].busyDuration;
						siftUp(heapSize);
						heapSize++;
					} else if (pass == 2 && bIdle != 0){
						// Serving a business class server (21)
						heap[heapSize].time = now + heap[0].serviceDuration;
						heap[heapSize].eventType = 2;
						heap[heapSize].passengerClass = heap[0].passengerClass;
						heap[heapSize].serviceDuration = heap[0].serviceDuration;
						heap[heapSize].queueDuration = 0;
						bIdle--;
						heap[heapSize].busyDuration = bStack[bIdle].busyDuration;
						siftUp(heapSize);
						heapSize++;
					} else {
						// Add a tourist class passenger to the queue (22)
						if(tTail == QUEUE_MAX){
							tTail = 0;
						}
						tQueue[tTail].time = now ;
						tQueue[tTail].serviceDuration = heap[0].serviceDuration;
						tTail++;
						tqSize++;
						tqMax = max(tqMax, tqSize );
						qMax = max(qMax, tqSize + bqSize);
					}

				} else {
					
					if(bIdle != 0){
						// Serving a business class server (21)
						heap[heapSize].time = now + heap[0].serviceDuration;
						heap[heapSize].eventType = 2;
						heap[heapSize].passengerClass = heap[0].passengerClass;
						heap[heapSize].serviceDuration = heap[0].serviceDuration;
						heap[heapSize].queueDuration = 0;
						bIdle--;
						heap[heapSize].busyDuration = bStack[bIdle].busyDuration;
						siftUp(heapSize);
						heapSize++;
					
					} else {
						// Add a business class passenger to the queue (23)
						if (bTail == QUEUE_MAX) {
							bTail = 0;
						}

						bQueue[bTail].time = now ;
						bQueue[bTail].serviceDuration = heap[0].serviceDuration ;
						bTail++;
						bqSize++;
						bqMax = max (bqMax, bqSize );
						qMax = max (qMax, tqSize + bqSize );
						
					}
				}
			
				// Get the next passenger (24)
				fin >> inArrival >> inClass >> inService;
				if (inArrival > 0.0 || inService > 0.0) {
					heap[0].time = inArrival ;
					heap[0].passengerClass = inClass ;
					heap[0].serviceDuration = inService ;
					heap[0].queueDuration = 0.0;
				} else {
					heapSize--;
					heap[0] = heap[heapSize];
				}
				siftDown(0);


			} else {
				// Process a service completion (25)

				//Update the passenger statistics (26)
				if (heap[0].passengerClass == 0) {
					totalService[0] += heap[0].serviceDuration + heap[0].queueDuration;
					pureService[0] += heap[0].serviceDuration;
					nServed[0]++;
				} else {
					totalService[1] += heap[0].serviceDuration + heap[0].queueDuration;
					pureService[1] += heap[0].serviceDuration;
					nServed[1]++;
				}
				totalService[2] += heap[0].serviceDuration + heap[0].queueDuration;
				pureService[2] += heap[0].serviceDuration;
				nServed[2]++;
				heap[0].busyDuration += heap[0].serviceDuration;

				// Fix the server (29)
				if (heap[0].eventType == 1) {

					// Fix the tourist class server (30)
					if(tHead != tTail){
						// Keep the server busy with a tourist class passenger (32)
						heap[0].time = now + tQueue[tHead].serviceDuration ;
						heap[0].passengerClass = 0;
						heap[0].queueDuration = now - tQueue[tHead].time ;
						heap[0].serviceDuration = tQueue[tHead].serviceDuration ;
						tHead++;
						if (tHead == QUEUE_MAX){
							tHead = 0;
						}
						tqSize--;
						
					} else  {
						// Make the server idle (34)
						if(heap[0].eventType == 1){
							tStack[tIdle].busyDuration = heap[0].busyDuration;
							tIdle++;				
						} else {
							bStack[bIdle].busyDuration = heap[0].busyDuration;
							bIdle++;
						}
						heapSize--;
						heap[0] = heap[heapSize];
						siftDown(0);
					}
				} else {

					// Fix the business class server (31)
					if (bHead != bTail ) {
						// Keep the server busy with a business class passenger (33)
						heap[0].time = now+bQueue[bHead].serviceDuration ;
						heap[0].passengerClass = 1;
						heap[0].queueDuration = now - bQueue[bHead].time ;
						heap[0].serviceDuration = bQueue[bHead].serviceDuration ;
						bHead++;
						if (bHead == QUEUE_MAX){
							bHead = 0;
						}
						bqSize--;
					} else if ( pass == 2 && tHead != tTail){
						// Keep the server busy with a tourist class passenger (32)
						heap[0].time = now + tQueue[tHead].serviceDuration ;
						heap[0].passengerClass = 0;
						heap[0].queueDuration = now - tQueue[tHead].time ;
						heap[0].serviceDuration = tQueue[tHead].serviceDuration ;
						tHead++;
						if (tHead == QUEUE_MAX){
							tHead = 0;
						}
						tqSize--;
					} else {
						// Make the server idle (34)
						if(heap[0].eventType == 1){
							tStack[tIdle].busyDuration = heap[0].busyDuration ;
							tIdle++;				
						} else {
							bStack[bIdle].busyDuration = heap[0].busyDuration;
							bIdle++;
						}
						heapSize--;
						heap[0] = heap[heapSize];
						siftDown(0);
					}
						
				}
				
				siftDown (0);
				
			}
		}

		// Write the results
		cout << fixed << setprecision(2);
		cout << "Pass " << pass << ": ";
		if(pass == 1){
			cout << "Business servers exclusively serve business class" << endl;
		} else {
			cout << "Idle business servers may serve tourist class" << endl;
		}
		cout << setw(50) << "   Time last service is completed: " << setw(10) << now << endl;

		// Passenger Stats
		cout << "  Business class passengers:" << endl;
		cout << setw (50) << "    Number of people served:" << setw (10) << nServed[1] << endl ;
		cout << setw (50) << "    Average total service time:" << setw (10) << totalService[1]/nServed[1] << endl ;
		cout << setw (50) << "    Average total time in service:" << setw (10) << pureService[1]/nServed[1] << endl ;
		cout << setw (50) << "    Average length of queue:" << setw (10) << bqTotal/now << endl ;
		cout << setw (50) << "    Maximum number queued:" << setw (10) << bqMax << endl ;
		cout << "  Tourist class passengers:" << endl ;
		cout << setw (50) << "    Number of people served:" << setw (10) << nServed[0] << endl ;
		cout << setw (50) << "    Average total service time:" << setw (10) << totalService[0]/nServed[0] << endl ;
		cout << setw (50) << "    Average total time in service:" << setw (10) << pureService[0]/nServed[0] << endl ;
		cout << setw (50) << "    Average length of queue:" << setw (10) << tqTotal/now << endl ;
		cout << setw (50) << "    Maximum number queued:" << setw (10) << tqMax << endl ;
		cout << "  All passengers:" << endl ;
		cout << setw (50) << "    Number of people served:" << setw (10) << nServed[2] << endl ;
		cout << setw (50) << "    Average total service time:" << setw (10) << totalService[2]/nServed[2] << endl ;
		cout << setw (50) << "    Average total time in service:" << setw (10) << pureService[2]/nServed[2] << endl ;
		cout << setw (50) << "    Average length of queue:" << setw (10) << (bqTotal +tqTotal)/now << endl ;
		cout << setw (50) << "    Maximum number queued:" << setw (10) << qMax << endl ;

		// Server Stats
		cout << "  Business class servers:" << endl;
		for (int i = 0; i < bCount ; i++){
			cout << "    Total idle time for business class server " << setw (2) << i + 1 << ": " << setw (10) << now - bStack[i].busyDuration << endl;
		}
		cout << "  Tourist class servers:" << endl ;
		for (int i = 0; i < tCount ; i++){
			cout << "    Total idle time for tourist class server " << setw (2) << i + 1 << ":  " << setw (10) << now - tStack[i].busyDuration << endl;
		}
		cout << endl;

		fin.close();

	}
	
	// Finish up
	delete[] heap;
	delete[] bStack;
	delete[] tStack;

	return 0;
}

void siftUp(int i){
	static event temp;

	if (i == 0){
		return;
	}

	int p = (i-1)/2;

	if (heap[p].time <= heap[i].time ){
		return;
	}

	temp = heap[p];
	heap[p] = heap[i];
	heap[i] = temp;
	siftUp(p);
	return;
}

void siftDown(int i){
	static event temp;
	int c = 2 * i + 1;
	if (c >= heapSize ){
		return;
	}

	if (c + 1 < heapSize && (heap[c + 1].time < heap[c].time )){
		c++; 
	}

	if (heap[i].time <= heap[c].time ){
		return;
	}
	temp = heap[c];
	heap[c] = heap[i];
	heap[i] = temp;
	siftDown (c);
	return;
}