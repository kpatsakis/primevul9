mask_color_matches(const byte *v, const gx_image_enum *penum,
                   int num_components)
{
    int i;

    for (i = num_components * 2, v += num_components - 1; (i -= 2) >= 0; --v)
        if (*v < penum->mask_color.values[i] ||
            *v > penum->mask_color.values[i + 1]
            )
            return false;
    return true;
}