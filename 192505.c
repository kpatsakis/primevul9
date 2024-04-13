sg_unlink_reserve(Sg_fd * sfp, Sg_request * srp)
{
	Sg_scatter_hold *req_schp = &srp->data;

	SCSI_LOG_TIMEOUT(4, sg_printk(KERN_INFO, srp->parentfp->parentdp,
				      "sg_unlink_reserve: req->k_use_sg=%d\n",
				      (int) req_schp->k_use_sg));
	req_schp->k_use_sg = 0;
	req_schp->bufflen = 0;
	req_schp->pages = NULL;
	req_schp->page_order = 0;
	req_schp->sglist_len = 0;
	sfp->save_scat_len = 0;
	srp->res_used = 0;
}