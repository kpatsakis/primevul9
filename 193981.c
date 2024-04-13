void blk_account_io_completion(struct request *req, unsigned int bytes)
{
	if (blk_do_io_stat(req)) {
		const int sgrp = op_stat_group(req_op(req));
		struct hd_struct *part;

		part_stat_lock();
		part = req->part;
		part_stat_add(part, sectors[sgrp], bytes >> 9);
		part_stat_unlock();
	}
}