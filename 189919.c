static bool is_obsolete_root(struct kvm *kvm, hpa_t root_hpa)
{
	struct kvm_mmu_page *sp;

	if (!VALID_PAGE(root_hpa))
		return false;

	/*
	 * When freeing obsolete roots, treat roots as obsolete if they don't
	 * have an associated shadow page.  This does mean KVM will get false
	 * positives and free roots that don't strictly need to be freed, but
	 * such false positives are relatively rare:
	 *
	 *  (a) only PAE paging and nested NPT has roots without shadow pages
	 *  (b) remote reloads due to a memslot update obsoletes _all_ roots
	 *  (c) KVM doesn't track previous roots for PAE paging, and the guest
	 *      is unlikely to zap an in-use PGD.
	 */
	sp = to_shadow_page(root_hpa);
	return !sp || is_obsolete_sp(kvm, sp);
}