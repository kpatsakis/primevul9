void JPXStreamPrivate::init2(OPJ_CODEC_FORMAT format, unsigned char *buf, int length, bool indexed)
{
  JPXData jpxData;

  jpxData.data = buf;
  jpxData.pos = 0;
  jpxData.size = length;

  opj_stream_t *stream;

  stream = opj_stream_default_create(OPJ_TRUE);

#if OPENJPEG_VERSION >= OPENJPEG_VERSION_ENCODE(2, 1, 0)
  opj_stream_set_user_data (stream, &jpxData, nullptr);
#else
  opj_stream_set_user_data (stream, &jpxData);
#endif

  opj_stream_set_read_function(stream, jpxRead_callback);
  opj_stream_set_skip_function(stream, jpxSkip_callback);
  opj_stream_set_seek_function(stream, jpxSeek_callback);
  /* Set the length to avoid an assert */
  opj_stream_set_user_data_length(stream, length);

  opj_codec_t *decoder;

  /* Use default decompression parameters */
  opj_dparameters_t parameters;
  opj_set_default_decoder_parameters(&parameters);
  if (indexed)
    parameters.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;

  /* Get the decoder handle of the format */
  decoder = opj_create_decompress(format);
  if (decoder == nullptr) {
    error(errSyntaxWarning, -1, "Unable to create decoder");
    goto error;
  }

  /* Catch events using our callbacks */
  opj_set_warning_handler(decoder, libopenjpeg_warning_callback, nullptr);
  opj_set_error_handler(decoder, libopenjpeg_error_callback, nullptr);

  /* Setup the decoder decoding parameters */
  if (!opj_setup_decoder(decoder, &parameters)) {
    error(errSyntaxWarning, -1, "Unable to set decoder parameters");
    goto error;
  }

  /* Decode the stream and fill the image structure */
  image = nullptr;
  if (!opj_read_header(stream, decoder, &image)) {
    error(errSyntaxWarning, -1, "Unable to read header");
    goto error;
  }

  /* Optional if you want decode the entire image */
  if (!opj_set_decode_area(decoder, image, parameters.DA_x0,
                           parameters.DA_y0, parameters.DA_x1, parameters.DA_y1)){
    error(errSyntaxWarning, -1, "X2");
    goto error;
  }

  /* Get the decoded image */
  if (!(opj_decode(decoder, stream, image) && opj_end_decompress(decoder, stream))) {
    error(errSyntaxWarning, -1, "Unable to decode image");
    goto error;
  }

  opj_destroy_codec(decoder);
  opj_stream_destroy(stream);

  if (image != nullptr)
    return;

error:
  opj_stream_destroy(stream);
  opj_destroy_codec(decoder);
  if (format == OPJ_CODEC_JP2) {
    error(errSyntaxWarning, -1, "Did no succeed opening JPX Stream as JP2, trying as J2K.");
    init2(OPJ_CODEC_J2K, buf, length, indexed);
  } else if (format == OPJ_CODEC_J2K) {
    error(errSyntaxWarning, -1, "Did no succeed opening JPX Stream as J2K, trying as JPT.");
    init2(OPJ_CODEC_JPT, buf, length, indexed);
  } else {
    error(errSyntaxError, -1, "Did no succeed opening JPX Stream.");
  }
}