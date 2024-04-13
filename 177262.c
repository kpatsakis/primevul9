static void sdma_complete(struct kref *kref)
{
	struct sdma_state *ss =
		container_of(kref, struct sdma_state, kref);

	complete(&ss->comp);
}