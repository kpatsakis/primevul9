static bool need_debug_guardpage(void)
{
	/* If we don't use debug_pagealloc, we don't need guard page */
	if (!debug_pagealloc_enabled())
		return false;

	if (!debug_guardpage_minorder())
		return false;

	return true;
}