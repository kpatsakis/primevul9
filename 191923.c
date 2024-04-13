dump_node(FormatNode *node, int max)
{
	FormatNode *n;
	int			a;

	elog(DEBUG_elog_output, "to_from-char(): DUMP FORMAT");

	for (a = 0, n = node; a <= max; n++, a++)
	{
		if (n->type == NODE_TYPE_ACTION)
			elog(DEBUG_elog_output, "%d:\t NODE_TYPE_ACTION '%s'\t(%s,%s)",
				 a, n->key->name, DUMP_THth(n->suffix), DUMP_FM(n->suffix));
		else if (n->type == NODE_TYPE_CHAR)
			elog(DEBUG_elog_output, "%d:\t NODE_TYPE_CHAR '%c'", a, n->character);
		else if (n->type == NODE_TYPE_END)
		{
			elog(DEBUG_elog_output, "%d:\t NODE_TYPE_END", a);
			return;
		}
		else
			elog(DEBUG_elog_output, "%d:\t unknown NODE!", a);
	}
}