sg_finish_rem_req(Sg_request *srp)
{
	int ret = 0;

	Sg_fd *sfp = srp->parentfp;
	Sg_scatter_hold *req_schp = &srp->data;

	SCSI_LOG_TIMEOUT(4, sg_printk(KERN_INFO, sfp->parentdp,
				      "sg_finish_rem_req: res_used=%d\n",
				      (int) srp->res_used));
	if (srp->bio)
		ret = blk_rq_unmap_user(srp->bio);

	if (srp->rq) {
		scsi_req_free_cmd(scsi_req(srp->rq));
		blk_put_request(srp->rq);
	}

	if (srp->res_used)
		sg_unlink_reserve(sfp, srp);
	else
		sg_remove_scat(sfp, req_schp);

	sg_remove_request(sfp, srp);

	return ret;
}