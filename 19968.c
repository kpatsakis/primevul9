static void io_mem_init(struct uc_struct *uc)
{
    memory_region_init_io(uc, &uc->io_mem_unassigned, &unassigned_mem_ops, NULL,
                          UINT64_MAX);
}