v3d_lookup_bos(struct drm_device *dev,
	       struct drm_file *file_priv,
	       struct v3d_job *job,
	       u64 bo_handles,
	       u32 bo_count)
{
	u32 *handles;
	int ret = 0;
	int i;

	job->bo_count = bo_count;

	if (!job->bo_count) {
		/* See comment on bo_index for why we have to check
		 * this.
		 */
		DRM_DEBUG("Rendering requires BOs\n");
		return -EINVAL;
	}

	job->bo = kvmalloc_array(job->bo_count,
				 sizeof(struct drm_gem_cma_object *),
				 GFP_KERNEL | __GFP_ZERO);
	if (!job->bo) {
		DRM_DEBUG("Failed to allocate validated BO pointers\n");
		return -ENOMEM;
	}

	handles = kvmalloc_array(job->bo_count, sizeof(u32), GFP_KERNEL);
	if (!handles) {
		ret = -ENOMEM;
		DRM_DEBUG("Failed to allocate incoming GEM handles\n");
		goto fail;
	}

	if (copy_from_user(handles,
			   (void __user *)(uintptr_t)bo_handles,
			   job->bo_count * sizeof(u32))) {
		ret = -EFAULT;
		DRM_DEBUG("Failed to copy in GEM handles\n");
		goto fail;
	}

	spin_lock(&file_priv->table_lock);
	for (i = 0; i < job->bo_count; i++) {
		struct drm_gem_object *bo = idr_find(&file_priv->object_idr,
						     handles[i]);
		if (!bo) {
			DRM_DEBUG("Failed to look up GEM BO %d: %d\n",
				  i, handles[i]);
			ret = -ENOENT;
			spin_unlock(&file_priv->table_lock);
			goto fail;
		}
		drm_gem_object_get(bo);
		job->bo[i] = bo;
	}
	spin_unlock(&file_priv->table_lock);

fail:
	kvfree(handles);
	return ret;
}