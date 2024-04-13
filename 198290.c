Mat_VarReadDataLinear5(mat_t *mat,matvar_t *matvar,void *data,int start,
                      int stride,int edge)
{
    int err = 0, real_bytes = 0;
    mat_int32_t tag[2];
#if defined(HAVE_ZLIB)
    z_stream z;
#endif
    size_t bytesread = 0, nelems = 1;

    if ( mat->version == MAT_FT_MAT4 )
        return -1;
    (void)fseek((FILE*)mat->fp,matvar->internal->datapos,SEEK_SET);
    if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        bytesread += fread(tag,4,2,(FILE*)mat->fp);
        if ( mat->byteswap ) {
            (void)Mat_int32Swap(tag);
            (void)Mat_int32Swap(tag+1);
        }
        matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
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
                err = GetDataLinear(ci->Re, co->Re, matvar->class_type,
                    matvar->data_type, start, stride, edge);
                if ( err == 0 )
                    err = GetDataLinear(ci->Im, co->Im, matvar->class_type,
                        matvar->data_type, start, stride, edge);
                return err;
            } else {
                return GetDataLinear(matvar->internal->data, data, matvar->class_type,
                    matvar->data_type, start, stride, edge);
            }
        }

        matvar->internal->z->avail_in = 0;
        err = inflateCopy(&z,matvar->internal->z);
        if ( err != Z_OK ) {
            Mat_Critical("inflateCopy returned error %s",zError(err));
            return -1;
        }
        InflateDataType(mat,&z,tag);
        if ( mat->byteswap ) {
            (void)Mat_int32Swap(tag);
            (void)Mat_int32Swap(tag+1);
        }
        matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
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

    err = SafeMulDims(matvar, &nelems);
    if ( err ) {
        Mat_Critical("Integer multiplication overflow");
        return -1;
    }

    if ( (size_t)stride*(edge-1)+start+1 > nelems ) {
        err = 1;
    } else if ( matvar->compression == MAT_COMPRESSION_NONE ) {
        if ( matvar->isComplex ) {
            mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

            ReadDataSlab1(mat,complex_data->Re,matvar->class_type,
                          matvar->data_type,start,stride,edge);
            (void)fseek((FILE*)mat->fp,matvar->internal->datapos+real_bytes,SEEK_SET);
            bytesread += fread(tag,4,2,(FILE*)mat->fp);
            if ( mat->byteswap ) {
                (void)Mat_int32Swap(tag);
                (void)Mat_int32Swap(tag+1);
            }
            matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
            if ( tag[0] & 0xffff0000 ) { /* Data is packed in the tag */
                (void)fseek((FILE*)mat->fp,-4,SEEK_CUR);
            }
            ReadDataSlab1(mat,complex_data->Im,matvar->class_type,
                          matvar->data_type,start,stride,edge);
        } else {
            ReadDataSlab1(mat,data,matvar->class_type,
                          matvar->data_type,start,stride,edge);
        }
#if defined(HAVE_ZLIB)
    } else if ( matvar->compression == MAT_COMPRESSION_ZLIB ) {
        if ( matvar->isComplex ) {
            mat_complex_split_t *complex_data = (mat_complex_split_t*)data;

            ReadCompressedDataSlab1(mat,&z,complex_data->Re,
                matvar->class_type,matvar->data_type,start,stride,edge);

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
            matvar->data_type = (enum matio_types)(tag[0] & 0x000000ff);
            if ( !(tag[0] & 0xffff0000) ) {/*Data is NOT packed in the tag*/
                InflateSkip(mat,&z,4);
            }
            ReadCompressedDataSlab1(mat,&z,complex_data->Im,
                matvar->class_type,matvar->data_type,start,stride,edge);
        } else {
            ReadCompressedDataSlab1(mat,&z,data,matvar->class_type,
                matvar->data_type,start,stride,edge);
        }
        inflateEnd(&z);
#endif
    }

    matvar->data_type = ClassType2DataType(matvar->class_type);
    matvar->data_size = Mat_SizeOfClass(matvar->class_type);

    return err;
}