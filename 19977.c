static void memory_map_init(struct uc_struct *uc)
{
    uc->system_memory = g_malloc(sizeof(*(uc->system_memory)));
    memory_region_init(uc, uc->system_memory, UINT64_MAX);
    address_space_init(uc, &uc->address_space_memory, uc->system_memory);

    uc->system_io = g_malloc(sizeof(*(uc->system_io)));
    memory_region_init_io(uc, uc->system_io, &unassigned_io_ops, NULL, 65536);
    address_space_init(uc, &uc->address_space_io, uc->system_io);
}