void cpu_address_space_init(CPUState *cpu, int asidx, MemoryRegion *mr)
{
    /* Target code should have set num_ases before calling us */
    assert(asidx < cpu->num_ases);

    if (!cpu->cpu_ases) {
        cpu->cpu_ases = g_new0(CPUAddressSpace, cpu->num_ases);
        cpu->cpu_ases[0].cpu = cpu;
        cpu->cpu_ases[0].as = &(cpu->uc->address_space_memory);
        cpu->cpu_ases[0].tcg_as_listener.commit = tcg_commit;
        memory_listener_register(&(cpu->cpu_ases[0].tcg_as_listener), cpu->cpu_ases[0].as);
    }
    /* arm security memory */
    if (asidx > 0) {
        cpu->cpu_ases[asidx].cpu = cpu;
        cpu->cpu_ases[asidx].as = &(cpu->uc->address_space_memory);
        cpu->cpu_ases[asidx].tcg_as_listener.commit = tcg_commit;
        memory_listener_register(&(cpu->cpu_ases[asidx].tcg_as_listener), cpu->cpu_ases[asidx].as);
    }
}