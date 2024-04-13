Mat_VarPrint( matvar_t *matvar, int printdata )
{
    size_t nelems = 0, i, j;
    const char *class_type_desc[18] = {"Undefined","Cell Array","Structure",
       "Object","Character Array","Sparse Array","Double Precision Array",
       "Single Precision Array", "8-bit, signed integer array",
       "8-bit, unsigned integer array","16-bit, signed integer array",
       "16-bit, unsigned integer array","32-bit, signed integer array",
       "32-bit, unsigned integer array","64-bit, signed integer array",
       "64-bit, unsigned integer array","Function","Opaque"};

    if ( matvar == NULL )
        return;
    if ( NULL != matvar->name )
        printf("      Name: %s\n", matvar->name);
    printf("      Rank: %d\n", matvar->rank);
    if ( matvar->rank <= 0 )
        return;
    if ( NULL != matvar->dims ) {
        int k;
        nelems = 1;
        SafeMulDims(matvar, &nelems);
        printf("Dimensions: %" SIZE_T_FMTSTR,matvar->dims[0]);
        for ( k = 1; k < matvar->rank; k++ ) {
            printf(" x %" SIZE_T_FMTSTR,matvar->dims[k]);
        }
        printf("\n");
    }
    printf("Class Type: %s",class_type_desc[matvar->class_type]);
    if ( matvar->isComplex )
        printf(" (complex)");
    else if ( matvar->isLogical )
        printf(" (logical)");
    printf("\n");
    if ( matvar->data_type ) {
        const char *data_type_desc[25] = {"Unknown","8-bit, signed integer",
           "8-bit, unsigned integer","16-bit, signed integer",
           "16-bit, unsigned integer","32-bit, signed integer",
           "32-bit, unsigned integer","IEEE 754 single-precision","RESERVED",
           "IEEE 754 double-precision","RESERVED","RESERVED",
           "64-bit, signed integer","64-bit, unsigned integer", "Matlab Array",
           "Compressed Data","Unicode UTF-8 Encoded Character Data",
           "Unicode UTF-16 Encoded Character Data",
           "Unicode UTF-32 Encoded Character Data","RESERVED","String","Cell Array",
           "Structure","Array","Function"};
        printf(" Data Type: %s\n", data_type_desc[matvar->data_type]);
    }

    if ( MAT_C_STRUCT == matvar->class_type ) {
        matvar_t **fields = (matvar_t **)matvar->data;
        size_t nfields = matvar->internal->num_fields;
        size_t nelems_x_nfields = 1;
        SafeMul(&nelems_x_nfields, nelems, nfields);
        if ( nelems_x_nfields > 0 ) {
            printf("Fields[%" SIZE_T_FMTSTR "] {\n", nelems_x_nfields);
            for ( i = 0; i < nelems_x_nfields; i++ ) {
                if ( NULL == fields[i] ) {
                    printf("      Name: %s\n      Rank: %d\n",
                           matvar->internal->fieldnames[i%nfields],0);
                } else {
                    Mat_VarPrint(fields[i],printdata);
                }
            }
            printf("}\n");
        } else {
            printf("Fields[%" SIZE_T_FMTSTR "] {\n", nfields);
            for ( i = 0; i < nfields; i++ )
                printf("      Name: %s\n      Rank: %d\n",
                       matvar->internal->fieldnames[i],0);
            printf("}\n");
        }
        return;
    } else if ( matvar->data == NULL || matvar->data_size < 1 ) {
        if ( printdata )
            printf("{\n}\n");
        return;
    } else if ( MAT_C_CELL == matvar->class_type ) {
        matvar_t **cells = (matvar_t **)matvar->data;
        nelems = matvar->nbytes / matvar->data_size;
        printf("{\n");
        for ( i = 0; i < nelems; i++ )
            Mat_VarPrint(cells[i],printdata);
        printf("}\n");
        return;
    } else if ( !printdata ) {
        return;
    }

    printf("{\n");

    if ( matvar->rank > 2 ) {
        printf("I can't print more than 2 dimensions\n");
    } else if ( matvar->rank == 1 && NULL != matvar->dims && matvar->dims[0] > 15 ) {
        printf("I won't print more than 15 elements in a vector\n");
    } else if ( matvar->rank == 2 && NULL != matvar->dims ) {
        switch( matvar->class_type ) {
            case MAT_C_DOUBLE:
            case MAT_C_SINGLE:
#ifdef HAVE_MAT_INT64_T
            case MAT_C_INT64:
#endif
#ifdef HAVE_MAT_UINT64_T
            case MAT_C_UINT64:
#endif
            case MAT_C_INT32:
            case MAT_C_UINT32:
            case MAT_C_INT16:
            case MAT_C_UINT16:
            case MAT_C_INT8:
            case MAT_C_UINT8:
            {
                size_t stride = Mat_SizeOf(matvar->data_type);
                if ( matvar->isComplex ) {
                    mat_complex_split_t *complex_data = (mat_complex_split_t*)matvar->data;
                    char *rp = (char*)complex_data->Re;
                    char *ip = (char*)complex_data->Im;
                    for ( i = 0; i < matvar->dims[0] && i < 15; i++ ) {
                        for ( j = 0; j < matvar->dims[1] && j < 15; j++ ) {
                            size_t idx = matvar->dims[0]*j+i;
                            Mat_PrintNumber(matvar->data_type,rp+idx*stride);
                            printf(" + ");
                            Mat_PrintNumber(matvar->data_type,ip+idx*stride);
                            printf("i ");
                        }
                        if ( j < matvar->dims[1] )
                            printf("...");
                        printf("\n");
                    }
                    if ( i < matvar->dims[0] )
                        printf(".\n.\n.\n");
               } else {
                   char *data = (char*)matvar->data;
                   for ( i = 0; i < matvar->dims[0] && i < 15; i++ ) {
                        for ( j = 0; j < matvar->dims[1] && j < 15; j++ ) {
                            size_t idx = matvar->dims[0]*j+i;
                            Mat_PrintNumber(matvar->data_type,
                                            data+idx*stride);
                            printf(" ");
                        }
                        if ( j < matvar->dims[1] )
                            printf("...");
                        printf("\n");
                    }
                    if ( i < matvar->dims[0] )
                        printf(".\n.\n.\n");
                }
                break;
            }
            case MAT_C_CHAR:
            {
                switch ( matvar->data_type ) {
                    case MAT_T_UINT16:
                    case MAT_T_UTF16:
                    {
                        const mat_uint16_t *data = (const mat_uint16_t*)matvar->data;
                        for ( i = 0; i < matvar->dims[0]; i++ ) {
                            for ( j = 0; j < matvar->dims[1]; j++ ) {
                                const mat_uint16_t c = data[j*matvar->dims[0]+i];
#if defined VARPRINT_UTF16
                                printf("%c%c", c & 0xFF, (c>>8) & 0xFF);
#elif defined VARPRINT_UTF16_DECIMAL
                                Mat_PrintNumber(MAT_T_UINT16, &c);
                                printf(" ");
#else
                                /* Convert to UTF-8 */
                                if (c <= 0x7F) {
                                    printf("%c", c);
                                }
                                else if (c <= 0x7FF) {
                                    printf("%c%c", 0xC0 | (c>>6), 0x80 | (c & 0x3F));
                                }
                                else /* if (c <= 0xFFFF) */ {
                                    printf("%c%c%c", 0xE0 | (c>>12), 0x80 | ((c>>6) & 0x3F), 0x80 | (c & 0x3F));
                                }
#endif
                            }
                            printf("\n");
                        }
                        break;
                    }
                    default:
                    {
                        const char *data = (const char*)matvar->data;
                        for ( i = 0; i < matvar->dims[0]; i++ ) {
                            for ( j = 0; j < matvar->dims[1]; j++ )
                                printf("%c",data[j*matvar->dims[0]+i]);
                            printf("\n");
                        }
                        break;
                    }
                }
                break;
            }
            case MAT_C_SPARSE:
            {
                mat_sparse_t *sparse;
                size_t stride = Mat_SizeOf(matvar->data_type);
#if !defined(EXTENDED_SPARSE)
                if ( MAT_T_DOUBLE != matvar->data_type )
                    break;
#endif
                sparse = (mat_sparse_t*)matvar->data;
                if ( matvar->isComplex ) {
                    mat_complex_split_t *complex_data = (mat_complex_split_t*)sparse->data;
                    char *re = (char*)complex_data->Re;
                    char *im = (char*)complex_data->Im;
                    for ( i = 0; i < (size_t)sparse->njc-1; i++ ) {
                        for ( j = sparse->jc[i];
                              j < (size_t)sparse->jc[i+1] && j < (size_t)sparse->ndata; j++ ) {
                            printf("    (%d,%" SIZE_T_FMTSTR ")  ",sparse->ir[j]+1,i+1);
                            Mat_PrintNumber(matvar->data_type,re+j*stride);
                            printf(" + ");
                            Mat_PrintNumber(matvar->data_type,im+j*stride);
                            printf("i\n");
                        }
                    }
                } else {
                    char *data = (char*)sparse->data;
                    for ( i = 0; i < (size_t)sparse->njc-1; i++ ) {
                        for ( j = sparse->jc[i];
                              j < (size_t)sparse->jc[i+1] && j < (size_t)sparse->ndata; j++ ) {
                            printf("    (%d,%" SIZE_T_FMTSTR ")  ",sparse->ir[j]+1,i+1);
                            Mat_PrintNumber(matvar->data_type,data+j*stride);
                            printf("\n");
                        }
                    }
                }
                break;
            } /* case MAT_C_SPARSE: */
            default:
                break;
        } /* switch( matvar->class_type ) */
    }

    printf("}\n");

    return;
}