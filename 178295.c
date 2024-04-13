static struct cache_entry *cache_check(gpointer request, int *qtype, int proto)
{
	char *question;
	struct cache_entry *entry;
	struct domain_question *q;
	uint16_t type;
	int offset, proto_offset;

	if (!request)
		return NULL;

	proto_offset = protocol_offset(proto);
	if (proto_offset < 0)
		return NULL;

	question = request + proto_offset + 12;

	offset = strlen(question) + 1;
	q = (void *) (question + offset);
	type = ntohs(q->type);

	/* We only cache either A (1) or AAAA (28) requests */
	if (type != 1 && type != 28)
		return NULL;

	if (!cache) {
		create_cache();
		return NULL;
	}

	entry = g_hash_table_lookup(cache, question);
	if (!entry)
		return NULL;

	type = cache_check_validity(question, type, entry);
	if (type == 0)
		return NULL;

	*qtype = type;
	return entry;
}