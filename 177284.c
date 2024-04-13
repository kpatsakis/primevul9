static void sdma_map_rcu_callback(struct rcu_head *list)
{
	struct sdma_vl_map *m = container_of(list, struct sdma_vl_map, list);

	sdma_map_free(m);
}