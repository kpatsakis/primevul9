static void kvm_mmu_page_set_gfn(struct kvm_mmu_page *sp, int index, gfn_t gfn)
{
	if (!sp->role.direct) {
		sp->gfns[index] = gfn;
		return;
	}

	if (WARN_ON(gfn != kvm_mmu_page_get_gfn(sp, index)))
		pr_err_ratelimited("gfn mismatch under direct page %llx "
				   "(expected %llx, got %llx)\n",
				   sp->gfn,
				   kvm_mmu_page_get_gfn(sp, index), gfn);
}