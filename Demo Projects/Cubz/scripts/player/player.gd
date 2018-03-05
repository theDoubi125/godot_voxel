extends Spatial

# class member variables go here, for example:
# var a = 2
# var b = "textvar"
export var movementDuration = 1

var movementDir = Vector3(0, 0, 0)
var isMoving = false
var movementTime = 0
var movementStartPos

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	pass
	
func startMovement(direction):
	movementDir = direction
	isMoving = true
	movementTime = 0
	movementStartPos = get_global_transform().origin

func updateMovement(delta):
	if(isMoving):
		movementTime += delta
		
		var rotationAxis = movementDir.cross(Vector3(0, 1, 0))
		var fixedPointOffset = movementDir + Vector3(0, 1, 0)
		var rotatedOffset = fixedPointOffset.rotated(rotationAxis, movementTime * PI / 2 / movementDuration)
		
		global_translate(movementStartPos + rotatedOffset - fixedPointOffset - get_global_transform().origin)
		
		if(movementTime > movementDuration):
			global_rotate(rotationAxis, (delta - (movementTime - movementDuration)) * PI / 2 / movementDuration)
			isMoving = false
		else:
			global_rotate(rotationAxis, deg2rad(delta * 90 / movementDuration))

func _process(delta):
	if(!isMoving):
		if(Input.is_action_pressed("move_up")):
			startMovement(Vector3(0, 0, 1))
		if(Input.is_action_pressed("move_down")):
			startMovement(Vector3(0, 0, -1))
		if(Input.is_action_pressed("move_left")):
			startMovement(Vector3(1, 0, 0))
		if(Input.is_action_pressed("move_right")):
			startMovement(Vector3(-1, 0, 0))
	updateMovement(delta)
#	# Called every frame. Delta is time since last frame.
#	# Update game logic here.
	pass
