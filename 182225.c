profCallgraphAdd(xsltTemplatePtr templ, xsltTemplatePtr parent)
{
    int i;
  
    if (templ->templMax == 0) {
        templ->templMax = 4;
        templ->templCalledTab =
            (xsltTemplatePtr *) xmlMalloc(templ->templMax *
                                          sizeof(templ->templCalledTab[0]));
        templ->templCountTab =
            (int *) xmlMalloc(templ->templMax *
                                          sizeof(templ->templCountTab[0]));
        if (templ->templCalledTab == NULL || templ->templCountTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "malloc failed !\n");
            return;
        }
    }
    else if (templ->templNr >= templ->templMax) {
        templ->templMax *= 2;
        templ->templCalledTab =
            (xsltTemplatePtr *) xmlRealloc(templ->templCalledTab,
                                           templ->templMax *
                                           sizeof(templ->templCalledTab[0]));
        templ->templCountTab =
            (int *) xmlRealloc(templ->templCountTab,
                                           templ->templMax *
                                           sizeof(templ->templCountTab[0]));
        if (templ->templCalledTab == NULL || templ->templCountTab == NULL) {
            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return;
        }
    }

    for (i = 0; i < templ->templNr; i++) {
        if (templ->templCalledTab[i] == parent) {
            templ->templCountTab[i]++;
            break;
        }
    }
    if (i == templ->templNr) {
        /* not found, add new one */
        templ->templCalledTab[templ->templNr] = parent;
        templ->templCountTab[templ->templNr] = 1;
        templ->templNr++;
    }
}