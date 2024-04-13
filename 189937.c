static void paging32_init_context(struct kvm_mmu *context)
{
	context->page_fault = paging32_page_fault;
	context->gva_to_gpa = paging32_gva_to_gpa;
	context->sync_page = paging32_sync_page;
	context->invlpg = paging32_invlpg;
	context->direct_map = false;
}