/*
 * A2.c - The Sleeping Teaching Assistant Problem
 * 
 * Compilation: gcc -pthread -o A2 A2.c
 * Execution: ./A2
 * Sava Jankovic
 * Karn Bansal
 
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_STUDENTS 5   // Custom number of student threads for this case, can be changed. 
#define NUM_CHAIRS 3      // As per assignment instructions, 3 waiting chairs. 

pthread_mutex_t mutex;    // Mutex for synchronizing access
sem_t ta_sleep;           // Ta sleeping semaphore. 
sem_t students;           // students waiting semaphore
int waiting_students = 0; // number of students that are waiting to start with, 0. 
int total_students = 0;   // total students to start with, 0. this gets incremented

void *student_thread(void *param);
void *ta_thread(void *param);

int main() {
    pthread_t students_threads[NUM_STUDENTS];
    pthread_t ta;
    
    // mutex and semaphore initialization
    pthread_mutex_init(&mutex, NULL);
    sem_init(&ta_sleep, 0, 0);
    sem_init(&students, 0, 0);
    
    //TA thread creation
    pthread_create(&ta, NULL, ta_thread, NULL);
    
    // student thread
    for (int i = 1; i <= NUM_STUDENTS; i++) {
        pthread_create(&students_threads[i - 1], NULL, student_thread, (void *)(intptr_t)i);
    }
    
 
    for (int i = 1; i <= NUM_STUDENTS; i++) {
        pthread_join(students_threads[i - 1], NULL);
    }
    
    // terminate TA thread (in real-world, we'd have a condition to end TA)
    pthread_cancel(ta);
    
    // destroy mutex and semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&ta_sleep);
    sem_destroy(&students);
    
    return 0;
}

void *student_thread(void *param) {
    int id = (intptr_t)param;
    while (1) {
        // programming time simulation
        printf("Student number %d is programming...\n", id);
        sleep(rand() % 5 + 1);
        
        
        pthread_mutex_lock(&mutex);
        if (waiting_students < NUM_CHAIRS) {
            waiting_students++;
            total_students++;
            printf("Student %d is waiting in the hallway. There are currently %d\n waiting students", id, waiting_students);
            pthread_mutex_unlock(&mutex);
            
            sem_post(&students); // notify TA
            sem_wait(&ta_sleep); // wait for TA to help
            
            printf("Student %d is currently getting help from the TA. \n", id);
            sleep(rand() % 3 + 1); // simulate help time
            
            pthread_mutex_lock(&mutex);
            waiting_students--;
            total_students--;
            pthread_mutex_unlock(&mutex);
        } else {
            printf("Student number %d found no chairs, will try again in a bit. \n", id); // if chairs are not available (else) print this statement
            pthread_mutex_unlock(&mutex);
        }
        
        sleep(rand() % 5 + 1); // Wait before trying again
    }
    return NULL;
}

void *ta_thread(void *param) {
    while (1) {
        pthread_mutex_lock(&mutex);
        if (total_students == 0) {
            printf("TA is currently sleeping.\n");
        }
        pthread_mutex_unlock(&mutex);
        
        sem_wait(&students); // wait for students
        
        while (1) {
            pthread_mutex_lock(&mutex);
            if (waiting_students == 0) {
                pthread_mutex_unlock(&mutex);
                break; // no more students, TA goes back to sleep
            }
            pthread_mutex_unlock(&mutex);
            
            printf("TA helping a student right now. \n");
            sleep(rand() % 3 + 1); // simulate help time
            sem_post(&ta_sleep); // signal student that help is done
        }
    }
    return NULL;
}
