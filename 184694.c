static void tcg_commit(MemoryListener *listener)
{
    CPUState *cpu;

    /* since each CPU stores ram addresses in its TLB cache, we must
       reset the modified entries */
    /* XXX: slow ! */
    CPU_FOREACH(cpu) {
        /* FIXME: Disentangle the cpu.h circular files deps so we can
           directly get the right CPU from listener.  */
        if (cpu->tcg_as_listener != listener) {
            continue;
        }
        cpu_reload_memory_map(cpu);
    }
}