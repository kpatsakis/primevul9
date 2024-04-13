qtdemux_parse_samples (GstQTDemux * qtdemux, QtDemuxStream * stream, guint32 n)
{
  gint i, j, k;
  QtDemuxSample *samples, *first, *cur, *last;
  guint32 n_samples_per_chunk;
  guint32 n_samples;

  GST_LOG_OBJECT (qtdemux, "parsing samples for stream fourcc %"
      GST_FOURCC_FORMAT ", pad %s", GST_FOURCC_ARGS (stream->fourcc),
      stream->pad ? GST_PAD_NAME (stream->pad) : "(NULL)");

  n_samples = stream->n_samples;

  if (n >= n_samples)
    goto out_of_samples;

  GST_OBJECT_LOCK (qtdemux);
  if (n <= stream->stbl_index)
    goto already_parsed;

  GST_DEBUG_OBJECT (qtdemux, "parsing up to sample %u", n);

  if (!stream->stsz.data) {
    /* so we already parsed and passed all the moov samples;
     * onto fragmented ones */
    g_assert (qtdemux->fragmented);
    goto done;
  }

  /* pointer to the sample table */
  samples = stream->samples;

  /* starts from -1, moves to the next sample index to parse */
  stream->stbl_index++;

  /* keep track of the first and last sample to fill */
  first = &samples[stream->stbl_index];
  last = &samples[n];

  if (!stream->chunks_are_samples) {
    /* set the sample sizes */
    if (stream->sample_size == 0) {
      /* different sizes for each sample */
      for (cur = first; cur <= last; cur++) {
        cur->size = gst_byte_reader_get_uint32_be_unchecked (&stream->stsz);
        GST_LOG_OBJECT (qtdemux, "sample %d has size %u",
            (guint) (cur - samples), cur->size);
      }
    } else {
      /* samples have the same size */
      GST_LOG_OBJECT (qtdemux, "all samples have size %u", stream->sample_size);
      for (cur = first; cur <= last; cur++)
        cur->size = stream->sample_size;
    }
  }

  n_samples_per_chunk = stream->n_samples_per_chunk;
  cur = first;

  for (i = stream->stsc_index; i < n_samples_per_chunk; i++) {
    guint32 last_chunk;

    if (stream->stsc_chunk_index >= stream->last_chunk
        || stream->stsc_chunk_index < stream->first_chunk) {
      stream->first_chunk =
          gst_byte_reader_get_uint32_be_unchecked (&stream->stsc);
      stream->samples_per_chunk =
          gst_byte_reader_get_uint32_be_unchecked (&stream->stsc);
      gst_byte_reader_skip_unchecked (&stream->stsc, 4);

      /* chunk numbers are counted from 1 it seems */
      if (G_UNLIKELY (stream->first_chunk == 0))
        goto corrupt_file;

      --stream->first_chunk;

      /* the last chunk of each entry is calculated by taking the first chunk
       * of the next entry; except if there is no next, where we fake it with
       * INT_MAX */
      if (G_UNLIKELY (i == (stream->n_samples_per_chunk - 1))) {
        stream->last_chunk = G_MAXUINT32;
      } else {
        stream->last_chunk =
            gst_byte_reader_peek_uint32_be_unchecked (&stream->stsc);
        if (G_UNLIKELY (stream->last_chunk == 0))
          goto corrupt_file;

        --stream->last_chunk;
      }

      GST_LOG_OBJECT (qtdemux,
          "entry %d has first_chunk %d, last_chunk %d, samples_per_chunk %d", i,
          stream->first_chunk, stream->last_chunk, stream->samples_per_chunk);

      if (G_UNLIKELY (stream->last_chunk < stream->first_chunk))
        goto corrupt_file;

      if (stream->last_chunk != G_MAXUINT32) {
        if (!qt_atom_parser_peek_sub (&stream->stco,
                stream->first_chunk * stream->co_size,
                (stream->last_chunk - stream->first_chunk) * stream->co_size,
                &stream->co_chunk))
          goto corrupt_file;

      } else {
        stream->co_chunk = stream->stco;
        if (!gst_byte_reader_skip (&stream->co_chunk,
                stream->first_chunk * stream->co_size))
          goto corrupt_file;
      }

      stream->stsc_chunk_index = stream->first_chunk;
    }

    last_chunk = stream->last_chunk;

    if (stream->chunks_are_samples) {
      cur = &samples[stream->stsc_chunk_index];

      for (j = stream->stsc_chunk_index; j < last_chunk; j++) {
        if (j > n) {
          /* save state */
          stream->stsc_chunk_index = j;
          goto done;
        }

        cur->offset =
            qt_atom_parser_get_offset_unchecked (&stream->co_chunk,
            stream->co_size);

        GST_LOG_OBJECT (qtdemux, "Created entry %d with offset "
            "%" G_GUINT64_FORMAT, j, cur->offset);

        if (stream->samples_per_frame * stream->bytes_per_frame) {
          cur->size =
              (stream->samples_per_chunk * stream->n_channels) /
              stream->samples_per_frame * stream->bytes_per_frame;
        } else {
          cur->size = stream->samples_per_chunk;
        }

        GST_DEBUG_OBJECT (qtdemux,
            "keyframe sample %d: timestamp %" GST_TIME_FORMAT ", size %u",
            j, GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream,
                    stream->stco_sample_index)), cur->size);

        cur->timestamp = stream->stco_sample_index;
        cur->duration = stream->samples_per_chunk;
        cur->keyframe = TRUE;
        cur++;

        stream->stco_sample_index += stream->samples_per_chunk;
      }
      stream->stsc_chunk_index = j;
    } else {
      for (j = stream->stsc_chunk_index; j < last_chunk; j++) {
        guint32 samples_per_chunk;
        guint64 chunk_offset;

        if (!stream->stsc_sample_index
            && !qt_atom_parser_get_offset (&stream->co_chunk, stream->co_size,
                &stream->chunk_offset))
          goto corrupt_file;

        samples_per_chunk = stream->samples_per_chunk;
        chunk_offset = stream->chunk_offset;

        for (k = stream->stsc_sample_index; k < samples_per_chunk; k++) {
          GST_LOG_OBJECT (qtdemux, "creating entry %d with offset %"
              G_GUINT64_FORMAT " and size %d",
              (guint) (cur - samples), chunk_offset, cur->size);

          cur->offset = chunk_offset;
          chunk_offset += cur->size;
          cur++;

          if (G_UNLIKELY (cur > last)) {
            /* save state */
            stream->stsc_sample_index = k + 1;
            stream->chunk_offset = chunk_offset;
            stream->stsc_chunk_index = j;
            goto done2;
          }
        }
        stream->stsc_sample_index = 0;
      }
      stream->stsc_chunk_index = j;
    }
    stream->stsc_index++;
  }

  if (stream->chunks_are_samples)
    goto ctts;
done2:
  {
    guint32 n_sample_times;

    n_sample_times = stream->n_sample_times;
    cur = first;

    for (i = stream->stts_index; i < n_sample_times; i++) {
      guint32 stts_samples;
      gint32 stts_duration;
      gint64 stts_time;

      if (stream->stts_sample_index >= stream->stts_samples
          || !stream->stts_sample_index) {

        stream->stts_samples =
            gst_byte_reader_get_uint32_be_unchecked (&stream->stts);
        stream->stts_duration =
            gst_byte_reader_get_uint32_be_unchecked (&stream->stts);

        GST_LOG_OBJECT (qtdemux, "block %d, %u timestamps, duration %u",
            i, stream->stts_samples, stream->stts_duration);

        stream->stts_sample_index = 0;
      }

      stts_samples = stream->stts_samples;
      stts_duration = stream->stts_duration;
      stts_time = stream->stts_time;

      for (j = stream->stts_sample_index; j < stts_samples; j++) {
        GST_DEBUG_OBJECT (qtdemux,
            "sample %d: index %d, timestamp %" GST_TIME_FORMAT,
            (guint) (cur - samples), j,
            GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream, stts_time)));

        cur->timestamp = stts_time;
        cur->duration = stts_duration;

        /* avoid 32-bit wrap-around,
         * but still mind possible 'negative' duration */
        stts_time += (gint64) stts_duration;
        cur++;

        if (G_UNLIKELY (cur > last)) {
          /* save values */
          stream->stts_time = stts_time;
          stream->stts_sample_index = j + 1;
          goto done3;
        }
      }
      stream->stts_sample_index = 0;
      stream->stts_time = stts_time;
      stream->stts_index++;
    }
    /* fill up empty timestamps with the last timestamp, this can happen when
     * the last samples do not decode and so we don't have timestamps for them.
     * We however look at the last timestamp to estimate the track length so we
     * need something in here. */
    for (; cur < last; cur++) {
      GST_DEBUG_OBJECT (qtdemux,
          "fill sample %d: timestamp %" GST_TIME_FORMAT,
          (guint) (cur - samples),
          GST_TIME_ARGS (QTSTREAMTIME_TO_GSTTIME (stream, stream->stts_time)));
      cur->timestamp = stream->stts_time;
      cur->duration = -1;
    }
  }
done3:
  {
    /* sample sync, can be NULL */
    if (stream->stss_present == TRUE) {
      guint32 n_sample_syncs;

      n_sample_syncs = stream->n_sample_syncs;

      if (!n_sample_syncs) {
        GST_DEBUG_OBJECT (qtdemux, "all samples are keyframes");
        stream->all_keyframe = TRUE;
      } else {
        for (i = stream->stss_index; i < n_sample_syncs; i++) {
          /* note that the first sample is index 1, not 0 */
          guint32 index;

          index = gst_byte_reader_get_uint32_be_unchecked (&stream->stss);

          if (G_LIKELY (index > 0 && index <= n_samples)) {
            index -= 1;
            samples[index].keyframe = TRUE;
            GST_DEBUG_OBJECT (qtdemux, "samples at %u is keyframe", index);
            /* and exit if we have enough samples */
            if (G_UNLIKELY (index >= n)) {
              i++;
              break;
            }
          }
        }
        /* save state */
        stream->stss_index = i;
      }

      /* stps marks partial sync frames like open GOP I-Frames */
      if (stream->stps_present == TRUE) {
        guint32 n_sample_partial_syncs;

        n_sample_partial_syncs = stream->n_sample_partial_syncs;

        /* if there are no entries, the stss table contains the real
         * sync samples */
        if (n_sample_partial_syncs) {
          for (i = stream->stps_index; i < n_sample_partial_syncs; i++) {
            /* note that the first sample is index 1, not 0 */
            guint32 index;

            index = gst_byte_reader_get_uint32_be_unchecked (&stream->stps);

            if (G_LIKELY (index > 0 && index <= n_samples)) {
              index -= 1;
              samples[index].keyframe = TRUE;
              GST_DEBUG_OBJECT (qtdemux, "samples at %u is keyframe", index);
              /* and exit if we have enough samples */
              if (G_UNLIKELY (index >= n)) {
                i++;
                break;
              }
            }
          }
          /* save state */
          stream->stps_index = i;
        }
      }
    } else {
      /* no stss, all samples are keyframes */
      stream->all_keyframe = TRUE;
      GST_DEBUG_OBJECT (qtdemux, "setting all keyframes");
    }
  }

ctts:
  /* composition time to sample */
  if (stream->ctts_present == TRUE) {
    guint32 n_composition_times;
    guint32 ctts_count;
    gint32 ctts_soffset;

    /* Fill in the pts_offsets */
    cur = first;
    n_composition_times = stream->n_composition_times;

    for (i = stream->ctts_index; i < n_composition_times; i++) {
      if (stream->ctts_sample_index >= stream->ctts_count
          || !stream->ctts_sample_index) {
        stream->ctts_count =
            gst_byte_reader_get_uint32_be_unchecked (&stream->ctts);
        stream->ctts_soffset =
            gst_byte_reader_get_int32_be_unchecked (&stream->ctts);
        stream->ctts_sample_index = 0;
      }

      ctts_count = stream->ctts_count;
      ctts_soffset = stream->ctts_soffset;

      for (j = stream->ctts_sample_index; j < ctts_count; j++) {
        cur->pts_offset = ctts_soffset;
        cur++;

        if (G_UNLIKELY (cur > last)) {
          /* save state */
          stream->ctts_sample_index = j + 1;
          goto done;
        }
      }
      stream->ctts_sample_index = 0;
      stream->ctts_index++;
    }
  }
done:
  stream->stbl_index = n;
  /* if index has been completely parsed, free data that is no-longer needed */
  if (n + 1 == stream->n_samples) {
    gst_qtdemux_stbl_free (stream);
    GST_DEBUG_OBJECT (qtdemux, "parsed all available samples;");
    if (qtdemux->pullbased) {
      GST_DEBUG_OBJECT (qtdemux, "checking for more samples");
      while (n + 1 == stream->n_samples)
        if (qtdemux_add_fragmented_samples (qtdemux) != GST_FLOW_OK)
          break;
    }
  }
  GST_OBJECT_UNLOCK (qtdemux);

  return TRUE;

  /* SUCCESS */
already_parsed:
  {
    GST_LOG_OBJECT (qtdemux,
        "Tried to parse up to sample %u but this sample has already been parsed",
        n);
    /* if fragmented, there may be more */
    if (qtdemux->fragmented && n == stream->stbl_index)
      goto done;
    GST_OBJECT_UNLOCK (qtdemux);
    return TRUE;
  }
  /* ERRORS */
out_of_samples:
  {
    GST_LOG_OBJECT (qtdemux,
        "Tried to parse up to sample %u but there are only %u samples", n + 1,
        stream->n_samples);
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file is corrupt and cannot be played.")), (NULL));
    return FALSE;
  }
corrupt_file:
  {
    GST_OBJECT_UNLOCK (qtdemux);
    GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
        (_("This file is corrupt and cannot be played.")), (NULL));
    return FALSE;
  }
}