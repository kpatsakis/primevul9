xmlFACompareAtomTypes(xmlRegAtomType type1, xmlRegAtomType type2) {
    if ((type1 == XML_REGEXP_EPSILON) ||
        (type1 == XML_REGEXP_CHARVAL) ||
	(type1 == XML_REGEXP_RANGES) ||
	(type1 == XML_REGEXP_SUBREG) ||
	(type1 == XML_REGEXP_STRING) ||
	(type1 == XML_REGEXP_ANYCHAR))
	return(1);
    if ((type2 == XML_REGEXP_EPSILON) ||
        (type2 == XML_REGEXP_CHARVAL) ||
	(type2 == XML_REGEXP_RANGES) ||
	(type2 == XML_REGEXP_SUBREG) ||
	(type2 == XML_REGEXP_STRING) ||
	(type2 == XML_REGEXP_ANYCHAR))
	return(1);

    if (type1 == type2) return(1);

    /* simplify subsequent compares by making sure type1 < type2 */
    if (type1 > type2) {
        xmlRegAtomType tmp = type1;
	type1 = type2;
	type2 = tmp;
    }
    switch (type1) {
        case XML_REGEXP_ANYSPACE: /* \s */
	    /* can't be a letter, number, mark, pontuation, symbol */
	    if ((type2 == XML_REGEXP_NOTSPACE) ||
		((type2 >= XML_REGEXP_LETTER) &&
		 (type2 <= XML_REGEXP_LETTER_OTHERS)) ||
	        ((type2 >= XML_REGEXP_NUMBER) &&
		 (type2 <= XML_REGEXP_NUMBER_OTHERS)) ||
	        ((type2 >= XML_REGEXP_MARK) &&
		 (type2 <= XML_REGEXP_MARK_ENCLOSING)) ||
	        ((type2 >= XML_REGEXP_PUNCT) &&
		 (type2 <= XML_REGEXP_PUNCT_OTHERS)) ||
	        ((type2 >= XML_REGEXP_SYMBOL) &&
		 (type2 <= XML_REGEXP_SYMBOL_OTHERS))
	        ) return(0);
	    break;
        case XML_REGEXP_NOTSPACE: /* \S */
	    break;
        case XML_REGEXP_INITNAME: /* \l */
	    /* can't be a number, mark, separator, pontuation, symbol or other */
	    if ((type2 == XML_REGEXP_NOTINITNAME) ||
	        ((type2 >= XML_REGEXP_NUMBER) &&
		 (type2 <= XML_REGEXP_NUMBER_OTHERS)) ||
	        ((type2 >= XML_REGEXP_MARK) &&
		 (type2 <= XML_REGEXP_MARK_ENCLOSING)) ||
	        ((type2 >= XML_REGEXP_SEPAR) &&
		 (type2 <= XML_REGEXP_SEPAR_PARA)) ||
	        ((type2 >= XML_REGEXP_PUNCT) &&
		 (type2 <= XML_REGEXP_PUNCT_OTHERS)) ||
	        ((type2 >= XML_REGEXP_SYMBOL) &&
		 (type2 <= XML_REGEXP_SYMBOL_OTHERS)) ||
	        ((type2 >= XML_REGEXP_OTHER) &&
		 (type2 <= XML_REGEXP_OTHER_NA))
		) return(0);
	    break;
        case XML_REGEXP_NOTINITNAME: /* \L */
	    break;
        case XML_REGEXP_NAMECHAR: /* \c */
	    /* can't be a mark, separator, pontuation, symbol or other */
	    if ((type2 == XML_REGEXP_NOTNAMECHAR) ||
	        ((type2 >= XML_REGEXP_MARK) &&
		 (type2 <= XML_REGEXP_MARK_ENCLOSING)) ||
	        ((type2 >= XML_REGEXP_PUNCT) &&
		 (type2 <= XML_REGEXP_PUNCT_OTHERS)) ||
	        ((type2 >= XML_REGEXP_SEPAR) &&
		 (type2 <= XML_REGEXP_SEPAR_PARA)) ||
	        ((type2 >= XML_REGEXP_SYMBOL) &&
		 (type2 <= XML_REGEXP_SYMBOL_OTHERS)) ||
	        ((type2 >= XML_REGEXP_OTHER) &&
		 (type2 <= XML_REGEXP_OTHER_NA))
		) return(0);
	    break;
        case XML_REGEXP_NOTNAMECHAR: /* \C */
	    break;
        case XML_REGEXP_DECIMAL: /* \d */
	    /* can't be a letter, mark, separator, pontuation, symbol or other */
	    if ((type2 == XML_REGEXP_NOTDECIMAL) ||
	        (type2 == XML_REGEXP_REALCHAR) ||
		((type2 >= XML_REGEXP_LETTER) &&
		 (type2 <= XML_REGEXP_LETTER_OTHERS)) ||
	        ((type2 >= XML_REGEXP_MARK) &&
		 (type2 <= XML_REGEXP_MARK_ENCLOSING)) ||
	        ((type2 >= XML_REGEXP_PUNCT) &&
		 (type2 <= XML_REGEXP_PUNCT_OTHERS)) ||
	        ((type2 >= XML_REGEXP_SEPAR) &&
		 (type2 <= XML_REGEXP_SEPAR_PARA)) ||
	        ((type2 >= XML_REGEXP_SYMBOL) &&
		 (type2 <= XML_REGEXP_SYMBOL_OTHERS)) ||
	        ((type2 >= XML_REGEXP_OTHER) &&
		 (type2 <= XML_REGEXP_OTHER_NA))
		)return(0);
	    break;
        case XML_REGEXP_NOTDECIMAL: /* \D */
	    break;
        case XML_REGEXP_REALCHAR: /* \w */
	    /* can't be a mark, separator, pontuation, symbol or other */
	    if ((type2 == XML_REGEXP_NOTDECIMAL) ||
	        ((type2 >= XML_REGEXP_MARK) &&
		 (type2 <= XML_REGEXP_MARK_ENCLOSING)) ||
	        ((type2 >= XML_REGEXP_PUNCT) &&
		 (type2 <= XML_REGEXP_PUNCT_OTHERS)) ||
	        ((type2 >= XML_REGEXP_SEPAR) &&
		 (type2 <= XML_REGEXP_SEPAR_PARA)) ||
	        ((type2 >= XML_REGEXP_SYMBOL) &&
		 (type2 <= XML_REGEXP_SYMBOL_OTHERS)) ||
	        ((type2 >= XML_REGEXP_OTHER) &&
		 (type2 <= XML_REGEXP_OTHER_NA))
		)return(0);
	    break;
        case XML_REGEXP_NOTREALCHAR: /* \W */
	    break;
	/*
	 * at that point we know both type 1 and type2 are from
	 * character categories are ordered and are different,
	 * it becomes simple because this is a partition
	 */
        case XML_REGEXP_LETTER:
	    if (type2 <= XML_REGEXP_LETTER_OTHERS)
	        return(1);
	    return(0);
        case XML_REGEXP_LETTER_UPPERCASE:
        case XML_REGEXP_LETTER_LOWERCASE:
        case XML_REGEXP_LETTER_TITLECASE:
        case XML_REGEXP_LETTER_MODIFIER:
        case XML_REGEXP_LETTER_OTHERS:
	    return(0);
        case XML_REGEXP_MARK:
	    if (type2 <= XML_REGEXP_MARK_ENCLOSING)
	        return(1);
	    return(0);
        case XML_REGEXP_MARK_NONSPACING:
        case XML_REGEXP_MARK_SPACECOMBINING:
        case XML_REGEXP_MARK_ENCLOSING:
	    return(0);
        case XML_REGEXP_NUMBER:
	    if (type2 <= XML_REGEXP_NUMBER_OTHERS)
	        return(1);
	    return(0);
        case XML_REGEXP_NUMBER_DECIMAL:
        case XML_REGEXP_NUMBER_LETTER:
        case XML_REGEXP_NUMBER_OTHERS:
	    return(0);
        case XML_REGEXP_PUNCT:
	    if (type2 <= XML_REGEXP_PUNCT_OTHERS)
	        return(1);
	    return(0);
        case XML_REGEXP_PUNCT_CONNECTOR:
        case XML_REGEXP_PUNCT_DASH:
        case XML_REGEXP_PUNCT_OPEN:
        case XML_REGEXP_PUNCT_CLOSE:
        case XML_REGEXP_PUNCT_INITQUOTE:
        case XML_REGEXP_PUNCT_FINQUOTE:
        case XML_REGEXP_PUNCT_OTHERS:
	    return(0);
        case XML_REGEXP_SEPAR:
	    if (type2 <= XML_REGEXP_SEPAR_PARA)
	        return(1);
	    return(0);
        case XML_REGEXP_SEPAR_SPACE:
        case XML_REGEXP_SEPAR_LINE:
        case XML_REGEXP_SEPAR_PARA:
	    return(0);
        case XML_REGEXP_SYMBOL:
	    if (type2 <= XML_REGEXP_SYMBOL_OTHERS)
	        return(1);
	    return(0);
        case XML_REGEXP_SYMBOL_MATH:
        case XML_REGEXP_SYMBOL_CURRENCY:
        case XML_REGEXP_SYMBOL_MODIFIER:
        case XML_REGEXP_SYMBOL_OTHERS:
	    return(0);
        case XML_REGEXP_OTHER:
	    if (type2 <= XML_REGEXP_OTHER_NA)
	        return(1);
	    return(0);
        case XML_REGEXP_OTHER_CONTROL:
        case XML_REGEXP_OTHER_FORMAT:
        case XML_REGEXP_OTHER_PRIVATE:
        case XML_REGEXP_OTHER_NA:
	    return(0);
	default:
	    break;
    }
    return(1);
}