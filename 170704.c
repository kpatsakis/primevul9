color_halftone_callback(cal_halftone_data_t *ht, void *arg)
{
    gx_device *dev = arg;
    gx_color_index dev_white = gx_device_white(dev);
    gx_color_index dev_black = gx_device_black(dev);

    if (dev->is_planar) {
        (*dev_proc(dev, copy_planes)) (dev, ht->data, ht->x + (ht->offset_x<<3), ht->raster,
            gx_no_bitmap_id, ht->x, ht->y, ht->w, ht->h,
            ht->plane_raster);
    } else {
        (*dev_proc(dev, copy_mono)) (dev, ht->data, ht->x + (ht->offset_x<<3), ht->raster,
            gx_no_bitmap_id, ht->x, ht->y, ht->w, ht->h, dev_white,
            dev_black);
    }
}