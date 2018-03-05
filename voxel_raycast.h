#include "vector3i.h"
#include <vector3.h>
#include <reference.h>
#include "voxel_terrain.h"

// TODO that could be a template function
// pos: voxel position
// context: arguments to carry (as a lamdbda capture)
typedef bool (*VoxelPredicate)(Vector3i pos, void *context);

class VoxelRaycastResult : public Reference {
	GDCLASS(VoxelRaycastResult, Reference)

public:
	VoxelRaycastResult() : _has_hit(false) {}
	VoxelRaycastResult(Vector3i position, Vector3i prev_cell) : _has_hit(true), _hit_cell(position), _hit_prev_cell(prev_cell) {}
	~VoxelRaycastResult() {}

	static void _bind_methods();

	_FORCE_INLINE_ bool has_raycast_hit() const { return _has_hit; }
	_FORCE_INLINE_ int get_hit_cell_x() const { return _hit_cell.x; }
	_FORCE_INLINE_ int get_hit_cell_y() const { return _hit_cell.y; }
	_FORCE_INLINE_ int get_hit_cell_z() const { return _hit_cell.z; }
	_FORCE_INLINE_ int get_hit_prev_cell_x() const { return _hit_prev_cell.x; }
	_FORCE_INLINE_ int get_hit_prev_cell_y() const { return _hit_prev_cell.y; }
	_FORCE_INLINE_ int get_hit_prev_cell_z() const { return _hit_prev_cell.z; }

	void set_hit_data(Vector3i hit_cell, Vector3i hit_prev_cell);
	void set_has_hit(bool has_hit);

private:
	bool _has_hit;
	Vector3i _hit_cell, _hit_prev_cell;
};

class VoxelRaycast : public Reference {
	GDCLASS(VoxelRaycast, Reference)

public:
	VoxelRaycast() {}
	~VoxelRaycast() {}

	static void _bind_methods();

	void set_voxel_provider(Ref<VoxelMap> voxel_map) { _voxel_map = voxel_map; }
	void set_voxel_terrain(Object* voxel_terrain) { _voxel_terrain = (VoxelTerrain*)voxel_terrain; }
	bool simple_raycast_predicate(Vector3i pos, void *context);
	Ref<VoxelRaycastResult> simple_raycast(Vector3 origin, Vector3 direction, float max_distance);

private:
	Ref<VoxelMap> _voxel_map;
	VoxelTerrain* _voxel_terrain;
};

bool voxel_raycast(
		Vector3 ray_origin,
		Vector3 ray_direction,
		VoxelPredicate predicate,
		void *predicate_context, // Handle that one with care
		real_t max_distance,
		Vector3i &out_hit_pos,
		Vector3i &out_prev_pos);
