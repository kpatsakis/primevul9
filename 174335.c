add_async_iorequest(uint32 device, uint32 file, uint32 id, uint32 major, uint32 length,
		    DEVICE_FNS * fns, uint32 total_timeout, uint32 interval_timeout, uint8 * buffer,
		    uint32 offset)
{
	struct async_iorequest *iorq;

	if (g_iorequest == NULL)
	{
		g_iorequest = (struct async_iorequest *) xmalloc(sizeof(struct async_iorequest));
		if (!g_iorequest)
			return False;
		g_iorequest->fd = 0;
		g_iorequest->next = NULL;
	}

	iorq = g_iorequest;

	while (iorq->fd != 0)
	{
		/* create new element if needed */
		if (iorq->next == NULL)
		{
			iorq->next =
				(struct async_iorequest *) xmalloc(sizeof(struct async_iorequest));
			if (!iorq->next)
				return False;
			iorq->next->fd = 0;
			iorq->next->next = NULL;
		}
		iorq = iorq->next;
	}
	iorq->device = device;
	iorq->fd = file;
	iorq->id = id;
	iorq->major = major;
	iorq->length = length;
	iorq->partial_len = 0;
	iorq->fns = fns;
	iorq->timeout = total_timeout;
	iorq->itv_timeout = interval_timeout;
	iorq->buffer = buffer;
	iorq->offset = offset;
	return True;
}