GetDataLinear(void *data_in, void *data_out, enum matio_classes class_type,
    enum matio_types data_type, int start, int stride, int edge)
{
    int err = 0;
    size_t data_size = Mat_SizeOf(data_type);

    switch ( class_type ) {
        case MAT_C_DOUBLE:
        {
            double *ptr = (double *)data_out;
            double *ptr_in = (double*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_SINGLE:
        {
            float *ptr = (float *)data_out;
            float *ptr_in = (float*)data_in;
            GET_DATA_LINEAR;
            break;
        }
#ifdef HAVE_MAT_INT64_T
        case MAT_C_INT64:
        {
            mat_int64_t *ptr = (mat_int64_t *)data_out;
            mat_int64_t *ptr_in = (mat_int64_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
#endif /* HAVE_MAT_INT64_T */
#ifdef HAVE_MAT_UINT64_T
        case MAT_C_UINT64:
        {
            mat_uint64_t *ptr = (mat_uint64_t *)data_out;
            mat_uint64_t *ptr_in = (mat_uint64_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
#endif /* HAVE_MAT_UINT64_T */
        case MAT_C_INT32:
        {
            mat_int32_t *ptr = (mat_int32_t *)data_out;
            mat_int32_t *ptr_in = (mat_int32_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_UINT32:
        {
            mat_uint32_t *ptr = (mat_uint32_t *)data_out;
            mat_uint32_t *ptr_in = (mat_uint32_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_INT16:
        {
            mat_int16_t *ptr = (mat_int16_t *)data_out;
            mat_int16_t *ptr_in = (mat_int16_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_UINT16:
        {
            mat_uint16_t *ptr = (mat_uint16_t *)data_out;
            mat_uint16_t *ptr_in = (mat_uint16_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_INT8:
        {
            mat_int8_t *ptr = (mat_int8_t *)data_out;
            mat_int8_t *ptr_in = (mat_int8_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        case MAT_C_UINT8:
        {
            mat_uint8_t *ptr = (mat_uint8_t *)data_out;
            mat_uint8_t *ptr_in = (mat_uint8_t*)data_in;
            GET_DATA_LINEAR;
            break;
        }
        default:
            err = 1;
            break;
    }

    return err;
}