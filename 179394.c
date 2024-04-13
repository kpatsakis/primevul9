static Image *decompress_block(Image *orig, unsigned int *Size, ImageInfo *clone_info, ExceptionInfo *exception)
{

Image *image2;
void *cache_block, *decompress_block;
z_stream zip_info;
FILE *mat_file;
size_t magick_size;
size_t extent;
int file;

int status;
int zip_status;
ssize_t TotalSize = 0;

  if(clone_info==NULL) return NULL;
  if(clone_info->file)    /* Close file opened from previous transaction. */
  {
    fclose(clone_info->file);
    clone_info->file = NULL;
    (void) remove_utf8(clone_info->filename);
  }

  cache_block = AcquireQuantumMemory((size_t)(*Size< 16384) ? *Size: 16384,sizeof(unsigned char *));
  if(cache_block==NULL) return NULL;
  decompress_block = AcquireQuantumMemory((size_t)(4096),sizeof(unsigned char *));
  if(decompress_block==NULL)
  {
    RelinquishMagickMemory(cache_block);
    return NULL;
  }

  mat_file=0;
  file = AcquireUniqueFileResource(clone_info->filename);
  if (file != -1)
    mat_file = fdopen(file,"w");
  if(!mat_file)
  {
    RelinquishMagickMemory(cache_block);
    RelinquishMagickMemory(decompress_block);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Cannot create file stream for decompressed image");
    return NULL;
  }

  zip_info.zalloc=AcquireZIPMemory;
  zip_info.zfree=RelinquishZIPMemory;
  zip_info.opaque = (voidpf) NULL;
  zip_status = inflateInit(&zip_info);
  if (zip_status != Z_OK)
    {
      RelinquishMagickMemory(cache_block);
      RelinquishMagickMemory(decompress_block);
      (void) ThrowMagickException(exception,GetMagickModule(),CorruptImageError,
        "UnableToUncompressImage","`%s'",clone_info->filename);
      (void) fclose(mat_file);
      RelinquishUniqueFileResource(clone_info->filename);
      return NULL;
    }
  /* zip_info.next_out = 8*4;*/

  zip_info.avail_in = 0;
  zip_info.total_out = 0;
  while(*Size>0 && !EOFBlob(orig))
  {
    magick_size = ReadBlob(orig, (*Size < 16384) ? *Size : 16384, (unsigned char *) cache_block);
    if (magick_size == 0)
      break;
    zip_info.next_in = (Bytef *) cache_block;
    zip_info.avail_in = (uInt) magick_size;

    while(zip_info.avail_in>0)
    {
      zip_info.avail_out = 4096;
      zip_info.next_out = (Bytef *) decompress_block;
      zip_status = inflate(&zip_info,Z_NO_FLUSH);
			if ((zip_status != Z_OK) && (zip_status != Z_STREAM_END))
        break;
      extent=fwrite(decompress_block, 4096-zip_info.avail_out, 1, mat_file);
      (void) extent;
      TotalSize += 4096-zip_info.avail_out;

      if(zip_status == Z_STREAM_END) goto DblBreak;
    }
 	  if ((zip_status != Z_OK) && (zip_status != Z_STREAM_END))
      break;

    *Size -= (unsigned int) magick_size;
  }
DblBreak:

  inflateEnd(&zip_info);
  (void)fclose(mat_file);
  RelinquishMagickMemory(cache_block);
  RelinquishMagickMemory(decompress_block);
  *Size = TotalSize;

  if((clone_info->file=fopen(clone_info->filename,"rb"))==NULL) goto UnlinkFile;
  if( (image2 = AcquireImage(clone_info))==NULL ) goto EraseFile;
  status = OpenBlob(clone_info,image2,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
  {
    DeleteImageFromList(&image2);
EraseFile:
    fclose(clone_info->file);
    clone_info->file = NULL;
UnlinkFile:
    RelinquishUniqueFileResource(clone_info->filename);
    return NULL;
  }

  return image2;
}