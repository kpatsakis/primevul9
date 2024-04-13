static void paging64_init_context(struct kvm_mmu *context)
{
	context->page_fault = paging64_page_fault;
	context->gva_to_gpa = paging64_gva_to_gpa;
	context->sync_page = paging64_sync_page;
	context->invlpg = paging64_invlpg;
	context->direct_map = false;
}