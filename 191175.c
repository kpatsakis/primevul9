qtdemux_video_caps (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 fourcc, const guint8 * stsd_data, gchar ** codec_name)
{
  GstCaps *caps = NULL;
  GstVideoFormat format = GST_VIDEO_FORMAT_UNKNOWN;

  switch (fourcc) {
    case GST_MAKE_FOURCC ('p', 'n', 'g', ' '):
      _codec ("PNG still images");
      caps = gst_caps_new_empty_simple ("image/png");
      break;
    case FOURCC_jpeg:
      _codec ("JPEG still images");
      caps =
          gst_caps_new_simple ("image/jpeg", "parsed", G_TYPE_BOOLEAN, TRUE,
          NULL);
      break;
    case GST_MAKE_FOURCC ('m', 'j', 'p', 'a'):
    case GST_MAKE_FOURCC ('A', 'V', 'D', 'J'):
    case GST_MAKE_FOURCC ('M', 'J', 'P', 'G'):
    case GST_MAKE_FOURCC ('d', 'm', 'b', '1'):
      _codec ("Motion-JPEG");
      caps =
          gst_caps_new_simple ("image/jpeg", "parsed", G_TYPE_BOOLEAN, TRUE,
          NULL);
      break;
    case GST_MAKE_FOURCC ('m', 'j', 'p', 'b'):
      _codec ("Motion-JPEG format B");
      caps = gst_caps_new_empty_simple ("video/x-mjpeg-b");
      break;
    case FOURCC_mjp2:
      _codec ("JPEG-2000");
      /* override to what it should be according to spec, avoid palette_data */
      stream->bits_per_sample = 24;
      caps = gst_caps_new_simple ("image/x-j2c", "fields", G_TYPE_INT, 1, NULL);
      break;
    case FOURCC_SVQ3:
      _codec ("Sorensen video v.3");
      caps = gst_caps_new_simple ("video/x-svq",
          "svqversion", G_TYPE_INT, 3, NULL);
      break;
    case GST_MAKE_FOURCC ('s', 'v', 'q', 'i'):
    case GST_MAKE_FOURCC ('S', 'V', 'Q', '1'):
      _codec ("Sorensen video v.1");
      caps = gst_caps_new_simple ("video/x-svq",
          "svqversion", G_TYPE_INT, 1, NULL);
      break;
    case GST_MAKE_FOURCC ('W', 'R', 'A', 'W'):
      caps = gst_caps_new_empty_simple ("video/x-raw");
      gst_caps_set_simple (caps, "format", G_TYPE_STRING, "RGB8P", NULL);
      _codec ("Windows Raw RGB");
      stream->alignment = 32;
      break;
    case FOURCC_raw_:
    {
      guint16 bps;

      bps = QT_UINT16 (stsd_data + 98);
      switch (bps) {
        case 15:
          format = GST_VIDEO_FORMAT_RGB15;
          break;
        case 16:
          format = GST_VIDEO_FORMAT_RGB16;
          break;
        case 24:
          format = GST_VIDEO_FORMAT_RGB;
          break;
        case 32:
          format = GST_VIDEO_FORMAT_ARGB;
          break;
        default:
          /* unknown */
          break;
      }
      break;
    }
    case GST_MAKE_FOURCC ('y', 'v', '1', '2'):
      format = GST_VIDEO_FORMAT_I420;
      break;
    case GST_MAKE_FOURCC ('y', 'u', 'v', '2'):
    case GST_MAKE_FOURCC ('Y', 'u', 'v', '2'):
      format = GST_VIDEO_FORMAT_I420;
      break;
    case FOURCC_2vuy:
    case GST_MAKE_FOURCC ('2', 'V', 'u', 'y'):
      format = GST_VIDEO_FORMAT_UYVY;
      break;
    case GST_MAKE_FOURCC ('v', '3', '0', '8'):
      format = GST_VIDEO_FORMAT_v308;
      break;
    case GST_MAKE_FOURCC ('v', '2', '1', '6'):
      format = GST_VIDEO_FORMAT_v216;
      break;
    case FOURCC_v210:
      format = GST_VIDEO_FORMAT_v210;
      break;
    case GST_MAKE_FOURCC ('r', '2', '1', '0'):
      format = GST_VIDEO_FORMAT_r210;
      break;
      /* Packed YUV 4:4:4 10 bit in 32 bits, complex
         case GST_MAKE_FOURCC ('v', '4', '1', '0'):
         format = GST_VIDEO_FORMAT_v410;
         break;
       */
      /* Packed YUV 4:4:4:4 8 bit in 32 bits
       * but different order than AYUV
       case GST_MAKE_FOURCC ('v', '4', '0', '8'):
       format = GST_VIDEO_FORMAT_v408;
       break;
       */
    case GST_MAKE_FOURCC ('m', 'p', 'e', 'g'):
    case GST_MAKE_FOURCC ('m', 'p', 'g', '1'):
      _codec ("MPEG-1 video");
      caps = gst_caps_new_simple ("video/mpeg", "mpegversion", G_TYPE_INT, 1,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case GST_MAKE_FOURCC ('h', 'd', 'v', '1'): /* HDV 720p30 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '2'): /* HDV 1080i60 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '3'): /* HDV 1080i50 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '4'): /* HDV 720p24 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '5'): /* HDV 720p25 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '6'): /* HDV 1080p24 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '7'): /* HDV 1080p25 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '8'): /* HDV 1080p30 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', '9'): /* HDV 720p60 */
    case GST_MAKE_FOURCC ('h', 'd', 'v', 'a'): /* HDV 720p50 */
    case GST_MAKE_FOURCC ('m', 'x', '5', 'n'): /* MPEG2 IMX NTSC 525/60 50mb/s produced by FCP */
    case GST_MAKE_FOURCC ('m', 'x', '5', 'p'): /* MPEG2 IMX PAL 625/60 50mb/s produced by FCP */
    case GST_MAKE_FOURCC ('m', 'x', '4', 'n'): /* MPEG2 IMX NTSC 525/60 40mb/s produced by FCP */
    case GST_MAKE_FOURCC ('m', 'x', '4', 'p'): /* MPEG2 IMX PAL 625/60 40mb/s produced by FCP */
    case GST_MAKE_FOURCC ('m', 'x', '3', 'n'): /* MPEG2 IMX NTSC 525/60 30mb/s produced by FCP */
    case GST_MAKE_FOURCC ('m', 'x', '3', 'p'): /* MPEG2 IMX PAL 625/50 30mb/s produced by FCP */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '1'): /* XDCAM HD 720p30 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '2'): /* XDCAM HD 1080i60 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '3'): /* XDCAM HD 1080i50 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '4'): /* XDCAM HD 720p24 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '5'): /* XDCAM HD 720p25 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '6'): /* XDCAM HD 1080p24 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '7'): /* XDCAM HD 1080p25 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '8'): /* XDCAM HD 1080p30 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', '9'): /* XDCAM HD 720p60 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'a'): /* XDCAM HD 720p50 35Mb/s */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'b'): /* XDCAM EX 1080i60 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'c'): /* XDCAM EX 1080i50 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'd'): /* XDCAM HD 1080p24 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'e'): /* XDCAM HD 1080p25 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', 'v', 'f'): /* XDCAM HD 1080p30 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', '1'): /* XDCAM HD422 720p30 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', '4'): /* XDCAM HD422 720p24 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', '5'): /* XDCAM HD422 720p25 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', '9'): /* XDCAM HD422 720p60 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'a'): /* XDCAM HD422 720p50 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'b'): /* XDCAM HD422 1080i50 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'c'): /* XDCAM HD422 1080i50 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'd'): /* XDCAM HD422 1080p24 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'e'): /* XDCAM HD422 1080p25 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', '5', 'f'): /* XDCAM HD422 1080p30 50Mb/s CBR */
    case GST_MAKE_FOURCC ('x', 'd', 'h', 'd'): /* XDCAM HD 540p */
    case GST_MAKE_FOURCC ('x', 'd', 'h', '2'): /* XDCAM HD422 540p */
    case GST_MAKE_FOURCC ('A', 'V', 'm', 'p'): /* AVID IMX PAL */
    case GST_MAKE_FOURCC ('m', 'p', 'g', '2'): /* AVID IMX PAL */
    case GST_MAKE_FOURCC ('m', 'p', '2', 'v'): /* AVID IMX PAL */
      _codec ("MPEG-2 video");
      caps = gst_caps_new_simple ("video/mpeg", "mpegversion", G_TYPE_INT, 2,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case GST_MAKE_FOURCC ('g', 'i', 'f', ' '):
      _codec ("GIF still images");
      caps = gst_caps_new_empty_simple ("image/gif");
      break;
    case FOURCC_h263:
    case GST_MAKE_FOURCC ('H', '2', '6', '3'):
    case FOURCC_s263:
    case GST_MAKE_FOURCC ('U', '2', '6', '3'):
      _codec ("H.263");
      /* ffmpeg uses the height/width props, don't know why */
      caps = gst_caps_new_simple ("video/x-h263",
          "variant", G_TYPE_STRING, "itu", NULL);
      break;
    case FOURCC_mp4v:
    case FOURCC_MP4V:
      _codec ("MPEG-4 video");
      caps = gst_caps_new_simple ("video/mpeg", "mpegversion", G_TYPE_INT, 4,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case GST_MAKE_FOURCC ('3', 'i', 'v', 'd'):
    case GST_MAKE_FOURCC ('3', 'I', 'V', 'D'):
      _codec ("Microsoft MPEG-4 4.3");  /* FIXME? */
      caps = gst_caps_new_simple ("video/x-msmpeg",
          "msmpegversion", G_TYPE_INT, 43, NULL);
      break;
    case GST_MAKE_FOURCC ('D', 'I', 'V', '3'):
      _codec ("DivX 3");
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 3, NULL);
      break;
    case GST_MAKE_FOURCC ('D', 'I', 'V', 'X'):
    case GST_MAKE_FOURCC ('d', 'i', 'v', 'x'):
      _codec ("DivX 4");
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 4, NULL);
      break;
    case GST_MAKE_FOURCC ('D', 'X', '5', '0'):
      _codec ("DivX 5");
      caps = gst_caps_new_simple ("video/x-divx",
          "divxversion", G_TYPE_INT, 5, NULL);
      break;

    case GST_MAKE_FOURCC ('F', 'F', 'V', '1'):
      _codec ("FFV1");
      caps = gst_caps_new_simple ("video/x-ffv",
          "ffvversion", G_TYPE_INT, 1, NULL);
      break;

    case GST_MAKE_FOURCC ('3', 'I', 'V', '1'):
    case GST_MAKE_FOURCC ('3', 'I', 'V', '2'):
    case GST_MAKE_FOURCC ('X', 'V', 'I', 'D'):
    case GST_MAKE_FOURCC ('x', 'v', 'i', 'd'):
    case FOURCC_FMP4:
    case GST_MAKE_FOURCC ('U', 'M', 'P', '4'):
      caps = gst_caps_new_simple ("video/mpeg",
          "mpegversion", G_TYPE_INT, 4, NULL);
      _codec ("MPEG-4");
      break;

    case GST_MAKE_FOURCC ('c', 'v', 'i', 'd'):
      _codec ("Cinepak");
      caps = gst_caps_new_empty_simple ("video/x-cinepak");
      break;
    case GST_MAKE_FOURCC ('q', 'd', 'r', 'w'):
      _codec ("Apple QuickDraw");
      caps = gst_caps_new_empty_simple ("video/x-qdrw");
      break;
    case GST_MAKE_FOURCC ('r', 'p', 'z', 'a'):
      _codec ("Apple video");
      caps = gst_caps_new_empty_simple ("video/x-apple-video");
      break;
    case FOURCC_H264:
    case FOURCC_avc1:
      _codec ("H.264 / AVC");
      caps = gst_caps_new_simple ("video/x-h264",
          "stream-format", G_TYPE_STRING, "avc",
          "alignment", G_TYPE_STRING, "au", NULL);
      break;
    case FOURCC_avc3:
      _codec ("H.264 / AVC");
      caps = gst_caps_new_simple ("video/x-h264",
          "stream-format", G_TYPE_STRING, "avc3",
          "alignment", G_TYPE_STRING, "au", NULL);
      break;
    case FOURCC_H265:
    case FOURCC_hvc1:
      _codec ("H.265 / HEVC");
      caps = gst_caps_new_simple ("video/x-h265",
          "stream-format", G_TYPE_STRING, "hvc1",
          "alignment", G_TYPE_STRING, "au", NULL);
      break;
    case FOURCC_hev1:
      _codec ("H.265 / HEVC");
      caps = gst_caps_new_simple ("video/x-h265",
          "stream-format", G_TYPE_STRING, "hev1",
          "alignment", G_TYPE_STRING, "au", NULL);
      break;
    case FOURCC_rle_:
      _codec ("Run-length encoding");
      caps = gst_caps_new_simple ("video/x-rle",
          "layout", G_TYPE_STRING, "quicktime", NULL);
      break;
    case FOURCC_WRLE:
      _codec ("Run-length encoding");
      caps = gst_caps_new_simple ("video/x-rle",
          "layout", G_TYPE_STRING, "microsoft", NULL);
      break;
    case GST_MAKE_FOURCC ('I', 'V', '3', '2'):
    case GST_MAKE_FOURCC ('i', 'v', '3', '2'):
      _codec ("Indeo Video 3");
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 3, NULL);
      break;
    case GST_MAKE_FOURCC ('I', 'V', '4', '1'):
    case GST_MAKE_FOURCC ('i', 'v', '4', '1'):
      _codec ("Intel Video 4");
      caps = gst_caps_new_simple ("video/x-indeo",
          "indeoversion", G_TYPE_INT, 4, NULL);
      break;
    case FOURCC_dvcp:
    case FOURCC_dvc_:
    case GST_MAKE_FOURCC ('d', 'v', 's', 'd'):
    case GST_MAKE_FOURCC ('D', 'V', 'S', 'D'):
    case GST_MAKE_FOURCC ('d', 'v', 'c', 's'):
    case GST_MAKE_FOURCC ('D', 'V', 'C', 'S'):
    case GST_MAKE_FOURCC ('d', 'v', '2', '5'):
    case GST_MAKE_FOURCC ('d', 'v', 'p', 'p'):
      _codec ("DV Video");
      caps = gst_caps_new_simple ("video/x-dv", "dvversion", G_TYPE_INT, 25,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case FOURCC_dv5n:          /* DVCPRO50 NTSC */
    case FOURCC_dv5p:          /* DVCPRO50 PAL */
      _codec ("DVCPro50 Video");
      caps = gst_caps_new_simple ("video/x-dv", "dvversion", G_TYPE_INT, 50,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case GST_MAKE_FOURCC ('d', 'v', 'h', '5'): /* DVCPRO HD 50i produced by FCP */
    case GST_MAKE_FOURCC ('d', 'v', 'h', '6'): /* DVCPRO HD 60i produced by FCP */
      _codec ("DVCProHD Video");
      caps = gst_caps_new_simple ("video/x-dv", "dvversion", G_TYPE_INT, 100,
          "systemstream", G_TYPE_BOOLEAN, FALSE, NULL);
      break;
    case GST_MAKE_FOURCC ('s', 'm', 'c', ' '):
      _codec ("Apple Graphics (SMC)");
      caps = gst_caps_new_empty_simple ("video/x-smc");
      break;
    case GST_MAKE_FOURCC ('V', 'P', '3', '1'):
      _codec ("VP3");
      caps = gst_caps_new_empty_simple ("video/x-vp3");
      break;
    case GST_MAKE_FOURCC ('V', 'P', '6', 'F'):
      _codec ("VP6 Flash");
      caps = gst_caps_new_empty_simple ("video/x-vp6-flash");
      break;
    case FOURCC_XiTh:
      _codec ("Theora");
      caps = gst_caps_new_empty_simple ("video/x-theora");
      /* theora uses one byte of padding in the data stream because it does not
       * allow 0 sized packets while theora does */
      stream->padding = 1;
      break;
    case FOURCC_drac:
      _codec ("Dirac");
      caps = gst_caps_new_empty_simple ("video/x-dirac");
      break;
    case GST_MAKE_FOURCC ('t', 'i', 'f', 'f'):
      _codec ("TIFF still images");
      caps = gst_caps_new_empty_simple ("image/tiff");
      break;
    case GST_MAKE_FOURCC ('i', 'c', 'o', 'd'):
      _codec ("Apple Intermediate Codec");
      caps = gst_caps_from_string ("video/x-apple-intermediate-codec");
      break;
    case GST_MAKE_FOURCC ('A', 'V', 'd', 'n'):
      _codec ("AVID DNxHD");
      caps = gst_caps_from_string ("video/x-dnxhd");
      break;
    case FOURCC_VP80:
      _codec ("On2 VP8");
      caps = gst_caps_from_string ("video/x-vp8");
      break;
    case FOURCC_apcs:
      _codec ("Apple ProRes LT");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING, "lt",
          NULL);
      break;
    case FOURCC_apch:
      _codec ("Apple ProRes HQ");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING, "hq",
          NULL);
      break;
    case FOURCC_apcn:
      _codec ("Apple ProRes");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING,
          "standard", NULL);
      break;
    case FOURCC_apco:
      _codec ("Apple ProRes Proxy");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING,
          "proxy", NULL);
      break;
    case FOURCC_ap4h:
      _codec ("Apple ProRes 4444");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING,
          "4444", NULL);
      break;
    case FOURCC_ap4x:
      _codec ("Apple ProRes 4444 XQ");
      caps =
          gst_caps_new_simple ("video/x-prores", "variant", G_TYPE_STRING,
          "4444xq", NULL);
      break;
    case FOURCC_vc_1:
    case FOURCC_ovc1:
      _codec ("VC-1");
      caps = gst_caps_new_simple ("video/x-wmv",
          "wmvversion", G_TYPE_INT, 3, "format", G_TYPE_STRING, "WVC1", NULL);
      break;
    case GST_MAKE_FOURCC ('k', 'p', 'c', 'd'):
    default:
    {
      char *s, fourstr[5];

      g_snprintf (fourstr, 5, "%" GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
      s = g_strdup_printf ("video/x-gst-fourcc-%s", g_strstrip (fourstr));
      caps = gst_caps_new_empty_simple (s);
      g_free (s);
      break;
    }
  }

  if (format != GST_VIDEO_FORMAT_UNKNOWN) {
    GstVideoInfo info;

    gst_video_info_init (&info);
    gst_video_info_set_format (&info, format, stream->width, stream->height);

    caps = gst_video_info_to_caps (&info);
    *codec_name = gst_pb_utils_get_codec_description (caps);

    /* enable clipping for raw video streams */
    stream->need_clip = TRUE;
    stream->alignment = 32;
  }

  return caps;
}