**Sleepy TA C project hosted on Linux**

This project simulates the **Sleeping Teaching Assistant** problem using **POSIX threads, mutex locks, and semaphores**. The TA helps students with programming assignments, but if no students are present, they take a nap. Students arrive randomly, seek help, and either wake up the TA, wait in the hallway, or leave if no chairs are available. The simulation ensures proper synchronization between students and the TA.

### Features:
- Multi-threaded implementation using **pthreads**.
- Synchronization using **mutex locks** and **semaphores**.
- Realistic behavior with randomized student arrival and help times.

Run using:  
```bash
gcc -pthread -o A2 A2.c && ./A2
