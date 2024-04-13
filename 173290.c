static int bnx2x_prev_mark_path(struct bnx2x *bp, bool after_undi)
{
	struct bnx2x_prev_path_list *tmp_list;
	int rc;

	rc = down_interruptible(&bnx2x_prev_sem);
	if (rc) {
		BNX2X_ERR("Received %d when tried to take lock\n", rc);
		return rc;
	}

	/* Check whether the entry for this path already exists */
	tmp_list = bnx2x_prev_path_get_entry(bp);
	if (tmp_list) {
		if (!tmp_list->aer) {
			BNX2X_ERR("Re-Marking the path.\n");
		} else {
			DP(NETIF_MSG_HW, "Removing AER indication from path %d\n",
			   BP_PATH(bp));
			tmp_list->aer = 0;
		}
		up(&bnx2x_prev_sem);
		return 0;
	}
	up(&bnx2x_prev_sem);

	/* Create an entry for this path and add it */
	tmp_list = kmalloc(sizeof(struct bnx2x_prev_path_list), GFP_KERNEL);
	if (!tmp_list) {
		BNX2X_ERR("Failed to allocate 'bnx2x_prev_path_list'\n");
		return -ENOMEM;
	}

	tmp_list->bus = bp->pdev->bus->number;
	tmp_list->slot = PCI_SLOT(bp->pdev->devfn);
	tmp_list->path = BP_PATH(bp);
	tmp_list->aer = 0;
	tmp_list->undi = after_undi ? (1 << BP_PORT(bp)) : 0;

	rc = down_interruptible(&bnx2x_prev_sem);
	if (rc) {
		BNX2X_ERR("Received %d when tried to take lock\n", rc);
		kfree(tmp_list);
	} else {
		DP(NETIF_MSG_HW, "Marked path [%d] - finished previous unload\n",
		   BP_PATH(bp));
		list_add(&tmp_list->list, &bnx2x_prev_list);
		up(&bnx2x_prev_sem);
	}

	return rc;
}