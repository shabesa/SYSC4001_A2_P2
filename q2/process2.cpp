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

int main() {
    setupSignalMask();
    
    int counter = 0;
    int cycle = 0;
    
    while (true) {
        std::cout << "Process 2 - Cycle number: " << cycle << " (PID: " << getpid() << ")";
        
        if (counter % 3 == 0) {
            std::cout << " : " << counter << " is a multiple of 3";
        }
        std::cout << '\n' << std::flush;
        
        counter--;  // Decrement
        cycle++;
        sleep(1);
    }
    
    return 0;
}
