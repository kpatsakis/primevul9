static void sdma_put(struct sdma_state *ss)
{
	kref_put(&ss->kref, sdma_complete);
}