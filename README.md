# synchronizacja

Bufor 9-elementowy FIFO. Dwóch konsumentów i dwóch producentów. Producent A produkuje literę A, producent B - literę B. Elementy A odczytuje (usuwa) konsument A. Elementy B odczytuje (usuwa) konsument B. Odczyt (usunięcie) jest dozwolone, gdy liczba elementów w buforze jest większa niż 3.

Realizacja synchronizacji na semaforach: 

g++ semafory.cpp -l pthread

na monitorach: 

g++ monitory.cpp -l pthread
