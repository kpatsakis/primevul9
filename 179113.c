void sigterm_handler(int s) {
	int parent=0;

	g_hash_table_foreach(children, killchild, &parent);

	if(parent) {
		unlink(pidfname);
	}

	exit(0);
}