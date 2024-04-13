attributeDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar * elem,
                   const xmlChar * name, int type, int def,
                   const xmlChar * defaultValue, xmlEnumerationPtr tree)
{
    callbacks++;
    if (noout)
        return;
    if (defaultValue == NULL)
        fprintf(stdout, "SAX.attributeDecl(%s, %s, %d, %d, NULL, ...)\n",
                elem, name, type, def);
    else
        fprintf(stdout, "SAX.attributeDecl(%s, %s, %d, %d, %s, ...)\n",
                elem, name, type, def, defaultValue);
    xmlFreeEnumeration(tree);
}