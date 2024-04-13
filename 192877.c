png_image_error(png_imagep image, png_const_charp error_message)
{
   /* Utility to log an error. */
   png_safecat(image->message, (sizeof image->message), 0, error_message);
   image->warning_or_error |= PNG_IMAGE_ERROR;
   png_image_free(image);
   return 0;
}