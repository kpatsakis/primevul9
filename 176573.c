static inline void set_user_asce(struct mm_struct *mm)
{
	S390_lowcore.user_asce = mm->context.asce_bits | __pa(mm->pgd);
	if (current->thread.mm_segment.ar4)
		__ctl_load(S390_lowcore.user_asce, 7, 7);
	set_cpu_flag(CIF_ASCE);
}