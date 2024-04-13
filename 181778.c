static void agraph_print_node_dot(RANode *n, void *user) {
	char *label = strdup (n->body);
	//label = r_str_replace (label, "\n", "\\l", 1);
	if (!label || !*label) {
		r_cons_printf ("\"%s\" [URL=\"%s\", color=\"lightgray\", label=\"%s\"]\n",
				n->title, n->title, n->title);
	} else {
		r_cons_printf ("\"%s\" [URL=\"%s\", color=\"lightgray\", label=\"%s\\n%s\"]\n",
				n->title, n->title, n->title, label);
	}
	free (label);
}