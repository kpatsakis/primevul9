static bool bnx2x_prev_is_path_marked(struct bnx2x *bp)
{
	struct bnx2x_prev_path_list *tmp_list;
	bool rc = false;

	if (down_trylock(&bnx2x_prev_sem))
		return false;

	tmp_list = bnx2x_prev_path_get_entry(bp);
	if (tmp_list) {
		if (tmp_list->aer) {
			DP(NETIF_MSG_HW, "Path %d was marked by AER\n",
			   BP_PATH(bp));
		} else {
			rc = true;
			BNX2X_DEV_INFO("Path %d was already cleaned from previous drivers\n",
				       BP_PATH(bp));
		}
	}

	up(&bnx2x_prev_sem);

	return rc;
}