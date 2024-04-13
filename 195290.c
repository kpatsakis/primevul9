Mat_VarDuplicate(const matvar_t *in, int opt)
{
    matvar_t *out;
    size_t i;

    out = Mat_VarCalloc();
    if ( out == NULL )
        return NULL;

    out->nbytes       = in->nbytes;
    out->rank         = in->rank;
    out->data_type    = in->data_type;
    out->data_size    = in->data_size;
    out->class_type   = in->class_type;
    out->isComplex    = in->isComplex;
    out->isGlobal     = in->isGlobal;
    out->isLogical    = in->isLogical;
    out->mem_conserve = in->mem_conserve;
    out->compression  = in->compression;

    if ( NULL != in->name ) {
        size_t len = strlen(in->name) + 1;
        out->name = (char*)malloc(len);
        if ( NULL != out->name )
            memcpy(out->name,in->name,len);
    }

    out->dims = (size_t*)malloc(in->rank*sizeof(*out->dims));
    if ( out->dims != NULL )
        memcpy(out->dims,in->dims,in->rank*sizeof(*out->dims));

    if ( NULL != in->internal ) {
#if defined(MAT73) && MAT73
        if ( NULL != in->internal->hdf5_name )
            out->internal->hdf5_name = strdup(in->internal->hdf5_name);

        out->internal->hdf5_ref = in->internal->hdf5_ref;
        out->internal->id       = in->internal->id;
#endif
        out->internal->datapos  = in->internal->datapos;
#if HAVE_ZLIB
        out->internal->z        = NULL;
        out->internal->data     = NULL;
#endif
        out->internal->num_fields = in->internal->num_fields;
        if ( NULL != in->internal->fieldnames && in->internal->num_fields > 0 ) {
            out->internal->fieldnames = (char**)calloc(in->internal->num_fields,
                                               sizeof(*in->internal->fieldnames));
            for ( i = 0; i < in->internal->num_fields; i++ ) {
                if ( NULL != in->internal->fieldnames[i] )
                    out->internal->fieldnames[i] =
                        strdup(in->internal->fieldnames[i]);
            }
        }

#if HAVE_ZLIB
        if ( (in->internal->z != NULL) && (NULL != (out->internal->z = (z_streamp)malloc(sizeof(z_stream)))) )
            inflateCopy(out->internal->z,in->internal->z);
        if ( in->internal->data != NULL ) {
            if ( in->class_type == MAT_C_SPARSE ) {
                out->internal->data = malloc(sizeof(mat_sparse_t));
                if ( out->internal->data != NULL ) {
                    mat_sparse_t *out_sparse = (mat_sparse_t*)out->internal->data;
                    mat_sparse_t *in_sparse  = (mat_sparse_t*)in->internal->data;
                    out_sparse->nzmax = in_sparse->nzmax;
                    out_sparse->nir = in_sparse->nir;
                    out_sparse->ir = (mat_uint32_t*)malloc(in_sparse->nir*sizeof(*out_sparse->ir));
                    if ( out_sparse->ir != NULL )
                        memcpy(out_sparse->ir, in_sparse->ir, in_sparse->nir*sizeof(*out_sparse->ir));
                    out_sparse->njc = in_sparse->njc;
                    out_sparse->jc = (mat_uint32_t*)malloc(in_sparse->njc*sizeof(*out_sparse->jc));
                    if ( out_sparse->jc != NULL )
                        memcpy(out_sparse->jc, in_sparse->jc, in_sparse->njc*sizeof(*out_sparse->jc));
                    out_sparse->ndata = in_sparse->ndata;
                    if ( out->isComplex && NULL != in_sparse->data ) {
                        out_sparse->data = malloc(sizeof(mat_complex_split_t));
                        if ( out_sparse->data != NULL ) {
                            mat_complex_split_t *out_data = (mat_complex_split_t*)out_sparse->data;
                            mat_complex_split_t *in_data  = (mat_complex_split_t*)in_sparse->data;
                            out_data->Re = malloc(
                                in_sparse->ndata*Mat_SizeOf(in->data_type));
                            if ( NULL != out_data->Re )
                                memcpy(out_data->Re,in_data->Re,
                                    in_sparse->ndata*Mat_SizeOf(in->data_type));
                            out_data->Im = malloc(
                                in_sparse->ndata*Mat_SizeOf(in->data_type));
                            if ( NULL != out_data->Im )
                                memcpy(out_data->Im,in_data->Im,
                                    in_sparse->ndata*Mat_SizeOf(in->data_type));
                        }
                    } else if ( in_sparse->data != NULL ) {
                        out_sparse->data = malloc(in_sparse->ndata*Mat_SizeOf(in->data_type));
                        if ( NULL != out_sparse->data )
                            memcpy(out_sparse->data, in_sparse->data,
                                in_sparse->ndata*Mat_SizeOf(in->data_type));
                    }
                }
            } else if ( out->isComplex ) {
                out->internal->data = malloc(sizeof(mat_complex_split_t));
                if ( out->internal->data != NULL ) {
                    mat_complex_split_t *out_data = (mat_complex_split_t*)out->internal->data;
                    mat_complex_split_t *in_data  = (mat_complex_split_t*)in->internal->data;
                    out_data->Re = malloc(out->nbytes);
                    if ( NULL != out_data->Re )
                        memcpy(out_data->Re,in_data->Re,out->nbytes);
                    out_data->Im = malloc(out->nbytes);
                    if ( NULL != out_data->Im )
                        memcpy(out_data->Im,in_data->Im,out->nbytes);
                }
            } else if ( NULL != (out->internal->data = malloc(in->nbytes)) ) {
                memcpy(out->internal->data, in->internal->data, in->nbytes);
            }
        }
#endif
    } else {
        free(out->internal);
        out->internal = NULL;
    }

    if ( !opt ) {
        out->data = in->data;
    } else if ( (in->data != NULL) && (in->class_type == MAT_C_STRUCT) ) {
        out->data = malloc(in->nbytes);
        if ( out->data != NULL && in->data_size > 0 ) {
            size_t nfields = in->nbytes / in->data_size;
            matvar_t **infields  = (matvar_t **)in->data;
            matvar_t **outfields = (matvar_t **)out->data;
            for ( i = 0; i < nfields; i++ ) {
                outfields[i] = Mat_VarDuplicate(infields[i],opt);
            }
        }
    } else if ( (in->data != NULL) && (in->class_type == MAT_C_CELL) ) {
        out->data = malloc(in->nbytes);
        if ( out->data != NULL && in->data_size > 0 ) {
            size_t nelems = in->nbytes / in->data_size;
            matvar_t **incells  = (matvar_t **)in->data;
            matvar_t **outcells = (matvar_t **)out->data;
            for ( i = 0; i < nelems; i++ ) {
                outcells[i] = Mat_VarDuplicate(incells[i],opt);
            }
        }
    } else if ( (in->data != NULL) && (in->class_type == MAT_C_SPARSE) ) {
        out->data = malloc(sizeof(mat_sparse_t));
        if ( out->data != NULL ) {
            mat_sparse_t *out_sparse = (mat_sparse_t*)out->data;
            mat_sparse_t *in_sparse  = (mat_sparse_t*)in->data;
            out_sparse->nzmax = in_sparse->nzmax;
            out_sparse->nir = in_sparse->nir;
            out_sparse->ir = (mat_uint32_t*)malloc(in_sparse->nir*sizeof(*out_sparse->ir));
            if ( out_sparse->ir != NULL )
                memcpy(out_sparse->ir, in_sparse->ir, in_sparse->nir*sizeof(*out_sparse->ir));
            out_sparse->njc = in_sparse->njc;
            out_sparse->jc = (mat_uint32_t*)malloc(in_sparse->njc*sizeof(*out_sparse->jc));
            if ( out_sparse->jc != NULL )
                memcpy(out_sparse->jc, in_sparse->jc, in_sparse->njc*sizeof(*out_sparse->jc));
            out_sparse->ndata = in_sparse->ndata;
            if ( out->isComplex && NULL != in_sparse->data ) {
                out_sparse->data = malloc(sizeof(mat_complex_split_t));
                if ( out_sparse->data != NULL ) {
                    mat_complex_split_t *out_data = (mat_complex_split_t*)out_sparse->data;
                    mat_complex_split_t *in_data  = (mat_complex_split_t*)in_sparse->data;
                    out_data->Re = malloc(in_sparse->ndata*Mat_SizeOf(in->data_type));
                    if ( NULL != out_data->Re )
                        memcpy(out_data->Re,in_data->Re,in_sparse->ndata*Mat_SizeOf(in->data_type));
                    out_data->Im = malloc(in_sparse->ndata*Mat_SizeOf(in->data_type));
                    if ( NULL != out_data->Im )
                        memcpy(out_data->Im,in_data->Im,in_sparse->ndata*Mat_SizeOf(in->data_type));
                }
            } else if ( in_sparse->data != NULL ) {
                out_sparse->data = malloc(in_sparse->ndata*Mat_SizeOf(in->data_type));
                if ( NULL != out_sparse->data )
                    memcpy(out_sparse->data, in_sparse->data, in_sparse->ndata*Mat_SizeOf(in->data_type));
            } else {
                out_sparse->data = NULL;
            }
        }
    } else if ( in->data != NULL ) {
        if ( out->isComplex ) {
            out->data = malloc(sizeof(mat_complex_split_t));
            if ( out->data != NULL ) {
                mat_complex_split_t *out_data = (mat_complex_split_t*)out->data;
                mat_complex_split_t *in_data  = (mat_complex_split_t*)in->data;
                out_data->Re = malloc(out->nbytes);
                if ( NULL != out_data->Re )
                    memcpy(out_data->Re,in_data->Re,out->nbytes);
                out_data->Im = malloc(out->nbytes);
                if ( NULL != out_data->Im )
                    memcpy(out_data->Im,in_data->Im,out->nbytes);
            }
        } else {
            out->data = malloc(in->nbytes);
            if ( out->data != NULL )
                memcpy(out->data,in->data,in->nbytes);
        }
    }

    return out;
}