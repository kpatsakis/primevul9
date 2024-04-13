static void print_enc_string(FILE* fp, OnigEncoding enc,
                             const UChar *s, const UChar *end)
{
  if (ONIGENC_MBC_MINLEN(enc) > 1) {
    const UChar *p;
    OnigCodePoint code;

    p = s;
    while (p < end) {
      code = ONIGENC_MBC_TO_CODE(enc, p, end);
      if (code >= 0x80) {
        fprintf(fp, " 0x%04x ", (int )code);
      }
      else {
        fputc((int )code, fp);
      }

      p += enclen(enc, p);
    }
  }
  else {
    while (s < end) {
      fputc((int )*s, fp);
      s++;
    }
  }

  fprintf(fp, "/\n");
}