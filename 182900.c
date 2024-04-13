move_matching_tuples(dns_difftuple_t *cur, dns_diff_t *src, dns_diff_t *dst) {
	do {
		dns_difftuple_t *next = find_next_matching_tuple(cur);
		ISC_LIST_UNLINK(src->tuples, cur, link);
		dns_diff_appendminimal(dst, &cur);
		cur = next;
	} while (cur != NULL);
}