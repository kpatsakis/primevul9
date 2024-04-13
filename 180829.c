inline void post_alloc_hook(struct page *page, unsigned int order,
				gfp_t gfp_flags)
{
	set_page_private(page, 0);
	set_page_refcounted(page);

	arch_alloc_page(page, order);
	kernel_map_pages(page, 1 << order, 1);
	kernel_poison_pages(page, 1 << order, 1);
	kasan_alloc_pages(page, order);
	set_page_owner(page, order, gfp_flags);
}