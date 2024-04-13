zonemgr_cancelio(dns_io_t *io) {
	bool send_event = false;

	REQUIRE(DNS_IO_VALID(io));

	/*
	 * If we are queued to be run then dequeue.
	 */
	LOCK(&io->zmgr->iolock);
	if (ISC_LINK_LINKED(io, link)) {
		if (io->high)
			ISC_LIST_UNLINK(io->zmgr->high, io, link);
		else
			ISC_LIST_UNLINK(io->zmgr->low, io, link);

		send_event = true;
		INSIST(io->event != NULL);
	}
	UNLOCK(&io->zmgr->iolock);
	if (send_event) {
		io->event->ev_attributes |= ISC_EVENTATTR_CANCELED;
		isc_task_send(io->task, &io->event);
	}
}