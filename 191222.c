qtdemux_audio_caps (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 fourcc, const guint8 * data, int len, gchar ** codec_name)
{
  GstCaps *caps;
  const GstStructure *s;
  const gchar *name;
  gint endian = 0;
  GstAudioFormat format = 0;
  gint depth;

  GST_DEBUG_OBJECT (qtdemux, "resolve fourcc 0x%08x", GUINT32_TO_BE (fourcc));

  depth = stream->bytes_per_packet * 8;

  switch (fourcc) {
    case GST_MAKE_FOURCC ('N', 'O', 'N', 'E'):
    case FOURCC_raw_:
      /* 8-bit audio is unsigned */
      if (depth == 8)
        format = GST_AUDIO_FORMAT_U8;
      /* otherwise it's signed and big-endian just like 'twos' */
    case FOURCC_twos:
      endian = G_BIG_ENDIAN;
      /* fall-through */
    case FOURCC_sowt:
    {
      gchar *str;

      if (!endian)
        endian = G_LITTLE_ENDIAN;

      if (!format)
        format = gst_audio_format_build_integer (TRUE, endian, depth, depth);

      str = g_strdup_printf ("Raw %d-bit PCM audio", depth);
      _codec (str);
      g_free (str);

      caps = gst_caps_new_simple ("audio/x-raw",
          "format", G_TYPE_STRING, gst_audio_format_to_string (format),
          "layout", G_TYPE_STRING, "interleaved", NULL);
      stream->alignment = GST_ROUND_UP_8 (depth);
      stream->alignment = round_up_pow2 (stream->alignment);
      break;
    }
    case GST_MAKE_FOURCC ('f', 'l', '6', '4'):
      _codec ("Raw 64-bit floating-point audio");
      caps = gst_caps_new_simple ("audio/x-raw",
          "format", G_TYPE_STRING, "F64BE",
          "layout", G_TYPE_STRING, "interleaved", NULL);
      stream->alignment = 8;
      break;
    case GST_MAKE_FOURCC ('f', 'l', '3', '2'):
      _codec ("Raw 32-bit floating-point audio");
      caps = gst_caps_new_simple ("audio/x-raw",
          "format", G_TYPE_STRING, "F32BE",
          "layout", G_TYPE_STRING, "interleaved", NULL);
      stream->alignment = 4;
      break;
    case FOURCC_in24:
      _codec ("Raw 24-bit PCM audio");
      /* we assume BIG ENDIAN, an enda box will tell us to change this to little
       * endian later */
      caps = gst_caps_new_simple ("audio/x-raw",
          "format", G_TYPE_STRING, "S24BE",
          "layout", G_TYPE_STRING, "interleaved", NULL);
      stream->alignment = 4;
      break;
    case GST_MAKE_FOURCC ('i', 'n', '3', '2'):
      _codec ("Raw 32-bit PCM audio");
      caps = gst_caps_new_simple ("audio/x-raw",
          "format", G_TYPE_STRING, "S32BE",
          "layout", G_TYPE_STRING, "interleaved", NULL);
      stream->alignment = 4;
      break;
    case FOURCC_ulaw:
      _codec ("Mu-law audio");
      caps = gst_caps_new_empty_simple ("audio/x-mulaw");
      break;
    case FOURCC_alaw:
      _codec ("A-law audio");
      caps = gst_caps_new_empty_simple ("audio/x-alaw");
      break;
    case 0x0200736d:
    case 0x6d730002:
      _codec ("Microsoft ADPCM");
      /* Microsoft ADPCM-ACM code 2 */
      caps = gst_caps_new_simple ("audio/x-adpcm",
          "layout", G_TYPE_STRING, "microsoft", NULL);
      break;
    case 0x1100736d:
    case 0x6d730011:
      _codec ("DVI/IMA ADPCM");
      caps = gst_caps_new_simple ("audio/x-adpcm",
          "layout", G_TYPE_STRING, "dvi", NULL);
      break;
    case 0x1700736d:
    case 0x6d730017:
      _codec ("DVI/Intel IMA ADPCM");
      /* FIXME DVI/Intel IMA ADPCM/ACM code 17 */
      caps = gst_caps_new_simple ("audio/x-adpcm",
          "layout", G_TYPE_STRING, "quicktime", NULL);
      break;
    case 0x5500736d:
    case 0x6d730055:
      /* MPEG layer 3, CBR only (pre QT4.1) */
    case FOURCC__mp3:
      _codec ("MPEG-1 layer 3");
      /* MPEG layer 3, CBR & VBR (QT4.1 and later) */
      caps = gst_caps_new_simple ("audio/mpeg", "layer", G_TYPE_INT, 3,
          "mpegversion", G_TYPE_INT, 1, NULL);
      break;
    case 0x20736d:
    case GST_MAKE_FOURCC ('e', 'c', '-', '3'):
      _codec ("EAC-3 audio");
      caps = gst_caps_new_simple ("audio/x-eac3",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      stream->sampled = TRUE;
      break;
    case GST_MAKE_FOURCC ('s', 'a', 'c', '3'): // Nero Recode
    case FOURCC_ac_3:
      _codec ("AC-3 audio");
      caps = gst_caps_new_simple ("audio/x-ac3",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      stream->sampled = TRUE;
      break;
    case GST_MAKE_FOURCC ('d', 't', 's', 'c'):
    case GST_MAKE_FOURCC ('D', 'T', 'S', ' '):
      _codec ("DTS audio");
      caps = gst_caps_new_simple ("audio/x-dts",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      stream->sampled = TRUE;
      break;
    case GST_MAKE_FOURCC ('d', 't', 's', 'h'): // DTS-HD
    case GST_MAKE_FOURCC ('d', 't', 's', 'l'): // DTS-HD Lossless
      _codec ("DTS-HD audio");
      caps = gst_caps_new_simple ("audio/x-dts",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      stream->sampled = TRUE;
      break;
    case FOURCC_MAC3:
      _codec ("MACE-3");
      caps = gst_caps_new_simple ("audio/x-mace",
          "maceversion", G_TYPE_INT, 3, NULL);
      break;
    case FOURCC_MAC6:
      _codec ("MACE-6");
      caps = gst_caps_new_simple ("audio/x-mace",
          "maceversion", G_TYPE_INT, 6, NULL);
      break;
    case GST_MAKE_FOURCC ('O', 'g', 'g', 'V'):
      /* ogg/vorbis */
      caps = gst_caps_new_empty_simple ("application/ogg");
      break;
    case GST_MAKE_FOURCC ('d', 'v', 'c', 'a'):
      _codec ("DV audio");
      caps = gst_caps_new_empty_simple ("audio/x-dv");
      break;
    case FOURCC_mp4a:
      _codec ("MPEG-4 AAC audio");
      caps = gst_caps_new_simple ("audio/mpeg",
          "mpegversion", G_TYPE_INT, 4, "framed", G_TYPE_BOOLEAN, TRUE,
          "stream-format", G_TYPE_STRING, "raw", NULL);
      break;
    case GST_MAKE_FOURCC ('Q', 'D', 'M', 'C'):
      _codec ("QDesign Music");
      caps = gst_caps_new_empty_simple ("audio/x-qdm");
      break;
    case FOURCC_QDM2:
      _codec ("QDesign Music v.2");
      /* FIXME: QDesign music version 2 (no constant) */
      if (FALSE && data) {
        caps = gst_caps_new_simple ("audio/x-qdm2",
            "framesize", G_TYPE_INT, QT_UINT32 (data + 52),
            "bitrate", G_TYPE_INT, QT_UINT32 (data + 40),
            "blocksize", G_TYPE_INT, QT_UINT32 (data + 44), NULL);
      } else {
        caps = gst_caps_new_empty_simple ("audio/x-qdm2");
      }
      break;
    case FOURCC_agsm:
      _codec ("GSM audio");
      caps = gst_caps_new_empty_simple ("audio/x-gsm");
      break;
    case FOURCC_samr:
      _codec ("AMR audio");
      caps = gst_caps_new_empty_simple ("audio/AMR");
      break;
    case FOURCC_sawb:
      _codec ("AMR-WB audio");
      caps = gst_caps_new_empty_simple ("audio/AMR-WB");
      break;
    case FOURCC_ima4:
      _codec ("Quicktime IMA ADPCM");
      caps = gst_caps_new_simple ("audio/x-adpcm",
          "layout", G_TYPE_STRING, "quicktime", NULL);
      break;
    case FOURCC_alac:
      _codec ("Apple lossless audio");
      caps = gst_caps_new_empty_simple ("audio/x-alac");
      break;
    case FOURCC_fLaC:
      _codec ("Free Lossless Audio Codec");
      caps = gst_caps_new_simple ("audio/x-flac",
          "framed", G_TYPE_BOOLEAN, TRUE, NULL);
      break;
    case GST_MAKE_FOURCC ('Q', 'c', 'l', 'p'):
      _codec ("QualComm PureVoice");
      caps = gst_caps_from_string ("audio/qcelp");
      break;
    case FOURCC_wma_:
    case FOURCC_owma:
      _codec ("WMA");
      caps = gst_caps_new_empty_simple ("audio/x-wma");
      break;
    case FOURCC_opus:
      _codec ("Opus");
      caps = gst_caps_new_empty_simple ("audio/x-opus");
      break;
    case GST_MAKE_FOURCC ('l', 'p', 'c', 'm'):
    {
      guint32 flags = 0;
      guint32 depth = 0;
      guint32 width = 0;
      GstAudioFormat format;
      enum
      {
        FLAG_IS_FLOAT = 0x1,
        FLAG_IS_BIG_ENDIAN = 0x2,
        FLAG_IS_SIGNED = 0x4,
        FLAG_IS_PACKED = 0x8,
        FLAG_IS_ALIGNED_HIGH = 0x10,
        FLAG_IS_NON_INTERLEAVED = 0x20
      };
      _codec ("Raw LPCM audio");

      if (data && len >= 56) {
        depth = QT_UINT32 (data + 40);
        flags = QT_UINT32 (data + 44);
        width = QT_UINT32 (data + 48) * 8 / stream->n_channels;
      }
      if ((flags & FLAG_IS_FLOAT) == 0) {
        if (depth == 0)
          depth = 16;
        if (width == 0)
          width = 16;
        format = gst_audio_format_build_integer ((flags & FLAG_IS_SIGNED) ?
            TRUE : FALSE, (flags & FLAG_IS_BIG_ENDIAN) ?
            G_BIG_ENDIAN : G_LITTLE_ENDIAN, width, depth);
        caps = gst_caps_new_simple ("audio/x-raw",
            "format", G_TYPE_STRING, gst_audio_format_to_string (format),
            "layout", G_TYPE_STRING, (flags & FLAG_IS_NON_INTERLEAVED) ?
            "non-interleaved" : "interleaved", NULL);
        stream->alignment = GST_ROUND_UP_8 (depth);
        stream->alignment = round_up_pow2 (stream->alignment);
      } else {
        if (width == 0)
          width = 32;
        if (width == 64) {
          if (flags & FLAG_IS_BIG_ENDIAN)
            format = GST_AUDIO_FORMAT_F64BE;
          else
            format = GST_AUDIO_FORMAT_F64LE;
        } else {
          if (flags & FLAG_IS_BIG_ENDIAN)
            format = GST_AUDIO_FORMAT_F32BE;
          else
            format = GST_AUDIO_FORMAT_F32LE;
        }
        caps = gst_caps_new_simple ("audio/x-raw",
            "format", G_TYPE_STRING, gst_audio_format_to_string (format),
            "layout", G_TYPE_STRING, (flags & FLAG_IS_NON_INTERLEAVED) ?
            "non-interleaved" : "interleaved", NULL);
        stream->alignment = width / 8;
      }
      break;
    }
    case GST_MAKE_FOURCC ('q', 't', 'v', 'r'):
      /* ? */
    default:
    {
      char *s, fourstr[5];

      g_snprintf (fourstr, 5, "%" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
      s = g_strdup_printf ("audio/x-gst-fourcc-%s", g_strstrip (fourstr));
      caps = gst_caps_new_empty_simple (s);
      g_free (s);
      break;
    }
  }

  if (caps) {
    GstCaps *templ_caps =
        gst_static_pad_template_get_caps (&gst_qtdemux_audiosrc_template);
    GstCaps *intersection = gst_caps_intersect (caps, templ_caps);
    gst_caps_unref (caps);
    gst_caps_unref (templ_caps);
    caps = intersection;
  }

  /* enable clipping for raw audio streams */
  s = gst_caps_get_structure (caps, 0);
  name = gst_structure_get_name (s);
  if (g_str_has_prefix (name, "audio/x-raw")) {
    stream->need_clip = TRUE;
    stream->max_buffer_size = 4096 * stream->bytes_per_frame;
    GST_DEBUG ("setting max buffer size to %d", stream->max_buffer_size);
  }
  return caps;
}