struct request_queue *blk_alloc_queue(gfp_t gfp_mask)
{
	return blk_alloc_queue_node(gfp_mask, NUMA_NO_NODE);
}