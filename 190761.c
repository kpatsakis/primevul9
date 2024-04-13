void hash__reserve_context_id(int id)
{
	int result = ida_alloc_range(&mmu_context_ida, id, id, GFP_KERNEL);

	WARN(result != id, "mmu: Failed to reserve context id %d (rc %d)\n", id, result);
}