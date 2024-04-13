static MagickBooleanType load_hierarchy(Image *image,XCFDocInfo *inDocInfo,
   XCFLayerInfo *inLayer, ExceptionInfo *exception)
{
  MagickOffsetType
    saved_pos,
    offset,
    junk;

  size_t
    width,
    height,
    bytes_per_pixel;

  width=ReadBlobMSBLong(image);
  (void) width;
  height=ReadBlobMSBLong(image);
  (void) height;
  bytes_per_pixel=inDocInfo->bytes_per_pixel=ReadBlobMSBLong(image);
  (void) bytes_per_pixel;

  /* load in the levels...we make sure that the number of levels
   *  calculated when the TileManager was created is the same
   *  as the number of levels found in the file.
   */
  offset=(MagickOffsetType) ReadBlobMSBLong(image);  /* top level */

  /* discard offsets for layers below first, if any.
   */
  do
  {
    junk=(MagickOffsetType) ReadBlobMSBLong(image);
  }
  while (junk != 0);

  /* save the current position as it is where the
   *  next level offset is stored.
   */
  saved_pos=TellBlob(image);

  /* seek to the level offset */
  if (SeekBlob(image, offset, SEEK_SET) != offset)
    ThrowBinaryException(CorruptImageError,"InsufficientImageDataInFile",
      image->filename);

  /* read in the level */
  if (load_level (image, inDocInfo, inLayer, exception) == 0)
    return(MagickFalse);
  /* restore the saved position so we'll be ready to
   *  read the next offset.
   */
  offset=SeekBlob(image, saved_pos, SEEK_SET);
  return(MagickTrue);
}