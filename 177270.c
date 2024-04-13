static void sdma_err_progress_check_schedule(struct sdma_engine *sde)
{
	if (!is_bx(sde->dd) && HFI1_CAP_IS_KSET(SDMA_AHG)) {
		unsigned index;
		struct hfi1_devdata *dd = sde->dd;

		for (index = 0; index < dd->num_sdma; index++) {
			struct sdma_engine *curr_sdma = &dd->per_sdma[index];

			if (curr_sdma != sde)
				curr_sdma->progress_check_head =
							curr_sdma->descq_head;
		}
		dd_dev_err(sde->dd,
			   "SDMA engine %d - check scheduled\n",
				sde->this_idx);
		mod_timer(&sde->err_progress_check_timer, jiffies + 10);
	}
}