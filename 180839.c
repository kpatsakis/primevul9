static inline bool free_pages_prezeroed(void)
{
	return IS_ENABLED(CONFIG_PAGE_POISONING_ZERO) &&
		page_poisoning_enabled();
}