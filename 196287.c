static int32_t keywordIndex(const UnicodeString& tok) {
    for (int32_t i=0; KEYWORDS[i]!=0; ++i) {
        if (tok==KEYWORDS[i]) {
            return i;
        }
    }
    return -1;
}