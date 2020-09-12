#include "includes.hpp"

#include "Memory/memory.hpp"
#include "globals.hpp"
#include "offsets.hpp"
#include "Features/aimbot.hpp"
#include "Features/visuals.hpp"
#include "Features/misc.hpp"

// Thread loops
void loop_aimbot() {
    printf("[+] Aimbot thread sucessfully initialized\n");
    for (;;) {

    }
}
void loop_visuals() {
    printf("[+] Visuals thread sucessfully initialized\n");
    
    for (;;) {
        
    }
}
void loop_misc() {
    printf("[+] Misc thread sucessfully initialized\n");
    for (;;) {

    }
}
void loop_discovery() {
    printf("[+] Discovery thread sucessfully initialized\n");
    // Get Entity Realm base
    offsets::entityrealm_base = mem->mem_read<uint64_t>(offsets::entityrealm + 0x18);
    for (;;) {
        Sleep(1);

        // Get Entity Realm size
        entityrealm_size = mem->mem_read<int>(offsets::entityrealm + 0x10);
        //printf("[+] %s: %d\n", "Entity Realm Size", entityrealm_size);

        // Create buffer for Base Networkable list and read Base Networkable into entity list
        uint64_t* entities = new uint64_t[entityrealm_size * sizeof(uint64_t)];
        mem->mem_read(offsets::entityrealm_base + 0x20, entities, entityrealm_size * sizeof(uint64_t));
    }
}

void offsetinfo(uint64_t offset, std::string name) {
    // If offset is null, print failed to get <name>; otherwise print <name>: <offset>
    if (!offset) {
        printf("[-] Failed to get %s offset\n", name);
        Sleep(5000);
        exit(0);
    }
    else {
        printf("[+] %s: 0x%llx\n", name.c_str(), offset);
    }
}

int main() {
    // Initialize mem object
    printf("[+] Initializing memory object\n");
    mem = new Memory();

    // "Attach" to process
    if (!mem->mem_attach(L"RustClient.exe")) {
        printf("[-] Failed to open handle\n");
        Sleep(5000);
        exit(0);
    }
    printf("[+] Succeeded in opening handle\n\n"); // extra new line to seperate offsets from this

    // Get base networkable address
    offsets::basenetworkable = mem->mem_il2cppscan("BaseNetworkable");
    offsetinfo(offsets::basenetworkable, "Base Networkable");

    // Get entity realm address
    offsets::entityrealm = mem->mem_readchain<uint64_t>(offsets::basenetworkable, { 0xb8, 0x0, 0x10, 0x28 }); // cl_ent, 0x0, ent_realm, buf_list
    offsetinfo(offsets::entityrealm, "Entity Realm");

    // Create threads; also print blank line so thread init prints are below rest
    printf("\n");
    std::thread thread_aimbot(loop_aimbot);
    std::thread thread_visuals(loop_visuals);
    std::thread thread_misc(loop_misc);
    std::thread thread_discovery(loop_discovery);

    // Keep main thread alive but asleep
    for (;;) {
        Sleep(5);
    }
    return 0;
}