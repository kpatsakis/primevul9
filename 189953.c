static bool rmap_write_protect(struct kvm_rmap_head *rmap_head,
			       bool pt_protect)
{
	u64 *sptep;
	struct rmap_iterator iter;
	bool flush = false;

	for_each_rmap_spte(rmap_head, &iter, sptep)
		flush |= spte_write_protect(sptep, pt_protect);

	return flush;
}