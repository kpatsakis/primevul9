png_zstream_error(png_structrp png_ptr, int ret)
{
   /* Translate 'ret' into an appropriate error string, priority is given to the
    * one in zstream if set.  This always returns a string, even in cases like
    * Z_OK or Z_STREAM_END where the error code is a success code.
    */
   if (png_ptr->zstream.msg == NULL) switch (ret)
   {
      default:
      case Z_OK:
         png_ptr->zstream.msg = PNGZ_MSG_CAST("unexpected zlib return code");
         break;

      case Z_STREAM_END:
         /* Normal exit */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("unexpected end of LZ stream");
         break;

      case Z_NEED_DICT:
         /* This means the deflate stream did not have a dictionary; this
          * indicates a bogus PNG.
          */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("missing LZ dictionary");
         break;

      case Z_ERRNO:
         /* gz APIs only: should not happen */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("zlib IO error");
         break;

      case Z_STREAM_ERROR:
         /* internal libpng error */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("bad parameters to zlib");
         break;

      case Z_DATA_ERROR:
         png_ptr->zstream.msg = PNGZ_MSG_CAST("damaged LZ stream");
         break;

      case Z_MEM_ERROR:
         png_ptr->zstream.msg = PNGZ_MSG_CAST("insufficient memory");
         break;

      case Z_BUF_ERROR:
         /* End of input or output; not a problem if the caller is doing
          * incremental read or write.
          */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("truncated");
         break;

      case Z_VERSION_ERROR:
         png_ptr->zstream.msg = PNGZ_MSG_CAST("unsupported zlib version");
         break;

      case PNG_UNEXPECTED_ZLIB_RETURN:
         /* Compile errors here mean that zlib now uses the value co-opted in
          * pngpriv.h for PNG_UNEXPECTED_ZLIB_RETURN; update the switch above
          * and change pngpriv.h.  Note that this message is "... return",
          * whereas the default/Z_OK one is "... return code".
          */
         png_ptr->zstream.msg = PNGZ_MSG_CAST("unexpected zlib return");
         break;
   }
}