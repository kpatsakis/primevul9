Mat_PrintNumber(enum matio_types type, void *data)
{
    switch ( type ) {
        case MAT_T_DOUBLE:
            printf("%g",*(double*)data);
            break;
        case MAT_T_SINGLE:
            printf("%g",*(float*)data);
            break;
#ifdef HAVE_MAT_INT64_T
        case MAT_T_INT64:
#if HAVE_INTTYPES_H
            printf("%" PRIi64,*(mat_int64_t*)data);
#elif defined(_MSC_VER) && _MSC_VER >= 1200
            printf("%I64i",*(mat_int64_t*)data);
#elif defined(HAVE_LONG_LONG_INT)
            printf("%lld",(long long)(*(mat_int64_t*)data));
#else
            printf("%ld",(long)(*(mat_int64_t*)data));
#endif
            break;
#endif
#ifdef HAVE_MAT_UINT64_T
        case MAT_T_UINT64:
#if HAVE_INTTYPES_H
            printf("%" PRIu64,*(mat_uint64_t*)data);
#elif defined(_MSC_VER) && _MSC_VER >= 1200
            printf("%I64u",*(mat_uint64_t*)data);
#elif defined(HAVE_UNSIGNED_LONG_LONG_INT)
            printf("%llu",(unsigned long long)(*(mat_uint64_t*)data));
#else
            printf("%lu",(unsigned long)(*(mat_uint64_t*)data));
#endif
            break;
#endif
        case MAT_T_INT32:
            printf("%d",*(mat_int32_t*)data);
            break;
        case MAT_T_UINT32:
            printf("%u",*(mat_uint32_t*)data);
            break;
        case MAT_T_INT16:
            printf("%hd",*(mat_int16_t*)data);
            break;
        case MAT_T_UINT16:
            printf("%hu",*(mat_uint16_t*)data);
            break;
        case MAT_T_INT8:
            printf("%hhd",*(mat_int8_t*)data);
            break;
        case MAT_T_UINT8:
            printf("%hhu",*(mat_uint8_t*)data);
            break;
        default:
            break;
    }
}