static void sdma_get(struct sdma_state *ss)
{
	kref_get(&ss->kref);
}