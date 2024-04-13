png_gamma_significant(png_fixed_point gamma_val)
{
   return gamma_val < PNG_FP_1 - PNG_GAMMA_THRESHOLD_FIXED ||
       gamma_val > PNG_FP_1 + PNG_GAMMA_THRESHOLD_FIXED;
}