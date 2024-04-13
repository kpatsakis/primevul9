xmlXPathCastNumberToString (double val) {
    xmlChar *ret;
    switch (xmlXPathIsInf(val)) {
    case 1:
	ret = xmlStrdup((const xmlChar *) "Infinity");
	break;
    case -1:
	ret = xmlStrdup((const xmlChar *) "-Infinity");
	break;
    default:
	if (xmlXPathIsNaN(val)) {
	    ret = xmlStrdup((const xmlChar *) "NaN");
	} else if (val == 0) {
            /* Omit sign for negative zero. */
	    ret = xmlStrdup((const xmlChar *) "0");
	} else {
	    /* could be improved */
	    char buf[100];
	    xmlXPathFormatNumber(val, buf, 99);
	    buf[99] = 0;
	    ret = xmlStrdup((const xmlChar *) buf);
	}
    }
    return(ret);
}