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

int count;

pthread_barrier_t thread_barrier;

sem_t mutex;

int number_of_threads = N;

void * rendezvous(void * arg) {
	int s;

	printf("rendezvous\n");

	// https://linux.die.net/man/3/pthread_barrier_wait
	s = pthread_barrier_wait(&thread_barrier);

	/*
	 The pthread_barrier_wait() function shall synchronize participating threads at
	 the barrier referenced by barrier. The calling thread shall block until
	 the required number of threads have called pthread_barrier_wait() specifying the barrier.
	*/

	printf("critical point\n");

	if (sem_wait(&mutex) == -1) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}

	count++;

	if (sem_post(&mutex) == -1) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}

	return NULL;
}

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }
#define CHECK_ERR2(a,msg) {if ((a) != 0) { perror((msg)); exit(EXIT_FAILURE); } }

int main() {

	int s;
	pthread_t threads[N];

	s = sem_init(&mutex,
					0, // 1 => il semaforo è condiviso tra processi,
					   // 0 => il semaforo è condiviso tra threads del processo
					1 // valore iniziale del semaforo
				  );

	CHECK_ERR(s,"sem_init")


	// https://linux.die.net/man/3/pthread_barrier_init
	s = pthread_barrier_init(&thread_barrier, NULL, N);
	CHECK_ERR(s,"pthread_barrier_init")

	for (int i=0; i < number_of_threads; i++) {
		s = pthread_create(&threads[i], NULL, rendezvous, NULL);
		CHECK_ERR2(s,"pthread_create")
	}

	for (int i=0; i < number_of_threads; i++) {
		s = pthread_join(threads[i], NULL);
		CHECK_ERR2(s,"pthread_join")
	}

	// https://linux.die.net/man/3/pthread_barrier_init
	s = pthread_barrier_destroy(&thread_barrier);
	CHECK_ERR(s,"pthread_barrier_destroy")

	s = sem_destroy(&mutex);
	CHECK_ERR(s,"sem_destroy")

	printf("count expected %d, count obtained %d\n", N, count);

	printf("bye\n");

	return 0;
}
