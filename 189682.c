void xsltNormalizeCompSteps(void *payload,
        void *data, const xmlChar *name ATTRIBUTE_UNUSED) {
    xsltCompMatchPtr comp = payload;
    xsltStylesheetPtr style = data;
    int ix;

    for (ix = 0; ix < comp->nbStep; ix++) {
        comp->steps[ix].previousExtra += style->extrasNr;
        comp->steps[ix].indexExtra += style->extrasNr;
        comp->steps[ix].lenExtra += style->extrasNr;
    }
}