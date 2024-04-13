void tcg_cpu_address_space_init(CPUState *cpu, AddressSpace *as)
{
    /* We only support one address space per cpu at the moment.  */
    assert(cpu->as == as);

    if (cpu->tcg_as_listener) {
        memory_listener_unregister(cpu->tcg_as_listener);
    } else {
        cpu->tcg_as_listener = g_new0(MemoryListener, 1);
    }
    cpu->tcg_as_listener->commit = tcg_commit;
    memory_listener_register(cpu->tcg_as_listener, as);
}