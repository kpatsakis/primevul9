xcf_init (Gimp *gimp)
{
  GimpPlugInProcedure *proc;
  GFile               *file;
  GimpProcedure       *procedure;

  g_return_if_fail (GIMP_IS_GIMP (gimp));

  /* So this is sort of a hack, but its better than it was before.  To
   * do this right there would be a file load-save handler type and
   * the whole interface would change but there isn't, and currently
   * the plug-in structure contains all the load-save info, so it
   * makes sense to use that for the XCF load/save handlers, even
   * though they are internal.  The only thing it requires is using a
   * PlugInProcDef struct.  -josh
   */

  /*  gimp-xcf-save  */
  file = g_file_new_for_path ("gimp-xcf-save");
  procedure = gimp_plug_in_procedure_new (GIMP_PLUGIN, file);
  g_object_unref (file);

  procedure->proc_type    = GIMP_INTERNAL;
  procedure->marshal_func = xcf_save_invoker;

  proc = GIMP_PLUG_IN_PROCEDURE (procedure);
  proc->menu_label = g_strdup (N_("GIMP XCF image"));
  gimp_plug_in_procedure_set_icon (proc, GIMP_ICON_TYPE_ICON_NAME,
                                   (const guint8 *) "gimp-wilber",
                                   strlen ("gimp-wilber") + 1);
  gimp_plug_in_procedure_set_image_types (proc, "RGB*, GRAY*, INDEXED*");
  gimp_plug_in_procedure_set_file_proc (proc, "xcf", "", NULL);
  gimp_plug_in_procedure_set_mime_types (proc, "image/x-xcf");
  gimp_plug_in_procedure_set_handles_uri (proc);

  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-xcf-save");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-xcf-save",
                                     "Saves file in the .xcf file format",
                                     "The XCF file format has been designed "
                                     "specifically for loading and saving "
                                     "tiled and layered images in GIMP. "
                                     "This procedure will save the specified "
                                     "image in the xcf file format.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_int32 ("dummy-param",
                                                      "Dummy Param",
                                                      "Dummy parameter",
                                                      G_MININT32, G_MAXINT32, 0,
                                                      GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "Image",
                                                         "Input image",
                                                         gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_drawable_id ("drawable",
                                                            "Drawable",
                                                            "Active drawable of input image",
                                                            gimp, TRUE,
                                                            GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("filename",
                                                       "Filename",
                                                       "The name of the file "
                                                       "to save the image in, "
                                                       "in URI format and "
                                                       "UTF-8 encoding",
                                                       TRUE, FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("raw-filename",
                                                       "Raw filename",
                                                       "The basename of the "
                                                       "file, in UTF-8",
                                                       FALSE, FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_plug_in_manager_add_procedure (gimp->plug_in_manager, proc);
  g_object_unref (procedure);

  /*  gimp-xcf-load  */
  file = g_file_new_for_path ("gimp-xcf-load");
  procedure = gimp_plug_in_procedure_new (GIMP_PLUGIN, file);
  g_object_unref (file);

  procedure->proc_type    = GIMP_INTERNAL;
  procedure->marshal_func = xcf_load_invoker;

  proc = GIMP_PLUG_IN_PROCEDURE (procedure);
  proc->menu_label = g_strdup (N_("GIMP XCF image"));
  gimp_plug_in_procedure_set_icon (proc, GIMP_ICON_TYPE_ICON_NAME,
                                   (const guint8 *) "gimp-wilber",
                                   strlen ("gimp-wilber") + 1);
  gimp_plug_in_procedure_set_image_types (proc, NULL);
  gimp_plug_in_procedure_set_file_proc (proc, "xcf", "",
                                        "0,string,gimp\\040xcf\\040");
  gimp_plug_in_procedure_set_mime_types (proc, "image/x-xcf");
  gimp_plug_in_procedure_set_handles_uri (proc);

  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-xcf-load");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-xcf-load",
                                     "Loads file saved in the .xcf file format",
                                     "The XCF file format has been designed "
                                     "specifically for loading and saving "
                                     "tiled and layered images in GIMP. "
                                     "This procedure will load the specified "
                                     "file.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);

  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_int32 ("dummy-param",
                                                      "Dummy Param",
                                                      "Dummy parameter",
                                                      G_MININT32, G_MAXINT32, 0,
                                                      GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("filename",
                                                       "Filename",
                                                       "The name of the file "
                                                       "to load, in the "
                                                       "on-disk character "
                                                       "set and encoding",
                                                       TRUE, FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("raw-filename",
                                                       "Raw filename",
                                                       "The basename of the "
                                                       "file, in UTF-8",
                                                       FALSE, FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));

  gimp_procedure_add_return_value (procedure,
                                   gimp_param_spec_image_id ("image",
                                                             "Image",
                                                             "Output image",
                                                             gimp, FALSE,
                                                             GIMP_PARAM_READWRITE));
  gimp_plug_in_manager_add_procedure (gimp->plug_in_manager, proc);
  g_object_unref (procedure);
}