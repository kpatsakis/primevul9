xmlExpDump(xmlBufferPtr buf, xmlExpNodePtr expr) {
    if ((buf == NULL) || (expr == NULL))
        return;
    xmlExpDumpInt(buf, expr, 0);
}