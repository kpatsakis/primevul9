void LibRaw::derror()
{
  if (!libraw_internal_data.unpacker_data.data_error && libraw_internal_data.internal_data.input) 
    {
      if (libraw_internal_data.internal_data.input->eof())
        {
          if(callbacks.data_cb)(*callbacks.data_cb)(callbacks.datacb_data,
                                                    libraw_internal_data.internal_data.input->fname(),-1);
          throw LIBRAW_EXCEPTION_IO_EOF;
        }
      else
        {
          if(callbacks.data_cb)(*callbacks.data_cb)(callbacks.datacb_data,
                                                    libraw_internal_data.internal_data.input->fname(),
                                                    libraw_internal_data.internal_data.input->tell());
          throw LIBRAW_EXCEPTION_IO_CORRUPT;
        }
    }
  libraw_internal_data.unpacker_data.data_error++;
}