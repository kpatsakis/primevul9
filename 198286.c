GetDataSlab(void *data_in, void *data_out, enum matio_classes class_type,
    enum matio_types data_type, size_t *dims, int *start, int *stride, int *edge,
    int rank, size_t nbytes)
{
    int err = 0;
    int same_type = 0;
    if (( class_type == MAT_C_DOUBLE && data_type == MAT_T_DOUBLE ) ||
        ( class_type == MAT_C_SINGLE && data_type == MAT_T_SINGLE ) ||
        ( class_type == MAT_C_INT16 && data_type == MAT_T_INT16 ) ||
        ( class_type == MAT_C_INT32 && data_type == MAT_T_INT32 ) ||
        ( class_type == MAT_C_INT64 && data_type == MAT_T_INT64 ) ||
        ( class_type == MAT_C_INT8 && data_type == MAT_T_INT8 ) ||
        ( class_type == MAT_C_UINT16 && data_type == MAT_T_UINT16 ) ||
        ( class_type == MAT_C_UINT32 && data_type == MAT_T_UINT32 ) ||
        ( class_type == MAT_C_UINT64 && data_type == MAT_T_UINT64 ) ||
        ( class_type == MAT_C_UINT8 && data_type == MAT_T_UINT8 ))
        same_type = 1;

    if ( rank == 2 ) {
        if ( (size_t)stride[0]*(edge[0]-1)+start[0]+1 > dims[0] )
            err = 1;
        else if ( (size_t)stride[1]*(edge[1]-1)+start[1]+1 > dims[1] )
            err = 1;
        else if ( ( stride[0] == 1 && edge[0] == dims[0] ) &&
                  ( stride[1] == 1 ) && ( same_type == 1 ) )
            memcpy(data_out, data_in, nbytes);
        else {
            int i, j;

            switch ( class_type ) {
                case MAT_C_DOUBLE:
                {
                    double *ptr = (double *)data_out;
                    GET_DATA_SLAB2_TYPE(double);
                    break;
                }
                case MAT_C_SINGLE:
                {
                    float *ptr = (float *)data_out;
                    GET_DATA_SLAB2_TYPE(float);
                    break;
                }
#ifdef HAVE_MAT_INT64_T
                case MAT_C_INT64:
                {
                    mat_int64_t *ptr = (mat_int64_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_int64_t);
                    break;
                }
#endif /* HAVE_MAT_INT64_T */
#ifdef HAVE_MAT_UINT64_T
                case MAT_C_UINT64:
                {
                    mat_uint64_t *ptr = (mat_uint64_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_uint64_t);
                    break;
                }
#endif /* HAVE_MAT_UINT64_T */
                case MAT_C_INT32:
                {
                    mat_int32_t *ptr = (mat_int32_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_int32_t);
                    break;
                }
                case MAT_C_UINT32:
                {
                    mat_uint32_t *ptr = (mat_uint32_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_uint32_t);
                    break;
                }
                case MAT_C_INT16:
                {
                    mat_int16_t *ptr = (mat_int16_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_int16_t);
                    break;
                }
                case MAT_C_UINT16:
                {
                    mat_uint16_t *ptr = (mat_uint16_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_uint16_t);
                    break;
                }
                case MAT_C_INT8:
                {
                    mat_int8_t *ptr = (mat_int8_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_int8_t);
                    break;
                }
                case MAT_C_UINT8:
                {
                    mat_uint8_t *ptr = (mat_uint8_t *)data_out;
                    GET_DATA_SLAB2_TYPE(mat_uint8_t);
                    break;
                }
                default:
                    err = 1;
                    break;
            }
        }
    } else {
        int i, j, N, I = 0;
        int inc[10] = {0,}, cnt[10] = {0,}, dimp[10] = {0,};

        switch ( class_type ) {
            case MAT_C_DOUBLE:
            {
                double *ptr = (double *)data_out;
                GET_DATA_SLABN_TYPE(double);
                break;
            }
            case MAT_C_SINGLE:
            {
                float *ptr = (float *)data_out;
                GET_DATA_SLABN_TYPE(float);
                break;
            }
#ifdef HAVE_MAT_INT64_T
            case MAT_C_INT64:
            {
                mat_int64_t *ptr = (mat_int64_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_int64_t);
                break;
            }
#endif /* HAVE_MAT_INT64_T */
#ifdef HAVE_MAT_UINT64_T
            case MAT_C_UINT64:
            {
                mat_uint64_t *ptr = (mat_uint64_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_uint64_t);
                break;
            }
#endif /* HAVE_MAT_UINT64_T */
            case MAT_C_INT32:
            {
                mat_int32_t *ptr = (mat_int32_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_int32_t);
                break;
            }
            case MAT_C_UINT32:
            {
                mat_uint32_t *ptr = (mat_uint32_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_uint32_t);
                break;
            }
            case MAT_C_INT16:
            {
                mat_int16_t *ptr = (mat_int16_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_int16_t);
                break;
            }
            case MAT_C_UINT16:
            {
                mat_uint16_t *ptr = (mat_uint16_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_uint16_t);
                break;
            }
            case MAT_C_INT8:
            {
                mat_int8_t *ptr = (mat_int8_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_int8_t);
                break;
            }
            case MAT_C_UINT8:
            {
                mat_uint8_t *ptr = (mat_uint8_t *)data_out;
                GET_DATA_SLABN_TYPE(mat_uint8_t);
                break;
            }
            default:
                err = 1;
                break;
        }
    }

    return err;
}