static void scrub_block_get(struct scrub_block *sblock)
{
	refcount_inc(&sblock->refs);
}