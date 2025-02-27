/*
 * A2.c - The Sleeping Teaching Assistant Problem
 * 
 * Compilation: gcc -pthread -o A2 A2.c
 * Execution: ./A2
 * 
 * This program simulates the "Sleeping Teaching Assistant" problem using
 * POSIX threads, mutex locks, and semaphores.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <unistd.h>
 
 #define NUM_STUDENTS 5   // Number of student threads
 #define NUM_CHAIRS 3      // Number of waiting chairs
 
 pthread_mutex_t mutex;    // Mutex for synchronizing access
 sem_t ta_sleep;           // Semaphore for TA sleeping
 sem_t students;           // Semaphore for students waiting
 int waiting_students = 0; // Number of students waiting
 
 void *student_thread(void *param);
 void *ta_thread(void *param);
 
 int main() {
     pthread_t students_threads[NUM_STUDENTS];
     pthread_t ta;
     
     // Initialize mutex and semaphores
     pthread_mutex_init(&mutex, NULL);
     sem_init(&ta_sleep, 0, 0);
     sem_init(&students, 0, 0);
     
     // Create TA thread
     pthread_create(&ta, NULL, ta_thread, NULL);
     
     // Create student threads
     for (int i = 0; i < NUM_STUDENTS; i++) {
         pthread_create(&students_threads[i], NULL, student_thread, (void *)(intptr_t)i);
     }
     
     // Join student threads
     for (int i = 0; i < NUM_STUDENTS; i++) {
         pthread_join(students_threads[i], NULL);
     }
     
     // Terminate TA thread (in real-world, we'd have a condition to end TA)
     pthread_cancel(ta);
     
     // Destroy mutex and semaphores
     pthread_mutex_destroy(&mutex);
     sem_destroy(&ta_sleep);
     sem_destroy(&students);
     
     return 0;
 }
 
 void *student_thread(void *param) {
     int id = (intptr_t)param;
     while (1) {
         // Simulate programming time
         printf("Student %d is programming...\n", id);
         sleep(rand() % 5 + 1);
         
         // Try to get help
         pthread_mutex_lock(&mutex);
         if (waiting_students < NUM_CHAIRS) {
             waiting_students++;
             printf("Student %d is waiting in the hallway. Waiting students: %d\n", id, waiting_students);
             pthread_mutex_unlock(&mutex);
             
             sem_post(&students); // Notify TA
             sem_wait(&ta_sleep); // Wait for TA to help
             
             printf("Student %d is getting help.\n", id);
             sleep(rand() % 3 + 1); // Simulate help time
             
             pthread_mutex_lock(&mutex);
             waiting_students--;
             pthread_mutex_unlock(&mutex);
         } else {
             printf("Student %d found no available chairs, will try later.\n", id);
             pthread_mutex_unlock(&mutex);
         }
         
         sleep(rand() % 5 + 1); // Wait before trying again
     }
     return NULL;
 }
 
 void *ta_thread(void *param) {
     while (1) {
         sem_wait(&students); // Wait for students
         
         while (1) {
             pthread_mutex_lock(&mutex);
             if (waiting_students == 0) {
                 pthread_mutex_unlock(&mutex);
                 break; // No more students, TA goes back to sleep
             }
             pthread_mutex_unlock(&mutex);
             
             printf("TA is helping a student...\n");
             sleep(rand() % 3 + 1); // Simulate help time
             sem_post(&ta_sleep); // Signal student that help is done
         }
     }
     return NULL;
 }
 