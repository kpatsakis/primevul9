from_char_parse_int(int *dest, char **src, FormatNode *node)
{
	return from_char_parse_int_len(dest, src, node->key->len, node);
}