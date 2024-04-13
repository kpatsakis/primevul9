sg_add_request(Sg_fd * sfp)
{
	int k;
	unsigned long iflags;
	Sg_request *resp;
	Sg_request *rp = sfp->req_arr;

	write_lock_irqsave(&sfp->rq_list_lock, iflags);
	resp = sfp->headrp;
	if (!resp) {
		memset(rp, 0, sizeof (Sg_request));
		rp->parentfp = sfp;
		resp = rp;
		sfp->headrp = resp;
	} else {
		if (0 == sfp->cmd_q)
			resp = NULL;	/* command queuing disallowed */
		else {
			for (k = 0; k < SG_MAX_QUEUE; ++k, ++rp) {
				if (!rp->parentfp)
					break;
			}
			if (k < SG_MAX_QUEUE) {
				memset(rp, 0, sizeof (Sg_request));
				rp->parentfp = sfp;
				while (resp->nextrp)
					resp = resp->nextrp;
				resp->nextrp = rp;
				resp = rp;
			} else
				resp = NULL;
		}
	}
	if (resp) {
		resp->nextrp = NULL;
		resp->header.duration = jiffies_to_msecs(jiffies);
	}
	write_unlock_irqrestore(&sfp->rq_list_lock, iflags);
	return resp;
}