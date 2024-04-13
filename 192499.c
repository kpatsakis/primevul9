sg_res_in_use(Sg_fd * sfp)
{
	const Sg_request *srp;
	unsigned long iflags;

	read_lock_irqsave(&sfp->rq_list_lock, iflags);
	for (srp = sfp->headrp; srp; srp = srp->nextrp)
		if (srp->res_used)
			break;
	read_unlock_irqrestore(&sfp->rq_list_lock, iflags);
	return srp ? 1 : 0;
}