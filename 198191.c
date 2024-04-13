xmlRegCheckCharacterRange(xmlRegAtomType type, int codepoint, int neg,
	                  int start, int end, const xmlChar *blockName) {
    int ret = 0;

    switch (type) {
        case XML_REGEXP_STRING:
        case XML_REGEXP_SUBREG:
        case XML_REGEXP_RANGES:
        case XML_REGEXP_EPSILON:
	    return(-1);
        case XML_REGEXP_ANYCHAR:
	    ret = ((codepoint != '\n') && (codepoint != '\r'));
	    break;
        case XML_REGEXP_CHARVAL:
	    ret = ((codepoint >= start) && (codepoint <= end));
	    break;
        case XML_REGEXP_NOTSPACE:
	    neg = !neg;
        case XML_REGEXP_ANYSPACE:
	    ret = ((codepoint == '\n') || (codepoint == '\r') ||
		   (codepoint == '\t') || (codepoint == ' '));
	    break;
        case XML_REGEXP_NOTINITNAME:
	    neg = !neg;
        case XML_REGEXP_INITNAME:
	    ret = (IS_LETTER(codepoint) ||
		   (codepoint == '_') || (codepoint == ':'));
	    break;
        case XML_REGEXP_NOTNAMECHAR:
	    neg = !neg;
        case XML_REGEXP_NAMECHAR:
	    ret = (IS_LETTER(codepoint) || IS_DIGIT(codepoint) ||
		   (codepoint == '.') || (codepoint == '-') ||
		   (codepoint == '_') || (codepoint == ':') ||
		   IS_COMBINING(codepoint) || IS_EXTENDER(codepoint));
	    break;
        case XML_REGEXP_NOTDECIMAL:
	    neg = !neg;
        case XML_REGEXP_DECIMAL:
	    ret = xmlUCSIsCatNd(codepoint);
	    break;
        case XML_REGEXP_REALCHAR:
	    neg = !neg;
        case XML_REGEXP_NOTREALCHAR:
	    ret = xmlUCSIsCatP(codepoint);
	    if (ret == 0)
		ret = xmlUCSIsCatZ(codepoint);
	    if (ret == 0)
		ret = xmlUCSIsCatC(codepoint);
	    break;
        case XML_REGEXP_LETTER:
	    ret = xmlUCSIsCatL(codepoint);
	    break;
        case XML_REGEXP_LETTER_UPPERCASE:
	    ret = xmlUCSIsCatLu(codepoint);
	    break;
        case XML_REGEXP_LETTER_LOWERCASE:
	    ret = xmlUCSIsCatLl(codepoint);
	    break;
        case XML_REGEXP_LETTER_TITLECASE:
	    ret = xmlUCSIsCatLt(codepoint);
	    break;
        case XML_REGEXP_LETTER_MODIFIER:
	    ret = xmlUCSIsCatLm(codepoint);
	    break;
        case XML_REGEXP_LETTER_OTHERS:
	    ret = xmlUCSIsCatLo(codepoint);
	    break;
        case XML_REGEXP_MARK:
	    ret = xmlUCSIsCatM(codepoint);
	    break;
        case XML_REGEXP_MARK_NONSPACING:
	    ret = xmlUCSIsCatMn(codepoint);
	    break;
        case XML_REGEXP_MARK_SPACECOMBINING:
	    ret = xmlUCSIsCatMc(codepoint);
	    break;
        case XML_REGEXP_MARK_ENCLOSING:
	    ret = xmlUCSIsCatMe(codepoint);
	    break;
        case XML_REGEXP_NUMBER:
	    ret = xmlUCSIsCatN(codepoint);
	    break;
        case XML_REGEXP_NUMBER_DECIMAL:
	    ret = xmlUCSIsCatNd(codepoint);
	    break;
        case XML_REGEXP_NUMBER_LETTER:
	    ret = xmlUCSIsCatNl(codepoint);
	    break;
        case XML_REGEXP_NUMBER_OTHERS:
	    ret = xmlUCSIsCatNo(codepoint);
	    break;
        case XML_REGEXP_PUNCT:
	    ret = xmlUCSIsCatP(codepoint);
	    break;
        case XML_REGEXP_PUNCT_CONNECTOR:
	    ret = xmlUCSIsCatPc(codepoint);
	    break;
        case XML_REGEXP_PUNCT_DASH:
	    ret = xmlUCSIsCatPd(codepoint);
	    break;
        case XML_REGEXP_PUNCT_OPEN:
	    ret = xmlUCSIsCatPs(codepoint);
	    break;
        case XML_REGEXP_PUNCT_CLOSE:
	    ret = xmlUCSIsCatPe(codepoint);
	    break;
        case XML_REGEXP_PUNCT_INITQUOTE:
	    ret = xmlUCSIsCatPi(codepoint);
	    break;
        case XML_REGEXP_PUNCT_FINQUOTE:
	    ret = xmlUCSIsCatPf(codepoint);
	    break;
        case XML_REGEXP_PUNCT_OTHERS:
	    ret = xmlUCSIsCatPo(codepoint);
	    break;
        case XML_REGEXP_SEPAR:
	    ret = xmlUCSIsCatZ(codepoint);
	    break;
        case XML_REGEXP_SEPAR_SPACE:
	    ret = xmlUCSIsCatZs(codepoint);
	    break;
        case XML_REGEXP_SEPAR_LINE:
	    ret = xmlUCSIsCatZl(codepoint);
	    break;
        case XML_REGEXP_SEPAR_PARA:
	    ret = xmlUCSIsCatZp(codepoint);
	    break;
        case XML_REGEXP_SYMBOL:
	    ret = xmlUCSIsCatS(codepoint);
	    break;
        case XML_REGEXP_SYMBOL_MATH:
	    ret = xmlUCSIsCatSm(codepoint);
	    break;
        case XML_REGEXP_SYMBOL_CURRENCY:
	    ret = xmlUCSIsCatSc(codepoint);
	    break;
        case XML_REGEXP_SYMBOL_MODIFIER:
	    ret = xmlUCSIsCatSk(codepoint);
	    break;
        case XML_REGEXP_SYMBOL_OTHERS:
	    ret = xmlUCSIsCatSo(codepoint);
	    break;
        case XML_REGEXP_OTHER:
	    ret = xmlUCSIsCatC(codepoint);
	    break;
        case XML_REGEXP_OTHER_CONTROL:
	    ret = xmlUCSIsCatCc(codepoint);
	    break;
        case XML_REGEXP_OTHER_FORMAT:
	    ret = xmlUCSIsCatCf(codepoint);
	    break;
        case XML_REGEXP_OTHER_PRIVATE:
	    ret = xmlUCSIsCatCo(codepoint);
	    break;
        case XML_REGEXP_OTHER_NA:
	    /* ret = xmlUCSIsCatCn(codepoint); */
	    /* Seems it doesn't exist anymore in recent Unicode releases */
	    ret = 0;
	    break;
        case XML_REGEXP_BLOCK_NAME:
	    ret = xmlUCSIsBlock(codepoint, (const char *) blockName);
	    break;
    }
    if (neg)
	return(!ret);
    return(ret);
}