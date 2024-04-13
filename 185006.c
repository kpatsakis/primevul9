xfs_attr3_leaf_list_int(
	struct xfs_buf			*bp,
	struct xfs_attr_list_context	*context)
{
	struct attrlist_cursor_kern	*cursor;
	struct xfs_attr_leafblock	*leaf;
	struct xfs_attr3_icleaf_hdr	ichdr;
	struct xfs_attr_leaf_entry	*entries;
	struct xfs_attr_leaf_entry	*entry;
	int				retval;
	int				i;

	trace_xfs_attr_list_leaf(context);

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&ichdr, leaf);
	entries = xfs_attr3_leaf_entryp(leaf);

	cursor = context->cursor;
	cursor->initted = 1;

	/*
	 * Re-find our place in the leaf block if this is a new syscall.
	 */
	if (context->resynch) {
		entry = &entries[0];
		for (i = 0; i < ichdr.count; entry++, i++) {
			if (be32_to_cpu(entry->hashval) == cursor->hashval) {
				if (cursor->offset == context->dupcnt) {
					context->dupcnt = 0;
					break;
				}
				context->dupcnt++;
			} else if (be32_to_cpu(entry->hashval) >
					cursor->hashval) {
				context->dupcnt = 0;
				break;
			}
		}
		if (i == ichdr.count) {
			trace_xfs_attr_list_notfound(context);
			return 0;
		}
	} else {
		entry = &entries[0];
		i = 0;
	}
	context->resynch = 0;

	/*
	 * We have found our place, start copying out the new attributes.
	 */
	retval = 0;
	for (; i < ichdr.count; entry++, i++) {
		if (be32_to_cpu(entry->hashval) != cursor->hashval) {
			cursor->hashval = be32_to_cpu(entry->hashval);
			cursor->offset = 0;
		}

		if (entry->flags & XFS_ATTR_INCOMPLETE)
			continue;		/* skip incomplete entries */

		if (entry->flags & XFS_ATTR_LOCAL) {
			xfs_attr_leaf_name_local_t *name_loc =
				xfs_attr3_leaf_name_local(leaf, i);

			retval = context->put_listent(context,
						entry->flags,
						name_loc->nameval,
						(int)name_loc->namelen,
						be16_to_cpu(name_loc->valuelen),
						&name_loc->nameval[name_loc->namelen]);
			if (retval)
				return retval;
		} else {
			xfs_attr_leaf_name_remote_t *name_rmt =
				xfs_attr3_leaf_name_remote(leaf, i);

			int valuelen = be32_to_cpu(name_rmt->valuelen);

			if (context->put_value) {
				xfs_da_args_t args;

				memset((char *)&args, 0, sizeof(args));
				args.dp = context->dp;
				args.whichfork = XFS_ATTR_FORK;
				args.valuelen = valuelen;
				args.value = kmem_alloc(valuelen, KM_SLEEP | KM_NOFS);
				args.rmtblkno = be32_to_cpu(name_rmt->valueblk);
				args.rmtblkcnt = XFS_B_TO_FSB(args.dp->i_mount, valuelen);
				retval = xfs_attr_rmtval_get(&args);
				if (retval)
					return retval;
				retval = context->put_listent(context,
						entry->flags,
						name_rmt->name,
						(int)name_rmt->namelen,
						valuelen,
						args.value);
				kmem_free(args.value);
			} else {
				retval = context->put_listent(context,
						entry->flags,
						name_rmt->name,
						(int)name_rmt->namelen,
						valuelen,
						NULL);
			}
			if (retval)
				return retval;
		}
		if (context->seen_enough)
			break;
		cursor->offset++;
	}
	trace_xfs_attr_list_leaf_end(context);
	return retval;
}