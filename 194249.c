static int find_earliest_filled_buffer(struct camera_data *cam)
{
	int i;
	int found = -1;
	for (i=0; i<cam->num_frames; i++) {
		if(cam->buffers[i].status == FRAME_READY) {
			if(found < 0) {
				found = i;
			} else {
				/* find which buffer is earlier */
				if (cam->buffers[i].ts < cam->buffers[found].ts)
					found = i;
			}
		}
	}
	return found;
}