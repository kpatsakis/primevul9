static void update_range(struct sw_flow_match *match,
			 size_t offset, size_t size, bool is_mask)
{
	struct sw_flow_key_range *range;
	size_t start = rounddown(offset, sizeof(long));
	size_t end = roundup(offset + size, sizeof(long));

	if (!is_mask)
		range = &match->range;
	else
		range = &match->mask->range;

	if (range->start == range->end) {
		range->start = start;
		range->end = end;
		return;
	}

	if (range->start > start)
		range->start = start;

	if (range->end < end)
		range->end = end;
}