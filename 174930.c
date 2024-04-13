archive_read_format_rar_seek_data(struct archive_read *a, int64_t offset,
    int whence)
{
  int64_t client_offset, ret;
  unsigned int i;
  struct rar *rar = (struct rar *)(a->format->data);

  if (rar->compression_method == COMPRESS_METHOD_STORE)
  {
    /* Modify the offset for use with SEEK_SET */
    switch (whence)
    {
      case SEEK_CUR:
        client_offset = rar->offset_seek;
        break;
      case SEEK_END:
        client_offset = rar->unp_size;
        break;
      case SEEK_SET:
      default:
        client_offset = 0;
    }
    client_offset += offset;
    if (client_offset < 0)
    {
      /* Can't seek past beginning of data block */
      return -1;
    }
    else if (client_offset > rar->unp_size)
    {
      /*
       * Set the returned offset but only seek to the end of
       * the data block.
       */
      rar->offset_seek = client_offset;
      client_offset = rar->unp_size;
    }

    client_offset += rar->dbo[0].start_offset;
    i = 0;
    while (i < rar->cursor)
    {
      i++;
      client_offset += rar->dbo[i].start_offset - rar->dbo[i-1].end_offset;
    }
    if (rar->main_flags & MHD_VOLUME)
    {
      /* Find the appropriate offset among the multivolume archive */
      while (1)
      {
        if (client_offset < rar->dbo[rar->cursor].start_offset &&
          rar->file_flags & FHD_SPLIT_BEFORE)
        {
          /* Search backwards for the correct data block */
          if (rar->cursor == 0)
          {
            archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
              "Attempt to seek past beginning of RAR data block");
            return (ARCHIVE_FAILED);
          }
          rar->cursor--;
          client_offset -= rar->dbo[rar->cursor+1].start_offset -
            rar->dbo[rar->cursor].end_offset;
          if (client_offset < rar->dbo[rar->cursor].start_offset)
            continue;
          ret = __archive_read_seek(a, rar->dbo[rar->cursor].start_offset -
            rar->dbo[rar->cursor].header_size, SEEK_SET);
          if (ret < (ARCHIVE_OK))
            return ret;
          ret = archive_read_format_rar_read_header(a, a->entry);
          if (ret != (ARCHIVE_OK))
          {
            archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
              "Error during seek of RAR file");
            return (ARCHIVE_FAILED);
          }
          rar->cursor--;
          break;
        }
        else if (client_offset > rar->dbo[rar->cursor].end_offset &&
          rar->file_flags & FHD_SPLIT_AFTER)
        {
          /* Search forward for the correct data block */
          rar->cursor++;
          if (rar->cursor < rar->nodes &&
            client_offset > rar->dbo[rar->cursor].end_offset)
          {
            client_offset += rar->dbo[rar->cursor].start_offset -
              rar->dbo[rar->cursor-1].end_offset;
            continue;
          }
          rar->cursor--;
          ret = __archive_read_seek(a, rar->dbo[rar->cursor].end_offset,
                                    SEEK_SET);
          if (ret < (ARCHIVE_OK))
            return ret;
          ret = archive_read_format_rar_read_header(a, a->entry);
          if (ret == (ARCHIVE_EOF))
          {
            rar->has_endarc_header = 1;
            ret = archive_read_format_rar_read_header(a, a->entry);
          }
          if (ret != (ARCHIVE_OK))
          {
            archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
              "Error during seek of RAR file");
            return (ARCHIVE_FAILED);
          }
          client_offset += rar->dbo[rar->cursor].start_offset -
            rar->dbo[rar->cursor-1].end_offset;
          continue;
        }
        break;
      }
    }

    ret = __archive_read_seek(a, client_offset, SEEK_SET);
    if (ret < (ARCHIVE_OK))
      return ret;
    rar->bytes_remaining = rar->dbo[rar->cursor].end_offset - ret;
    i = rar->cursor;
    while (i > 0)
    {
      i--;
      ret -= rar->dbo[i+1].start_offset - rar->dbo[i].end_offset;
    }
    ret -= rar->dbo[0].start_offset;

    /* Always restart reading the file after a seek */
    __archive_reset_read_data(&a->archive);

    rar->bytes_unconsumed = 0;
    rar->offset = 0;

    /*
     * If a seek past the end of file was requested, return the requested
     * offset.
     */
    if (ret == rar->unp_size && rar->offset_seek > rar->unp_size)
      return rar->offset_seek;

    /* Return the new offset */
    rar->offset_seek = ret;
    return rar->offset_seek;
  }
  else
  {
    archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
      "Seeking of compressed RAR files is unsupported");
  }
  return (ARCHIVE_FAILED);
}