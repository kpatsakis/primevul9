void sdma_exit(struct hfi1_devdata *dd)
{
	unsigned this_idx;
	struct sdma_engine *sde;

	for (this_idx = 0; dd->per_sdma && this_idx < dd->num_sdma;
			++this_idx) {
		sde = &dd->per_sdma[this_idx];
		if (!list_empty(&sde->dmawait))
			dd_dev_err(dd, "sde %u: dmawait list not empty!\n",
				   sde->this_idx);
		sdma_process_event(sde, sdma_event_e00_go_hw_down);

		del_timer_sync(&sde->err_progress_check_timer);

		/*
		 * This waits for the state machine to exit so it is not
		 * necessary to kill the sdma_sw_clean_up_task to make sure
		 * it is not running.
		 */
		sdma_finalput(&sde->state);
	}
}