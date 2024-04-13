static void agraph_print_node(RANode *n, void *user) {
	char *encbody, *cmd;
	int len = strlen (n->body);

	if (len > 0 && n->body[len - 1] == '\n') {
		len--;
	}
	encbody = r_base64_encode_dyn (n->body, len);
	cmd = r_str_newf ("agn \"%s\" base64:%s\n", n->title, encbody);
	r_cons_printf (cmd);
	free (cmd);
	free (encbody);
}