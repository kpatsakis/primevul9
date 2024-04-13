void radix__switch_mmu_context(struct mm_struct *prev, struct mm_struct *next)
{
	mtspr(SPRN_PID, next->context.id);
	isync();
}