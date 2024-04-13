void bufSizeTest(void)
{
	int w, h, i, subsamp;
	unsigned char *srcBuf=NULL, *dstBuf=NULL;
	tjhandle handle=NULL;
	unsigned long dstSize=0;

	if((handle=tjInitCompress())==NULL) _throwtj();

	printf("Buffer size regression test\n");
	for(subsamp=0; subsamp<TJ_NUMSAMP; subsamp++)
	{
		for(w=1; w<48; w++)
		{
			int maxh=(w==1)? 2048:48;
			for(h=1; h<maxh; h++)
			{
				if(h%100==0) printf("%.4d x %.4d\b\b\b\b\b\b\b\b\b\b\b", w, h);
				if((srcBuf=(unsigned char *)malloc(w*h*4))==NULL)
					_throw("Memory allocation failure");
				if(!alloc || doyuv)
				{
					if(doyuv) dstSize=tjBufSizeYUV2(w, pad, h, subsamp);
					else dstSize=tjBufSize(w, h, subsamp);
					if((dstBuf=(unsigned char *)tjAlloc(dstSize))==NULL)
						_throw("Memory allocation failure");
				}

				for(i=0; i<w*h*4; i++)
				{
					if(random()<RAND_MAX/2) srcBuf[i]=0;
					else srcBuf[i]=255;
				}

				if(doyuv)
				{
					_tj(tjEncodeYUV3(handle, srcBuf, w, 0, h, TJPF_BGRX, dstBuf, pad,
						subsamp, 0));
				}
				else
				{
					_tj(tjCompress2(handle, srcBuf, w, 0, h, TJPF_BGRX, &dstBuf,
						&dstSize, subsamp, 100, alloc? 0:TJFLAG_NOREALLOC));
				}
				free(srcBuf);  srcBuf=NULL;
				if(!alloc || doyuv)
				{
					tjFree(dstBuf);  dstBuf=NULL;
				}

				if((srcBuf=(unsigned char *)malloc(h*w*4))==NULL)
					_throw("Memory allocation failure");
				if(!alloc || doyuv)
				{
					if(doyuv) dstSize=tjBufSizeYUV2(h, pad, w, subsamp);
					else dstSize=tjBufSize(h, w, subsamp);
					if((dstBuf=(unsigned char *)tjAlloc(dstSize))==NULL)
						_throw("Memory allocation failure");
				}

				for(i=0; i<h*w*4; i++)
				{
					if(random()<RAND_MAX/2) srcBuf[i]=0;
					else srcBuf[i]=255;
				}

				if(doyuv)
				{
					_tj(tjEncodeYUV3(handle, srcBuf, h, 0, w, TJPF_BGRX, dstBuf, pad,
						subsamp, 0));
				}
				else
				{
					_tj(tjCompress2(handle, srcBuf, h, 0, w, TJPF_BGRX, &dstBuf,
						&dstSize, subsamp, 100, alloc? 0:TJFLAG_NOREALLOC));
				}
				free(srcBuf);  srcBuf=NULL;
				if(!alloc || doyuv)
				{
					tjFree(dstBuf);  dstBuf=NULL;
				}
			}
		}
	}
	printf("Done.      \n");

	bailout:
	if(srcBuf) free(srcBuf);
	if(dstBuf) tjFree(dstBuf);
	if(handle) tjDestroy(handle);
}