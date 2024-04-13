static void sdma_finalput(struct sdma_state *ss)
{
	sdma_put(ss);
	wait_for_completion(&ss->comp);
}