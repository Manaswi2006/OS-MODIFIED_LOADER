Here is a README file for your project with all the necessary details:

```markdown
# OS-LOADER

## OVERVIEW
This project involves creating a SimpleLoader for loading and executing ELF 32-bit executables in plain C. The loader is implemented from scratch without using any external library APIs for manipulating ELF files. It handles segment loading, memory management, and segmentation faults.

## PROJECT STRUCTURE

```
├── src

│      ├── loader

│      ├── loader.c

│      └── loader.h

│      └── Makefile

├── test

│      ├── Makefile

│      └── fib

|      └──sum.c

│      └── fib.c

├── README.md

└── Makefile
```

### src/
- **loader.c**: Contains the implementation of the loader.
- **loader.h**: Header file with necessary declarations.
- **Makefile**: Build instructions for the loader.
  
### test/
- **fib.c**: A test ELF executable containing a simple Fibonacci computation (`fib(40)`).
- **fib**: The compiled ELF file generated from `fib.c`.
- **Makefile**: Build instructions for the test.

## INSTALLATION

To build the project, follow these steps:

1. Clone the repository:
   ```bash
   git clone git@github.com:Manaswi2006/OS-LOADER-.git
   ```

2. Install necessary software:
   Ensure you have a Unix-based OS with a C compiler and GNU make installed. For Windows, use WSL with a Linux distribution (e.g., Ubuntu).

## USAGE

### Prepare the test case:
1. Compile `fib.c` after running the `make all` command:
   ```bash
   make all
   ```

2. Run the loader:
   - Use the launch executable to run the loader with the compiled ELF file as a command-line argument:
     ```bash
     ./src/loader ../test/fib
     ```
   - This will execute the `fib.c` test code and output the value of `fib(40)`.

## REFERENCES

1. ELF Specification: [ELF Manpage](https://man7.org/linux/man-pages/man5/elf.5.html)
2. OSDev ELF Tutorial: [OSDev ELF Tutorial](https://wiki.osdev.org/ELF_Tutorial)

## CONTRIBUTORS
1. **Manaswi Singh** (2023307)
2. **Paridhi Kotarya** (2023367)

---

## Code Implementation Details

### Key Functions:

- **segfault_handler**: Handles segmentation faults by mmaping a page when a fault occurs and loading the corresponding segment from the ELF file.
  
- **load_and_run_elf**: Loads the ELF file, reads the headers, and maps the required segments into memory. It then calls the entry point of the program.

- **loader_cleanup**: Cleans up allocated resources (currently empty, but can be extended in future).

### Key Variables:
- `ehdr`: ELF header.
- `phdr`: Program header.
- `phdrs`: Array of program headers.
- `fd`: File descriptor for the ELF file.
- `entry_segment_base`: Base address of the entry segment.
- `page_size`: System page size.
  
---
 
