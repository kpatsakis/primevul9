sd_markdown_new(
	unsigned int extensions,
	size_t max_nesting,
	const struct sd_callbacks *callbacks,
	void *opaque)
{
	struct sd_markdown *md = NULL;

	assert(max_nesting > 0 && callbacks);

	md = malloc(sizeof(struct sd_markdown));
	if (!md)
		return NULL;

	memcpy(&md->cb, callbacks, sizeof(struct sd_callbacks));

	redcarpet_stack_init(&md->work_bufs[BUFFER_BLOCK], 4);
	redcarpet_stack_init(&md->work_bufs[BUFFER_SPAN], 8);

	memset(md->active_char, 0x0, 256);

	if (md->cb.emphasis || md->cb.double_emphasis || md->cb.triple_emphasis) {
		md->active_char['*'] = MD_CHAR_EMPHASIS;
		md->active_char['_'] = MD_CHAR_EMPHASIS;
		if (extensions & MKDEXT_STRIKETHROUGH)
			md->active_char['~'] = MD_CHAR_EMPHASIS;
		if (extensions & MKDEXT_HIGHLIGHT)
			md->active_char['='] = MD_CHAR_EMPHASIS;
	}

	if (md->cb.codespan)
		md->active_char['`'] = MD_CHAR_CODESPAN;

	if (md->cb.linebreak)
		md->active_char['\n'] = MD_CHAR_LINEBREAK;

	if (md->cb.image || md->cb.link)
		md->active_char['['] = MD_CHAR_LINK;

	md->active_char['<'] = MD_CHAR_LANGLE;
	md->active_char['\\'] = MD_CHAR_ESCAPE;
	md->active_char['&'] = MD_CHAR_ENTITITY;

	if (extensions & MKDEXT_AUTOLINK) {
		md->active_char[':'] = MD_CHAR_AUTOLINK_URL;
		md->active_char['@'] = MD_CHAR_AUTOLINK_EMAIL;
		md->active_char['w'] = MD_CHAR_AUTOLINK_WWW;
	}

	if (extensions & MKDEXT_SUPERSCRIPT)
		md->active_char['^'] = MD_CHAR_SUPERSCRIPT;

	if (extensions & MKDEXT_QUOTE)
		md->active_char['"'] = MD_CHAR_QUOTE;

	/* Extension data */
	md->ext_flags = extensions;
	md->opaque = opaque;
	md->max_nesting = max_nesting;
	md->in_link_body = 0;

	return md;
}