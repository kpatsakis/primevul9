_jpeg_color_space_name (const J_COLOR_SPACE jpeg_color_space)
{
	switch (jpeg_color_space) {
	case JCS_UNKNOWN: return "UNKNOWN";
	case JCS_GRAYSCALE: return "GRAYSCALE";
	case JCS_RGB: return "RGB";
	case JCS_YCbCr: return "YCbCr";
	case JCS_CMYK: return "CMYK";
	case JCS_YCCK: return "YCCK";
	default: return "invalid";
	}
}