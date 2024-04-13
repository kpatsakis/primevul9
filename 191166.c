htmlSaveFileEnc(const char *filename, xmlDocPtr cur, const char *encoding) {
    return(htmlSaveFileFormat(filename, cur, encoding, 1));
}