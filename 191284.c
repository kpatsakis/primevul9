qtdemux_inspect_transformation_matrix (GstQTDemux * qtdemux,
    QtDemuxStream * stream, guint32 * matrix, GstTagList ** taglist)
{

/* [a b c]
 * [d e f]
 * [g h i]
 *
 * This macro will only compare value abdegh, it expects cfi to have already
 * been checked
 */
#define QTCHECK_MATRIX(m,a,b,d,e) ((m)[0] == (a << 16) && (m)[1] == (b << 16) && \
                                   (m)[3] == (d << 16) && (m)[4] == (e << 16))

  /* only handle the cases where the last column has standard values */
  if (matrix[2] == 0 && matrix[5] == 0 && matrix[8] == 1 << 30) {
    const gchar *rotation_tag = NULL;

    /* no rotation needed */
    if (QTCHECK_MATRIX (matrix, 1, 0, 0, 1)) {
      /* NOP */
    } else if (QTCHECK_MATRIX (matrix, 0, 1, G_MAXUINT16, 0)) {
      rotation_tag = "rotate-90";
    } else if (QTCHECK_MATRIX (matrix, G_MAXUINT16, 0, 0, G_MAXUINT16)) {
      rotation_tag = "rotate-180";
    } else if (QTCHECK_MATRIX (matrix, 0, G_MAXUINT16, 1, 0)) {
      rotation_tag = "rotate-270";
    } else {
      GST_FIXME_OBJECT (qtdemux, "Unhandled transformation matrix values");
    }

    GST_DEBUG_OBJECT (qtdemux, "Transformation matrix rotation %s",
        rotation_tag);
    if (rotation_tag != NULL) {
      if (*taglist == NULL)
        *taglist = gst_tag_list_new_empty ();
      gst_tag_list_add (*taglist, GST_TAG_MERGE_REPLACE,
          GST_TAG_IMAGE_ORIENTATION, rotation_tag, NULL);
    }
  } else {
    GST_FIXME_OBJECT (qtdemux, "Unhandled transformation matrix values");
  }
}