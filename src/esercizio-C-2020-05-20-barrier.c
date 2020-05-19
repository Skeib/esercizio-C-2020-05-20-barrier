#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>


// barriera di n threads
// mutex e barrier sono implementati tramite due semafori senza nome

// vedere anche:
// https://github.com/marcotessarotto/exOpSys/tree/master/008.04threads-barrier

#define N 10

sem_t mutex;
sem_t barrier;

int count; // variabile condivisa tra i due thread
int number_of_threads = N;

/*
rendezvous

mutex.wait()
   count = count + 1
mutex.signal()

if count == n :
   barrier.signal()

barrier.wait()
barrier.signal()

critical point
 */

void * thread_function(void * arg) {

	printf("rendezvous\n");

	//	mutex.wait()
	//	   count = count + 1
	//	mutex.signal()
	if (sem_wait(&mutex) == -1) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}

	count++;

	if (sem_post(&mutex) == -1) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}
	//

	//	if count == n :
	//	   barrier.signal()
	if (count == number_of_threads) {
		if (sem_post(&barrier) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	// turnstile (tornello)
	//	barrier.wait()
	//	barrier.signal()
	if (sem_wait(&barrier) == -1) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}
	if (sem_post(&barrier) == -1) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}
	//

	printf("critical point\n");

	return NULL;
}

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

int main() {

	int s;
	pthread_t threads[N];

	s = sem_init(&mutex,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo
				  );

	CHECK_ERR(s,"sem_init")

	s = sem_init(&barrier,
						0, // 1 => il semaforo è condiviso tra processi,
						   // 0 => il semaforo è condiviso tra threads del processo
						0 // valore iniziale del semaforo
					  );

	CHECK_ERR(s,"sem_init")

	for (int i=0; i < number_of_threads; i++) {
		s = pthread_create(&threads[i], NULL, thread_function, NULL);

		if (s != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	for (int i=0; i < number_of_threads; i++) {
		s = pthread_join(threads[i], NULL);

		if (s != 0) {
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}

	}

	s = sem_destroy(&mutex);
	CHECK_ERR(s,"sem_destroy")

	s = sem_destroy(&barrier);
	CHECK_ERR(s,"sem_destroy")

	printf("bye\n");

	return 0;
}
