xsltFreeDecimalFormatList(xsltStylesheetPtr self)
{
    xsltDecimalFormatPtr iter;
    xsltDecimalFormatPtr tmp;

    if (self == NULL)
	return;
    
    iter = self->decimalFormat;
    while (iter != NULL) {
	tmp = iter->next;
	xsltFreeDecimalFormat(iter);
	iter = tmp;
    }
}