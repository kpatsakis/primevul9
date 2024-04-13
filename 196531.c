void LibRaw::parse_x3f()
{
  x3f_t *x3f = x3f_new_from_file(libraw_internal_data.internal_data.input);
  if(!x3f)
      return;
  _x3f_data = x3f;

  x3f_header_t *H = NULL;
  x3f_directory_section_t *DS = NULL;

  H = &x3f->header;
  // Parse RAW size from RAW section
  x3f_directory_entry_t *DE = x3f_get_raw(x3f);
  if(!DE) return;
  imgdata.sizes.flip = H->rotation;
  x3f_directory_entry_header_t *DEH = &DE->header;
  x3f_image_data_t *ID = &DEH->data_subsection.image_data;
  imgdata.sizes.raw_width = ID->columns;
  imgdata.sizes.raw_height = ID->rows;
  // Parse other params from property section
  DE = x3f_get_prop(x3f);
  if(! (x3f_load_data(x3f,DE) == X3F_OK))
    return;
  DEH = &DE->header;
  x3f_property_list_t *PL = &DEH->data_subsection.property_list;
  if (PL->property_table.size != 0) {
    int i;
    x3f_property_t *P = PL->property_table.element;
    for (i=0; i<PL->num_properties; i++) {
      char name[100], value[100];
      utf2char(P[i].name,name);
      utf2char(P[i].value,value);
      if (!strcmp (name, "ISO"))
	imgdata.other.iso_speed = atoi(value);
      if (!strcmp (name, "CAMMANUF"))
        strcpy (imgdata.idata.make, value);
      if (!strcmp (name, "CAMMODEL"))
        strcpy (imgdata.idata.model, value);
      if (!strcmp (name, "WB_DESC"))
        strcpy (imgdata.color.model2, value);
      if (!strcmp (name, "TIME"))
	    imgdata.other.timestamp = atoi(value);
      if (!strcmp (name, "EXPTIME"))
        imgdata.other.shutter = atoi(value) / 1000000.0;
      if (!strcmp (name, "APERTURE"))
        imgdata.other.aperture = atof(value);
      if (!strcmp (name, "FLENGTH"))
        imgdata.other.focal_len = atof(value);
    }
    imgdata.idata.raw_count=1;
    load_raw = &LibRaw::x3f_load_raw;
    imgdata.sizes.raw_pitch = imgdata.sizes.raw_width*6;
    imgdata.idata.is_foveon = 1;
    libraw_internal_data.internal_output_params.raw_color=1; // Force adobe coeff
    imgdata.color.maximum=0x3fff; // To be reset by color table
    libraw_internal_data.unpacker_data.order = 0x4949;
  }
  // Try to get thumbnail data
  LibRaw_thumbnail_formats format = LIBRAW_THUMBNAIL_UNKNOWN;
  if(DE = x3f_get_thumb_jpeg(x3f))
    {
      format = LIBRAW_THUMBNAIL_JPEG;
    }
  else if(DE = x3f_get_thumb_plain(x3f))
    {
      format = LIBRAW_THUMBNAIL_BITMAP;
    }
  if(DE)
    {
      x3f_directory_entry_header_t *DEH = &DE->header;
      x3f_image_data_t *ID = &DEH->data_subsection.image_data;
      imgdata.thumbnail.twidth = ID->columns;
      imgdata.thumbnail.theight = ID->rows;
      imgdata.thumbnail.tcolors = 3;
      imgdata.thumbnail.tformat = format;
      libraw_internal_data.internal_data.toffset = DE->input.offset;
      write_thumb = &LibRaw::x3f_thumb_loader;
    }
}