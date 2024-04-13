xfs_bmap_select_minlen(
	struct xfs_bmalloca	*ap,
	struct xfs_alloc_arg	*args,
	xfs_extlen_t		*blen,
	int			notinit)
{
	if (notinit || *blen < ap->minlen) {
		/*
		 * Since we did a BUF_TRYLOCK above, it is possible that
		 * there is space for this request.
		 */
		args->minlen = ap->minlen;
	} else if (*blen < args->maxlen) {
		/*
		 * If the best seen length is less than the request length,
		 * use the best as the minimum.
		 */
		args->minlen = *blen;
	} else {
		/*
		 * Otherwise we've seen an extent as big as maxlen, use that
		 * as the minimum.
		 */
		args->minlen = args->maxlen;
	}
}