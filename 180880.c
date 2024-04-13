static void __init find_usable_zone_for_movable(void)
{
	int zone_index;
	for (zone_index = MAX_NR_ZONES - 1; zone_index >= 0; zone_index--) {
		if (zone_index == ZONE_MOVABLE)
			continue;

		if (arch_zone_highest_possible_pfn[zone_index] >
				arch_zone_lowest_possible_pfn[zone_index])
			break;
	}

	VM_BUG_ON(zone_index == -1);
	movable_zone = zone_index;
}