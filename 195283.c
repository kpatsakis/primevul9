ClassType2DataType(enum matio_classes class_type)
{
    switch ( class_type ) {
        case MAT_C_DOUBLE:
            return MAT_T_DOUBLE;
        case MAT_C_SINGLE:
            return MAT_T_SINGLE;
#ifdef HAVE_MAT_INT64_T
        case MAT_C_INT64:
            return MAT_T_INT64;
#endif
#ifdef HAVE_MAT_UINT64_T
        case MAT_C_UINT64:
            return MAT_T_UINT64;
#endif
        case MAT_C_INT32:
            return MAT_T_INT32;
        case MAT_C_UINT32:
            return MAT_T_UINT32;
        case MAT_C_INT16:
            return MAT_T_INT16;
        case MAT_C_UINT16:
            return MAT_T_UINT16;
        case MAT_C_INT8:
            return MAT_T_INT8;
        case MAT_C_CHAR:
            return MAT_T_UINT8;
        case MAT_C_UINT8:
            return MAT_T_UINT8;
        case MAT_C_CELL:
            return MAT_T_CELL;
        case MAT_C_STRUCT:
            return MAT_T_STRUCT;
        default:
            return MAT_T_UNKNOWN;
    }
}