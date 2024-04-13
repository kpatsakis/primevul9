void remove_deferred_open_smb_message(uint16 mid)
{
	struct pending_message_list *pml;

	for (pml = deferred_open_queue; pml; pml = pml->next) {
		if (mid == SVAL(pml->buf.data,smb_mid)) {
			DEBUG(10,("remove_sharing_violation_open_smb_message: "
				  "deleting mid %u len %u\n",
				  (unsigned int)mid,
				  (unsigned int)pml->buf.length ));
			DLIST_REMOVE(deferred_open_queue, pml);
			TALLOC_FREE(pml);
			return;
		}
	}
}