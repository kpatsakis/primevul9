static void phys_map_node_reserve(PhysPageMap *map, unsigned nodes)
{
    if (map->nodes_nb + nodes > map->nodes_nb_alloc) {
        map->nodes_nb_alloc = MAX(map->nodes_nb_alloc * 2, 16);
        map->nodes_nb_alloc = MAX(map->nodes_nb_alloc, map->nodes_nb + nodes);
        map->nodes = g_renew(Node, map->nodes, map->nodes_nb_alloc);
    }
}