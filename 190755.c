void __destroy_context(int context_id)
{
	ida_free(&mmu_context_ida, context_id);
}