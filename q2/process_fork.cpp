#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <csignal>
#include <cstdlib>

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
    pid_t pid;
    
    // Fork to create child process
    pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed\n" << std::flush;
        exit(1);
    } else if (pid == 0) {
        // Child Process: Execute process2
        setupSignalMask();
        execl("./process2", "process2", nullptr);
        
        // If execl fails
        std::cerr << "Exec failed\n" << std::flush;
        exit(1);
    }
    
    // Parent Process (Process 1)
    setupSignalMask();
    run_process("Process 1", 1);
    
    return 0;
}
