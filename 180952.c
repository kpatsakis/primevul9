int local_memory_node(int node)
{
	struct zoneref *z;

	z = first_zones_zonelist(node_zonelist(node, GFP_KERNEL),
				   gfp_zone(GFP_KERNEL),
				   NULL);
	return z->zone->node;
}