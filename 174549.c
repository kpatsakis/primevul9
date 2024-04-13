query (void)
{
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode", "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "uri",      "The URI of the file to load" },
    { GIMP_PDB_STRING, "raw-uri",  "The URI of the file to load" }
  };
  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE,  "image",        "Output image" }
  };

  static const GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32,    "run-mode",    "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",       "Input image" },
    { GIMP_PDB_DRAWABLE, "drawable",    "Drawable to export" },
    { GIMP_PDB_STRING,   "uri",         "The URI of the file to export the image in" },
    { GIMP_PDB_STRING,   "raw-uri",     "The URI of the file to export the image in" },
    { GIMP_PDB_INT32,    "spacing",     "Spacing of the brush" },
    { GIMP_PDB_STRING,   "description", "Short description of the brush" }
  };

  gimp_install_procedure (LOAD_PROC,
                          "Loads GIMP brushes",
                          "Loads GIMP brushes (1 or 4 bpp and old .gpb format)",
                          "Tim Newsome, Jens Lautenbacher, Sven Neumann",
                          "Tim Newsome, Jens Lautenbacher, Sven Neumann",
                          "1997-2005",
                          N_("GIMP brush"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_plugin_icon_register (LOAD_PROC, GIMP_ICON_TYPE_ICON_NAME,
                             (const guint8 *) GIMP_ICON_BRUSH);
  gimp_register_file_handler_mime (LOAD_PROC, "image/x-gimp-gbr");
  gimp_register_file_handler_uri (LOAD_PROC);
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "gbr, gpb",
                                    "",
                                    "20, string, GIMP");

  gimp_install_procedure (SAVE_PROC,
                          "Exports files in the GIMP brush file format",
                          "Exports files in the GIMP brush file format",
                          "Tim Newsome, Jens Lautenbacher, Sven Neumann",
                          "Tim Newsome, Jens Lautenbacher, Sven Neumann",
                          "1997-2000",
                          N_("GIMP brush"),
                          "RGB*, GRAY*, INDEXED*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (save_args), 0,
                          save_args, NULL);

  gimp_plugin_icon_register (SAVE_PROC, GIMP_ICON_TYPE_ICON_NAME,
                             (const guint8 *) GIMP_ICON_BRUSH);
  gimp_register_file_handler_mime (SAVE_PROC, "image/x-gimp-gbr");
  gimp_register_file_handler_uri (SAVE_PROC);
  gimp_register_save_handler (SAVE_PROC, "gbr", "");
}