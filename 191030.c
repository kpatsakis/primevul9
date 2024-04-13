NTSTATUS allow_new_trans(struct trans_state *list, int mid)
{
	int count = 0;
	for (; list != NULL; list = list->next) {

		if (list->mid == mid) {
			return NT_STATUS_INVALID_PARAMETER;
		}

		count += 1;
	}
	if (count > 5) {
		return NT_STATUS_INSUFFICIENT_RESOURCES;
	}

	return NT_STATUS_OK;
}