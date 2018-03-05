tool
extends ImmediateGeometry

export var draw_path = true
var m = SpatialMaterial.new()
var target_pos = Vector3(0, 0, 0)
var _color = Color(1.0, 1.0, 0, 1.0)

func _ready():
	if Engine.is_editor_hint():
		m.flags_unshaded = true
		m.flags_use_point_size = true
		m.albedo_color = _color
		print(m.albedo_color)

func set_color(color):
	if Engine.is_editor_hint():
		_color = color
		print(_color)
		if m != null:
			m.albedo_color = _color

func _process(delta):
	if Engine.is_editor_hint():
		draw_path_tool()

func draw_path_tool():
	if (draw_path): 
		var p = Array()
		p.append(Vector3(0, 0, 0))
		p.append(Vector3(1, 0, 0))
		p.append(Vector3(1, 0, 1))
		p.append(Vector3(0, 0, 1))
		p.append(Vector3(0, 0, 0))
		p.append(Vector3(0, 1, 0))
		p.append(Vector3(1, 1, 0))
		p.append(Vector3(1, 1, 1))
		p.append(Vector3(0, 1, 1))
		p.append(Vector3(0, 1, 0))
		set_material_override(m)
		clear()
		begin(Mesh.PRIMITIVE_POINTS, null)
		add_vertex(target_pos + Vector3(0, 0, 0))
		add_vertex(target_pos + Vector3(1, 0, 0))
		end()
		begin(Mesh.PRIMITIVE_LINE_STRIP, null)
		for x in p:
			add_vertex(target_pos + x)
		end()
		
		p.clear()
		p.append(Vector3(1, 0, 0))
		p.append(Vector3(1, 1, 0))
		p.append(Vector3(1, 0, 1))
		p.append(Vector3(1, 1, 1))
		p.append(Vector3(0, 0, 1))
		p.append(Vector3(0, 1, 1))
		begin(Mesh.PRIMITIVE_LINES, null)
		for x in p:
			add_vertex(target_pos + x)
		end()
		