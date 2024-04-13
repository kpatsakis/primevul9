static inline void __try_update_largest_extent(struct extent_tree *et,
						struct extent_node *en)
{
	if (en->ei.len > et->largest.len) {
		et->largest = en->ei;
		et->largest_updated = true;
	}
}