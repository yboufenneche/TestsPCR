all:	 TestMessage TestRedirection TestLectureEcriture

message.o: message.c message.h
	gcc -Wall -c message.c

alea.o: alea.h alea.c
	gcc -Wall -c alea.c

TestMessage: message.o alea.o TestMessage.c
	gcc -Wall TestMessage.c message.o alea.o -o  TestMessage

TestRedirection: TestRedirection.c
	gcc -Wall TestRedirection.c -o  TestRedirection

lectureEcriture.o: lectureEcriture.c lectureEcriture.h
	gcc -c -Wall lectureEcriture.c

TestLectureEcriture: lectureEcriture.o TestLectureEcriture.c
	gcc lectureEcriture.o TestLectureEcriture.c -o TestLectureEcriture

Terminal: Terminal.o alea.o message.o lectureEcriture.o
	gcc -Wall Terminal.o alea.o message.o lectureEcriture.o -o Terminal

resultats.o: resultats.c resultats.h
	gcc -c -Wall resultats.c

Validation: Validation.o message.o lectureEcriture.o resultats.o
	gcc -Wall Validation.o message.o lectureEcriture.o resultats.o -o Validation

memoire.o: memoire.c memoire.h
	gcc -c -Wall memoire.c

Aquisition: Aquisition.o message.o lectureEcriture.o memoire.o
	gcc -Wall Aquisition.o message.o lectureEcriture.o memoire.o -o Aquisition
	
clean:	
	rm -f *.o *~ 

cleanall: clean
	rm TestRedirection TestMessage TestLectureEcriture
