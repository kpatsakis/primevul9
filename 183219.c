static int sub_alloc(struct idr *idp, int *starting_id, struct idr_layer **pa)
{
	int n, m, sh;
	struct idr_layer *p, *new;
	int l, id, oid;
	unsigned long bm;

	id = *starting_id;
 restart:
	p = idp->top;
	l = idp->layers;
	pa[l--] = NULL;
	while (1) {
		/*
		 * We run around this while until we reach the leaf node...
		 */
		n = (id >> (IDR_BITS*l)) & IDR_MASK;
		bm = ~p->bitmap;
		m = find_next_bit(&bm, IDR_SIZE, n);
		if (m == IDR_SIZE) {
			/* no space available go back to previous layer. */
			l++;
			oid = id;
			id = (id | ((1 << (IDR_BITS * l)) - 1)) + 1;

			/* if already at the top layer, we need to grow */
			if (id >= 1 << (idp->layers * IDR_BITS)) {
				*starting_id = id;
				return IDR_NEED_TO_GROW;
			}
			p = pa[l];
			BUG_ON(!p);

			/* If we need to go up one layer, continue the
			 * loop; otherwise, restart from the top.
			 */
			sh = IDR_BITS * (l + 1);
			if (oid >> sh == id >> sh)
				continue;
			else
				goto restart;
		}
		if (m != n) {
			sh = IDR_BITS*l;
			id = ((id >> sh) ^ n ^ m) << sh;
		}
		if ((id >= MAX_ID_BIT) || (id < 0))
			return IDR_NOMORE_SPACE;
		if (l == 0)
			break;
		/*
		 * Create the layer below if it is missing.
		 */
		if (!p->ary[m]) {
			new = get_from_free_list(idp);
			if (!new)
				return -1;
			new->layer = l-1;
			rcu_assign_pointer(p->ary[m], new);
			p->count++;
		}
		pa[l--] = p;
		p = p->ary[m];
	}

	pa[l] = p;
	return id;
}