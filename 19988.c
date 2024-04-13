void memory_region_init_io(struct uc_struct *uc,
                           MemoryRegion *mr,
                           const MemoryRegionOps *ops,
                           void *opaque,
                           uint64_t size)
{
    memory_region_init(uc, mr, size);
    mr->ops = ops ? ops : &unassigned_mem_ops;
    mr->opaque = opaque;
    mr->terminates = true;
}