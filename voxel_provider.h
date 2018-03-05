#ifndef VOXEL_PROVIDER_H
#define VOXEL_PROVIDER_H

#include <resource.h>
#include "voxel_buffer.h"

class VoxelProvider : public Resource{
	GDCLASS(VoxelProvider, Resource)
public:
	virtual void emerge_block(Ref<VoxelBuffer> out_buffer, Vector3i origin_in_voxels);
	virtual void immerge_block(Ref<VoxelBuffer> buffer, Vector3i origin_in_voxels);

protected:
	static void _bind_methods();

	void _emerge_block(Ref<VoxelBuffer> out_buffer, Vector3 origin_in_voxels);
	void _immerge_block(Ref<VoxelBuffer> buffer, Vector3 origin_in_voxels);
};

#endif // VOXEL_PROVIDER_H
