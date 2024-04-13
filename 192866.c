png_image_free(png_imagep image)
{
   /* Safely call the real function, but only if doing so is safe at this point
    * (if not inside an error handling context).  Otherwise assume
    * png_safe_execute will call this API after the return.
    */
   if (image != NULL && image->opaque != NULL &&
      image->opaque->error_buf == NULL)
   {
      /* Ignore errors here: */
      (void)png_safe_execute(image, png_image_free_function, image);
      image->opaque = NULL;
   }
}