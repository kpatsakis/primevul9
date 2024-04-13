MagickExport void XGetResourceInfo(const ImageInfo *image_info,
  XrmDatabase database,const char *client_name,XResourceInfo *resource_info)
{
  char
    *directory,
    *resource_value;

  extern const char
    BorderColor[],
    ForegroundColor[];

  /*
    Initialize resource info fields.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(resource_info != (XResourceInfo *) NULL);
  (void) memset(resource_info,0,sizeof(*resource_info));
  resource_info->resource_database=database;
  resource_info->image_info=(ImageInfo *) image_info;
  (void) SetImageInfoProgressMonitor(resource_info->image_info,
    XMagickProgressMonitor,(void *) NULL);
  resource_info->quantize_info=CloneQuantizeInfo((QuantizeInfo *) NULL);
  resource_info->close_server=MagickTrue;
  resource_info->client_name=AcquireString(client_name);
  resource_value=XGetResourceClass(database,client_name,"backdrop",
    (char *) "False");
  resource_info->backdrop=IsStringTrue(resource_value);
  resource_info->background_color=XGetResourceInstance(database,client_name,
    "background",(char *) "#d6d6d6d6d6d6");
  resource_info->border_color=XGetResourceInstance(database,client_name,
    "borderColor",BorderColor);
  resource_value=XGetResourceClass(database,client_name,"borderWidth",
    (char *) "2");
  resource_info->border_width=(unsigned int) StringToUnsignedLong(
    resource_value);
  resource_value=XGetResourceClass(database,client_name,"colormap",
    (char *) "shared");
  resource_info->colormap=UndefinedColormap;
  if (LocaleCompare("private",resource_value) == 0)
    resource_info->colormap=PrivateColormap;
  if (LocaleCompare("shared",resource_value) == 0)
    resource_info->colormap=SharedColormap;
  if (resource_info->colormap == UndefinedColormap)
    ThrowXWindowException(OptionError,"UnrecognizedColormapType",
      resource_value);
  resource_value=XGetResourceClass(database,client_name,
    "colorRecovery",(char *) "False");
  resource_info->color_recovery=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"confirmExit",
    (char *) "False");
  resource_info->confirm_exit=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"confirmEdit",
    (char *) "False");
  resource_info->confirm_edit=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"delay",(char *) "1");
  resource_info->delay=(unsigned int) StringToUnsignedLong(resource_value);
  resource_info->display_gamma=XGetResourceClass(database,client_name,
    "displayGamma",(char *) "2.2");
  resource_value=XGetResourceClass(database,client_name,"displayWarnings",
    (char *) "True");
  resource_info->display_warnings=IsStringTrue(resource_value);
  resource_info->font=XGetResourceClass(database,client_name,"font",
    (char *) NULL);
  resource_info->font=XGetResourceClass(database,client_name,"fontList",
    resource_info->font);
  resource_info->font_name[0]=XGetResourceClass(database,client_name,"font1",
    (char *) "fixed");
  resource_info->font_name[1]=XGetResourceClass(database,client_name,"font2",
    (char *) "variable");
  resource_info->font_name[2]=XGetResourceClass(database,client_name,"font3",
    (char *) "5x8");
  resource_info->font_name[3]=XGetResourceClass(database,client_name,"font4",
    (char *) "6x10");
  resource_info->font_name[4]=XGetResourceClass(database,client_name,"font5",
    (char *) "7x13bold");
  resource_info->font_name[5]=XGetResourceClass(database,client_name,"font6",
    (char *) "8x13bold");
  resource_info->font_name[6]=XGetResourceClass(database,client_name,"font7",
    (char *) "9x15bold");
  resource_info->font_name[7]=XGetResourceClass(database,client_name,"font8",
    (char *) "10x20");
  resource_info->font_name[8]=XGetResourceClass(database,client_name,"font9",
    (char *) "12x24");
  resource_info->font_name[9]=XGetResourceClass(database,client_name,"font0",
    (char *) "fixed");
  resource_info->font_name[10]=XGetResourceClass(database,client_name,"font0",
    (char *) "fixed");
  resource_info->foreground_color=XGetResourceInstance(database,client_name,
    "foreground",ForegroundColor);
  resource_value=XGetResourceClass(database,client_name,"gammaCorrect",
    (char *) "False");
  resource_info->gamma_correct=IsStringTrue(resource_value);
  resource_info->image_geometry=ConstantString(XGetResourceClass(database,
    client_name,"geometry",(char *) NULL));
  resource_value=XGetResourceClass(database,client_name,"gravity",
    (char *) "Center");
  resource_info->gravity=(GravityType) ParseCommandOption(MagickGravityOptions,
    MagickFalse,resource_value);
  directory=getcwd(resource_info->home_directory,MagickPathExtent);
  (void) directory;
  resource_info->icon_geometry=XGetResourceClass(database,client_name,
    "iconGeometry",(char *) NULL);
  resource_value=XGetResourceClass(database,client_name,"iconic",
    (char *) "False");
  resource_info->iconic=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"immutable",
    LocaleCompare(client_name,"PerlMagick") == 0 ? (char *) "True" :
    (char *) "False");
  resource_info->immutable=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"magnify",
    (char *) "3");
  resource_info->magnify=(unsigned int) StringToUnsignedLong(resource_value);
  resource_info->map_type=XGetResourceClass(database,client_name,"map",
    (char *) NULL);
  resource_info->matte_color=XGetResourceInstance(database,client_name,
    "mattecolor",(char *) NULL);
  resource_info->name=ConstantString(XGetResourceClass(database,client_name,
    "name",(char *) NULL));
  resource_info->pen_colors[0]=XGetResourceClass(database,client_name,"pen1",
    (char *) "black");
  resource_info->pen_colors[1]=XGetResourceClass(database,client_name,"pen2",
    (char *) "blue");
  resource_info->pen_colors[2]=XGetResourceClass(database,client_name,"pen3",
    (char *) "cyan");
  resource_info->pen_colors[3]=XGetResourceClass(database,client_name,"pen4",
    (char *) "green");
  resource_info->pen_colors[4]=XGetResourceClass(database,client_name,"pen5",
    (char *) "gray");
  resource_info->pen_colors[5]=XGetResourceClass(database,client_name,"pen6",
    (char *) "red");
  resource_info->pen_colors[6]=XGetResourceClass(database,client_name,"pen7",
    (char *) "magenta");
  resource_info->pen_colors[7]=XGetResourceClass(database,client_name,"pen8",
    (char *) "yellow");
  resource_info->pen_colors[8]=XGetResourceClass(database,client_name,"pen9",
    (char *) "white");
  resource_info->pen_colors[9]=XGetResourceClass(database,client_name,"pen0",
    (char *) "gray");
  resource_info->pen_colors[10]=XGetResourceClass(database,client_name,"pen0",
    (char *) "gray");
  resource_value=XGetResourceClass(database,client_name,"pause",(char *) "0");
  resource_info->pause=(unsigned int) StringToUnsignedLong(resource_value);
  resource_value=XGetResourceClass(database,client_name,"quantum",(char *) "1");
  resource_info->quantum=StringToLong(resource_value);
  resource_info->text_font=XGetResourceClass(database,client_name,(char *)
    "font",(char *) "fixed");
  resource_info->text_font=XGetResourceClass(database,client_name,
    "textFontList",resource_info->text_font);
  resource_info->title=XGetResourceClass(database,client_name,"title",
    (char *) NULL);
  resource_value=XGetResourceClass(database,client_name,"undoCache",
    (char *) "256");
  resource_info->undo_cache=(unsigned int) StringToUnsignedLong(resource_value);
  resource_value=XGetResourceClass(database,client_name,"update",
    (char *) "False");
  resource_info->update=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"usePixmap",
    (char *) "True");
  resource_info->use_pixmap=IsStringTrue(resource_value);
  resource_value=XGetResourceClass(database,client_name,"sharedMemory",
    (char *) "True");
  resource_info->use_shared_memory=IsStringTrue(resource_value);
  resource_info->visual_type=XGetResourceClass(database,client_name,"visual",
    (char *) NULL);
  resource_info->window_group=XGetResourceClass(database,client_name,
    "windowGroup",(char *) NULL);
  resource_info->window_id=XGetResourceClass(database,client_name,"window",
    (char *) NULL);
  resource_info->write_filename=XGetResourceClass(database,client_name,
    "writeFilename",(char *) NULL);
}