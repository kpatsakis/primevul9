gst_matroska_demux_search_pos (GstMatroskaDemux * demux, GstClockTime time)
{
  GstMatroskaIndex *entry = NULL;
  GstMatroskaReadState current_state;
  GstClockTime otime, prev_cluster_time, current_cluster_time, cluster_time;
  GstClockTime atime;
  gint64 opos, newpos, current_offset;
  gint64 prev_cluster_offset = -1, current_cluster_offset, cluster_offset;
  gint64 apos, maxpos;
  guint64 cluster_size = 0;
  GstFlowReturn ret;
  guint64 length;
  guint32 id;
  guint needed;

  /* estimate new position, resync using cluster ebml id,
   * and bisect further or scan forward to appropriate cluster */

  /* save some current global state which will be touched by our scanning */
  current_state = demux->common.state;
  g_return_val_if_fail (current_state == GST_MATROSKA_READ_STATE_DATA, NULL);

  current_cluster_offset = demux->cluster_offset;
  current_cluster_time = demux->cluster_time;
  current_offset = demux->common.offset;

  demux->common.state = GST_MATROSKA_READ_STATE_SCANNING;

  /* estimate using start and last known cluster */
  GST_OBJECT_LOCK (demux);
  apos = demux->first_cluster_offset;
  atime = demux->stream_start_time;
  opos = demux->last_cluster_offset;
  otime = demux->stream_last_time;
  GST_OBJECT_UNLOCK (demux);

  /* sanitize */
  time = MAX (time, atime);
  otime = MAX (otime, atime);
  opos = MAX (opos, apos);

  maxpos = gst_matroska_read_common_get_length (&demux->common);

  /* invariants;
   * apos <= opos
   * atime <= otime
   * apos always refer to a cluster before target time;
   * opos may or may not be after target time, but if it is once so,
   * then also in next iteration
   * */

retry:
  GST_LOG_OBJECT (demux,
      "apos: %" G_GUINT64_FORMAT ", atime: %" GST_TIME_FORMAT ", %"
      GST_TIME_FORMAT " in stream time, "
      "opos: %" G_GUINT64_FORMAT ", otime: %" GST_TIME_FORMAT ", %"
      GST_TIME_FORMAT " in stream time (start %" GST_TIME_FORMAT "), time %"
      GST_TIME_FORMAT, apos, GST_TIME_ARGS (atime),
      GST_TIME_ARGS (atime - demux->stream_start_time), opos,
      GST_TIME_ARGS (otime), GST_TIME_ARGS (otime - demux->stream_start_time),
      GST_TIME_ARGS (demux->stream_start_time), GST_TIME_ARGS (time));

  g_assert (atime <= otime);
  g_assert (apos <= opos);
  if (time == GST_CLOCK_TIME_NONE) {
    GST_DEBUG_OBJECT (demux, "searching last cluster");
    newpos = maxpos;
    if (newpos == -1) {
      GST_DEBUG_OBJECT (demux, "unknown file size; bailing out");
      goto exit;
    }
  } else if (otime <= atime) {
    newpos = apos;
  } else {
    newpos = apos +
        gst_util_uint64_scale (opos - apos, time - atime, otime - atime);
    if (maxpos != -1 && newpos > maxpos)
      newpos = maxpos;
  }

  GST_DEBUG_OBJECT (demux,
      "estimated offset for %" GST_TIME_FORMAT ": %" G_GINT64_FORMAT,
      GST_TIME_ARGS (time), newpos);

  /* search backwards */
  if (newpos > apos) {
    ret = gst_matroska_demux_search_cluster (demux, &newpos, FALSE);
    if (ret != GST_FLOW_OK)
      goto exit;
  }

  /* then start scanning and parsing for cluster time,
   * re-estimate if possible, otherwise next cluster and so on */
  /* note that each re-estimate is entered with a change in apos or opos,
   * avoiding infinite loop */
  demux->common.offset = newpos;
  demux->cluster_time = cluster_time = GST_CLOCK_TIME_NONE;
  cluster_size = 0;
  prev_cluster_time = GST_CLOCK_TIME_NONE;
  while (1) {
    /* peek and parse some elements */
    ret = gst_matroska_read_common_peek_id_length_pull (&demux->common,
        GST_ELEMENT_CAST (demux), &id, &length, &needed);
    if (ret != GST_FLOW_OK)
      goto error;
    GST_LOG_OBJECT (demux, "Offset %" G_GUINT64_FORMAT ", Element id 0x%x, "
        "size %" G_GUINT64_FORMAT ", needed %d", demux->common.offset, id,
        length, needed);
    ret = gst_matroska_demux_parse_id (demux, id, length, needed);
    if (ret != GST_FLOW_OK)
      goto error;

    if (id == GST_MATROSKA_ID_CLUSTER) {
      cluster_time = GST_CLOCK_TIME_NONE;
      if (length == G_MAXUINT64)
        cluster_size = 0;
      else
        cluster_size = length + needed;
    }
    if (demux->cluster_time != GST_CLOCK_TIME_NONE &&
        cluster_time == GST_CLOCK_TIME_NONE) {
      cluster_time = demux->cluster_time * demux->common.time_scale;
      cluster_offset = demux->cluster_offset;
      GST_DEBUG_OBJECT (demux, "found cluster at offset %" G_GINT64_FORMAT
          " with time %" GST_TIME_FORMAT, cluster_offset,
          GST_TIME_ARGS (cluster_time));
      if (time == GST_CLOCK_TIME_NONE) {
        GST_DEBUG_OBJECT (demux, "found last cluster");
        prev_cluster_time = cluster_time;
        prev_cluster_offset = cluster_offset;
        break;
      }
      if (cluster_time > time) {
        GST_DEBUG_OBJECT (demux, "overshot target");
        /* cluster overshoots */
        if (cluster_offset == demux->first_cluster_offset) {
          /* but no prev one */
          GST_DEBUG_OBJECT (demux, "but using first cluster anyway");
          prev_cluster_time = cluster_time;
          prev_cluster_offset = cluster_offset;
          break;
        }
        if (prev_cluster_time != GST_CLOCK_TIME_NONE) {
          /* prev cluster did not overshoot, so prev cluster is target */
          break;
        } else {
          /* re-estimate using this new position info */
          opos = cluster_offset;
          otime = cluster_time;
          goto retry;
        }
      } else {
        /* cluster undershoots */
        GST_DEBUG_OBJECT (demux, "undershot target");
        /* ok if close enough */
        if (GST_CLOCK_DIFF (cluster_time, time) < 5 * GST_SECOND) {
          GST_DEBUG_OBJECT (demux, "target close enough");
          prev_cluster_time = cluster_time;
          prev_cluster_offset = cluster_offset;
          break;
        }
        if (otime > time) {
          /* we are in between atime and otime => can bisect if worthwhile */
          if (prev_cluster_time != GST_CLOCK_TIME_NONE &&
              cluster_time > prev_cluster_time &&
              (GST_CLOCK_DIFF (prev_cluster_time, cluster_time) * 10 <
                  GST_CLOCK_DIFF (cluster_time, time))) {
            /* we moved at least one cluster forward,
             * and it looks like target is still far away,
             * let's estimate again */
            GST_DEBUG_OBJECT (demux, "bisecting with new apos");
            apos = cluster_offset;
            atime = cluster_time;
            goto retry;
          }
        }
        /* cluster undershoots, goto next one */
        prev_cluster_time = cluster_time;
        prev_cluster_offset = cluster_offset;
        /* skip cluster if length is defined,
         * otherwise will be skippingly parsed into */
        if (cluster_size) {
          GST_DEBUG_OBJECT (demux, "skipping to next cluster");
          demux->common.offset = cluster_offset + cluster_size;
          demux->cluster_time = GST_CLOCK_TIME_NONE;
        } else {
          GST_DEBUG_OBJECT (demux, "parsing/skipping cluster elements");
        }
      }
    }
    continue;

  error:
    if (ret == GST_FLOW_EOS) {
      if (prev_cluster_time != GST_CLOCK_TIME_NONE)
        break;
    }
    goto exit;
  }

  /* In the bisect loop above we always undershoot and then jump forward
   * cluster-by-cluster until we overshoot, so if we get here we've gone
   * over and the previous cluster is where we need to go to. */
  cluster_offset = prev_cluster_offset;
  cluster_time = prev_cluster_time;

  /* If we have video and can easily backtrack, check if we landed on a cluster
   * that starts with a keyframe - and if not backtrack until we find one that
   * does. */
  if (demux->have_nonintraonly_v_streams && demux->max_backtrack_distance > 0) {
    if (gst_matroska_demux_scan_back_for_keyframe_cluster (demux,
            &cluster_offset, &cluster_time)) {
      GST_INFO_OBJECT (demux, "Adjusted cluster to %" GST_TIME_FORMAT " @ "
          "%" G_GUINT64_FORMAT, GST_TIME_ARGS (cluster_time), cluster_offset);
    }
  }

  entry = g_new0 (GstMatroskaIndex, 1);
  entry->time = cluster_time;
  entry->pos = cluster_offset - demux->common.ebml_segment_start;
  GST_DEBUG_OBJECT (demux, "simulated index entry; time %" GST_TIME_FORMAT
      ", pos %" G_GUINT64_FORMAT, GST_TIME_ARGS (entry->time), entry->pos);

exit:

  /* restore some state */
  demux->cluster_offset = current_cluster_offset;
  demux->cluster_time = current_cluster_time;
  demux->common.offset = current_offset;
  demux->common.state = current_state;

  return entry;
}