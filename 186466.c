static void php_libxml_ctx_error_level(int level, void *ctx, const char *msg TSRMLS_DC)
{
	xmlParserCtxtPtr parser;

	parser = (xmlParserCtxtPtr) ctx;

	if (parser != NULL && parser->input != NULL) {
		if (parser->input->filename) {
			php_error_docref(NULL TSRMLS_CC, level, "%s in %s, line: %d", msg, parser->input->filename, parser->input->line);
		} else {
			php_error_docref(NULL TSRMLS_CC, level, "%s in Entity, line: %d", msg, parser->input->line);
		}
	}
}