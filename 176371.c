struct page *follow_page(struct vm_area_struct *vma, unsigned long address,
			 unsigned int foll_flags)
{
	struct follow_page_context ctx = { NULL };
	struct page *page;

	page = follow_page_mask(vma, address, foll_flags, &ctx);
	if (ctx.pgmap)
		put_dev_pagemap(ctx.pgmap);
	return page;
}