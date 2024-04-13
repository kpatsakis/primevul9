static void agraph_print_node_gml(RANode *n, void *user) {
	r_cons_printf ("  node [\n"
		"    id  %d\n"
		"    label  \"%s\"\n"
		"  ]\n", n->gnode->idx, n->title);
}