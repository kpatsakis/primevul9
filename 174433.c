rdpdr_remove_iorequest(struct async_iorequest *prev, struct async_iorequest *iorq)
{
	if (!iorq)
		return NULL;

	if (iorq->buffer)
		xfree(iorq->buffer);
	if (prev)
	{
		prev->next = iorq->next;
		xfree(iorq);
		iorq = prev->next;
	}
	else
	{
		/* Even if NULL */
		g_iorequest = iorq->next;
		xfree(iorq);
		iorq = NULL;
	}
	return iorq;
}