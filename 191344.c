static size_t ComplexTextLayout(const Image *image,const DrawInfo *draw_info,
  const char *text,const size_t length,const FT_Face face,const FT_Int32 flags,
  GraphemeInfo **grapheme)
{
#if defined(MAGICKCORE_RAQM_DELEGATE)
  const char
    *features;

  raqm_t
    *rq;

  raqm_glyph_t
    *glyphs;

  register ssize_t
    i;

  size_t
    extent;

  extent=0;
  rq=raqm_create();
  if (rq == (raqm_t *) NULL)
    goto cleanup;
  if (raqm_set_text_utf8(rq,text,length) == 0)
    goto cleanup;
  if (raqm_set_par_direction(rq,(raqm_direction_t) draw_info->direction) == 0)
    goto cleanup;
  if (raqm_set_freetype_face(rq,face) == 0)
    goto cleanup;
  features=GetImageProperty(image,"type:features");
  if (features != (const char *) NULL)
    {
      char
        breaker,
        quote,
        *token;

      int
        next,
        status_token;

      TokenInfo
        *token_info;

      next=0;
      token_info=AcquireTokenInfo();
      token=AcquireString("");
      status_token=Tokenizer(token_info,0,token,50,features,"",",","",'\0',
        &breaker,&next,&quote);
      while (status_token == 0)
      {
        raqm_add_font_feature(rq,token,strlen(token));
        status_token=Tokenizer(token_info,0,token,50,features,"",",","",'\0',
          &breaker,&next,&quote);
      }
      token_info=DestroyTokenInfo(token_info);
      token=DestroyString(token);
    }
  if (raqm_layout(rq) == 0)
    goto cleanup;
  glyphs=raqm_get_glyphs(rq,&extent);
  if (glyphs == (raqm_glyph_t *) NULL)
    {
      extent=0;
      goto cleanup;
    }
  *grapheme=(GraphemeInfo *) AcquireQuantumMemory(extent,sizeof(**grapheme));
  if (*grapheme == (GraphemeInfo *) NULL)
    {
      extent=0;
      goto cleanup;
    }
  for (i=0; i < (ssize_t) extent; i++)
  {
    (*grapheme)[i].index=glyphs[i].index;
    (*grapheme)[i].x_offset=glyphs[i].x_offset;
    (*grapheme)[i].x_advance=glyphs[i].x_advance;
    (*grapheme)[i].y_offset=glyphs[i].y_offset;
    (*grapheme)[i].cluster=glyphs[i].cluster;
  }

cleanup:
  raqm_destroy(rq);
  return(extent);
#else
  const char
    *p;

  FT_Error
    ft_status;

  register ssize_t
    i;

  ssize_t
    last_glyph;

  /*
    Simple layout for bi-directional text (right-to-left or left-to-right).
  */
  magick_unreferenced(image);
  *grapheme=(GraphemeInfo *) AcquireQuantumMemory(length+1,sizeof(**grapheme));
  if (*grapheme == (GraphemeInfo *) NULL)
    return(0);
  last_glyph=0;
  p=text;
  for (i=0; GetUTFCode(p) != 0; p+=GetUTFOctets(p), i++)
  {
    (*grapheme)[i].index=FT_Get_Char_Index(face,GetUTFCode(p));
    (*grapheme)[i].x_offset=0;
    (*grapheme)[i].y_offset=0;
    if (((*grapheme)[i].index != 0) && (last_glyph != 0))
      {
        if (FT_HAS_KERNING(face))
          {
            FT_Vector
              kerning;

            ft_status=FT_Get_Kerning(face,(FT_UInt) last_glyph,(FT_UInt)
              (*grapheme)[i].index,ft_kerning_default,&kerning);
            if (ft_status == 0)
              (*grapheme)[i-1].x_advance+=(FT_Pos) ((draw_info->direction ==
                RightToLeftDirection ? -1.0 : 1.0)*kerning.x);
          }
      }
    ft_status=FT_Load_Glyph(face,(*grapheme)[i].index,flags);
    (*grapheme)[i].x_advance=face->glyph->advance.x;
    (*grapheme)[i].cluster=p-text;
    last_glyph=(*grapheme)[i].index;
  }
  return((size_t) i);
#endif
}