qtdemux_parse_trun (GstQTDemux * qtdemux, GstByteReader * trun,
    QtDemuxStream * stream, guint32 d_sample_duration, guint32 d_sample_size,
    guint32 d_sample_flags, gint64 moof_offset, gint64 moof_length,
    gint64 * base_offset, gint64 * running_offset, gint64 decode_ts)
{
  GstClockTime gst_ts = GST_CLOCK_TIME_NONE;
  guint64 timestamp;
  gint32 data_offset = 0;
  guint32 flags = 0, first_flags = 0, samples_count = 0;
  gint i;
  guint8 *data;
  guint entry_size, dur_offset, size_offset, flags_offset = 0, ct_offset = 0;
  QtDemuxSample *sample;
  gboolean ismv = FALSE;
  gint64 initial_offset;

  GST_LOG_OBJECT (qtdemux, "parsing trun stream %d; "
      "default dur %d, size %d, flags 0x%x, base offset %" G_GINT64_FORMAT ", "
      "decode ts %" G_GINT64_FORMAT, stream->track_id, d_sample_duration,
      d_sample_size, d_sample_flags, *base_offset, decode_ts);

  if (stream->pending_seek && moof_offset < stream->pending_seek->moof_offset) {
    GST_INFO_OBJECT (stream->pad, "skipping trun before seek target fragment");
    return TRUE;
  }

  /* presence of stss or not can't really tell us much,
   * and flags and so on tend to be marginally reliable in these files */
  if (stream->subtype == FOURCC_soun) {
    GST_DEBUG_OBJECT (qtdemux,
        "sound track in fragmented file; marking all keyframes");
    stream->all_keyframe = TRUE;
  }

  if (!gst_byte_reader_skip (trun, 1) ||
      !gst_byte_reader_get_uint24_be (trun, &flags))
    goto fail;

  if (!gst_byte_reader_get_uint32_be (trun, &samples_count))
    goto fail;

  if (flags & TR_DATA_OFFSET) {
    /* note this is really signed */
    if (!gst_byte_reader_get_int32_be (trun, &data_offset))
      goto fail;
    GST_LOG_OBJECT (qtdemux, "trun data offset %d", data_offset);
    /* default base offset = first byte of moof */
    if (*base_offset == -1) {
      GST_LOG_OBJECT (qtdemux, "base_offset at moof");
      *base_offset = moof_offset;
    }
    *running_offset = *base_offset + data_offset;
  } else {
    /* if no offset at all, that would mean data starts at moof start,
     * which is a bit wrong and is ismv crappy way, so compensate
     * assuming data is in mdat following moof */
    if (*base_offset == -1) {
      *base_offset = moof_offset + moof_length + 8;
      GST_LOG_OBJECT (qtdemux, "base_offset assumed in mdat after moof");
      ismv = TRUE;
    }
    if (*running_offset == -1)
      *running_offset = *base_offset;
  }

  GST_LOG_OBJECT (qtdemux, "running offset now %" G_GINT64_FORMAT,
      *running_offset);
  GST_LOG_OBJECT (qtdemux, "trun offset %d, flags 0x%x, entries %d",
      data_offset, flags, samples_count);

  if (flags & TR_FIRST_SAMPLE_FLAGS) {
    if (G_UNLIKELY (flags & TR_SAMPLE_FLAGS)) {
      GST_DEBUG_OBJECT (qtdemux,
          "invalid flags; SAMPLE and FIRST_SAMPLE present, discarding latter");
      flags ^= TR_FIRST_SAMPLE_FLAGS;
    } else {
      if (!gst_byte_reader_get_uint32_be (trun, &first_flags))
        goto fail;
      GST_LOG_OBJECT (qtdemux, "first flags: 0x%x", first_flags);
    }
  }

  /* FIXME ? spec says other bits should also be checked to determine
   * entry size (and prefix size for that matter) */
  entry_size = 0;
  dur_offset = size_offset = 0;
  if (flags & TR_SAMPLE_DURATION) {
    GST_LOG_OBJECT (qtdemux, "entry duration present");
    dur_offset = entry_size;
    entry_size += 4;
  }
  if (flags & TR_SAMPLE_SIZE) {
    GST_LOG_OBJECT (qtdemux, "entry size present");
    size_offset = entry_size;
    entry_size += 4;
  }
  if (flags & TR_SAMPLE_FLAGS) {
    GST_LOG_OBJECT (qtdemux, "entry flags present");
    flags_offset = entry_size;
    entry_size += 4;
  }
  if (flags & TR_COMPOSITION_TIME_OFFSETS) {
    GST_LOG_OBJECT (qtdemux, "entry ct offset present");
    ct_offset = entry_size;
    entry_size += 4;
  }

  if (!qt_atom_parser_has_chunks (trun, samples_count, entry_size))
    goto fail;
  data = (guint8 *) gst_byte_reader_peek_data_unchecked (trun);

  if (stream->n_samples + samples_count >=
      QTDEMUX_MAX_SAMPLE_INDEX_SIZE / sizeof (QtDemuxSample))
    goto index_too_big;

  GST_DEBUG_OBJECT (qtdemux, "allocating n_samples %u * %u (%.2f MB)",
      stream->n_samples + samples_count, (guint) sizeof (QtDemuxSample),
      (stream->n_samples + samples_count) *
      sizeof (QtDemuxSample) / (1024.0 * 1024.0));

  /* create a new array of samples if it's the first sample parsed */
  if (stream->n_samples == 0) {
    g_assert (stream->samples == NULL);
    stream->samples = g_try_new0 (QtDemuxSample, samples_count);
    /* or try to reallocate it with space enough to insert the new samples */
  } else
    stream->samples = g_try_renew (QtDemuxSample, stream->samples,
        stream->n_samples + samples_count);
  if (stream->samples == NULL)
    goto out_of_memory;

  if (qtdemux->fragment_start != -1) {
    timestamp = GSTTIME_TO_QTSTREAMTIME (stream, qtdemux->fragment_start);
    qtdemux->fragment_start = -1;
  } else {
    if (stream->n_samples == 0) {
      if (decode_ts > 0) {
        timestamp = decode_ts;
      } else if (stream->pending_seek != NULL) {
        /* if we don't have a timestamp from a tfdt box, we'll use the one
         * from the mfra seek table */
        GST_INFO_OBJECT (stream->pad, "pending seek ts = %" GST_TIME_FORMAT,
            GST_TIME_ARGS (stream->pending_seek->ts));

        /* FIXME: this is not fully correct, the timestamp refers to the random
         * access sample refered to in the tfra entry, which may not necessarily
         * be the first sample in the tfrag/trun (but hopefully/usually is) */
        timestamp = GSTTIME_TO_QTSTREAMTIME (stream, stream->pending_seek->ts);
      } else {
        timestamp = 0;
      }

      gst_ts = QTSTREAMTIME_TO_GSTTIME (stream, timestamp);
      GST_INFO_OBJECT (stream->pad, "first sample ts %" GST_TIME_FORMAT,
          GST_TIME_ARGS (gst_ts));
    } else {
      /* subsequent fragments extend stream */
      timestamp =
          stream->samples[stream->n_samples - 1].timestamp +
          stream->samples[stream->n_samples - 1].duration;

      /* If this is a GST_FORMAT_BYTES stream and there's a significant
       * difference (1 sec.) between decode_ts and timestamp, prefer the
       * former */
      if (!qtdemux->upstream_format_is_time
          && ABSDIFF (decode_ts, timestamp) >
          MAX (stream->duration_last_moof / 2,
              GSTTIME_TO_QTSTREAMTIME (stream, GST_SECOND))) {
        GST_INFO_OBJECT (qtdemux,
            "decode_ts (%" GST_TIME_FORMAT ") and timestamp (%" GST_TIME_FORMAT
            ") are significantly different (more than %" GST_TIME_FORMAT
            "), using decode_ts",
            GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream, decode_ts)),
            GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream, timestamp)),
            GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream,
                    MAX (stream->duration_last_moof / 2,
                        GSTTIME_TO_QTSTREAMTIME (stream, GST_SECOND)))));
        timestamp = decode_ts;
      }

      gst_ts = QTSTREAMTIME_TO_GSTTIME (stream, timestamp);
      GST_INFO_OBJECT (qtdemux, "first sample ts %" GST_TIME_FORMAT
          " (extends previous samples)", GST_TIME_ARGS (gst_ts));
    }
  }

  initial_offset = *running_offset;

  sample = stream->samples + stream->n_samples;
  for (i = 0; i < samples_count; i++) {
    guint32 dur, size, sflags, ct;

    /* first read sample data */
    if (flags & TR_SAMPLE_DURATION) {
      dur = QT_UINT32 (data + dur_offset);
    } else {
      dur = d_sample_duration;
    }
    if (flags & TR_SAMPLE_SIZE) {
      size = QT_UINT32 (data + size_offset);
    } else {
      size = d_sample_size;
    }
    if (flags & TR_FIRST_SAMPLE_FLAGS) {
      if (i == 0) {
        sflags = first_flags;
      } else {
        sflags = d_sample_flags;
      }
    } else if (flags & TR_SAMPLE_FLAGS) {
      sflags = QT_UINT32 (data + flags_offset);
    } else {
      sflags = d_sample_flags;
    }
    if (flags & TR_COMPOSITION_TIME_OFFSETS) {
      ct = QT_UINT32 (data + ct_offset);
    } else {
      ct = 0;
    }
    data += entry_size;

    /* fill the sample information */
    sample->offset = *running_offset;
    sample->pts_offset = ct;
    sample->size = size;
    sample->timestamp = timestamp;
    sample->duration = dur;
    /* sample-is-difference-sample */
    /* ismv seems to use 0x40 for keyframe, 0xc0 for non-keyframe,
     * now idea how it relates to bitfield other than massive LE/BE confusion */
    sample->keyframe = ismv ? ((sflags & 0xff) == 0x40) : !(sflags & 0x10000);
    *running_offset += size;
    timestamp += dur;
    stream->duration_moof += dur;
    sample++;
  }

  /* Update total duration if needed */
  check_update_duration (qtdemux, QTSTREAMTIME_TO_GSTTIME (stream, timestamp));

  /* Pre-emptively figure out size of mdat based on trun information.
   * If the [mdat] atom is effectivelly read, it will be replaced by the actual
   * size, else we will still be able to use this when dealing with gap'ed
   * input */
  qtdemux->mdatleft = *running_offset - initial_offset;

  stream->n_samples += samples_count;
  stream->n_samples_moof += samples_count;

  if (stream->pending_seek != NULL)
    stream->pending_seek = NULL;

  return TRUE;

fail:
  {
    GST_WARNING_OBJECT (qtdemux, "failed to parse trun");
    return FALSE;
  }
out_of_memory:
  {
    GST_WARNING_OBJECT (qtdemux, "failed to allocate %d samples",
        stream->n_samples);
    return FALSE;
  }
index_too_big:
  {
    GST_WARNING_OBJECT (qtdemux, "not allocating index of %d samples, would "
        "be larger than %uMB (broken file?)", stream->n_samples,
        QTDEMUX_MAX_SAMPLE_INDEX_SIZE >> 20);
    return FALSE;
  }
}