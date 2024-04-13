static void sdma_map_free(struct sdma_vl_map *m)
{
	int i;

	for (i = 0; m && i < m->actual_vls; i++)
		kfree(m->map[i]);
	kfree(m);
}