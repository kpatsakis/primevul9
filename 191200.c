qtdemux_parse_svq3_stsd_data (GstQTDemux * qtdemux, GNode * stsd,
    guint8 ** gamma, GstBuffer ** seqh)
{
  guint8 *_gamma = NULL;
  GstBuffer *_seqh = NULL;
  guint8 *stsd_data = stsd->data;
  guint32 length = QT_UINT32 (stsd_data);
  guint16 version;

  if (length < 32) {
    GST_WARNING_OBJECT (qtdemux, "stsd too short");
    goto end;
  }

  stsd_data += 32;
  length -= 32;
  version = QT_UINT16 (stsd_data);
  if (version == 3) {
    if (length >= 70) {
      length -= 70;
      stsd_data += 70;
      while (length > 8) {
        guint32 fourcc, size;
        guint8 *data;
        size = QT_UINT32 (stsd_data);
        fourcc = QT_FOURCC (stsd_data + 4);
        data = stsd_data + 8;

        if (size == 0) {
          GST_WARNING_OBJECT (qtdemux, "Atom of size 0 found, aborting "
              "svq3 atom parsing");
          goto end;
        }

        switch (fourcc) {
          case FOURCC_gama:{
            if (size == 12) {
              _gamma = data;
            } else {
              GST_WARNING_OBJECT (qtdemux, "Unexpected size %" G_GUINT32_FORMAT
                  " for gama atom, expected 12", size);
            }
            break;
          }
          case FOURCC_SMI_:{
            if (size > 16 && QT_FOURCC (data) == FOURCC_SEQH) {
              guint32 seqh_size;
              if (_seqh != NULL) {
                GST_WARNING_OBJECT (qtdemux, "Unexpected second SEQH SMI atom "
                    " found, ignoring");
              } else {
                seqh_size = QT_UINT32 (data + 4);
                if (seqh_size > 0) {
                  _seqh = gst_buffer_new_and_alloc (seqh_size);
                  gst_buffer_fill (_seqh, 0, data + 8, seqh_size);
                }
              }
            }
            break;
          }
          default:{
            GST_WARNING_OBJECT (qtdemux, "Unhandled atom %" GST_FOURCC_FORMAT
                " in SVQ3 entry in stsd atom", GST_FOURCC_ARGS (fourcc));
          }
        }

        if (size <= length) {
          length -= size;
          stsd_data += size;
        }
      }
    } else {
      GST_WARNING_OBJECT (qtdemux, "SVQ3 entry too short in stsd atom");
    }
  } else {
    GST_WARNING_OBJECT (qtdemux, "Unexpected version for SVQ3 entry %"
        G_GUINT16_FORMAT, version);
    goto end;
  }

end:
  if (gamma) {
    *gamma = _gamma;
  }
  if (seqh) {
    *seqh = _seqh;
  } else if (_seqh) {
    gst_buffer_unref (_seqh);
  }
}