static inline struct list_head *page_deferred_list(struct page *page)
{
	/*
	 * ->lru in the tail pages is occupied by compound_head.
	 * Let's use ->mapping + ->index in the second tail page as list_head.
	 */
	return (struct list_head *)&page[2].mapping;
}