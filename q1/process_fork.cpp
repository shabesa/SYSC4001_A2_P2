#include <iostream>
#include <unistd.h>
#include <sys/types.h>

void run_process(const char* process_name){
    
    int counter = 0;
    while (true){
        std::cout << process_name << " (PID: " << getpid() << ") - Counter: " << counter << '\n' << std::flush;
        counter++;
        sleep(1);
        
    }
}

int main() {
    pid_t pid;

    // First fork
    pid = fork();

    if (pid < 0) {
        std::cerr << "First fork failed\n";
        return 1;

    } else if (pid == 0) {
        // Child - Process 1
        run_process("Process 1");

    } else {
        // Parent - Process 2
        run_process("Process 2");
        
    }
    
    return EXIT_SUCCESS;
}