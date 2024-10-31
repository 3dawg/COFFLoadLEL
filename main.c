#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct {
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} CoffHeader;

typedef struct {
    char Name[8];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} SectionHeader;

typedef struct {
    uint32_t VirtualAddress;
    uint32_t SymbolTableIndex;
    uint16_t Type;
} RelocationEntry;

void* allocate_memory(size_t size) {
    void* memory = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!memory) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    return memory;
}

void load_section(FILE* file, void* destination, uint32_t offset, uint32_t size) {
    fseek(file, offset, SEEK_SET);
    fread(destination, size, 1, file);
}

void apply_relocations(void* base_address, RelocationEntry* relocations, int count, uint32_t section_offset) {
    for (int i = 0; i < count; i++) {
        RelocationEntry* reloc = &relocations[i];
        uint32_t* target = (uint32_t*)((uint8_t*)base_address + section_offset + reloc->VirtualAddress);
        
        if (reloc->Type == IMAGE_REL_AMD64_ADDR32) {
            *target += (uint32_t)(base_address);
        } else {
            fprintf(stderr, "Unsupported relocation type: %d\n", reloc->Type);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <coff_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening COFF file");
        return 1;
    }

    CoffHeader header;
    fread(&header, sizeof(CoffHeader), 1, file);

    void* base_address = allocate_memory(0x10000);
    SectionHeader* sections = malloc(header.NumberOfSections * sizeof(SectionHeader));
    fread(sections, sizeof(SectionHeader), header.NumberOfSections, file);

    for (int i = 0; i < header.NumberOfSections; i++) {
        SectionHeader* section = &sections[i];
        void* section_address = (void*)((uintptr_t)base_address + section->VirtualAddress);
        load_section(file, section_address, section->PointerToRawData, section->SizeOfRawData);
        
        if (section->NumberOfRelocations > 0) {
            fseek(file, section->PointerToRelocations, SEEK_SET);
            RelocationEntry* relocations = malloc(section->NumberOfRelocations * sizeof(RelocationEntry));
            fread(relocations, sizeof(RelocationEntry), section->NumberOfRelocations, file);
            apply_relocations(base_address, relocations, section->NumberOfRelocations, section->VirtualAddress);
            free(relocations);
        }
    }

    DWORD oldProtect;
    VirtualProtect(base_address, 0x10000, PAGE_EXECUTE_READ, &oldProtect);

    typedef void (*EntryPointFunction)();
    EntryPointFunction entry = (EntryPointFunction)(base_address + sections[0].VirtualAddress);
    entry();

    free(sections);
    fclose(file);
    VirtualFree(base_address, 0, MEM_RELEASE);

    return 0;
}
