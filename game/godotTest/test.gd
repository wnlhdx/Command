extends Node2D

var a=1
var b:String ="2"
var d:bool=true
var f=Vector2(1,1)
# Declare member variables here. Examples:
# var a = 2
# var b = "text"
func _enter_tree():
	var c:String=String.num_int64(a)
	var e:int=!d
	f.x=2
	print(a)
	self.test(1)
	pass

# Called when the node enters the scene tree for the first time.
func _ready():
	print(a) # Replace with function body.

func test(a:int,b:int=1):
	print("wc")

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
