static void sdma_start_hw_clean_up(struct sdma_engine *sde)
{
	tasklet_hi_schedule(&sde->sdma_hw_clean_up_task);
}