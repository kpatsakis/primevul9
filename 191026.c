bool open_was_deferred(uint16 mid)
{
	struct pending_message_list *pml;

	for (pml = deferred_open_queue; pml; pml = pml->next) {
		if (SVAL(pml->buf.data,smb_mid) == mid) {
			return True;
		}
	}
	return False;
}