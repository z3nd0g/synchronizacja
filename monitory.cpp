#include "monitor.h"
#include <queue> // kolejka FIFO (std::queue)
#include <iostream>
#include <unistd.h> //sleep()

#define MAX_SIZE 9
#define MIN_SIZE 4 //minimalny rozmiar bufora, by możliwy był odczyt przez konsumenta

using namespace std;


class ProducerConsumer : Monitor {
    
private:

    queue<char> buffer;   
    // Gdy bufor jest pełny, producent blokuje się za pomocą wait(full)
    // Gdy przestaje być pełny, jest odblokowany przez wątek konsumenta za pomocą signal(full)
    Condition full;
    // ConsumerA blokuje się w oczekiwaniu na literę 'A' rozmiar bufora min.MIN_SIZE za pomocą wait(can_read_A)
    Condition can_read_A;
    // ConsumerB - analogicznie
    Condition can_read_B;         

public:
  
    void wypisz_bufor() {
        enter();
            queue<char> copy = buffer;

            while(!copy.empty()){
                cout << copy.front() << " ";
                copy.pop();
            }
            cout << endl;
            
            if (buffer.size() < MIN_SIZE-1 || buffer.size()>MAX_SIZE)
                cout << buffer.size() << endl;
            
        leave();
    }
    
    void produceA() {
        enter();            
            if (buffer.size() == MAX_SIZE)
                wait(full);
            
            buffer.push('A');          
            
            if (buffer.size() >= MIN_SIZE)
                if (buffer.front() = 'A')
                    signal(can_read_A);
                else
                    signal(can_read_B);
        leave();
    }
    
    void produceB() {
        enter();
            if (buffer.size() == MAX_SIZE)
                wait(full);
            
            buffer.push('B');
            
            if (buffer.size() >= MIN_SIZE)
                if (buffer.front() = 'A')
                    signal(can_read_A);
                else
                    signal(can_read_B);
           
        leave();    
    }
    
    void consumeA() {
        enter();
            if (buffer.size() < MIN_SIZE || buffer.front()!='A')
                wait(can_read_A);
                
            buffer.pop();
            
	        if(!buffer.size()>=MIN_SIZE && buffer.front()=='B')
	            signal(can_read_B);
     
            if (buffer.size() < MAX_SIZE)
                signal(full); 
        leave();
    }
    
    void consumeB() {
        enter();
            if (buffer.size() < MIN_SIZE || buffer.front()!='B')
                wait(can_read_B);
            
            buffer.pop();          

            if(!buffer.size()>=MIN_SIZE && buffer.front()=='A')
	            signal(can_read_A);
            
            if (buffer.size() < MAX_SIZE)
                signal(full); 
        leave();
    }
};


ProducerConsumer pc;
 
void* producerA (void* unused){
    while(1) {
        pc.produceA();
        //sleep(1);    
   }
    return NULL;
}

void* producerB (void* unused){
    while(1) {
        pc.produceB();
        //sleep(1);
   }
    return NULL;
}

void* consumerA (void* unused){
    while(1){
        pc.consumeA();
        //sleep(1); 
    }
    return NULL;
}

void* consumerB (void* unused){
    while(1){
        pc.consumeB();
        //sleep(1);     
    }
    return NULL;
}
    

int main() {
      
    pthread_t producer_A_id;
    pthread_t producer_B_id;
    pthread_t consumer_A_id;
    pthread_t consumer_B_id;

    pthread_create (&producer_A_id, NULL, &producerA, NULL);
    pthread_create (&producer_B_id, NULL, &producerB, NULL);
    pthread_create (&consumer_A_id, NULL, &consumerA, NULL);
    pthread_create (&consumer_B_id, NULL, &consumerB, NULL);

    while (1) {
        pc.wypisz_bufor();
        //sleep(1);
    }

    return 0;
}
