tool

extends EditorPlugin

var dock
var handledTerrain
var remove_cursor
var add_cursor

var cursor_class = preload("editor/cursor.gd")

func _enter_tree():
	dock = preload("res://addons/voxel/Button.tscn").instance()

func _exit_tree():
	if dock != null:
		dock.free()
		dock = null
	
func make_visible(visible):
	if(visible):
		add_control_to_dock(DOCK_SLOT_RIGHT_UL, dock)
	else:
		if remove_cursor != null:
			remove_cursor.free()
			remove_cursor = null
		if add_cursor != null:
			add_cursor.free()
			add_cursor = null
		handledTerrain = null
		if dock != null:
			remove_control_from_docks(dock)

func handles(object):
	return object is VoxelTerrain
 
func forward_spatial_gui_input(camera, event):
	if(event is InputEventMouse):
		var ray_origin = camera.project_ray_origin(event.position)
		var ray_normal = camera.project_ray_normal(event.position)
		var raycast = VoxelRaycast.new()
		raycast.set_voxel_provider(handledTerrain.get_storage())
		raycast.set_voxel_terrain(handledTerrain)
		var raycast_result = raycast.simple_raycast(ray_origin, ray_normal, 50)
		var hit_pos = Vector3(raycast_result.get_hit_cell_x(), raycast_result.get_hit_cell_y(), raycast_result.get_hit_cell_z())
		var last_hit_pos = Vector3(raycast_result.get_hit_prev_cell_x(), raycast_result.get_hit_prev_cell_y(), raycast_result.get_hit_prev_cell_z())
		if event is InputEventMouseButton:
			if event.button_index == BUTTON_LEFT && event.pressed:
				handledTerrain.get_storage().set_voxel(1, last_hit_pos.x, last_hit_pos.y, last_hit_pos.z, 0)
				handledTerrain.make_voxel_dirty(last_hit_pos)
				print(last_hit_pos)
				return true
			if event.button_index == BUTTON_RIGHT && event.pressed:
				handledTerrain.get_storage().set_voxel(0, hit_pos.x, hit_pos.y, hit_pos.z, 0)
				handledTerrain.make_voxel_dirty(hit_pos)
				print(hit_pos)
				return true
		else:
			if remove_cursor != null:
				remove_cursor.transform.origin = hit_pos 
			if add_cursor != null:
				add_cursor.transform.origin = last_hit_pos
			return false
	return false
	
func edit(object):
	if object != handledTerrain:
		if remove_cursor != null:
			remove_cursor.free()
			remove_cursor = null
		if add_cursor != null:
			add_cursor.free()
			add_cursor = null
		handledTerrain = object
		print(cursor_class)
		remove_cursor = cursor_class.new()
		remove_cursor.set_name("remove_cursor")
		remove_cursor.set_color(Color(1.0, 0, 0, 1.0))
		add_cursor = cursor_class.new()
		add_cursor.set_name("add_cursor")
		add_cursor.set_color(Color(0, 1.0, 0, 1.0))
		handledTerrain.add_child(remove_cursor)
		handledTerrain.add_child(add_cursor)