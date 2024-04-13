GIT_INLINE(bool) verify_char(unsigned char c, unsigned int flags)
{
	if ((flags & GIT_PATH_REJECT_BACKSLASH) && c == '\\')
		return false;

	if ((flags & GIT_PATH_REJECT_SLASH) && c == '/')
		return false;

	if (flags & GIT_PATH_REJECT_NT_CHARS) {
		if (c < 32)
			return false;

		switch (c) {
		case '<':
		case '>':
		case ':':
		case '"':
		case '|':
		case '?':
		case '*':
			return false;
		}
	}

	return true;
}