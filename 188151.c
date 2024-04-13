static MagickBooleanType GetICCProperty(const Image *image,const char *property,
  ExceptionInfo *exception)
{
  const StringInfo
    *profile;

  magick_unreferenced(property);

  profile=GetImageProfile(image,"icc");
  if (profile == (StringInfo *) NULL)
    profile=GetImageProfile(image,"icm");
  if (profile == (StringInfo *) NULL)
    return(MagickFalse);
  if (GetStringInfoLength(profile) < 128)
    return(MagickFalse);  /* minimum ICC profile length */
#if defined(MAGICKCORE_LCMS_DELEGATE)
  {
    cmsHPROFILE
      icc_profile;

    icc_profile=cmsOpenProfileFromMem(GetStringInfoDatum(profile),
      (cmsUInt32Number) GetStringInfoLength(profile));
    if (icc_profile != (cmsHPROFILE *) NULL)
      {
#if defined(LCMS_VERSION) && (LCMS_VERSION < 2000)
        const char
          *name;

        name=cmsTakeProductName(icc_profile);
        if (name != (const char *) NULL)
          (void) SetImageProperty((Image *) image,"icc:name",name,exception);
#else
        char
          info[MagickPathExtent];

        unsigned int
          extent;

        (void) memset(info,0,sizeof(info));
        extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoDescription,"en","US",
          NULL,0);
        if (extent != 0)
          {
            extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoDescription,"en",
              "US",info,MagickMin(MagickPathExtent-1,extent));
            (void) SetImageProperty((Image *) image,"icc:description",info,
              exception);
         }
        extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoManufacturer,"en","US",
          NULL,0);
        if (extent != 0)
          {
            extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoManufacturer,"en",
              "US",info,MagickMin(MagickPathExtent-1,extent));
            (void) SetImageProperty((Image *) image,"icc:manufacturer",info,
              exception);
          }
        extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoModel,"en","US",
          NULL,0);
        if (extent != 0)
          {
            extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoModel,"en","US",
              info,MagickMin(MagickPathExtent-1,extent));
            (void) SetImageProperty((Image *) image,"icc:model",info,exception);
          }
        extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoCopyright,"en","US",
          NULL,0);
        if (extent != 0)
          {
            extent=cmsGetProfileInfoASCII(icc_profile,cmsInfoCopyright,"en",
              "US",info,MagickMin(MagickPathExtent-1,extent));
            (void) SetImageProperty((Image *) image,"icc:copyright",info,
              exception);
          }
#endif
        (void) cmsCloseProfile(icc_profile);
      }
  }
#endif
  return(MagickTrue);
}