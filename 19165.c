static void print_hint_tree(RBTree tree, int mode) {
#define END_ADDR if (mode == 'j') { pj_end (pj); } else if (mode != '*') { r_cons_newline (); }
	PJ *pj = NULL;
	if (mode == 'j') {
		pj = pj_new ();
		pj_a (pj);
	}
	RBIter it;
	HintNode *node;
	ut64 last_addr = 0;
	bool in_addr = false;
	r_rbtree_foreach (tree, it, node, HintNode, rb) {
		if (!in_addr || last_addr != node->addr) {
			if (in_addr) {
				END_ADDR
			}
			in_addr = true;
			last_addr = node->addr;
			if (pj) {
				pj_o (pj);
				pj_kn (pj, "addr", node->addr);
			} else if (mode != '*') {
				r_cons_printf (" 0x%08"PFMT64x" =>", node->addr);
			}
		}
		hint_node_print (node, mode, pj);
	}
	if (in_addr) {
		END_ADDR
	}
	if (pj) {
		pj_end (pj);
		r_cons_printf ("%s\n", pj_string (pj));
		pj_free (pj);
	}
#undef END_ADDR
}