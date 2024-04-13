static void shownested() {
	int i;
	eprintf ("[[[NESTED %d]]] ", context);
	for (i = 0; egg->lang.nested[i]; i++) {
		eprintf ("%s ", egg->lang.nested[i]);
	}
	eprintf ("\n");
}