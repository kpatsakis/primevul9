mat_copy(const char* src, const char* dst)
{
    size_t len;
    char buf[BUFSIZ] = {'\0'};
    FILE* in = NULL;
    FILE* out = NULL;

#if defined(_WIN32) && defined(_MSC_VER)
    {
        wchar_t* wname = utf82u(src);
        if ( NULL != wname ) {
            in = _wfopen(wname, L"rb");
            free(wname);
        }
    }
#else
    in = fopen(src, "rb");
#endif
    if ( in == NULL ) {
        Mat_Critical("Cannot open file \"%s\" for reading.", src);
        return -1;
    }

#if defined(_WIN32) && defined(_MSC_VER)
    {
        wchar_t* wname = utf82u(dst);
        if ( NULL != wname ) {
            out = _wfopen(wname, L"wb");
            free(wname);
        }
    }
#else
    out = fopen(dst, "wb");
#endif
    if ( out == NULL ) {
        fclose(in);
        Mat_Critical("Cannot open file \"%s\" for writing.", dst);
        return -1;
    }

    while ( (len = fread(buf, sizeof(char), BUFSIZ, in)) > 0 ) {
        if ( len != fwrite(buf, sizeof(char), len, out) ) {
            fclose(in);
            fclose(out);
            Mat_Critical("Error writing to file \"%s\".", dst);
            return -1;
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}