xsltFreeTemplate(xsltTemplatePtr template) {
    if (template == NULL)
	return;
    if (template->match) xmlFree(template->match);
/*
*   NOTE: @name and @nameURI are put into the string dict now.
*   if (template->name) xmlFree(template->name);
*   if (template->nameURI) xmlFree(template->nameURI);
*/
/*
    if (template->mode) xmlFree(template->mode);
    if (template->modeURI) xmlFree(template->modeURI);
 */
    if (template->inheritedNs) xmlFree(template->inheritedNs);
    memset(template, -1, sizeof(xsltTemplate));
    xmlFree(template);
}