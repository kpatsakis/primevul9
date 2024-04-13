png_zfree(voidpf png_ptr, voidpf ptr)
{
   png_free(png_voidcast(png_const_structrp,png_ptr), ptr);
}