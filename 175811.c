int epo_put_image(gx_device *dev, const byte **buffers, int num_chan, int x, int y,
            int width, int height, int row_stride,
            int alpha_plane_index, int tag_plane_index)
{
    int code = epo_handle_erase_page(dev);

    if (code != 0)
        return code;
    return dev_proc(dev, put_image)(dev, buffers, num_chan, x, y, width, height, row_stride, alpha_plane_index, tag_plane_index);
}