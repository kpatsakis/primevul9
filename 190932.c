   Read an overview of the information in the headers of the given message sequence */
PHP_FUNCTION(imap_fetch_overview)
{
	zval *streamind;
	zend_string *sequence;
	pils *imap_le_struct;
	zval myoverview;
	zend_string *address;
	zend_long status, flags = 0L;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "rS|l", &streamind, &sequence, &flags) == FAILURE) {
		return;
	}

	if (flags && ((flags & ~FT_UID) != 0)) {
		php_error_docref(NULL, E_WARNING, "invalid value for the options parameter");
		RETURN_FALSE;
	}

	if ((imap_le_struct = (pils *)zend_fetch_resource(Z_RES_P(streamind), "imap", le_imap)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	status = (flags & FT_UID)
		? mail_uid_sequence(imap_le_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence))
		: mail_sequence(imap_le_struct->imap_stream, (unsigned char*)ZSTR_VAL(sequence));

	if (status) {
		MESSAGECACHE *elt;
		ENVELOPE *env;
		unsigned long i;

		for (i = 1; i <= imap_le_struct->imap_stream->nmsgs; i++) {
			if (((elt = mail_elt (imap_le_struct->imap_stream, i))->sequence) &&
				(env = mail_fetch_structure (imap_le_struct->imap_stream, i, NIL, NIL))) {
				object_init(&myoverview);
				if (env->subject) {
					add_property_string(&myoverview, "subject", env->subject);
				}
				if (env->from) {
					env->from->next=NULL;
					address =_php_rfc822_write_address(env->from);
					if (address) {
						add_property_str(&myoverview, "from", address);
					}
				}
				if (env->to) {
					env->to->next = NULL;
					address = _php_rfc822_write_address(env->to);
					if (address) {
						add_property_str(&myoverview, "to", address);
					}
				}
				if (env->date) {
					add_property_string(&myoverview, "date", (char*)env->date);
				}
				if (env->message_id) {
					add_property_string(&myoverview, "message_id", env->message_id);
				}
				if (env->references) {
					add_property_string(&myoverview, "references", env->references);
				}
				if (env->in_reply_to) {
					add_property_string(&myoverview, "in_reply_to", env->in_reply_to);
				}
				add_property_long(&myoverview, "size", elt->rfc822_size);
				add_property_long(&myoverview, "uid", mail_uid(imap_le_struct->imap_stream, i));
				add_property_long(&myoverview, "msgno", i);
				add_property_long(&myoverview, "recent", elt->recent);
				add_property_long(&myoverview, "flagged", elt->flagged);
				add_property_long(&myoverview, "answered", elt->answered);
				add_property_long(&myoverview, "deleted", elt->deleted);
				add_property_long(&myoverview, "seen", elt->seen);
				add_property_long(&myoverview, "draft", elt->draft);
				add_property_long(&myoverview, "udate", mail_longdate(elt));
				add_next_index_object(return_value, &myoverview);
			}
		}
	}