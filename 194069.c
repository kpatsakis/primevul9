void blk_put_request(struct request *req)
{
	blk_mq_free_request(req);
}