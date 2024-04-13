PNG_FUNCTION(png_structp /* PRIVATE */,
png_create_png_struct,(png_const_charp user_png_ver, png_voidp error_ptr,
    png_error_ptr error_fn, png_error_ptr warn_fn, png_voidp mem_ptr,
    png_malloc_ptr malloc_fn, png_free_ptr free_fn),PNG_ALLOCATED)
{
   png_struct create_struct;
#  ifdef PNG_SETJMP_SUPPORTED
      jmp_buf create_jmp_buf;
#  endif

   /* This temporary stack-allocated structure is used to provide a place to
    * build enough context to allow the user provided memory allocator (if any)
    * to be called.
    */
   memset(&create_struct, 0, (sizeof create_struct));

   /* Added at libpng-1.2.6 */
#  ifdef PNG_USER_LIMITS_SUPPORTED
      create_struct.user_width_max = PNG_USER_WIDTH_MAX;
      create_struct.user_height_max = PNG_USER_HEIGHT_MAX;

#     ifdef PNG_USER_CHUNK_CACHE_MAX
      /* Added at libpng-1.2.43 and 1.4.0 */
      create_struct.user_chunk_cache_max = PNG_USER_CHUNK_CACHE_MAX;
#     endif

#     ifdef PNG_USER_CHUNK_MALLOC_MAX
      /* Added at libpng-1.2.43 and 1.4.1, required only for read but exists
       * in png_struct regardless.
       */
      create_struct.user_chunk_malloc_max = PNG_USER_CHUNK_MALLOC_MAX;
#     endif
#  endif

   /* The following two API calls simply set fields in png_struct, so it is safe
    * to do them now even though error handling is not yet set up.
    */
#  ifdef PNG_USER_MEM_SUPPORTED
      png_set_mem_fn(&create_struct, mem_ptr, malloc_fn, free_fn);
#  else
      PNG_UNUSED(mem_ptr)
      PNG_UNUSED(malloc_fn)
      PNG_UNUSED(free_fn)
#  endif

   /* (*error_fn) can return control to the caller after the error_ptr is set,
    * this will result in a memory leak unless the error_fn does something
    * extremely sophisticated.  The design lacks merit but is implicit in the
    * API.
    */
   png_set_error_fn(&create_struct, error_ptr, error_fn, warn_fn);

#  ifdef PNG_SETJMP_SUPPORTED
      if (!setjmp(create_jmp_buf))
#  endif
      {
#  ifdef PNG_SETJMP_SUPPORTED
         /* Temporarily fake out the longjmp information until we have
          * successfully completed this function.  This only works if we have
          * setjmp() support compiled in, but it is safe - this stuff should
          * never happen.
          */
         create_struct.jmp_buf_ptr = &create_jmp_buf;
         create_struct.jmp_buf_size = 0; /*stack allocation*/
         create_struct.longjmp_fn = longjmp;
#  endif
         /* Call the general version checker (shared with read and write code):
          */
         if (png_user_version_check(&create_struct, user_png_ver) != 0)
         {
            png_structrp png_ptr = png_voidcast(png_structrp,
                png_malloc_warn(&create_struct, (sizeof *png_ptr)));

            if (png_ptr != NULL)
            {
               /* png_ptr->zstream holds a back-pointer to the png_struct, so
                * this can only be done now:
                */
               create_struct.zstream.zalloc = png_zalloc;
               create_struct.zstream.zfree = png_zfree;
               create_struct.zstream.opaque = png_ptr;

#              ifdef PNG_SETJMP_SUPPORTED
               /* Eliminate the local error handling: */
               create_struct.jmp_buf_ptr = NULL;
               create_struct.jmp_buf_size = 0;
               create_struct.longjmp_fn = 0;
#              endif

               *png_ptr = create_struct;

               /* This is the successful return point */
               return png_ptr;
            }
         }
      }

   /* A longjmp because of a bug in the application storage allocator or a
    * simple failure to allocate the png_struct.
    */
   return NULL;
}