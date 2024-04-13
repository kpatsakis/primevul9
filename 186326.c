PHPAPI int php_session_valid_key(const char *key) /* {{{ */
{
	size_t len;
	const char *p;
	char c;
	int ret = SUCCESS;

	for (p = key; (c = *p); p++) {
		/* valid characters are a..z,A..Z,0..9 */
		if (!((c >= 'a' && c <= 'z')
				|| (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9')
				|| c == ','
				|| c == '-')) {
			ret = FAILURE;
			break;
		}
	}

	len = p - key;

	/* Somewhat arbitrary length limit here, but should be way more than
	   anyone needs and avoids file-level warnings later on if we exceed MAX_PATH */
	if (len == 0 || len > PS_MAX_SID_LENGTH) {
		ret = FAILURE;
	}

	return ret;
}