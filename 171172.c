g_file_load_contents_finish (GFile         *file,
                             GAsyncResult  *res,
                             char         **contents,
                             gsize         *length,
                             char         **etag_out,
                             GError       **error)
{
  return g_file_load_partial_contents_finish (file,
                                              res,
                                              contents,
                                              length,
                                              etag_out,
                                              error);
}