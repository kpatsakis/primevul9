zonemgr_getio(dns_zonemgr_t *zmgr, bool high,
	      isc_task_t *task, isc_taskaction_t action, void *arg,
	      dns_io_t **iop)
{
	dns_io_t *io;
	bool queue;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));
	REQUIRE(iop != NULL && *iop == NULL);

	io = isc_mem_get(zmgr->mctx, sizeof(*io));
	if (io == NULL)
		return (ISC_R_NOMEMORY);

	io->event = isc_event_allocate(zmgr->mctx, task, DNS_EVENT_IOREADY,
				       action, arg, sizeof(*io->event));
	if (io->event == NULL) {
		isc_mem_put(zmgr->mctx, io, sizeof(*io));
		return (ISC_R_NOMEMORY);
	}

	io->zmgr = zmgr;
	io->high = high;
	io->task = NULL;
	isc_task_attach(task, &io->task);
	ISC_LINK_INIT(io, link);
	io->magic = IO_MAGIC;

	LOCK(&zmgr->iolock);
	zmgr->ioactive++;
	queue = (zmgr->ioactive > zmgr->iolimit);
	if (queue) {
		if (io->high)
			ISC_LIST_APPEND(zmgr->high, io, link);
		else
			ISC_LIST_APPEND(zmgr->low, io, link);
	}
	UNLOCK(&zmgr->iolock);
	*iop = io;

	if (!queue)
		isc_task_send(io->task, &io->event);
	return (ISC_R_SUCCESS);
}