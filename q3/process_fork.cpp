#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

void run_process(const char* process_name, int sleep_duration) {
    int counter = 0;
    int cycle = 0;
    
    while (true) {
        std::cout << "Process 1 - Cycle number: " << cycle << " (PID: " << getpid() << ")";
        
        if (counter % 3 == 0) {
            std::cout << " : " << counter << " is a multiple of 3";
        }
        std::cout << '\n' << std::flush;

        counter++;
        cycle++;
        sleep(sleep_duration);
    }
}

int main() {
    pid_t pid1, pid2;
    int status;

    // First fork: Create Child Process 1
    pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "First fork failed\n" << std::flush;
        exit(1);
    } else if (pid1 == 0) {
        // Child Process 1 (Process 1)
        setupSignalMask();
        run_process("Process 1", 1);
    }

    // Parent process continues here
    if (pid1 > 0) {
        // Parent forks Process 2
        pid2 = fork();
        if (pid2 < 0) {
            std::cerr << "Second fork failed\n" << std::flush;
            exit(1);
        } else if (pid2 == 0) {
            // Child Process 2 (Process 2)
            setupSignalMask();
            execlp("./process2", "process2", nullptr);
            std::cerr << "Failed to exec process2\n" << std::flush;
            exit(1);
        }
        
        // Parent waits for Process 2 to finish
        wait(nullptr);
        std::cout << "Process 2 finished. Exiting...\n" << std::flush;
        kill(pid1, SIGTERM); // Terminate Process 1
        exit(0);
    }

    return 0;
}
