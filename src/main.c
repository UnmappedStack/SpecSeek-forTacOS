#include <stdio.h>
#include <string.h>

#include <main.h>
#include <utils/arguments.h>
#include <utils/terminal.h>
#include <utils/asciiart.h>
#include <system/hardware/CPU/cpu.h>
#include <system/hardware/CPU/microarch.h>

/// @brief Spec Seek Entry Point
/// @param argc argument count
/// @param argv argument vector
int main(int argc, const char** argv){
    construct_arguments(argc, argv);

    CLEAR_SCREEN();
    ASCII_DIVIDER("Spec Seek by Mellurboo", BLUE);
    printf("%sCompiled with GCC Version %d.%d.%d\n", BLUE,__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    printf("%sCompiled at: %s : %s\n", BLUE, __DATE__, __TIME__);
    if (get_arguments().verbose) printf("%sRunning in Verbose Mode {%d}\n", BLUE, get_arguments().verbose);
    
    cpu_t cpu = init_cpu(); 
    cpu_info(cpu);

    // Windows will close straight away after execution so this allows users to read it.
    #ifdef __MINGW32__
    printf("\n%sSpec Seek has finished, Press any key to exit: ", BWHITE);
    getchar();
    #endif

    printf("%s", RESET);
    fflush(stdout);
}

/// @brief Print and Display the CPU information
/// @param cpu populated cpu structure
void cpu_info(cpu_t cpu){
    IF_VENDOR_AMD({
        ASCII_DIVIDER("AMD CPU", RED);
        printf("%sCPU Name:             \t%s%s\n", BWHITE, RED, cpu.name);
        printf("%sVendor String:        \t%s%s\n", BWHITE, RED, cpu.vendor);
        printf("%sCPU Microarchitecture:\t%s%s\n", BWHITE, RED, amd_cpu_get_microarch(cpu.family, cpu.model));

        ASCII_DIVIDER_SMALL("OEM Info", RED);
        printf("%sCPU Model:    \t%s0x%X \t(%u)\n", BWHITE, RED, cpu.model, cpu.model);
        printf("%sCPU BaseModel:\t%s0x%X \t(%u)\n", BWHITE, RED, cpu.base_model, cpu.base_model);
        printf("%sCPU ExtModel: \t%s0x%X \t(%u)\n", BWHITE, RED, cpu.ext_model, cpu.ext_model);
        printf("%sCPU Family:   \t%s0x%X \t(%u)\n", BWHITE, RED, cpu.family, cpu.family);
        printf("%sCPU Revision: \t%s0x%X \t(%u)", BWHITE, RED, cpu.revision, cpu.revision);
        ASCII_DIVIDER_SMALL_NOTEXT(RED);
    });

    IF_VENDOR_INTEL({
        ASCII_DIVIDER("INTEL CPU", CYAN);
        printf("%sCPU Name:             \t%s%s\n", BWHITE, CYAN, cpu.name);
        printf("%sVendor String:        \t%s%s\n", BWHITE, CYAN, cpu.vendor);
        printf("%sCPU Microarchitecture:\t%s%s\n", BWHITE, CYAN, intel_get_cpu_microarch(cpu.family, cpu_get_base_model(), cpu.ext_model, cpu.revision));

        ASCII_DIVIDER_SMALL("OEM Info", CYAN);
        printf("%sCPU Model:    \t%s0x%X \t(%u)\n", BWHITE, CYAN, cpu.model, cpu.model);
        printf("%sCPU BaseModel:\t%s0x%X \t(%u)\n", BWHITE, CYAN, cpu.base_model, cpu.base_model);
        printf("%sCPU ExtModel: \t%s0x%X \t(%u)\n", BWHITE, CYAN, cpu.ext_model, cpu.ext_model);
        printf("%sCPU Family:   \t%s0x%X \t(%u)\n", BWHITE, CYAN, cpu.family, cpu.family);
        printf("%sCPU Revision: \t%s0x%X \t(%u)", BWHITE, CYAN, cpu.revision, cpu.revision);
        ASCII_DIVIDER_SMALL_NOTEXT(CYAN);
    });

    /*
        Printing the CPU features (Platform Specific logic is handled within the functions below.)
    */
    ASCII_DIVIDER("CPU Standard Features", BYELLOW);
    cpu_check_standard_features();
    ASCII_DIVIDER("CPU Extended Features", BYELLOW);
    cpu_check_extended_features();
    printf("\n");
}
