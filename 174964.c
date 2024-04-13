Error Box::parse(BitstreamRange& range)
{
  // skip box

  if (get_box_size() == size_until_end_of_file) {
    range.skip_to_end_of_file();
  }
  else {
    uint64_t content_size = get_box_size() - get_header_size();
    if (range.prepare_read(content_size)) {
      if (content_size > MAX_BOX_SIZE) {
        return Error(heif_error_Invalid_input,
                     heif_suberror_Invalid_box_size);
      }

      range.get_istream()->seek_cur(get_box_size() - get_header_size());
    }
  }

  // Note: seekg() clears the eof flag and it will not be set again afterwards,
  // hence we have to test for the fail flag.

  return range.get_error();
}