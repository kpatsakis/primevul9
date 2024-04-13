png_build_8bit_table(png_structrp png_ptr, png_bytepp ptable,
    PNG_CONST png_fixed_point gamma_val)
{
   unsigned int i;
   png_bytep table = *ptable = (png_bytep)png_malloc(png_ptr, 256);

   if (png_gamma_significant(gamma_val) != 0)
      for (i=0; i<256; i++)
         table[i] = png_gamma_8bit_correct(i, gamma_val);

   else
      for (i=0; i<256; ++i)
         table[i] = (png_byte)(i & 0xff);
}