static void php_session_remove_cookie(void) {
	sapi_header_struct *header;
	zend_llist *l = &SG(sapi_headers).headers;
	zend_llist_element *next;
	zend_llist_element *current;
	char *session_cookie;
	zend_string *e_session_name;
	size_t session_cookie_len;
	size_t len = sizeof("Set-Cookie")-1;

	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)));
	spprintf(&session_cookie, 0, "Set-Cookie: %s=", ZSTR_VAL(e_session_name));
	zend_string_free(e_session_name);

	session_cookie_len = strlen(session_cookie);
	current = l->head;
	while (current) {
		header = (sapi_header_struct *)(current->data);
		next = current->next;
		if (header->header_len > len && header->header[len] == ':'
			&& !strncmp(header->header, session_cookie, session_cookie_len)) {
			if (current->prev) {
				current->prev->next = next;
			} else {
				l->head = next;
			}
			if (next) {
				next->prev = current->prev;
			} else {
				l->tail = current->prev;
			}
			sapi_free_header(header);
			efree(current);
			--l->count;
		}
		current = next;
	}
	efree(session_cookie);
}