static int bnx2x_prev_path_mark_eeh(struct bnx2x *bp)
{
	struct bnx2x_prev_path_list *tmp_list;
	int rc;

	rc = down_interruptible(&bnx2x_prev_sem);
	if (rc) {
		BNX2X_ERR("Received %d when tried to take lock\n", rc);
		return rc;
	}

	tmp_list = bnx2x_prev_path_get_entry(bp);
	if (tmp_list) {
		tmp_list->aer = 1;
		rc = 0;
	} else {
		BNX2X_ERR("path %d: Entry does not exist for eeh; Flow occurs before initial insmod is over ?\n",
			  BP_PATH(bp));
	}

	up(&bnx2x_prev_sem);

	return rc;
}