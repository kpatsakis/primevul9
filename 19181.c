R_API void r_core_anal_propagate_noreturn(RCore *core, ut64 addr) {
	RList *todo = r_list_newf (free);
	if (!todo) {
		return;
	}

	HtUU *done = ht_uu_new0 ();
	if (!done) {
		r_list_free (todo);
		return;
	}

	RAnalFunction *request_fcn = NULL;
	if (addr != UT64_MAX) {
		request_fcn = r_anal_get_function_at (core->anal, addr);
		if (!request_fcn) {
			r_list_free (todo);
			ht_uu_free (done);
			return;
		}
	}

	// find known noreturn functions to propagate
	RListIter *iter;
	RAnalFunction *f;
	r_list_foreach (core->anal->fcns, iter, f) {
		if (f->is_noreturn) {
			ut64 *n = ut64_new (f->addr);
			r_list_append (todo, n);
		}
	}

	while (!r_list_empty (todo)) {
		ut64 *paddr = (ut64*)r_list_pop (todo);
		ut64 noret_addr = *paddr;
		free (paddr);
		if (r_cons_is_breaked ()) {
			break;
		}
		RList *xrefs = r_anal_xrefs_get (core->anal, noret_addr);
		RAnalRef *xref;
		r_list_foreach (xrefs, iter, xref) {
			RAnalOp *xrefop = r_core_op_anal (core, xref->addr, R_ANAL_OP_MASK_ALL);
			if (!xrefop) {
				eprintf ("Cannot analyze opcode at 0x%08" PFMT64x "\n", xref->addr);
				continue;
			}
			ut64 call_addr = xref->addr;
			ut64 chop_addr = call_addr + xrefop->size;
			r_anal_op_free (xrefop);
			if (xref->type != R_ANAL_REF_TYPE_CALL) {
				continue;
			}

			// Find the block that has an instruction at exactly the xref addr
			RList *blocks = r_anal_get_blocks_in (core->anal, call_addr);
			if (!blocks) {
				continue;
			}
			RAnalBlock *block = NULL;
			RListIter *bit;
			RAnalBlock *block_cur;
			r_list_foreach (blocks, bit, block_cur) {
				if (r_anal_block_op_starts_at (block_cur, call_addr)) {
					block = block_cur;
					break;
				}
			}
			if (block) {
				r_anal_block_ref (block);
			}
			r_list_free (blocks);
			if (!block) {
				continue;
			}

			RList *block_fcns = r_list_clone (block->fcns);
			if (request_fcn) {
				// specific function requested, check if it contains the bb
				if (!r_list_contains (block->fcns, request_fcn)) {
					goto kontinue;
				}
			} else {
				// r_anal_block_chop_noreturn() might free the block!
				block = r_anal_block_chop_noreturn (block, chop_addr);
			}

			RListIter *fit;
			r_list_foreach (block_fcns, fit, f) {
				bool found = ht_uu_find (done, f->addr, NULL) != 0;
				if (f->addr && !found && analyze_noreturn_function (core, f)) {
					f->is_noreturn = true;
					r_anal_noreturn_add (core->anal, NULL, f->addr);
					ut64 *n = malloc (sizeof (ut64));
					*n = f->addr;
					r_list_append (todo, n);
					ht_uu_insert (done, *n, 1);
				}
			}
kontinue:
			if (block) {
				r_anal_block_unref (block);
			}
			r_list_free (block_fcns);
		}
		r_list_free (xrefs);
	}
	r_list_free (todo);
	ht_uu_free (done);
}