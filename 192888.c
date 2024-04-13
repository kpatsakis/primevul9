png_check_fp_string(png_const_charp string, png_size_t size)
{
   int        state=0;
   png_size_t char_index=0;

   if (png_check_fp_number(string, size, &state, &char_index) != 0 &&
      (char_index == size || string[char_index] == 0))
      return state /* must be non-zero - see above */;

   return 0; /* i.e. fail */
}