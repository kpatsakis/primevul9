sg_remove_request(Sg_fd * sfp, Sg_request * srp)
{
	Sg_request *prev_rp;
	Sg_request *rp;
	unsigned long iflags;
	int res = 0;

	if ((!sfp) || (!srp) || (!sfp->headrp))
		return res;
	write_lock_irqsave(&sfp->rq_list_lock, iflags);
	prev_rp = sfp->headrp;
	if (srp == prev_rp) {
		sfp->headrp = prev_rp->nextrp;
		prev_rp->parentfp = NULL;
		res = 1;
	} else {
		while ((rp = prev_rp->nextrp)) {
			if (srp == rp) {
				prev_rp->nextrp = rp->nextrp;
				rp->parentfp = NULL;
				res = 1;
				break;
			}
			prev_rp = rp;
		}
	}
	write_unlock_irqrestore(&sfp->rq_list_lock, iflags);
	return res;
}