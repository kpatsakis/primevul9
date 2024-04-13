int gx_update_from_subclass(gx_device *dev)
{
    if (!dev->child)
        return 0;

    memcpy(&dev->color_info, &dev->child->color_info, sizeof(gx_device_color_info));
    memcpy(&dev->cached_colors, &dev->child->cached_colors, sizeof(gx_device_cached_colors_t));
    dev->max_fill_band = dev->child->max_fill_band;
    dev->width = dev->child->width;
    dev->height = dev->child->height;
    dev->pad = dev->child->pad;
    dev->log2_align_mod = dev->child->log2_align_mod;
    dev->max_fill_band = dev->child->max_fill_band;
    dev->is_planar = dev->child->is_planar;
    dev->LeadingEdge = dev->child->LeadingEdge;
    memcpy(&dev->ImagingBBox, &dev->child->ImagingBBox, sizeof(dev->child->ImagingBBox));
    dev->ImagingBBox_set = dev->child->ImagingBBox_set;
    memcpy(&dev->MediaSize, &dev->child->MediaSize, sizeof(dev->child->MediaSize));
    memcpy(&dev->HWResolution, &dev->child->HWResolution, sizeof(dev->child->HWResolution));
    memcpy(&dev->Margins, &dev->child->Margins, sizeof(dev->child->Margins));
    memcpy(&dev->HWMargins, &dev->child->HWMargins, sizeof(dev->child->HWMargins));
    dev->FirstPage = dev->child->FirstPage;
    dev->LastPage = dev->child->LastPage;
    dev->PageCount = dev->child->PageCount;
    dev->ShowpageCount = dev->child->ShowpageCount;
    dev->NumCopies = dev->child->NumCopies;
    dev->NumCopies_set = dev->child->NumCopies_set;
    dev->IgnoreNumCopies = dev->child->IgnoreNumCopies;
    dev->UseCIEColor = dev->child->UseCIEColor;
    dev->LockSafetyParams= dev->child->LockSafetyParams;
    dev->band_offset_x = dev->child->band_offset_y;
    dev->sgr = dev->child->sgr;
    dev->MaxPatternBitmap = dev->child->MaxPatternBitmap;
    dev->page_uses_transparency = dev->child->page_uses_transparency;
    memcpy(&dev->space_params, &dev->child->space_params, sizeof(gdev_space_params));
    dev->graphics_type_tag = dev->child->graphics_type_tag;

    return 0;
}