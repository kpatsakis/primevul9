static int __mmu_unsync_walk(struct kvm_mmu_page *sp,
			   struct kvm_mmu_pages *pvec)
{
	int i, ret, nr_unsync_leaf = 0;

	for_each_set_bit(i, sp->unsync_child_bitmap, 512) {
		struct kvm_mmu_page *child;
		u64 ent = sp->spt[i];

		if (!is_shadow_present_pte(ent) || is_large_pte(ent)) {
			clear_unsync_child_bit(sp, i);
			continue;
		}

		child = to_shadow_page(ent & PT64_BASE_ADDR_MASK);

		if (child->unsync_children) {
			if (mmu_pages_add(pvec, child, i))
				return -ENOSPC;

			ret = __mmu_unsync_walk(child, pvec);
			if (!ret) {
				clear_unsync_child_bit(sp, i);
				continue;
			} else if (ret > 0) {
				nr_unsync_leaf += ret;
			} else
				return ret;
		} else if (child->unsync) {
			nr_unsync_leaf++;
			if (mmu_pages_add(pvec, child, i))
				return -ENOSPC;
		} else
			clear_unsync_child_bit(sp, i);
	}

	return nr_unsync_leaf;
}