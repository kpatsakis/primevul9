
static xmlChar *
xmlXPathParseNameComplex(xmlXPathParserContextPtr ctxt, int qualified) {
    xmlChar buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;

    /*
     * Handler for more complex cases
     */
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
        (c == '[') || (c == ']') || (c == '@') || /* accelerators */
        (c == '*') || /* accelerators */
	(!IS_LETTER(c) && (c != '_') &&
         ((qualified) && (c != ':')))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || ((qualified) && (c == ':')) ||
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    /*
	     * Okay someone managed to make a huge name, so he's ready to pay
	     * for the processing speed.
	     */
	    xmlChar *buffer;
	    int max = len * 2;

            if (len > XML_MAX_NAME_LENGTH) {
                XP_ERRORNULL(XPATH_EXPR_ERROR);
            }
	    buffer = (xmlChar *) xmlMallocAtomic(max * sizeof(xmlChar));
	    if (buffer == NULL) {
		XP_ERRORNULL(XPATH_MEMORY_ERROR);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || /* test bigname.xml */
		   (c == '.') || (c == '-') ||
		   (c == '_') || ((qualified) && (c == ':')) ||
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (len + 10 > max) {
                    if (max > XML_MAX_NAME_LENGTH) {
                        XP_ERRORNULL(XPATH_EXPR_ERROR);
                    }
		    max *= 2;
		    buffer = (xmlChar *) xmlRealloc(buffer,
			                            max * sizeof(xmlChar));
		    if (buffer == NULL) {
			XP_ERRORNULL(XPATH_MEMORY_ERROR);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    if (len == 0)
	return(NULL);