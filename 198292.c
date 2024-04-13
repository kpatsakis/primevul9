Mat_VarReadData5(mat_t *mat,matvar_t *matvar,void *data,
    int *start,int *stride,int *edge)
{
    int err = 0,real_bytes = 0;
    mat_int32_t tag[2];
#if defined(HAVE_ZLIB)
    z_stream z;
#endif
    size_t bytesread = 0;

    (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
    if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        bytesread += fread(tag,4,2,(FILE*)mat->fp);
        if ( mat->byteswap ) {
            (void)Mat_int32Swap(tag);
            (void)Mat_int32Swap(tag+1);
        }
        matvar->data_type = TYPE_FROM_TAG(tag[0]);
        if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
            (void)fseek((FILE*)mat->fp,-4,SEEK_CUR);
            real_bytes = 4+(tag[0] >> 16);
        } else {
            real_bytes = 8+tag[1];
        }
#if defined(HAVE_ZLIB)
    } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
        if ( NULL != matvar->internal->data ) {
            /* Data already read in ReadNextStructField or ReadNextCell */
            if ( matvar->isComplex ) {
                mat_complex_split_t *ci, *co;

                co = (mat_complex_split_t*)data;
                ci = (mat_complex_split_t*)matvar->internal->data;
                err = GetDataSlab(ci->Re, co->Re, matvar->class_type,
                    matvar->data_type, matvar->dims, start, stride, edge,
                    matvar->rank, matvar->nbytes);
                if ( err == 0 )
                    err = GetDataSlab(ci->Im, co->Im, matvar->class_type,
                        matvar->data_type, matvar->dims, start, stride, edge,
                        matvar->rank, matvar->nbytes);
                return err;
            } else {
                return GetDataSlab(matvar->internal->data, data, matvar->class_type,
                    matvar->data_type, matvar->dims, start, stride, edge,
                    matvar->rank, matvar->nbytes);
            }
        }

        err = inflateCopy(&z,matvar->internal->z);
        if ( err != Z_OK ) {
            Mat_Critical("inflateCopy returned error %s",zError(err));
            return -1;
        }
        z.avail_in = 0;
        InflateDataType(mat,&z,tag);
        if ( mat->byteswap ) {
            (void)Mat_int32Swap(tag);
        }
        matvar->data_type = TYPE_FROM_TAG(tag[0]);
        if ( !(tag[0] & 0xffff0000) ) {/* Data is NOT packed in the tag */
            /* We're cheating, but InflateDataType just inflates 4 bytes */
            InflateDataType(mat,&z,tag+1);
            if ( mat->byteswap ) {
                (void)Mat_int32Swap(tag+1);
            }
            real_bytes = 8+tag[1];
        } else {
            real_bytes = 4+(tag[0] >> 16);
        }
#endif
    }
    if ( real_bytes % 8 )
        real_bytes += (8-(real_bytes % 8));

    if ( matvar->rank == 2 ) {
        if ( (size_t)stride[0]*(edge[0]-1)+start[0]+1 > matvar->dims[0] )
            err = 1;
        else if ( (size_t)stride[1]*(edge[1]-1)+start[1]+1 > matvar->dims[1] )
            err = 1;
        else if ( matvar->compression == MAT_COMPRESSION_NONE ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

                ReadDataSlab2(mat,complex_data->Re,matvar->class_type,
                    matvar->data_type,matvar->dims,start,stride,edge);
                (void)fseek((FILE*)mat->fp,matvar->internal->datapos+real_bytes,SEEK_SET);
                bytesread += fread(tag,4,2,(FILE*)mat->fp);
                if ( mat->byteswap ) {
                    (void)Mat_int32Swap(tag);
                    (void)Mat_int32Swap(tag+1);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                    (void)fseek((FILE*)mat->fp,-4,SEEK_CUR);
                }
                ReadDataSlab2(mat,complex_data->Im,matvar->class_type,
                              matvar->data_type,matvar->dims,start,stride,edge);
            } else {
                ReadDataSlab2(mat,data,matvar->class_type,
                    matvar->data_type,matvar->dims,start,stride,edge);
            }
        }
#if defined(HAVE_ZLIB)
        else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

                ReadCompressedDataSlab2(mat,&z,complex_data->Re,
                    matvar->class_type,matvar->data_type,matvar->dims,
                    start,stride,edge);

                (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);

                /* Reset zlib knowledge to before reading real tag */
                inflateEnd(&z);
                err = inflateCopy(&z,matvar->internal->z);
                if ( err != Z_OK ) {
                    Mat_Critical("inflateCopy returned error %s",zError(err));
                }
                InflateSkip(mat,&z,real_bytes);
                z.avail_in = 0;
                InflateDataType(mat,&z,tag);
                if ( mat->byteswap ) {
                    (void)Mat_int32Swap(tag);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( !(tag[0] & 0xffff0000) ) {/*Data is NOT packed in the tag*/
                    InflateSkip(mat,&z,4);
                }
                ReadCompressedDataSlab2(mat,&z,complex_data->Im,
                    matvar->class_type,matvar->data_type,matvar->dims,
                    start,stride,edge);
            } else {
                ReadCompressedDataSlab2(mat,&z,data,matvar->class_type,
                    matvar->data_type,matvar->dims,start,stride,edge);
            }
            inflateEnd(&z);
        }
#endif
    } else {
        if ( matvar->compression == MAT_COMPRESSION_NONE ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

                ReadDataSlabN(mat,complex_data->Re,matvar->class_type,
                    matvar->data_type,matvar->rank,matvar->dims,
                    start,stride,edge);

                (void)fseek((FILE*)mat->fp,matvar->internal->datapos+real_bytes,SEEK_SET);
                bytesread += fread(tag,4,2,(FILE*)mat->fp);
                if ( mat->byteswap ) {
                    (void)Mat_int32Swap(tag);
                    (void)Mat_int32Swap(tag+1);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                    (void)fseek((FILE*)mat->fp,-4,SEEK_CUR);
                }
                ReadDataSlabN(mat,complex_data->Im,matvar->class_type,
                    matvar->data_type,matvar->rank,matvar->dims,
                    start,stride,edge);
            } else {
                ReadDataSlabN(mat,data,matvar->class_type,matvar->data_type,
                    matvar->rank,matvar->dims,start,stride,edge);
            }
        }
#if defined(HAVE_ZLIB)
        else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
            if ( matvar->isComplex ) {
                mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

                ReadCompressedDataSlabN(mat,&z,complex_data->Re,
                    matvar->class_type,matvar->data_type,matvar->rank,
                    matvar->dims,start,stride,edge);

                (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
                /* Reset zlib knowledge to before reading real tag */
                inflateEnd(&z);
                err = inflateCopy(&z,matvar->internal->z);
                if ( err != Z_OK ) {
                    Mat_Critical("inflateCopy returned error %s",zError(err));
                }
                InflateSkip(mat,&z,real_bytes);
                z.avail_in = 0;
                InflateDataType(mat,&z,tag);
                if ( mat->byteswap ) {
                    (void)Mat_int32Swap(tag);
                }
                matvar->data_type = TYPE_FROM_TAG(tag[0]);
                if ( !(tag[0] & 0xffff0000) ) {/*Data is NOT packed in the tag*/
                    InflateSkip(mat,&z,4);
                }
                ReadCompressedDataSlabN(mat,&z,complex_data->Im,
                    matvar->class_type,matvar->data_type,matvar->rank,
                    matvar->dims,start,stride,edge);
            } else {
                ReadCompressedDataSlabN(mat,&z,data,matvar->class_type,
                    matvar->data_type,matvar->rank,matvar->dims,
                    start,stride,edge);
            }
            inflateEnd(&z);
        }
#endif
    }
    if ( err == 0 ) {
        matvar->data_type = ClassType2DataType(matvar->class_type);
        matvar->data_size = Mat_SizeOfClass(matvar->class_type);
    }
    return err;
}