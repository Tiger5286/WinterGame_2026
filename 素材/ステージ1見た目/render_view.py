import bpy, sys, math
from mathutils import Vector
# usage: blender -b --python render_view.py -- <fbx> <outprefix>
argv = sys.argv[sys.argv.index("--") + 1:]
fbx, out = argv[0], argv[1]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
sc = bpy.context.scene
sc.render.engine = 'BLENDER_EEVEE'
sc.render.resolution_x, sc.render.resolution_y = 960, 540
sc.view_settings.view_transform = 'Standard'
w = bpy.data.worlds.new("W"); sc.world = w
w.use_nodes = True
w.node_tree.nodes["Background"].inputs[0].default_value = (0.35, 0.35, 0.38, 1)
w.node_tree.nodes["Background"].inputs[1].default_value = 0.6
sun = bpy.data.objects.new("Sun", bpy.data.lights.new("Sun", 'SUN'))
sun.data.energy = 2.5
sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(30))
sc.collection.objects.link(sun)
cam = bpy.data.objects.new("Cam", bpy.data.cameras.new("Cam"))
cam.data.lens = 18
sc.collection.objects.link(cam); sc.camera = cam
views = [((0, -2, 2.6), (0, 10, 2.2)), ((30, -3, 2.6), (40, -3, 2.0)), ((60, 2, 3.0), (75, 8, 2.0)), ((16, 10, 2.6), (16, 22, 2.0))]
for i, (p, t) in enumerate(views):
    cam.location = Vector(p)
    cam.rotation_euler = (Vector(t) - Vector(p)).to_track_quat('-Z', 'Y').to_euler()
    sc.render.filepath = f"{out}_{i}.png"
    bpy.ops.render.render(write_still=True)
