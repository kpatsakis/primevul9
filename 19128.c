R_API RCoreAnalStats* r_core_anal_get_stats(RCore *core, ut64 from, ut64 to, ut64 step) {
	RAnalFunction *F;
	RAnalBlock  *B;
	RBinSymbol *S;
	RListIter *iter, *iter2;
	RCoreAnalStats *as = NULL;
	int piece, as_size, blocks;
	ut64 at;

	if (from == to || from == UT64_MAX || to == UT64_MAX) {
		eprintf ("Cannot alloc for this range\n");
		return NULL;
	}
	as = R_NEW0 (RCoreAnalStats);
	if (!as) {
		return NULL;
	}
	if (step < 1) {
		step = 1;
	}
	blocks = (to - from) / step;
	as_size = (1 + blocks) * sizeof (RCoreAnalStatsItem);
	as->block = malloc (as_size);
	if (!as->block) {
		free (as);
		return NULL;
	}
	memset (as->block, 0, as_size);
	for (at = from; at < to; at += step) {
		RIOMap *map = r_io_map_get_at (core->io, at);
		piece = (at - from) / step;
		as->block[piece].perm = map ? map->perm: (core->io->desc ? core->io->desc->perm: 0);
	}
	// iter all flags
	struct block_flags_stat_t u = { .step = step, .from = from, .as = as };
	r_flag_foreach_range (core->flags, from, to + 1, block_flags_stat, &u);
	// iter all functions
	r_list_foreach (core->anal->fcns, iter, F) {
		if (F->addr < from || F->addr > to) {
			continue;
		}
		piece = (F->addr - from) / step;
		as->block[piece].functions++;
		ut64 last_piece = R_MIN ((F->addr + r_anal_function_linear_size (F) - 1) / step, blocks - 1);
		for (; piece <= last_piece; piece++) {
			as->block[piece].in_functions++;
		}
		// iter all basic blocks
		r_list_foreach (F->bbs, iter2, B) {
			if (B->addr < from || B->addr > to) {
				continue;
			}
			piece = (B->addr - from) / step;
			as->block[piece].blocks++;
		}
	}
	// iter all symbols
	r_list_foreach (r_bin_get_symbols (core->bin), iter, S) {
		if (S->vaddr < from || S->vaddr > to) {
			continue;
		}
		piece = (S->vaddr - from) / step;
		as->block[piece].symbols++;
	}
	RPVector *metas = to > from ? r_meta_get_all_intersect (core->anal, from, to - from, R_META_TYPE_ANY) : NULL;
	if (metas) {
		void **it;
		r_pvector_foreach (metas, it) {
			RIntervalNode *node = *it;
			RAnalMetaItem *mi = node->data;
			if (node->start < from || node->end > to) {
				continue;
			}
			piece = (node->start - from) / step;
			switch (mi->type) {
			case R_META_TYPE_STRING:
				as->block[piece].strings++;
				break;
			case R_META_TYPE_COMMENT:
				as->block[piece].comments++;
				break;
			default:
				break;
			}
		}
		r_pvector_free (metas);
	}
	return as;
}