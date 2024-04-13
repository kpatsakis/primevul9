static inline void clear_unsync_child_bit(struct kvm_mmu_page *sp, int idx)
{
	--sp->unsync_children;
	WARN_ON((int)sp->unsync_children < 0);
	__clear_bit(idx, sp->unsync_child_bitmap);
}