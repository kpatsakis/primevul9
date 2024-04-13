void schedule_deferred_open_smb_message(uint16 mid)
{
	struct pending_message_list *pml;
	int i = 0;

	for (pml = deferred_open_queue; pml; pml = pml->next) {
		uint16 msg_mid = SVAL(pml->buf.data,smb_mid);

		DEBUG(10,("schedule_deferred_open_smb_message: [%d] msg_mid = %u\n", i++,
			(unsigned int)msg_mid ));

		if (mid == msg_mid) {
			struct timed_event *te;

			DEBUG(10,("schedule_deferred_open_smb_message: scheduling mid %u\n",
				mid ));

			te = event_add_timed(smbd_event_context(),
					     pml,
					     timeval_zero(),
					     smbd_deferred_open_timer,
					     pml);
			if (!te) {
				DEBUG(10,("schedule_deferred_open_smb_message: "
					  "event_add_timed() failed, skipping mid %u\n",
					  mid ));
			}

			TALLOC_FREE(pml->te);
			pml->te = te;
			DLIST_PROMOTE(deferred_open_queue, pml);
			return;
		}
	}

	DEBUG(10,("schedule_deferred_open_smb_message: failed to find message mid %u\n",
		mid ));
}