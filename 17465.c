bool DL_Dxf::stripWhiteSpace(char** s, bool stripSpace) {
    // last non-NULL char:
    int lastChar = strlen(*s) - 1;

    // Is last character CR or LF?
    while ( (lastChar >= 0) &&
            (((*s)[lastChar] == 10) || ((*s)[lastChar] == 13) ||
             (stripSpace && ((*s)[lastChar] == ' ' || ((*s)[lastChar] == '\t')))) ) {
        (*s)[lastChar] = '\0';
        lastChar--;
    }

    // Skip whitespace, excluding \n, at beginning of line
    if (stripSpace) {
        while ((*s)[0]==' ' || (*s)[0]=='\t') {
            ++(*s);
        }
    }
    
    return ((*s) ? true : false);
}