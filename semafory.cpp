#include "monitor.h"
#include <queue> // kolejka FIFO
#include <iostream>
#include <unistd.h> //sleep()

using namespace std;

queue<char> buffer;

Semaphore mutex(1); /* zarządza dostępem do regionu krytycznego */
Semaphore ile_pustych(9); /* zlicza puste miejsca w buforze */
Semaphore ile_do_usuniecia(0); /* liczba elementow w buforze, które można usunąć, czyli w sytuacji, gdy w buforze jest >3 elementów */
Semaphore a(0); //semafor binarny - 1 - można usunąć literę 'A'
Semaphore b(0);

void wypisz_bufor(queue<char> q){
    queue<char> copy = q;

    while(!copy.empty()){
        cout << copy.front() << " ";
        copy.pop();
    }
    cout << endl;
}

void* producerA (void* unused){
    while(1) {
        ile_pustych.p(); // dekrementacja licznika pustych

        mutex.p(); // wejście do regionu krytycznego
            if (buffer.empty())
                a.v();
            buffer.push('A');
            if (buffer.size()>3) {
                ile_do_usuniecia.v();
            }
        mutex.v();
        //sleep(1);    
   }
    return NULL;
}

void* producerB (void* unused){
    while(1) {
        ile_pustych.p();
        
        mutex.p();
            if (buffer.empty())
                b.v();
            buffer.push('B');
            if (buffer.size()>3)
                ile_do_usuniecia.v();
        mutex.v();
        //sleep(0.1);
   }
    return NULL;
}

void* consumerA (void* unused){
    while(1){
        a.p(); //jeśli było 'A' na początku, to przejdziemy dalej, żeby je skonsumować
        ile_do_usuniecia.p();
        
        mutex.p();
            buffer.pop();
            ile_pustych.v();
            if(!buffer.empty()) {
                if (buffer.front()=='A')
                    a.v();
                else
                    b.v();
            }
            wypisz_bufor(buffer);
        mutex.v();
    }
    return NULL;
}

void* consumerB (void* unused){
    while(1){
        b.p();
        ile_do_usuniecia.p();
        mutex.p();
            buffer.pop();
            ile_pustych.v();
            if(buffer.front() == 'A')
                a.v();
            else
                b.v();
        mutex.v();
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
        mutex.p();
        wypisz_bufor(buffer);
        if (buffer.size() < 3 || buffer.size()>9)
           cout << buffer.size() << endl;
        mutex.v();
    }

    return 0;
}
