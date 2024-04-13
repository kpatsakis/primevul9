is_ICC_signature_char(png_alloc_size_t it)
{
   return it == 32 || (it >= 48 && it <= 57) || (it >= 65 && it <= 90) ||
      (it >= 97 && it <= 122);
}