cgi_unlink_file(void)
{
  if (form_file)
  {
   /*
    * Remove the temporary file...
    */

    unlink(form_file->tempfile);

   /*
    * Free memory used...
    */

    free(form_file->name);
    free(form_file->filename);
    free(form_file->mimetype);
    free(form_file);

    form_file = NULL;
  }
}