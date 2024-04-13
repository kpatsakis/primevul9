SC_HTMLParser *sc_html_parser_new(FILE *fp, CodeConverter *conv)
{
	SC_HTMLParser *parser;

	cm_return_val_if_fail(fp != NULL, NULL);
	cm_return_val_if_fail(conv != NULL, NULL);

	parser = g_new0(SC_HTMLParser, 1);
	parser->fp = fp;
	parser->conv = conv;
	parser->str = g_string_new(NULL);
	parser->buf = g_string_new(NULL);
	parser->bufp = parser->buf->str;
	parser->state = SC_HTML_NORMAL;
	parser->href = NULL;
	parser->newline = TRUE;
	parser->empty_line = TRUE;
	parser->space = FALSE;
	parser->pre = FALSE;

#define SYMBOL_TABLE_ADD(table, list) \
{ \
	gint i; \
 \
	for (i = 0; i < sizeof(list) / sizeof(list[0]); i++) \
		g_hash_table_insert(table, list[i].key, list[i].val); \
}
#define SYMBOL_TABLE_REF_ADD(table, list) \
{ \
	gint i; \
 \
	for (i = 0; i < sizeof(list) / sizeof(list[0]); i++) \
		g_hash_table_insert(table, &list[i].key, list[i].val); \
}

	if (!default_symbol_table) {
		default_symbol_table =
			g_hash_table_new(g_str_hash, g_str_equal);
		SYMBOL_TABLE_ADD(default_symbol_table, symbol_list);
	}

#undef SYMBOL_TABLE_ADD
#undef SYMBOL_TABLE_REF_ADD

	parser->symbol_table = default_symbol_table;

	return parser;
}