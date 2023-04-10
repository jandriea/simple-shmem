#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>

#include "shared_memory.h"

int main(int argc, char *argv[]) {
    if (argc != 1) {
        printf("usage - %s", argv[0]);
        return -1;
    }

    // semaphore setup
    if (sem_unlink(SEM_PRODUCER_NAME) == -1) {
        perror("sem_unlink/producer");
    }

    if (sem_unlink(SEM_CONSUMER_NAME) == -1) {
        perror("sem_unlink/consumer");
    }

    sem_t *sem_prod = sem_open(SEM_PRODUCER_NAME, O_CREAT, 0660, 1);
    if (sem_prod == SEM_FAILED) {
        perror("sem_open/producer");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_cons = sem_open(SEM_CONSUMER_NAME, O_CREAT, 0660, 1);
    if (sem_cons == SEM_FAILED) {
        perror("sem_open/consumer");
        exit(EXIT_FAILURE);
    }

    // grab the shared memory block
    char *block = attach_memory_block(FILENAME, BLOCK_SIZE);
    if (block == NULL) {
        printf("Error: couldn't get block\n");
        return -1;
    }

    while (true) {
        sem_wait(sem_prod);
        if (strlen(block) > 0) {
            printf("Reading: \"%s\"\n", block);
            bool done = (strcmp(block, "quit") == 0);
            block[0] = 0;
            if (done) {break;}
        }
        sem_post(sem_prod);
    }

    if (sem_close(sem_prod) == -1) {
        perror("sem_close/producer");
        exit(EXIT_FAILURE);
    }

    if (sem_close(sem_cons) == -1) {
        perror("sem_close/consumer");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEM_PRODUCER_NAME) == -1) {
        perror("sem_unlink/producer");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SEM_CONSUMER_NAME) == -1) {
        perror("sem_unlink/consumer");
        exit(EXIT_FAILURE);
    }

    detach_memory_block(block);

    return 0;
}
