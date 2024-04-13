Mat_VarDelete(mat_t *mat, const char *name)
{
    int   err = 1;
    char *tmp_name;
    char temp[7] = "XXXXXX";

    if ( NULL == mat || NULL == name )
        return err;

    if ( (tmp_name = mktemp(temp)) != NULL ) {
        enum mat_ft mat_file_ver;
        mat_t *tmp;

        switch ( mat->version ) {
            case 0x0100:
                mat_file_ver = MAT_FT_MAT5;
                break;
            case 0x0200:
                mat_file_ver = MAT_FT_MAT73;
                break;
            case 0x0010:
                mat_file_ver = MAT_FT_MAT4;
                break;
            default:
                mat_file_ver = MAT_FT_DEFAULT;
                break;
        }

        tmp = Mat_CreateVer(tmp_name,mat->header,mat_file_ver);
        if ( tmp != NULL ) {
            matvar_t *matvar;
            char **dir;
            size_t n;

            Mat_Rewind(mat);
            while ( NULL != (matvar = Mat_VarReadNext(mat)) ) {
                if ( 0 != strcmp(matvar->name,name) )
                    Mat_VarWrite(tmp,matvar,matvar->compression);
                else
                    err = 0;
                Mat_VarFree(matvar);
            }
            dir = tmp->dir; /* Keep directory for later assignment */
            tmp->dir = NULL;
            n = tmp->num_datasets;
            Mat_Close(tmp);

            if ( 0 == err ) {
                char *new_name = strdup_printf("%s",mat->filename);
#if defined(MAT73) && MAT73
                if ( mat_file_ver == MAT_FT_MAT73 ) {
                    if ( mat->refs_id > -1 )
                        H5Gclose(mat->refs_id);
                    H5Fclose(*(hid_t*)mat->fp);
                    free(mat->fp);
                    mat->fp = NULL;
                }
#endif
                if ( mat->fp != NULL ) {
                    fclose((FILE*)mat->fp);
                    mat->fp = NULL;
                }

                if ( (err = mat_copy(tmp_name,new_name)) == -1 ) {
                    if ( NULL != dir ) {
                        size_t i;
                        for ( i = 0; i < n; i++ ) {
                            if ( dir[i] )
                                free(dir[i]);
                        }
                        free(dir);
                    }
                    Mat_Critical("Cannot copy file from \"%s\" to \"%s\".",
                        tmp_name, new_name);
                } else if ( (err = remove(tmp_name)) == -1 ) {
                    if ( NULL != dir ) {
                        size_t i;
                        for ( i = 0; i < n; i++ ) {
                            if ( dir[i] )
                                free(dir[i]);
                        }
                        free(dir);
                    }
                    Mat_Critical("Cannot remove file \"%s\".",tmp_name);
                } else {
                    tmp = Mat_Open(new_name,mat->mode);
                    if ( NULL != tmp ) {
                        if ( mat->header )
                            free(mat->header);
                        if ( mat->subsys_offset )
                            free(mat->subsys_offset);
                        if ( mat->filename )
                            free(mat->filename);
                        if ( mat->dir ) {
                            size_t i;
                            for ( i = 0; i < mat->num_datasets; i++ ) {
                                if ( mat->dir[i] )
                                    free(mat->dir[i]);
                            }
                            free(mat->dir);
                        }
                        memcpy(mat,tmp,sizeof(mat_t));
                        free(tmp);
                        mat->num_datasets = n;
                        mat->dir = dir;
                    } else {
                        Mat_Critical("Cannot open file \"%s\".",new_name);
                    }
                }
                free(new_name);
            } else if ( (err = remove(tmp_name)) == -1 ) {
                Mat_Critical("Cannot remove file \"%s\".",tmp_name);
            }
        }
    } else {
        Mat_Critical("Cannot create a unique file name.");
    }

    return err;
}