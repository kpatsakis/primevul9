MagickExport MagickBooleanType GetTypeMetrics(Image *image,
  const DrawInfo *draw_info,TypeMetric *metrics)
{
  DrawInfo
    *annotate_info;

  MagickBooleanType
    status;

  PointInfo
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (DrawInfo *) NULL);
  assert(draw_info->text != (char *) NULL);
  assert(draw_info->signature == MagickCoreSignature);
  annotate_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  annotate_info->render=MagickFalse;
  annotate_info->direction=UndefinedDirection;
  (void) memset(metrics,0,sizeof(*metrics));
  offset.x=0.0;
  offset.y=0.0;
  status=RenderType(image,annotate_info,&offset,metrics);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),"Metrics: text: %s; "
      "width: %g; height: %g; ascent: %g; descent: %g; max advance: %g; "
      "bounds: %g,%g  %g,%g; origin: %g,%g; pixels per em: %g,%g; "
      "underline position: %g; underline thickness: %g",annotate_info->text,
      metrics->width,metrics->height,metrics->ascent,metrics->descent,
      metrics->max_advance,metrics->bounds.x1,metrics->bounds.y1,
      metrics->bounds.x2,metrics->bounds.y2,metrics->origin.x,metrics->origin.y,
      metrics->pixels_per_em.x,metrics->pixels_per_em.y,
      metrics->underline_position,metrics->underline_thickness);
  annotate_info=DestroyDrawInfo(annotate_info);
  return(status);
}