static bool get_bool(struct platform_device *ofdev, const char *s)
{
	u32 *p = (u32 *)of_get_property(ofdev->dev.of_node, s, NULL);

	if (!p) {
		dev_warn(&ofdev->dev, "Parameter %s not found, defaulting to false\n", s);
		return false;
	}

	return (bool)*p;
}