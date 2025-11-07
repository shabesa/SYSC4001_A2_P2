#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
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
    pid_t pid1;

    // First fork: Create Child Process 1
    pid1 = fork();
    if (pid1 < 0) {
        std::cerr << "Failed to fork Process 1\n" << std::flush;
        exit(1);
    } else if (pid1 == 0) {
        // Child Process 1 (Process 1)
        setupSignalMask();
        run_process("Process 1", 1);
    }

    // Parent process continues here
    if (pid1 > 0) {
        pid_t pid2;
        
        // Parent forks Process 2
        pid2 = fork();
        if (pid2 < 0) {
            std::cerr << "Failed to fork Process 2\n" << std::flush;
            exit(1);
        } else if (pid2 == 0) {
            // Child Process 2 (Process 2)
            setupSignalMask();
            execlp("./process2", "process2", nullptr);
            std::cerr << "Failed to exec process2\n" << std::flush;
            exit(1);
        }
        
        // Parent just loops and waits
        while (true) {
            sleep(2);
        }
    }

    return 0;
}
