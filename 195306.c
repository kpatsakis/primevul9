Mat_VarCreate(const char *name,enum matio_classes class_type,
    enum matio_types data_type,int rank,size_t *dims,void *data,int opt)
{
    size_t nelems = 1, data_size;
    matvar_t *matvar = NULL;
    int j, err;

    if ( dims == NULL )
        return NULL;

    matvar = Mat_VarCalloc();
    if ( NULL == matvar )
        return NULL;

    matvar->compression = MAT_COMPRESSION_NONE;
    matvar->isComplex   = opt & MAT_F_COMPLEX;
    matvar->isGlobal    = opt & MAT_F_GLOBAL;
    matvar->isLogical   = opt & MAT_F_LOGICAL;
    if ( name )
        matvar->name = strdup_printf("%s",name);
    matvar->rank = rank;
    matvar->dims = (size_t*)malloc(matvar->rank*sizeof(*matvar->dims));
    for ( j = 0; j < matvar->rank; j++ ) {
        matvar->dims[j] = dims[j];
        nelems *= dims[j];
    }
    matvar->class_type = class_type;
    matvar->data_type  = data_type;
    switch ( data_type ) {
        case MAT_T_INT8:
            data_size = 1;
            break;
        case MAT_T_UINT8:
            data_size = 1;
            break;
        case MAT_T_INT16:
            data_size = 2;
            break;
        case MAT_T_UINT16:
            data_size = 2;
            break;
        case MAT_T_INT64:
            data_size = 8;
            break;
        case MAT_T_UINT64:
            data_size = 8;
            break;
        case MAT_T_INT32:
            data_size = 4;
            break;
        case MAT_T_UINT32:
            data_size = 4;
            break;
        case MAT_T_SINGLE:
            data_size = sizeof(float);
            break;
        case MAT_T_DOUBLE:
            data_size = sizeof(double);
            break;
        case MAT_T_UTF8:
            data_size = 1;
            break;
        case MAT_T_UTF16:
            data_size = 2;
            break;
        case MAT_T_UTF32:
            data_size = 4;
            break;
        case MAT_T_CELL:
            data_size = sizeof(matvar_t **);
            break;
        case MAT_T_STRUCT:
        {
            data_size = sizeof(matvar_t **);
            if ( data != NULL ) {
                matvar_t **fields = (matvar_t**)data;
                size_t nfields = 0;
                while ( fields[nfields] != NULL )
                    nfields++;
                if ( nelems )
                    nfields /= nelems;
                matvar->internal->num_fields = nfields;
                if ( nfields ) {
                    size_t i;
                    matvar->internal->fieldnames =
                        (char**)calloc(nfields,sizeof(*matvar->internal->fieldnames));
                    for ( i = 0; i < nfields; i++ )
                        matvar->internal->fieldnames[i] = strdup(fields[i]->name);
                    err = SafeMul(&nelems, nelems, nfields);
                    if ( err ) {
                        Mat_VarFree(matvar);
                        Mat_Critical("Integer multiplication overflow");
                        return NULL;
                    }
                }
            }
            break;
        }
        default:
            Mat_VarFree(matvar);
            Mat_Critical("Unrecognized data_type");
            return NULL;
    }
    if ( matvar->class_type == MAT_C_SPARSE ) {
        matvar->data_size = sizeof(mat_sparse_t);
        matvar->nbytes    = matvar->data_size;
    } else {
        matvar->data_size = data_size;
        err = SafeMul(&matvar->nbytes, nelems, matvar->data_size);
        if ( err ) {
            Mat_VarFree(matvar);
            Mat_Critical("Integer multiplication overflow");
            return NULL;
        }
    }
    if ( data == NULL ) {
        if ( MAT_C_CELL == matvar->class_type && nelems > 0 )
            matvar->data = calloc(nelems,sizeof(matvar_t*));
        else
            matvar->data = NULL;
    } else if ( opt & MAT_F_DONT_COPY_DATA ) {
        matvar->data         = data;
        matvar->mem_conserve = 1;
    } else if ( MAT_C_SPARSE == matvar->class_type ) {
        mat_sparse_t *sparse_data, *sparse_data_in;

        sparse_data_in = (mat_sparse_t*)data;
        sparse_data    = (mat_sparse_t*)malloc(sizeof(mat_sparse_t));
        if ( NULL != sparse_data ) {
            sparse_data->nzmax = sparse_data_in->nzmax;
            sparse_data->nir   = sparse_data_in->nir;
            sparse_data->njc   = sparse_data_in->njc;
            sparse_data->ndata = sparse_data_in->ndata;
            sparse_data->ir = (mat_uint32_t*)malloc(sparse_data->nir*sizeof(*sparse_data->ir));
            if ( NULL != sparse_data->ir )
                memcpy(sparse_data->ir,sparse_data_in->ir,
                       sparse_data->nir*sizeof(*sparse_data->ir));
            sparse_data->jc = (mat_uint32_t*)malloc(sparse_data->njc*sizeof(*sparse_data->jc));
            if ( NULL != sparse_data->jc )
                memcpy(sparse_data->jc,sparse_data_in->jc,
                       sparse_data->njc*sizeof(*sparse_data->jc));
            if ( matvar->isComplex ) {
                sparse_data->data = malloc(sizeof(mat_complex_split_t));
                if ( NULL != sparse_data->data ) {
                    mat_complex_split_t *complex_data,*complex_data_in;
                    complex_data     = (mat_complex_split_t*)sparse_data->data;
                    complex_data_in  = (mat_complex_split_t*)sparse_data_in->data;
                    complex_data->Re = malloc(sparse_data->ndata*data_size);
                    complex_data->Im = malloc(sparse_data->ndata*data_size);
                    if ( NULL != complex_data->Re )
                        memcpy(complex_data->Re,complex_data_in->Re,
                               sparse_data->ndata*data_size);
                    if ( NULL != complex_data->Im )
                        memcpy(complex_data->Im,complex_data_in->Im,
                               sparse_data->ndata*data_size);
                }
            } else {
                sparse_data->data = malloc(sparse_data->ndata*data_size);
                if ( NULL != sparse_data->data )
                    memcpy(sparse_data->data,sparse_data_in->data,
                           sparse_data->ndata*data_size);
            }
        }
        matvar->data = sparse_data;
    } else {
        if ( matvar->isComplex ) {
            matvar->data   = malloc(sizeof(mat_complex_split_t));
            if ( NULL != matvar->data && matvar->nbytes > 0 ) {
                mat_complex_split_t *complex_data    = (mat_complex_split_t*)matvar->data;
                mat_complex_split_t *complex_data_in = (mat_complex_split_t*)data;

                complex_data->Re = malloc(matvar->nbytes);
                complex_data->Im = malloc(matvar->nbytes);
                if ( NULL != complex_data->Re )
                    memcpy(complex_data->Re,complex_data_in->Re,matvar->nbytes);
                if ( NULL != complex_data->Im )
                    memcpy(complex_data->Im,complex_data_in->Im,matvar->nbytes);
            }
        } else if ( matvar->nbytes > 0 ) {
            matvar->data   = malloc(matvar->nbytes);
            if ( NULL != matvar->data )
                memcpy(matvar->data,data,matvar->nbytes);
        }
        matvar->mem_conserve = 0;
    }

    return matvar;
}