		bnx2x_prev_path_get_entry(struct bnx2x *bp)
{
	struct bnx2x_prev_path_list *tmp_list;

	list_for_each_entry(tmp_list, &bnx2x_prev_list, list)
		if (PCI_SLOT(bp->pdev->devfn) == tmp_list->slot &&
		    bp->pdev->bus->number == tmp_list->bus &&
		    BP_PATH(bp) == tmp_list->path)
			return tmp_list;

	return NULL;
}