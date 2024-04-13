pwg_ppdize_resolution(
    ipp_attribute_t *attr,		/* I - Attribute to convert */
    int             element,		/* I - Element to convert */
    int             *xres,		/* O - X resolution in DPI */
    int             *yres,		/* O - Y resolution in DPI */
    char            *name,		/* I - Name buffer */
    size_t          namesize)		/* I - Size of name buffer */
{
  ipp_res_t units;			/* Units for resolution */


  *xres = ippGetResolution(attr, element, yres, &units);

  if (units == IPP_RES_PER_CM)
  {
    *xres = (int)(*xres * 2.54);
    *yres = (int)(*yres * 2.54);
  }

  if (name && namesize > 4)
  {
    if (*xres == *yres)
      snprintf(name, namesize, "%ddpi", *xres);
    else
      snprintf(name, namesize, "%dx%ddpi", *xres, *yres);
  }
}