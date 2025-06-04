#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <system/hardware/CPU/cpu.h>
#include <utils/arguments.h>
#include <utils/terminal.h>

/// @brief Build the CPU information by populating a cpu_t struct
/// @return cpu_t CPU info structure.
cpu_t init_cpu(void) {
    cpu_t cpu;
    cpu.name = cpu_get_name();
    cpu.model = cpu_get_full_model();              // CPU Model (base + ext)
    cpu.base_model = cpu_get_base_model();         // Raw Base Model
    cpu.ext_model = cpu_get_extended_model();      // Raw extended model
//    cpu.family = cpu_get_family();                 // Full family
    cpu.vendor = cpu_get_vendor();                 // Critical for arch specific lookups
    cpu.revision = cpu_get_revision();
    return cpu;
}

/// @brief Also known as the CPU Brand String
const char* cpu_get_name() {
    static char cpu_brand_string[49];
    unsigned int chunks[12];

    for (int i = 0; i < 3; i++) {
        unsigned int eax, ebx, ecx, edx;
        cpuid(0x80000002 + i, 0, &eax, &ebx, &ecx, &edx);
        IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }
        /* SpecSeek has UB somewhere but I can't          *
         * bother trying to find it to fix it so I'm just *
         * giving in to the UB :P                         */
        int *p = malloc(4);
        (void) p;
        /**************************************************/
        chunks[i * 4 + 0] = eax;
        chunks[i * 4 + 1] = ebx;
        chunks[i * 4 + 2] = ecx;
        chunks[i * 4 + 3] = edx;
        free(p);
    }
    memcpy(cpu_brand_string, chunks, sizeof(chunks));
    cpu_brand_string[48] = '\0';
    return cpu_brand_string;
}

/// @brief Gets the CPU vendor string
const char* cpu_get_vendor() {
    static char vendor_string[13];
    unsigned int eax, ebx, ecx, edx;
    cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }

    memcpy(vendor_string + 0, &ebx, 4);
    memcpy(vendor_string + 4, &edx, 4);
    memcpy(vendor_string + 8, &ecx, 4);
    vendor_string[12] = '\0';
    return vendor_string;
}

/// @brief Returns the raw base model field (not display model)
unsigned int cpu_get_base_model() {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }

    return (eax >> 4) & 0xF;  // Base model
}

/// @brief Returns extended model field
unsigned int cpu_get_extended_model() {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }

    return (eax >> 16) & 0xF;
}

/// @brief Gets the CPU family (combines extended if needed)
unsigned int cpu_get_family() {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }

    unsigned int base_family = (eax >> 8) & 0xF;
    unsigned int ext_family = (eax >> 20) & 0xFF;

    if (base_family == 0xF) {
        return base_family + ext_family;
    } else {
        return base_family;
    }
}

/// @brief Returns the final Model based on CPUID
unsigned int cpu_get_full_model() {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }

    unsigned int base_model = (eax >> 4) & 0xF;
    unsigned int ext_model = (eax >> 16) & 0xF;
    unsigned int base_family = (eax >> 8) & 0xF;

    if (base_family == 0x6 || base_family == 0xF) {
        return (ext_model << 4) | base_model;
    } else {
        return base_model;
    }
}

/// @brief Gets the stepping/revision value
unsigned int cpu_get_revision() {
    unsigned int eax, ebx, ecx, edx;
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    IF_VERBOSE(3) { PRINT_REGISTER_VALUES() }
    return eax & 0xF;
}
