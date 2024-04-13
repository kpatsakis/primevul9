mmu_shrink_count(struct shrinker *shrink, struct shrink_control *sc)
{
	return percpu_counter_read_positive(&kvm_total_used_mmu_pages);
}