Mat_CreateVer(const char *matname,const char *hdr_str,enum mat_ft mat_file_ver)
{
    mat_t *mat;

    switch ( mat_file_ver ) {
        case MAT_FT_MAT4:
            mat = Mat_Create4(matname);
            break;
        case MAT_FT_MAT5:
            mat = Mat_Create5(matname,hdr_str);
            break;
        case MAT_FT_MAT73:
#if defined(MAT73) && MAT73
            mat = Mat_Create73(matname,hdr_str);
#else
            mat = NULL;
#endif
            break;
        default:
            mat = NULL;
            break;
    }

    return mat;
}