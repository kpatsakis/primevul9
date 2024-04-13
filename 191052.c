int af_update_page(AFFILE *af,int64_t pagenum,unsigned char *data,int datalen)
{
    char segname_buf[32];
    snprintf(segname_buf,sizeof(segname_buf),AF_PAGE,pagenum); // determine segment name

#ifdef USE_AFFSIGS
    /* Write out the signature if we have a private key */
    if(af->crypto && af->crypto->sign_privkey){
	af_sign_seg3(af,segname_buf,0,data,datalen,AF_SIGNATURE_MODE1);
    }
#endif

#ifdef HAVE_MD5
    /* Write out MD5 if requested */
    if(af->write_md5){
	unsigned char md5_buf[16];
	char md5name_buf[32];
	MD5(data,datalen,md5_buf);
	snprintf(md5name_buf,sizeof(md5name_buf),AF_PAGE_MD5,pagenum);
	af_update_segf(af,md5name_buf,0,md5_buf,sizeof(md5_buf),AF_SIGFLAG_NOSIG); // ignore failure
    }
#endif
#ifdef HAVE_SHA1
    /* Write out SHA1 if requested */
    if(af->write_sha1){
	unsigned char sha1_buf[20];
	char sha1name_buf[32];
	SHA1(data,datalen,sha1_buf);
	snprintf(sha1name_buf,sizeof(sha1name_buf),AF_PAGE_SHA1,pagenum);
	af_update_segf(af,sha1name_buf,0,sha1_buf,sizeof(sha1_buf),AF_SIGFLAG_NOSIG); // ignore failure
    }
#endif
    /* Write out SHA256 if requested and if SHA256 is available */
    if(af->write_sha256){
	unsigned char sha256_buf[32];
	if(af_SHA256(data,datalen,sha256_buf)==0){
	    char sha256name_buf[32];
	    snprintf(sha256name_buf,sizeof(sha256name_buf),AF_PAGE_SHA256,pagenum);
	    af_update_segf(af,sha256name_buf,0,sha256_buf,sizeof(sha256_buf),AF_SIGFLAG_NOSIG); // ignore failure
	}
    }

    /* Check for bypass */
    if(af->v->write){
	int r = (*af->v->write)(af,data,af->image_pagesize * pagenum,datalen);
	if(r!=datalen) return -1;
	return 0;
    }

    struct affcallback_info acbi;
    int ret = 0;
    uint64_t starting_pages_written = af->pages_written;

    /* Setup the callback structure */
    memset(&acbi,0,sizeof(acbi));
    acbi.info_version = 1;
    acbi.af = af->parent ? af->parent : af;
    acbi.pagenum = pagenum;
    acbi.bytes_to_write = datalen;

    size_t destLen = af->image_pagesize;	// it could be this big.

    /* Compress and write the data, if we are allowed to compress */
    if(af->compression_type != AF_COMPRESSION_ALG_NONE){
	unsigned char *cdata = (unsigned char *)malloc(destLen); // compressed data
	uint32_t *ldata = (uint32_t *)cdata; // allows me to reference as a buffer of uint32_ts
	if(cdata!=0){		// If data could be allocated
	    int cres = -1;		// compression results
	    uint32_t flag = 0;	// flag for data segment
	    int dont_compress = 0;

	    /* Try zero compression first; it's the best algorithm we have  */
	    if(is_buffer_zero(data,datalen)){
		acbi.compression_alg   = AF_PAGE_COMP_ALG_ZERO;
		acbi.compression_level = AF_COMPRESSION_MAX;

		if(af->w_callback) { acbi.phase = 1; (*af->w_callback)(&acbi); }

		*ldata = htonl(datalen); // store the data length
		destLen = 4;		 // 4 bytes
		flag = AF_PAGE_COMPRESSED | AF_PAGE_COMP_ALG_ZERO | AF_PAGE_COMP_MAX;
		cres = 0;

		acbi.compressed = 1;		// it was compressed
		if(af->w_callback) {acbi.phase = 2;(*af->w_callback)(&acbi);}
	    }

#ifdef USE_LZMA
	    if(cres!=0 && af->compression_type==AF_COMPRESSION_ALG_LZMA){ // try to compress with LZMA
		acbi.compression_alg   = AF_PAGE_COMP_ALG_LZMA;
		acbi.compression_level = 7; // right now, this is the level we use
		if(af->w_callback) { acbi.phase = 1; (*af->w_callback)(&acbi); }

		cres = lzma_compress(cdata,&destLen,data,datalen,9);
#if 0
		switch(cres){
		case 0:break;		// OKAY
		case 1: (*af->error_reporter)("LZMA: Unspecified Error\n");break;
		case 2: (*af->error_reporter)("LZMA: Memory Allocating Error\n");break;
		case 3: (*af->error_reporter)("LZMA: Output buffer OVERFLOW\n"); break;
		default: (*af->error_reporter)("LZMA: Unknown error %d\n",cres);break;
		}
#endif
		if(cres==0){
		    flag = AF_PAGE_COMPRESSED | AF_PAGE_COMP_ALG_LZMA;
		    acbi.compressed = 1;
		    if(af->w_callback) {acbi.phase = 2;(*af->w_callback)(&acbi);}
		}
		else {
		    /* Don't bother reporting LZMA errors; we just won't compress */
		    dont_compress = 1;
		    if(af->w_callback) {acbi.phase = 2;(*af->w_callback)(&acbi);}
		}
	    }
#endif

	    if(cres!=0
	       && af->compression_type==AF_COMPRESSION_ALG_ZLIB
	       && dont_compress==0){ // try to compress with zlib
		acbi.compression_alg   = AF_PAGE_COMP_ALG_ZLIB; // only one that we support
		acbi.compression_level = af->compression_level;
		if(af->w_callback) { acbi.phase = 1; (*af->w_callback)(&acbi); }

		cres = compress2((Bytef *)cdata, (uLongf *)&destLen,
				 (Bytef *)data,datalen, af->compression_level);

		if(cres==0){
		    flag = AF_PAGE_COMPRESSED | AF_PAGE_COMP_ALG_ZLIB;
		    if(af->compression_level == AF_COMPRESSION_MAX){
			flag |= AF_PAGE_COMP_MAX; // useful to know it can't be better
		    }
		}
		acbi.compressed = 1;	// it was compressed (or not compressed)
		if(af->w_callback) {acbi.phase = 2;(*af->w_callback)(&acbi);}
	    }

	    if(cres==0 && destLen < af->image_pagesize){
		/* Prepare to write out the compressed segment with compression */
		if(af->w_callback) {acbi.phase = 3;(*af->w_callback)(&acbi);}
		ret = af_update_segf(af,segname_buf,flag,cdata,destLen,AF_SIGFLAG_NOSIG);
		acbi.bytes_written = destLen;
		if(af->w_callback) {acbi.phase = 4;(*af->w_callback)(&acbi);}
		if(ret==0){
		    af->pages_written++;
		    af->pages_compressed++;
		}
	    }
	    free(cdata);
	    cdata = 0;
	}
    }

    /* If a compressed segment was not written, write it uncompressed */
    if(af->pages_written == starting_pages_written){
	if(af->w_callback) {acbi.phase = 3;(*af->w_callback)(&acbi);}
	ret = af_update_segf(af,segname_buf,0,data,datalen,AF_SIGFLAG_NOSIG);
	acbi.bytes_written = datalen;
	if(af->w_callback) {acbi.phase = 4;(*af->w_callback)(&acbi);}
	if(ret==0){
	    acbi.bytes_written = datalen;	// because that is how much we wrote
	    af->pages_written++;
	}
    }
    return ret;
}