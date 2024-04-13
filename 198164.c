xmlExpGetLanguageInt(xmlExpCtxtPtr ctxt, xmlExpNodePtr exp,
                     const xmlChar**list, int len, int nb) {
    int tmp, tmp2;
tail:
    switch (exp->type) {
        case XML_EXP_EMPTY:
	    return(0);
        case XML_EXP_ATOM:
	    for (tmp = 0;tmp < nb;tmp++)
	        if (list[tmp] == exp->exp_str)
		    return(0);
            if (nb >= len)
	        return(-2);
	    list[nb] = exp->exp_str;
	    return(1);
        case XML_EXP_COUNT:
	    exp = exp->exp_left;
	    goto tail;
        case XML_EXP_SEQ:
        case XML_EXP_OR:
	    tmp = xmlExpGetLanguageInt(ctxt, exp->exp_left, list, len, nb);
	    if (tmp < 0)
	        return(tmp);
	    tmp2 = xmlExpGetLanguageInt(ctxt, exp->exp_right, list, len,
	                                nb + tmp);
	    if (tmp2 < 0)
	        return(tmp2);
            return(tmp + tmp2);
    }
    return(-1);
}