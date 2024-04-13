static struct rt6_info *rt6_multipath_select(struct rt6_info *match,
					     struct flowi6 *fl6, int oif,
					     int strict)
{
	struct rt6_info *sibling, *next_sibling;
	int route_choosen;

	route_choosen = rt6_info_hash_nhsfn(match->rt6i_nsiblings + 1, fl6);
	/* Don't change the route, if route_choosen == 0
	 * (siblings does not include ourself)
	 */
	if (route_choosen)
		list_for_each_entry_safe(sibling, next_sibling,
				&match->rt6i_siblings, rt6i_siblings) {
			route_choosen--;
			if (route_choosen == 0) {
				if (rt6_score_route(sibling, oif, strict) < 0)
					break;
				match = sibling;
				break;
			}
		}
	return match;
}