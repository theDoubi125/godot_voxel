#include "voxel_raycast.h"
#include <math_funcs.h>

const float g_infinite = 9999999;

void VoxelRaycastResult::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("has_raycast_hit"), &VoxelRaycastResult::has_raycast_hit);

	ClassDB::bind_method(D_METHOD("get_hit_cell_x"), &VoxelRaycastResult::get_hit_cell_x);
	ClassDB::bind_method(D_METHOD("get_hit_cell_y"), &VoxelRaycastResult::get_hit_cell_y);
	ClassDB::bind_method(D_METHOD("get_hit_cell_z"), &VoxelRaycastResult::get_hit_cell_z);

	ClassDB::bind_method(D_METHOD("get_hit_prev_cell_x"), &VoxelRaycastResult::get_hit_prev_cell_x);
	ClassDB::bind_method(D_METHOD("get_hit_prev_cell_y"), &VoxelRaycastResult::get_hit_prev_cell_y);
	ClassDB::bind_method(D_METHOD("get_hit_prev_cell_z"), &VoxelRaycastResult::get_hit_prev_cell_z);
}

void VoxelRaycastResult::set_hit_data(Vector3i hit_cell, Vector3i hit_prev_cell)
{
	_hit_cell = hit_cell;
	_hit_prev_cell = hit_prev_cell;
}

void VoxelRaycastResult::set_has_hit(bool has_hit)
{
	_has_hit = has_hit;
}

void VoxelRaycast::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("simple_raycast", "origin", "direction", "max_distance"), &VoxelRaycast::simple_raycast);
	ClassDB::bind_method(D_METHOD("set_voxel_provider", "provider"), &VoxelRaycast::set_voxel_provider);
	ClassDB::bind_method(D_METHOD("set_voxel_terrain", "terrain"), &VoxelRaycast::set_voxel_terrain);
}

bool VoxelRaycast::simple_raycast_predicate(Vector3i pos, void *context)
{
	return _voxel_map.ptr()->get_block(pos) == nullptr;
}

bool test(Ref<VoxelMap> map, VoxelTerrain* terrain, Vector3i pos)
{
	int v0 = map->get_voxel(pos, Voxel::CHANNEL_TYPE);

	Ref<VoxelLibrary> lib_ref = terrain->get_voxel_library();
	if (lib_ref.is_null())
		return false;
	const VoxelLibrary &lib = **lib_ref;

	if (lib.has_voxel(v0) == false)
		return false;

	const Voxel &voxel = lib.get_voxel_const(v0);
	if (voxel.is_transparent() == false)
		return true;

	int v1 = map->get_voxel(pos, Voxel::CHANNEL_ISOLEVEL);
	return v1 - 128 >= 0;
}

Ref<VoxelRaycastResult> VoxelRaycast::simple_raycast(Vector3 origin, Vector3 direction, float max_distance)
{
	Vector3i hit_pos(
		Math::floor(origin.x),
		Math::floor(origin.y),
		Math::floor(origin.z));
	Vector3i hit_prev_pos = hit_pos;

	VoxelRaycastResult* result = memnew(VoxelRaycastResult);
	result->set_hit_data(hit_pos, hit_prev_pos);
	ERR_FAIL_COND_V(direction.is_normalized() == false, Ref<VoxelRaycastResult>(&result));
	ERR_FAIL_COND_V(_voxel_map == nullptr, Ref<VoxelRaycastResult>(&result));
	ERR_FAIL_COND_V(_voxel_terrain == nullptr, Ref<VoxelRaycastResult>(&result));

	// Voxel step
	const int xi_step = direction.x > 0 ? 1 : direction.x < 0 ? -1 : 0;
	const int yi_step = direction.y > 0 ? 1 : direction.y < 0 ? -1 : 0;
	const int zi_step = direction.z > 0 ? 1 : direction.z < 0 ? -1 : 0;

	// Parametric voxel step
	const real_t tdelta_x = xi_step != 0 ? 1.f / Math::abs(direction.x) : g_infinite;
	const real_t tdelta_y = yi_step != 0 ? 1.f / Math::abs(direction.y) : g_infinite;
	const real_t tdelta_z = zi_step != 0 ? 1.f / Math::abs(direction.z) : g_infinite;

	// Parametric grid-cross
	real_t tcross_x; // At which value of T we will cross a vertical line?
	real_t tcross_y; // At which value of T we will cross a horizontal line?
	real_t tcross_z; // At which value of T we will cross a depth line?

					 // X initialization
	if (xi_step != 0) {
		if (xi_step == 1)
			tcross_x = (Math::ceil(origin.x) - origin.x) * tdelta_x;
		else
			tcross_x = (origin.x - Math::floor(origin.x)) * tdelta_x;
	}
	else
		tcross_x = g_infinite; // Will never cross on X

							   // Y initialization
	if (yi_step != 0) {
		if (yi_step == 1)
			tcross_y = (Math::ceil(origin.y) - origin.y) * tdelta_y;
		else
			tcross_y = (origin.y - Math::floor(origin.y)) * tdelta_y;
	}
	else
		tcross_y = g_infinite; // Will never cross on X

							   // Z initialization
	if (zi_step != 0) {
		if (zi_step == 1)
			tcross_z = (Math::ceil(origin.z) - origin.z) * tdelta_z;
		else
			tcross_z = (origin.z - Math::floor(origin.z)) * tdelta_z;
	}
	else
		tcross_z = g_infinite; // Will never cross on X

							   /* Iteration */

	while (!test(_voxel_map, _voxel_terrain, hit_prev_pos)) {
		result->set_hit_data(hit_pos, hit_prev_pos);
		hit_prev_pos = hit_pos;
		if (tcross_x < tcross_y) {
			if (tcross_x < tcross_z) {
				// X collision
				//hit.prevPos.x = hit.pos.x;
				hit_pos.x += xi_step;
				if (tcross_x > max_distance)
				{
					result->set_has_hit(false);
					return Ref<VoxelRaycastResult>(result);
				}
				tcross_x += tdelta_x;
			}
			else {
				// Z collision (duplicate code)
				//hit.prevPos.z = hit.pos.z;
				hit_pos.z += zi_step;
				if (tcross_z > max_distance)
				{
					result->set_has_hit(false);
					return Ref<VoxelRaycastResult>(result);
				}
				tcross_z += tdelta_z;
			}
		}
		else {
			if (tcross_y < tcross_z) {
				// Y collision
				//hit.prevPos.y = hit.pos.y;
				hit_pos.y += yi_step;
				if (tcross_y > max_distance)
				{
					result->set_has_hit(false);
					return Ref<VoxelRaycastResult>(result);
				}
				tcross_y += tdelta_y;
			}
			else {
				// Z collision (duplicate code)
				//hit.prevPos.z = hit.pos.z;
				hit_pos.z += zi_step;
				if (tcross_z > max_distance)
				{
					result->set_has_hit(false);
					return Ref<VoxelRaycastResult>(result);
				}
				tcross_z += tdelta_z;
			}
		}

		
	}
	return Ref<VoxelRaycastResult>(result);
}

bool voxel_raycast(
		Vector3 ray_origin,
		Vector3 ray_direction,
		VoxelPredicate predicate,
		void *predicate_context,
		real_t max_distance,
		Vector3i &out_hit_pos,
		Vector3i &out_prev_pos) {
	// Equation : p + v*t
	// p : ray start position (ray.pos)
	// v : ray orientation vector (ray.dir)
	// t : parametric variable = a distance if v is normalized

	// This raycasting technique is described here :
	// http://www.cse.yorku.ca/~amana/research/grid.pdf

	// Note : the grid is assumed to have 1-unit square cells.

	ERR_FAIL_COND_V(predicate == 0, false);
	ERR_FAIL_COND_V(ray_direction.is_normalized() == false, false); // Must be normalized

	/* Initialisation */

	// Voxel position
	Vector3i hit_pos(
			Math::floor(ray_origin.x),
			Math::floor(ray_origin.y),
			Math::floor(ray_origin.z));
	Vector3i hit_prev_pos = hit_pos;

	// Voxel step
	const int xi_step = ray_direction.x > 0 ? 1 : ray_direction.x < 0 ? -1 : 0;
	const int yi_step = ray_direction.y > 0 ? 1 : ray_direction.y < 0 ? -1 : 0;
	const int zi_step = ray_direction.z > 0 ? 1 : ray_direction.z < 0 ? -1 : 0;

	// Parametric voxel step
	const real_t tdelta_x = xi_step != 0 ? 1.f / Math::abs(ray_direction.x) : g_infinite;
	const real_t tdelta_y = yi_step != 0 ? 1.f / Math::abs(ray_direction.y) : g_infinite;
	const real_t tdelta_z = zi_step != 0 ? 1.f / Math::abs(ray_direction.z) : g_infinite;

	// Parametric grid-cross
	real_t tcross_x; // At which value of T we will cross a vertical line?
	real_t tcross_y; // At which value of T we will cross a horizontal line?
	real_t tcross_z; // At which value of T we will cross a depth line?

	// X initialization
	if (xi_step != 0) {
		if (xi_step == 1)
			tcross_x = (Math::ceil(ray_origin.x) - ray_origin.x) * tdelta_x;
		else
			tcross_x = (ray_origin.x - Math::floor(ray_origin.x)) * tdelta_x;
	} else
		tcross_x = g_infinite; // Will never cross on X

	// Y initialization
	if (yi_step != 0) {
		if (yi_step == 1)
			tcross_y = (Math::ceil(ray_origin.y) - ray_origin.y) * tdelta_y;
		else
			tcross_y = (ray_origin.y - Math::floor(ray_origin.y)) * tdelta_y;
	} else
		tcross_y = g_infinite; // Will never cross on X

	// Z initialization
	if (zi_step != 0) {
		if (zi_step == 1)
			tcross_z = (Math::ceil(ray_origin.z) - ray_origin.z) * tdelta_z;
		else
			tcross_z = (ray_origin.z - Math::floor(ray_origin.z)) * tdelta_z;
	} else
		tcross_z = g_infinite; // Will never cross on X

	/* Iteration */

	do {
		hit_prev_pos = hit_pos;
		if (tcross_x < tcross_y) {
			if (tcross_x < tcross_z) {
				// X collision
				//hit.prevPos.x = hit.pos.x;
				hit_pos.x += xi_step;
				if (tcross_x > max_distance)
					return false;
				tcross_x += tdelta_x;
			} else {
				// Z collision (duplicate code)
				//hit.prevPos.z = hit.pos.z;
				hit_pos.z += zi_step;
				if (tcross_z > max_distance)
					return false;
				tcross_z += tdelta_z;
			}
		} else {
			if (tcross_y < tcross_z) {
				// Y collision
				//hit.prevPos.y = hit.pos.y;
				hit_pos.y += yi_step;
				if (tcross_y > max_distance)
					return false;
				tcross_y += tdelta_y;
			} else {
				// Z collision (duplicate code)
				//hit.prevPos.z = hit.pos.z;
				hit_pos.z += zi_step;
				if (tcross_z > max_distance)
					return false;
				tcross_z += tdelta_z;
			}
		}

	} while (!predicate(hit_pos, predicate_context));

	out_hit_pos = hit_pos;
	out_prev_pos = hit_prev_pos;

	return true;
}
