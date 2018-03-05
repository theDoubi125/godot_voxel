tool
extends ImmediateGeometry


var begin = Vector3()
var end = Vector3()
var begin_path
var end_path
var m = SpatialMaterial.new()

onready var start_node = Spatial.new()
onready var end_node = Spatial.new()

var path = []
var draw_path = true

export(NodePath) var draw_start setget set_draw_start, get_draw_start
export(NodePath) var draw_end setget set_draw_end, get_draw_end


func set_draw_start(val):
    if Engine.is_editor_hint():
        begin_path = val
        start_node = get_node(val)
        draw_path_tool( \
            start_node.get_transform().origin - start_node.get_transform().origin, \
            end_node.get_transform().origin - start_node.get_transform().origin)
    else:
        pass


func set_draw_end(val):
    if Engine.is_editor_hint():
        end_path = val
        end_node = get_node(val)
        draw_path_tool( \
            start_node.get_transform().origin - start_node.get_transform().origin, \
            end_node.get_transform().origin - start_node.get_transform().origin)
    else:
        pass


func get_draw_start():
    return begin_path


func get_draw_end():
    return end_path


func _ready():
    if Engine.is_editor_hint():
        draw_path_tool(start_node.get_transform().origin, end_node.get_transform().origin)
        set_process(true)
        m.flags_unshaded = true
        m.flags_use_point_size = true
        m.albedo_color = Color(1.0, 0.0, 0.0, 1.0)

        if get_tree().get_root().has_node("start"):
            print('ok')
        else:
            print('nok')
        if get_tree().get_root().has_node("end"):
            print('ok')
        else:
            print('nok')

func _process(delta):
    if Engine.is_editor_hint():
        draw_path_tool(start_node.get_transform().origin, end_node.get_transform().origin)

func draw_path_tool(_start_pos, _end_pos):
    var p = Array()
    p.append(_start_pos)
    p.append(_end_pos)
    path.invert()

    if (draw_path):
        set_material_override(m)
        clear()
        begin(Mesh.PRIMITIVE_POINTS, null)
        add_vertex(_start_pos)
        add_vertex(_end_pos)
        end()
        begin(Mesh.PRIMITIVE_LINE_STRIP, null)
        for x in p:
            add_vertex(x)
        end()