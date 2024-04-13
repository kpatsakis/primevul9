zonemgr_putio(dns_io_t **iop) {
	dns_io_t *io;
	dns_io_t *next;
	dns_zonemgr_t *zmgr;

	REQUIRE(iop != NULL);
	io = *iop;
	REQUIRE(DNS_IO_VALID(io));

	*iop = NULL;

	INSIST(!ISC_LINK_LINKED(io, link));
	INSIST(io->event == NULL);

	zmgr = io->zmgr;
	isc_task_detach(&io->task);
	io->magic = 0;
	isc_mem_put(zmgr->mctx, io, sizeof(*io));

	LOCK(&zmgr->iolock);
	INSIST(zmgr->ioactive > 0);
	zmgr->ioactive--;
	next = HEAD(zmgr->high);
	if (next == NULL)
		next = HEAD(zmgr->low);
	if (next != NULL) {
		if (next->high)
			ISC_LIST_UNLINK(zmgr->high, next, link);
		else
			ISC_LIST_UNLINK(zmgr->low, next, link);
		INSIST(next->event != NULL);
	}
	UNLOCK(&zmgr->iolock);
	if (next != NULL)
		isc_task_send(next->task, &next->event);
}