#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SHM_NAME "/my_shm"
#define SEM_NAME "/my_semaphore"
#define SHM_SIZE 256

void write_data(sem_t *sem, char *shm_ptr, const char *data) {
    int i = 0;
    while (i < 100) {
        // Wait for semaphore to become available
        sem_wait(sem);

        // Write new data to shared memory
        sprintf(shm_ptr, "%s %d", data, i++);
        printf("Wrote data to shared memory: %s\n", shm_ptr);

        // Release semaphore
        sem_post(sem);

        // Sleep for one second
        sleep(1);
    }
}

void read_data(sem_t *sem, char *shm_ptr) {
    // Wait for the semaphore
    sem_wait(sem);

    // Read data from shared memory
    printf("Received data: %s\n", shm_ptr);

    // Release the semaphore
    sem_post(sem);
    // Sleep for one second
    sleep(1);
}

int main(int argc, char* argv[]) {
    // Determine whether to act as data sender or receiver based on command line argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [sender | recv | unlink]\n", argv[0]);
        return 1;
    }   

    // Open the shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Map the shared memory block into this process's memory
    char *ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    if (errno) {
        perror("mmap");
        close(fd);
        return -1;
    }

    // Open the semaphore
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    char data_to_send[15] = "Hello, world!\n";
    char received_data[SHM_SIZE];
    char unlink = 0;

    if (strcmp(argv[1], "sender") == 0) {
        // Send data using the sender function
        write_data(sem, ptr, data_to_send);
    } else if (strcmp(argv[1], "recv") == 0) {
        // Receive data using the receiver function
        read_data(sem, ptr);
    } else {
        unlink = 1;
    }   

    printf("Enter any key to exit");
    getchar();

    // Unmap the shared memory object
    if (munmap(ptr, SHM_SIZE) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    // Close and unlink the semaphore
    if (sem_close(sem) == -1) {
        perror("sem_close failed");
        exit(EXIT_FAILURE);
    }

    if (unlink == 1) {
        // Close and unlink the shared memory object
        if (shm_unlink(SHM_NAME) == -1) {
            perror("shm_unlink failed");
            exit(EXIT_FAILURE);
        }
        if (sem_unlink(SEM_NAME) == -1) {
            perror("sem_unlink failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}