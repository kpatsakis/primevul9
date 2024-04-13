Mat_VarReadData(mat_t *mat,matvar_t *matvar,void *data,
      int *start,int *stride,int *edge)
{
    int err = 0;

    switch ( matvar->class_type ) {
        case MAT_C_DOUBLE:
        case MAT_C_SINGLE:
        case MAT_C_INT64:
        case MAT_C_UINT64:
        case MAT_C_INT32:
        case MAT_C_UINT32:
        case MAT_C_INT16:
        case MAT_C_UINT16:
        case MAT_C_INT8:
        case MAT_C_UINT8:
            break;
        default:
            return -1;
    }

    switch ( mat->version ) {
        case MAT_FT_MAT5:
            err = Mat_VarReadData5(mat,matvar,data,start,stride,edge);
            break;
        case MAT_FT_MAT73:
#if defined(MAT73) && MAT73
            err = Mat_VarReadData73(mat,matvar,data,start,stride,edge);
#else
            err = 1;
#endif
            break;
        case MAT_FT_MAT4:
            err = Mat_VarReadData4(mat,matvar,data,start,stride,edge);
            break;
        default:
            err = 2;
            break;
    }

    return err;
}