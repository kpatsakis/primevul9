void hexprint_base64(struct crypt_device *cd, json_object *jobj,
		     const char *sep, const char *line_sep)
{
	char *buf = NULL;
	size_t buf_len;
	unsigned int i;

	if (!base64_decode_alloc(json_object_get_string(jobj),
				 json_object_get_string_len(jobj),
				 &buf, &buf_len))
		return;

	for (i = 0; i < buf_len; i++) {
		if (i && !(i % 16))
			log_std(cd, "\n\t%s", line_sep);
		log_std(cd, "%02hhx%s", buf[i], sep);
	}
	log_std(cd, "\n");
	free(buf);
}