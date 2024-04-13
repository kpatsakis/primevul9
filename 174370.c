rdpdr_abort_io(uint32 fd, uint32 major, RD_NTSTATUS status)
{
	uint32 result;
	struct async_iorequest *iorq;
	struct async_iorequest *prev;

	iorq = g_iorequest;
	prev = NULL;
	while (iorq != NULL)
	{
		/* Only remove from table when major is not set, or when correct major is supplied.
		   Abort read should not abort a write io request. */
		if ((iorq->fd == fd) && (major == 0 || iorq->major == major))
		{
			result = 0;
			rdpdr_send_completion(iorq->device, iorq->id, status, result, (uint8 *) "",
					      1);

			iorq = rdpdr_remove_iorequest(prev, iorq);
			return True;
		}

		prev = iorq;
		iorq = iorq->next;
	}

	return False;
}