static void scrub_page_get(struct scrub_page *spage)
{
	atomic_inc(&spage->refs);
}