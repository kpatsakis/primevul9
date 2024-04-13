static void update_gfn_disallow_lpage_count(const struct kvm_memory_slot *slot,
					    gfn_t gfn, int count)
{
	struct kvm_lpage_info *linfo;
	int i;

	for (i = PG_LEVEL_2M; i <= KVM_MAX_HUGEPAGE_LEVEL; ++i) {
		linfo = lpage_info_slot(gfn, slot, i);
		linfo->disallow_lpage += count;
		WARN_ON(linfo->disallow_lpage < 0);
	}
}