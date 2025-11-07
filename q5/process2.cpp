#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <csignal>
#include <semaphore.h>
#include <fcntl.h>

#define SEM_NAME "/my_semaphore"

void setupSignalMask() {
    sigset_t set;
    if (sigemptyset(&set) == -1) {
        std::cerr << "Failed to initialize signal set\n" << std::flush;
        exit(1);
    }
    if (sigaddset(&set, SIGINT) == -1) {
        std::cerr << "Failed to add SIGINT to signal set\n" << std::flush;
        exit(1);
    }
    if (sigprocmask(SIG_BLOCK, &set, nullptr) == -1) {
        std::cerr << "Failed to block signals\n" << std::flush;
        exit(1);
    }
}

struct SharedData {
    int multiple;
    int counter;
};

int main() {
    setupSignalMask();

    key_t key = 1234;
    int shm_id;
    SharedData* shared_data;

    // Open existing named semaphore
    sem_t* sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        std::cerr << "sem_open failed\n" << std::flush;
        exit(1);
    }

    // Attach to existing shared memory
    shm_id = shmget(key, sizeof(SharedData), 0666);
    if (shm_id < 0) {
        std::cerr << "Failed to get shared memory\n" << std::flush;
        exit(1);
    }

    shared_data = (SharedData*)shmat(shm_id, nullptr, 0);
    if (shared_data == (SharedData*)-1) {
        std::cerr << "Failed to attach shared memory\n" << std::flush;
        exit(1);
    }

    int cycle = 0;

    while (true) {
        sleep(1);

        // Enter critical section
        sem_wait(sem);
        
        int current_counter = shared_data->counter;
        int current_multiple = shared_data->multiple;
        
        std::cout << "Process 1 - Cycle: " << cycle << " (PID: " << getpid() << ")"
                    << ", Counter: " << current_counter;

        if (current_counter % current_multiple == 0) {
            std::cout << " : " << current_counter 
                        << " is a multiple of " << current_multiple;
        }
        std::cout << std::endl;
        
        // Exit critical section
        sem_post(sem);

        cycle++;

        // Exit when counter > 500
        if (current_counter > 500) {
            std::cout << "Process 2: Counter > 500. Exiting...\n" << std::flush;
            
            if (shmdt(shared_data) == -1) {
                std::cerr << "shmdt failed\n" << std::flush;
            }
            
            sem_close(sem);
            exit(0);
        }
    }

    return 0;
}
