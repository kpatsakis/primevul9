epass2003_hook_file(struct sc_file *file, int inc)
{
	int fidl = file->id & 0xff;
	int fidh = file->id & 0xff00;
	if (epass2003_hook_path(&file->path, inc)) {
		if (inc)
			file->id = fidh + fidl * FID_STEP;
		else
			file->id = fidh + fidl / FID_STEP;
	}
}