htmlSaveErr(int code, xmlNodePtr node, const char *extra)
{
    const char *msg = NULL;

    switch(code) {
        case XML_SAVE_NOT_UTF8:
	    msg = "string is not in UTF-8\n";
	    break;
	case XML_SAVE_CHAR_INVALID:
	    msg = "invalid character value\n";
	    break;
	case XML_SAVE_UNKNOWN_ENCODING:
	    msg = "unknown encoding %s\n";
	    break;
	case XML_SAVE_NO_DOCTYPE:
	    msg = "HTML has no DOCTYPE\n";
	    break;
	default:
	    msg = "unexpected error number\n";
    }
    __xmlSimpleError(XML_FROM_OUTPUT, code, node, msg, extra);
}