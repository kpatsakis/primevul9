xsltNewDecimalFormat(xmlChar *name)
{
    xsltDecimalFormatPtr self;
    /* UTF-8 for 0x2030 */
    static const xmlChar permille[4] = {0xe2, 0x80, 0xb0, 0};

    self = xmlMalloc(sizeof(xsltDecimalFormat));
    if (self != NULL) {
	self->next = NULL;
	self->name = name;
	
	/* Default values */
	self->digit = xmlStrdup(BAD_CAST("#"));
	self->patternSeparator = xmlStrdup(BAD_CAST(";"));
	self->decimalPoint = xmlStrdup(BAD_CAST("."));
	self->grouping = xmlStrdup(BAD_CAST(","));
	self->percent = xmlStrdup(BAD_CAST("%"));
	self->permille = xmlStrdup(BAD_CAST(permille));
	self->zeroDigit = xmlStrdup(BAD_CAST("0"));
	self->minusSign = xmlStrdup(BAD_CAST("-"));
	self->infinity = xmlStrdup(BAD_CAST("Infinity"));
	self->noNumber = xmlStrdup(BAD_CAST("NaN"));
    }
    return self;
}