void blk_dump_rq_flags(struct request *rq, char *msg)
{
	printk(KERN_INFO "%s: dev %s: flags=%llx\n", msg,
		rq->rq_disk ? rq->rq_disk->disk_name : "?",
		(unsigned long long) rq->cmd_flags);

	printk(KERN_INFO "  sector %llu, nr/cnr %u/%u\n",
	       (unsigned long long)blk_rq_pos(rq),
	       blk_rq_sectors(rq), blk_rq_cur_sectors(rq));
	printk(KERN_INFO "  bio %p, biotail %p, len %u\n",
	       rq->bio, rq->biotail, blk_rq_bytes(rq));
}