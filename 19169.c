static void core_anal_color_curr_node(RCore *core, RAnalBlock *bbi) {
	bool color_current = r_config_get_i (core->config, "graph.gv.current");
	char *pal_curr = palColorFor ("graph.current");
	bool current = r_anal_block_contains (bbi, core->offset);

	if (current && color_current) {
		r_cons_printf ("\t\"0x%08"PFMT64x"\" ", bbi->addr);
		r_cons_printf ("\t[fillcolor=%s style=filled shape=box];\n", pal_curr);
	}
	free (pal_curr);
}