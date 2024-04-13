give_local_error(int errcode, uschar *text1, uschar *text2, int error_rc,
  FILE *f, header_line *hptr)
{
if (error_handling == ERRORS_SENDER)
  {
  error_block eblock;
  eblock.next = NULL;
  eblock.text1 = text1;
  eblock.text2 = US"";
  if (!moan_to_sender(errcode, &eblock, hptr, f, FALSE))
    error_rc = EXIT_FAILURE;
  }
else
  fprintf(stderr, "exim: %s%s\n", text2, text1);  /* Sic */
(void)fclose(f);
exim_exit(error_rc, US"");
}