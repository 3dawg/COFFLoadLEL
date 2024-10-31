
# COFFLoader

This project is a simple loader for Common Object File Format (COFF) files, demonstrating an in-memory loading approach for custom CTF setups or research projects. Inspired by the need to handle files in non-standard environments, this loader parses a COFF file, allocates memory, loads sections, applies relocations, and executes the entry point of the loaded code.

## Features

- **Memory Allocation with `VirtualAlloc`:** Loads COFF sections directly into memory with `PAGE_EXECUTE_READWRITE` permissions for ease of testing and execution.
- **Relocation Support:** Handles `IMAGE_REL_AMD64_ADDR32` relocations for basic COFF files. Ideal for environments where COFF debugging information is minimal or deprecated.
- **Section Loading:** Parses each section and relocates based on the offsets defined in the COFF header. Each section is loaded independently, which keeps it flexible for different CTF configurations or sandboxed execution.

## Prerequisites

- Windows environment to support `VirtualAlloc` and other Windows-specific API calls.
- C compiler (GCC or MSVC) that supports C99.
- Basic understanding of COFF structure and Windows file formats (this README assumes you’ve looked up the COFF specification for the basics).

## Usage

1. **Compile the Loader:**
   ```bash
   gcc coff_loader.c -o coff_loader
   ```

2. **Run the Loader:**
   ```bash
   ./coff_loader <path_to_coff_file>
   ```

3. **Example Command:**
   ```bash
   ./coff_loader sample.coff
   ```

The loader will read the COFF file, parse the headers, allocate memory, and load each section. It applies relocations based on the `NumberOfRelocations` in each section header and then jumps to the entry point for execution. Note: if your COFF file has more complex relocation types, you'll need to modify the `apply_relocations` function to handle them.

## Notes

1. **CTF Context**: This loader was designed with CTF challenges in mind, so it prioritizes flexibility and simplicity. For example, it works well for lightweight executables and straightforward relocation needs but isn’t built for highly complex COFF structures. Feel free to customize for any Capture the Flag (CTF) event requirements!
2. **Relocation Limitations**: The loader currently supports `IMAGE_REL_AMD64_ADDR32` relocations for simplicity. Any additional relocation types will require expanding `apply_relocations`.
3. **Executable Memory Permissions**: After loading and relocating, memory permissions are set to `PAGE_EXECUTE_READ` to secure the environment post-allocation. However, consider adjusting permissions based on specific needs in your environment.

## How It Works

1. **Allocate Memory**: The loader allocates a memory block using `VirtualAlloc` for code execution.
2. **Load Sections**: Each section in the COFF file is mapped to the allocated memory based on its `VirtualAddress`.
3. **Apply Relocations**: The loader checks each section for relocations and adjusts pointers accordingly.
4. **Execute Entry Point**: Finally, the loader calls the entry point of the loaded code, allowing for in-memory execution.

## Known Issues & Limitations

- **Limited Relocation Types**: Only basic relocation types are handled; complex COFF files may require further customization.
- **Windows-Only**: This loader is Windows-specific due to `VirtualAlloc` and other APIs.

This project is for educational and CTF purposes. Use responsibly in sandboxed environments!
