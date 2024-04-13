static void print_req_error(struct request *req, blk_status_t status)
{
	int idx = (__force int)status;

	if (WARN_ON_ONCE(idx >= ARRAY_SIZE(blk_errors)))
		return;

	printk_ratelimited(KERN_ERR "%s: %s error, dev %s, sector %llu flags %x\n",
				__func__, blk_errors[idx].name,
				req->rq_disk ?  req->rq_disk->disk_name : "?",
				(unsigned long long)blk_rq_pos(req),
				req->cmd_flags);
}