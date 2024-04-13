_create_stream (void)
{
  QtDemuxStream *stream;

  stream = g_new0 (QtDemuxStream, 1);
  /* new streams always need a discont */
  stream->discont = TRUE;
  /* we enable clipping for raw audio/video streams */
  stream->need_clip = FALSE;
  stream->need_process = FALSE;
  stream->segment_index = -1;
  stream->time_position = 0;
  stream->sample_index = -1;
  stream->offset_in_sample = 0;
  stream->new_stream = TRUE;
  stream->multiview_mode = GST_VIDEO_MULTIVIEW_MODE_NONE;
  stream->multiview_flags = GST_VIDEO_MULTIVIEW_FLAGS_NONE;
  stream->protected = FALSE;
  stream->protection_scheme_type = 0;
  stream->protection_scheme_version = 0;
  stream->protection_scheme_info = NULL;
  stream->n_samples_moof = 0;
  stream->duration_moof = 0;
  stream->duration_last_moof = 0;
  stream->alignment = 1;
  g_queue_init (&stream->protection_scheme_event_queue);
  return stream;
}