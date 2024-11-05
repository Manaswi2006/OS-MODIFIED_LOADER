#include "loader.h"

int total_page_faults = 0;
int total_page_allocations = 0;
int total_internal_fragmentation = 0;

Elf32_Ehdr ehdr;
Elf32_Phdr *phdrs = NULL;
int fd;
size_t page_size;
void* entry_segment_base = NULL;

// Function to clean up resources
void loader_cleanup() {
    // If we had allocated resources like file descriptors or memory,
    // we would clean them up here.
    // Currently, there is nothing to clean up.
}

void segfault_handler(int signum) {
//Handle Segmentation Fault here
}
// Function to load and run an ELF executable
void load_and_run_elf(char** exe) {
    
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = segfault_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);

    fd = open(exe[0], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // Read and validate the ELF header
    if (read(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        perror("read ELF header");
        exit(1);
    }

    // Check the ELF magic number
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Invalid ELF file\n");
        exit(1);
    }

    //printf("Entry point address: 0x%x\n", ehdr.e_entry);
    //printf("Number of program headers (e_phnum): %d\n", ehdr.e_phnum);

    // Move to the PHDR table
    for (int i = 0; i < ehdr.e_phnum; i++) {
        lseek(fd, ehdr.e_phoff + i * sizeof(Elf32_Phdr), SEEK_SET);
        if (read(fd, &phdr, sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
            perror("read PHDR");
            exit(1);
        }

        if (phdr.p_type == PT_LOAD && ehdr.e_entry >= phdr.p_vaddr &&
            ehdr.e_entry < phdr.p_vaddr + phdr.p_memsz) {
            entry_segment_base = (void*)phdr.p_vaddr;
            break;
        }
    }

    // If entry segment found, calculate and call entry point
    if (entry_segment_base) {
        void (*entry_point)() = (void (*)())(entry_segment_base + (ehdr.e_entry - phdr.p_vaddr));
        entry_point();
    } else {
        fprintf(stderr, "Entry point not within any loaded segment\n");
        exit(1);
    }
    close(fd);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }

    load_and_run_elf(&argv[1]);
    printf("Page faults: %d\n", page_fault_count);
    printf("Page allocations: %d\n", page_alloc_count);
    printf("Total internal fragmentation: %d KB\n", total_fragmentation / 1024);
    loader_cleanup();
    return 0;
}
