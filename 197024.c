long ContentEncoding::ParseCompressionEntry(long long start, long long size,
                                            IMkvReader* pReader,
                                            ContentCompression* compression) {
  assert(pReader);
  assert(compression);

  long long pos = start;
  const long long stop = start + size;

  bool valid = false;

  while (pos < stop) {
    long long id, size;
    const long status = ParseElementHeader(pReader, pos, stop, id, size);
    if (status < 0)  // error
      return status;

    if (id == libwebm::kMkvContentCompAlgo) {
      long long algo = UnserializeUInt(pReader, pos, size);
      if (algo < 0)
        return E_FILE_FORMAT_INVALID;
      compression->algo = algo;
      valid = true;
    } else if (id == libwebm::kMkvContentCompSettings) {
      if (size <= 0)
        return E_FILE_FORMAT_INVALID;

      const size_t buflen = static_cast<size_t>(size);
      unsigned char* buf = SafeArrayAlloc<unsigned char>(1, buflen);
      if (buf == NULL)
        return -1;

      const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
      if (read_status) {
        delete[] buf;
        return status;
      }

      // There should be only one settings element per content compression.
      if (compression->settings != NULL) {
        delete[] buf;
        return E_FILE_FORMAT_INVALID;
      }

      compression->settings = buf;
      compression->settings_len = buflen;
    }

    pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  // ContentCompAlgo is mandatory
  if (!valid)
    return E_FILE_FORMAT_INVALID;

  return 0;
}