htmlNodeDumpFile(FILE *out, xmlDocPtr doc, xmlNodePtr cur) {
    htmlNodeDumpFileFormat(out, doc, cur, NULL, 1);
}