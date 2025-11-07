#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

// Shared memory structure
struct SharedData {
    int multiple;  // First variable: multiple value (e.g., 3)
    int counter;   // Second variable: shared counter
};

void run_process(const char* process_name, int sleep_duration, int multiple) {
    key_t key = 1234;
    int shm_id;
    SharedData* shared_data;

    // Create shared memory segment
    shm_id = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);
    if (shm_id < 0) {
        std::cerr << "Failed to create shared memory\n" << std::flush;
        exit(1);
    }
    std::cout << "Shared memory ID: " << shm_id << std::endl;

    // Attach to shared memory
    shared_data = (SharedData*)shmat(shm_id, nullptr, 0);
    if (shared_data == (SharedData*)-1) {
        std::cerr << "Failed to attach shared memory\n" << std::flush;
        exit(1);
    }

    // Initialize shared variables
    shared_data->multiple = multiple;
    shared_data->counter = 0;

    int cycle = 0;
    bool process2_forked = false;

    while (true) {
        std::cout << "Process 1 - Cycle: " << cycle << " (PID: " << getpid() << ")"
                    << ", Counter: " << shared_data->counter;

        if (shared_data->counter % shared_data->multiple == 0) {
            std::cout << " : " << shared_data->counter 
                        << " is a multiple of " << shared_data->multiple;
        }
        std::cout << std::endl;

        shared_data->counter++;
        cycle++;
        sleep(sleep_duration);

        // Fork Process 2 when counter > 100
        if (shared_data->counter > 100 && !process2_forked) {
            process2_forked = true;
            pid_t pid2 = fork();
            if (pid2 < 0) {
                std::cerr << "Failed to fork Process 2\n" << std::flush;
                exit(1);
            } else if (pid2 == 0) {
                setupSignalMask();
                execl("./process2", "process2", nullptr);
                std::cerr << "Failed to exec process2\n" << std::flush;
                exit(1);
            }
        }

        // Exit when counter > 500
        if (shared_data->counter > 500) {
            std::cout << "Process 1: Counter > 500. Exiting...\n" << std::flush;
            
            // Detach and cleanup shared memory
            if (shmdt(shared_data) == -1) {
                std::cerr << "shmdt failed\n" << std::flush;
            }
            if (shmctl(shm_id, IPC_RMID, nullptr) == -1) {
                std::cerr << "shmctl(IPC_RMID) failed\n" << std::flush;
            }
            exit(0);
        }
    }
}

int main() {
    pid_t pid1;

    pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "First fork failed\n" << std::flush;
        exit(1);
    } else if (pid1 == 0) {
        setupSignalMask();
        run_process("Process 1", 1, 3);
    }

    // Parent waits for child
    wait(nullptr);
    std::cout << "All processes finished.\n";
    
    return 0;
}
