# esercizio-C-2020-05-20-barrier

implementare: 3.6 Barrier (pag. 21-22)

http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf


Generalize the rendezvous solution. Every thread should run the
following code:
- rendezvous
- critical point

The synchronization requirement is that no thread executes critical point until after all threads have executed rendezvous.

You can assume that there are n threads and that this value is stored in a variable, n, that is accessible from all threads.

When the first n − 1 threads arrive they should block until the nth thread arrives, at which point all the threads may proceed.


SOLUZIONE (pag. 29):
```
rendezvous
 
mutex.wait()
   count = count + 1
mutex.signal()
 
if count == n : 
   barrier.signal()
 
barrier.wait()
barrier.signal()
 
critical point
```

vedere anche:

https://github.com/marcotessarotto/exOpSys/tree/master/008.04threads-barrier

