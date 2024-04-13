read_tube_block (FILE     *f,
                 gint      image_ID,
                 guint     total_len,
                 PSPimage *ia)
{
  guint16            version;
  guchar             name[514];
  guint32            step_size, column_count, row_count, cell_count;
  guint32            placement_mode, selection_mode;
  gint               i;
  GimpPixPipeParams  params;
  GimpParasite      *pipe_parasite;
  gchar             *parasite_text;

  gimp_pixpipe_params_init (&params);

  if (fread (&version, 2, 1, f) < 1
      || fread (name, 513, 1, f) < 1
      || fread (&step_size, 4, 1, f) < 1
      || fread (&column_count, 4, 1, f) < 1
      || fread (&row_count, 4, 1, f) < 1
      || fread (&cell_count, 4, 1, f) < 1
      || fread (&placement_mode, 4, 1, f) < 1
      || fread (&selection_mode, 4, 1, f) < 1)
    {
      g_message ("Error reading tube data chunk");
      return -1;
    }

  name[513] = 0;
  version = GUINT16_FROM_LE (version);
  params.step = GUINT32_FROM_LE (step_size);
  params.cols = GUINT32_FROM_LE (column_count);
  params.rows = GUINT32_FROM_LE (row_count);
  params.ncells = GUINT32_FROM_LE (cell_count);
  placement_mode = GUINT32_FROM_LE (placement_mode);
  selection_mode = GUINT32_FROM_LE (selection_mode);

  for (i = 1; i < params.cols; i++)
    gimp_image_add_vguide (image_ID, (ia->width * i)/params.cols);
  for (i = 1; i < params.rows; i++)
    gimp_image_add_hguide (image_ID, (ia->height * i)/params.rows);

  /* We use a parasite to pass in the tube (pipe) parameters in
   * case we will have any use of those, for instance in the gpb
   * plug-in that saves a GIMP image pipe.
   */
  params.dim = 1;
  params.cellwidth = ia->width / params.cols;
  params.cellheight = ia->height / params.rows;
  params.placement = (placement_mode == tpmRandom ? "random" :
                      (placement_mode == tpmConstant ? "constant" :
                       "default"));
  params.rank[0] = params.ncells;
  params.selection[0] = (selection_mode == tsmRandom ? "random" :
                         (selection_mode == tsmIncremental ? "incremental" :
                          (selection_mode == tsmAngular ? "angular" :
                           (selection_mode == tsmPressure ? "pressure" :
                            (selection_mode == tsmVelocity ? "velocity" :
                             "default")))));
  parasite_text = gimp_pixpipe_params_build (&params);

  IFDBG(2) g_message ("parasite: %s", parasite_text);

  pipe_parasite = gimp_parasite_new ("gimp-brush-pipe-parameters",
                                     GIMP_PARASITE_PERSISTENT,
                                     strlen (parasite_text) + 1, parasite_text);
  gimp_image_parasite_attach (image_ID, pipe_parasite);
  gimp_parasite_free (pipe_parasite);
  g_free (parasite_text);

  return 0;
}