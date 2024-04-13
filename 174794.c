ntlm_set_string (NTLMString *string, int *offset, int len)
{
	string->offset = GUINT16_TO_LE (*offset);
	string->length = string->length2 = GUINT16_TO_LE (len);
	*offset += len;
}