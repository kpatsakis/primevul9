png_gamma_correct(png_structrp png_ptr, unsigned int value,
    png_fixed_point gamma_val)
{
   if (png_ptr->bit_depth == 8)
      return png_gamma_8bit_correct(value, gamma_val);

#ifdef PNG_16BIT_SUPPORTED
   else
      return png_gamma_16bit_correct(value, gamma_val);
#else
      /* should not reach this */
      return 0;
#endif /* 16BIT */
}