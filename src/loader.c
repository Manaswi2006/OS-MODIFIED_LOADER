#include "loader.h"

int total_page_faults = 0;
int total_page_allocations = 0;
int total_internal_fragmentation = 0;

Elf32_Ehdr ehdr;
Elf32_Phdr *phdr = NULL;
int fd;
size_t page_size;
void* entry_segment_base = NULL;

// Function to clean up resources
void loader_cleanup() {
    // If we had allocated resources like file descriptors or memory,
    // we would clean them up here.
    // Currently, there is nothing to clean up.
}


//Handle Segmentation Fault here
void segfault_handler(int signum, siginfo_t *info, void *context) {
    void *fault_addr = info->si_addr;  // Address that caused the fault
    int page_offset = ((uintptr_t)fault_addr) % page_size;
    uintptr_t page_start = (uintptr_t)fault_addr - page_offset;

    // Iterate through program headers to identify the faulting segment
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD &&
            (uintptr_t)fault_addr >= phdr[i].p_vaddr &&
            (uintptr_t)fault_addr < phdr[i].p_vaddr + phdr[i].p_memsz) {

            // mmap a single page at the page_start address within the segment bounds
            void *mapped_addr = mmap((void*)page_start, page_size, 
                                     PROT_READ | PROT_WRITE | PROT_EXEC,
                                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

            if (mapped_addr == MAP_FAILED) {
                perror("mmap");
                exit(1);
            }

            // Load segment data into the newly allocated page
            lseek(fd, phdr[i].p_offset + (page_start - phdr[i].p_vaddr), SEEK_SET);
            if (read(fd, mapped_addr, page_size - page_offset) < 0) {
                perror("read segment");
                exit(1);
            }

            // Update counters
            total_page_faults++;
            total_page_allocations++;
            total_internal_fragmentation += (page_size - (phdr[i].p_filesz % page_size)) % page_size;

            return;
        }
    }

    // If no matching segment is found, terminate
    fprintf(stderr, "Segmentation fault at invalid address: %p\n", fault_addr);
    exit(1);
}

// Function to load and run an ELF executable
void load_and_run_elf(char** exe) {
    Elf32_Phdr *phdrs;  // Declare phdrs as a pointer to Elf32_Phdr
phdrs = malloc(ehdr.e_phnum * sizeof(Elf32_Phdr));  // Allocate memory for phdrs

    
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

        if (phdr->p_type == PT_LOAD && ehdr.e_entry >= phdr->p_vaddr &&
            ehdr.e_entry < phdr->p_vaddr + phdr->p_memsz) {
            entry_segment_base = (void*)phdr->p_vaddr;
            break;
        }
    }

    // If entry segment found, calculate and call entry point
    // Read program headers into allocated memory
    phdrs = malloc(ehdr.e_phnum * sizeof(Elf32_Phdr));
    lseek(fd, ehdr.e_phoff, SEEK_SET);
    if (read(fd, phdr, ehdr.e_phnum * sizeof(Elf32_Phdr)) != ehdr.e_phnum * sizeof(Elf32_Phdr)) {
        perror("read PHDRs");
        exit(1);
    }

    // Set the entry point to start executing
    entry_segment_base = (void*)phdr->p_vaddr;
    void (*entry_point)() = (void (*)())entry_segment_base;
    entry_point();

    close(fd);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ELF Executable>\n", argv[0]);
        exit(1);
    }
 //    page_size = sysconf(_SC_PAGE_SIZE);
 

     load_and_run_elf(&argv[1]);
    printf("Page faults: %d\n", total_page_faults);
    printf("Page allocations: %d\n", total_page_allocations);
    printf("Total internal fragmentation: %d KB\n", total_internal_fragmentation / 1024);
    loader_cleanup();
    return 0;
}
