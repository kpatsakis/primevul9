static void init_debug_guardpage(void)
{
	if (!debug_pagealloc_enabled())
		return;

	if (!debug_guardpage_minorder())
		return;

	_debug_guardpage_enabled = true;
}