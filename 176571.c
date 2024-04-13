static inline unsigned long pgd_entry_type(struct mm_struct *mm)
{
	if (mm->context.asce_limit <= (1UL << 31))
		return _SEGMENT_ENTRY_EMPTY;
	if (mm->context.asce_limit <= (1UL << 42))
		return _REGION3_ENTRY_EMPTY;
	return _REGION2_ENTRY_EMPTY;
}