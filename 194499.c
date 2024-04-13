parse_regexp(const char *text) {
    const char	*te;
    int		options = 0;
	    
    te = text + strlen(text) - 1;
#if HAS_ONIG
    for (; text < te && '/' != *te; te--) {
	switch (*te) {
	case 'i':	options |= ONIG_OPTION_IGNORECASE;	break;
	case 'm':	options |= ONIG_OPTION_MULTILINE;	break;
	case 'x':	options |= ONIG_OPTION_EXTEND;		break;
	default:						break;
	}
    }
#endif
    return rb_reg_new(text + 1, te - text - 1, options);
}