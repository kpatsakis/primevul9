Mat_Open(const char *matname,int mode)
{
    FILE *fp = NULL;
    mat_int16_t tmp, tmp2;
    mat_t *mat = NULL;
    size_t bytesread = 0;

    if ( (mode & 0x01) == MAT_ACC_RDONLY ) {
#if defined(_WIN32) && defined(_MSC_VER)
        wchar_t* wname = utf82u(matname);
        if ( NULL != wname ) {
            fp = _wfopen(wname, L"rb");
            free(wname);
        }
#else
        fp = fopen(matname, "rb");
#endif
        if ( !fp )
            return NULL;
    } else if ( (mode & 0x01) == MAT_ACC_RDWR ) {
#if defined(_WIN32) && defined(_MSC_VER)
        wchar_t* wname = utf82u(matname);
        if ( NULL != wname ) {
            fp = _wfopen(wname, L"r+b");
            free(wname);
        }
#else
        fp = fopen(matname, "r+b");
#endif
        if ( !fp ) {
            mat = Mat_CreateVer(matname,NULL,(enum mat_ft)(mode&0xfffffffe));
            return mat;
        }
    } else {
        Mat_Critical("Invalid file open mode");
        return NULL;
    }

    mat = (mat_t*)malloc(sizeof(*mat));
    if ( NULL == mat ) {
        fclose(fp);
        Mat_Critical("Couldn't allocate memory for the MAT file");
        return NULL;
    }

    mat->fp = fp;
    mat->header        = (char*)calloc(128,sizeof(char));
    if ( NULL == mat->header ) {
        free(mat);
        fclose(fp);
        Mat_Critical("Couldn't allocate memory for the MAT file header");
        return NULL;
    }
    mat->subsys_offset = (char*)calloc(8,sizeof(char));
    if ( NULL == mat->subsys_offset ) {
        free(mat->header);
        free(mat);
        fclose(fp);
        Mat_Critical("Couldn't allocate memory for the MAT file subsys offset");
        return NULL;
    }
    mat->filename      = NULL;
    mat->version       = 0;
    mat->byteswap      = 0;
    mat->num_datasets  = 0;
#if defined(MAT73) && MAT73
    mat->refs_id       = -1;
#endif
    mat->dir           = NULL;

    bytesread += fread(mat->header,1,116,fp);
    mat->header[116] = '\0';
    bytesread += fread(mat->subsys_offset,1,8,fp);
    bytesread += 2*fread(&tmp2,2,1,fp);
    bytesread += fread(&tmp,1,2,fp);

    if ( 128 == bytesread ) {
        /* v5 and v7.3 files have at least 128 byte header */
        mat->byteswap = -1;
        if ( tmp == 0x4d49 )
            mat->byteswap = 0;
        else if ( tmp == 0x494d ) {
            mat->byteswap = 1;
            Mat_int16Swap(&tmp2);
        }

        mat->version = (int)tmp2;
        if ( (mat->version == 0x0100 || mat->version == 0x0200) &&
             -1 != mat->byteswap ) {
            mat->bof = ftell((FILE*)mat->fp);
            if ( mat->bof == -1L ) {
                free(mat->header);
                free(mat->subsys_offset);
                free(mat);
                fclose(fp);
                Mat_Critical("Couldn't determine file position");
                return NULL;
            }
            mat->next_index = 0;
        } else {
            mat->version = 0;
        }
    }

    if ( 0 == mat->version ) {
        /* Maybe a V4 MAT file */
        matvar_t *var;

        free(mat->header);
        free(mat->subsys_offset);

        mat->header        = NULL;
        mat->subsys_offset = NULL;
        mat->fp            = fp;
        mat->version       = MAT_FT_MAT4;
        mat->byteswap      = 0;
        mat->mode          = mode;
        mat->bof           = 0;
        mat->next_index    = 0;
#if defined(MAT73) && MAT73
        mat->refs_id       = -1;
#endif

        Mat_Rewind(mat);
        var = Mat_VarReadNextInfo4(mat);
        if ( NULL == var &&
             bytesread != 0 ) { /* Accept 0 bytes files as a valid V4 file */
            /* Does not seem to be a valid V4 file */
            Mat_Close(mat);
            mat = NULL;
            Mat_Critical("\"%s\" does not seem to be a valid MAT file",matname);
        } else {
            Mat_VarFree(var);
            Mat_Rewind(mat);
        }
    }

    if ( NULL == mat )
        return mat;

    mat->filename = strdup_printf("%s",matname);
    mat->mode = mode;

    if ( mat->version == 0x0200 ) {
        fclose((FILE*)mat->fp);
#if defined(MAT73) && MAT73
        mat->fp = malloc(sizeof(hid_t));

        if ( (mode & 0x01) == MAT_ACC_RDONLY )
            *(hid_t*)mat->fp=H5Fopen(matname,H5F_ACC_RDONLY,H5P_DEFAULT);
        else if ( (mode & 0x01) == MAT_ACC_RDWR ) {
            hid_t plist_ap;
            plist_ap = H5Pcreate(H5P_FILE_ACCESS);
#if H5_VERSION_GE(1,10,2)
            H5Pset_libver_bounds(plist_ap,H5F_LIBVER_EARLIEST,H5F_LIBVER_V18);
#endif
            *(hid_t*)mat->fp=H5Fopen(matname,H5F_ACC_RDWR,plist_ap);
            H5Pclose(plist_ap);
        }

        if ( -1 < *(hid_t*)mat->fp ) {
            H5G_info_t group_info;
            memset(&group_info, 0, sizeof(group_info));
            H5Gget_info(*(hid_t*)mat->fp, &group_info);
            mat->num_datasets = (size_t)group_info.nlinks;
            mat->refs_id      = -1;
        }
#else
        mat->fp = NULL;
        Mat_Close(mat);
        mat = NULL;
        Mat_Critical("No HDF5 support which is required to read the v7.3 "
                     "MAT file \"%s\"",matname);
#endif
    }

    return mat;
}