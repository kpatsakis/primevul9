 */
static void _php_make_header_object(zval *myzvalue, ENVELOPE *en)
{
	zval paddress;
	zend_string *fulladdress=NULL;

	object_init(myzvalue);

	if (en->remail) add_property_string(myzvalue, "remail", en->remail);
	if (en->date) add_property_string(myzvalue, "date", (char*)en->date);
	if (en->date) add_property_string(myzvalue, "Date", (char*)en->date);
	if (en->subject) add_property_string(myzvalue, "subject", en->subject);
	if (en->subject) add_property_string(myzvalue, "Subject", en->subject);
	if (en->in_reply_to) add_property_string(myzvalue, "in_reply_to", en->in_reply_to);
	if (en->message_id) add_property_string(myzvalue, "message_id", en->message_id);
	if (en->newsgroups) add_property_string(myzvalue, "newsgroups", en->newsgroups);
	if (en->followup_to) add_property_string(myzvalue, "followup_to", en->followup_to);
	if (en->references) add_property_string(myzvalue, "references", en->references);

	if (en->to) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->to, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "toaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "to", &paddress);
	}

	if (en->from) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->from, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "fromaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "from", &paddress);
	}

	if (en->cc) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->cc, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "ccaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "cc", &paddress);
	}

	if (en->bcc) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->bcc, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "bccaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "bcc", &paddress);
	}

	if (en->reply_to) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->reply_to, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "reply_toaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "reply_to", &paddress);
	}

	if (en->sender) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->sender, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "senderaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "sender", &paddress);
	}

	if (en->return_path) {
		array_init(&paddress);
		fulladdress = _php_imap_parse_address(en->return_path, &paddress);
		if (fulladdress) {
			add_property_str(myzvalue, "return_pathaddress", fulladdress);
		}
		add_assoc_object(myzvalue, "return_path", &paddress);
	}