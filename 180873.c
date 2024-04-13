static void build_zonelists(pg_data_t *pgdat)
{
	int node, local_node;
	struct zoneref *zonerefs;
	int nr_zones;

	local_node = pgdat->node_id;

	zonerefs = pgdat->node_zonelists[ZONELIST_FALLBACK]._zonerefs;
	nr_zones = build_zonerefs_node(pgdat, zonerefs);
	zonerefs += nr_zones;

	/*
	 * Now we build the zonelist so that it contains the zones
	 * of all the other nodes.
	 * We don't want to pressure a particular node, so when
	 * building the zones for node N, we make sure that the
	 * zones coming right after the local ones are those from
	 * node N+1 (modulo N)
	 */
	for (node = local_node + 1; node < MAX_NUMNODES; node++) {
		if (!node_online(node))
			continue;
		nr_zones = build_zonerefs_node(NODE_DATA(node), zonerefs);
		zonerefs += nr_zones;
	}
	for (node = 0; node < local_node; node++) {
		if (!node_online(node))
			continue;
		nr_zones = build_zonerefs_node(NODE_DATA(node), zonerefs);
		zonerefs += nr_zones;
	}

	zonerefs->zone = NULL;
	zonerefs->zone_idx = 0;
}