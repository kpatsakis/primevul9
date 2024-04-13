static void nonpaging_init_context(struct kvm_mmu *context)
{
	context->page_fault = nonpaging_page_fault;
	context->gva_to_gpa = nonpaging_gva_to_gpa;
	context->sync_page = nonpaging_sync_page;
	context->invlpg = NULL;
	context->direct_map = true;
}