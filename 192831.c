PNG_FUNCTION(voidpf /* PRIVATE */,
png_zalloc,(voidpf png_ptr, uInt items, uInt size),PNG_ALLOCATED)
{
   png_alloc_size_t num_bytes = size;

   if (png_ptr == NULL)
      return NULL;

   if (items >= (~(png_alloc_size_t)0)/size)
   {
      png_warning (png_voidcast(png_structrp, png_ptr),
          "Potential overflow in png_zalloc()");
      return NULL;
   }

   num_bytes *= items;
   return png_malloc_warn(png_voidcast(png_structrp, png_ptr), num_bytes);
}