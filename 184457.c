query (void)
{
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name of the file to load" }
  };
  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE, "image", "Output image" }
  };

#if 0
  static const GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32,    "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",        "Input image" },
    { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
    { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
    { GIMP_PDB_STRING,   "raw-filename", "The name of the file to save the image in" },
    { GIMP_PDB_INT32,    "compression",  "Specify 0 for no compression, 1 for RLE, and 2 for LZ77" }
  };
#endif

  gimp_install_procedure (LOAD_PROC,
                          "loads images from the Paint Shop Pro PSP file format",
                          "This plug-in loads and saves images in "
                          "Paint Shop Pro's native PSP format. "
                          "Vector layers aren't handled. Saving isn't "
                          "yet implemented.",
                          "Tor Lillqvist",
                          "Tor Lillqvist",
                          "1999",
                          N_("Paint Shop Pro image"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_register_file_handler_mime (LOAD_PROC, "image/x-psp");
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "psp,tub,pspimage",
                                    "",
                                    "0,string,Paint\\040Shop\\040Pro\\040Image\\040File\n\032");

  /* commented out until saving is implemented */
#if 0
  gimp_install_procedure (SAVE_PROC,
                          "saves images in the Paint Shop Pro PSP file format",
                          "This plug-in loads and saves images in "
                          "Paint Shop Pro's native PSP format. "
                          "Vector layers aren't handled. Saving isn't "
                          "yet implemented.",
                          "Tor Lillqvist",
                          "Tor Lillqvist",
                          "1999",
                          N_("Paint Shop Pro image"),
                          "RGB*, GRAY*, INDEXED*",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (save_args), 0,
                          save_args, NULL);

  gimp_register_save_handler (SAVE_PROC, "psp,tub", "");
#endif
}