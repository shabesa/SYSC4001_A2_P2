#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <csignal>

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
        std::cout << "Process 2 (PID: " << getpid() << ") - Cycle: " << cycle 
                    << ", Counter: " << shared_data->counter;

        if (shared_data->counter % shared_data->multiple == 0) {
            std::cout << " : " << shared_data->counter 
                        << " is a multiple of " << shared_data->multiple;
        }
        std::cout << std::endl;

        cycle++;
        sleep(1);

        // Exit when counter > 500
        if (shared_data->counter > 500) {
            std::cout << "Process 2: Counter > 500. Exiting...\n" << std::flush;
            
            if (shmdt(shared_data) == -1) {
                std::cerr << "shmdt failed\n" << std::flush;
            }
            exit(0);
        }
    }

    return 0;
}
