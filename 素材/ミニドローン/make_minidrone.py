import bpy, sys, math, os, mathutils

# 使い方: blender -b --factory-startup --python make_minidrone.py -- <出力フォルダ> <プレビューpng>
out_dir = sys.argv[sys.argv.index("--") + 1]
preview = sys.argv[sys.argv.index("--") + 2]

bpy.ops.wm.read_factory_settings(use_empty=True)
scn = bpy.context.scene
scn.render.fps = 30

def mat(name, rgb, rough=0.4, metal=0.0, emit=0.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (*rgb, 1)
    b.inputs["Roughness"].default_value = rough
    b.inputs["Metallic"].default_value = metal
    if emit > 0:
        b.inputs["Emission Color"].default_value = (*rgb, 1)
        b.inputs["Emission Strength"].default_value = emit
    m.diffuse_color = (*rgb, 1)
    return m

M = {
    "white": mat("MiniDrone_White", (0.85, 0.86, 0.88), 0.35),
    "grey":  mat("MiniDrone_Grey", (0.22, 0.23, 0.25), 0.5, 0.3),
    "dark":  mat("MiniDrone_Dark", (0.03, 0.03, 0.035), 0.45, 0.2),
    "blue":  mat("MiniDrone_Blue", (0.03, 0.07, 0.20), 0.35, 0.3),
    "red":   mat("MiniDrone_RedLight", (0.9, 0.01, 0.01), 0.2, 0.0, 1.2),
    "redD":  mat("MiniDrone_RedDark", (0.6, 0.01, 0.01), 0.4, 0.0, 0.3),
}

parts = []

def box(name, size, loc, m, bevel=0.012, seg=4):
    bpy.ops.mesh.primitive_cube_add(size=1, location=loc)
    o = bpy.context.object
    o.name = name
    o.scale = size
    bpy.ops.object.transform_apply(scale=True)
    o.data.materials.append(M[m])
    if bevel > 0:
        md = o.modifiers.new("Bevel", "BEVEL")
        md.width = bevel
        md.segments = seg
        md.limit_method = "ANGLE"
        md.harden_normals = True   # 面は平らなまま、角だけ丸く見せる
    parts.append(o)
    return o

def taper(o, scale, top=True):
    # 上面(top=True)または底面の頂点だけ縮めて台形にする
    zs = [v.co.z for v in o.data.vertices]
    target = max(zs) if top else min(zs)
    for v in o.data.vertices:
        if abs(v.co.z - target) < 1e-4:
            v.co.x *= scale
            v.co.y *= scale

# ---- 本体 (前 = -Y) ----
box("BodyLower", (0.48, 0.48, 0.09), (0, 0, -0.13), "grey", 0.015)
box("RedSeam",   (0.49, 0.49, 0.022), (0, 0, -0.075), "redD", 0.005)
box("BodyUpper", (0.50, 0.50, 0.15), (0, 0, 0.0), "white", 0.025)

# ---- 底の追加段 ----
box("BottomSeam", (0.40, 0.40, 0.02), (0, 0, -0.18), "dark", 0.005)
bottom = box("BodyBottom", (0.42, 0.42, 0.06), (0, 0, -0.215), "white", 0.018)
taper(bottom, 0.82, top=False)
box("BottomCore", (0.16, 0.16, 0.02), (0, 0, -0.25), "dark", 0.006)
bpy.ops.mesh.primitive_cylinder_add(radius=0.04, depth=0.012, vertices=48, location=(0, 0, -0.262))
blight = bpy.context.object
blight.name = "BottomLight"
blight.data.materials.append(M["red"])
parts.append(blight)

# 側面のスリット (左右・後ろ)
for sx in (-1, 1):
    for i, z in enumerate((0.03, -0.02)):
        box(f"SideSlot{sx}_{i}", (0.012, 0.30, 0.018), (sx * 0.252, 0.03, z), "grey", 0.004)
    box(f"SideSlotLow{sx}", (0.012, 0.32, 0.015), (sx * 0.242, 0.0, -0.13), "dark", 0.004)
for i, z in enumerate((0.03, -0.02)):
    box(f"BackSlot{i}", (0.30, 0.012, 0.018), (0, 0.252, z), "grey", 0.004)

# ---- 正面センサー ----
box("FrontFrame", (0.30, 0.05, 0.27), (0, -0.265, -0.04), "dark", 0.025)
box("FrontBezel", (0.24, 0.03, 0.21), (0, -0.29, -0.04), "white", 0.014)
box("LensHousing", (0.17, 0.02, 0.15), (0, -0.305, -0.04), "dark", 0.012)
bpy.ops.mesh.primitive_uv_sphere_add(radius=0.065, segments=64, ring_count=32, location=(0, -0.31, -0.04))
lens = bpy.context.object
lens.name = "Lens"
lens.scale = (1, 0.45, 1)
bpy.ops.object.transform_apply(scale=True)
lens.data.materials.append(M["red"])
parts.append(lens)
for sx in (-1, 1):
    box(f"FrontGuard{sx}", (0.05, 0.07, 0.20), (sx * 0.19, -0.265, -0.04), "white", 0.014)

# ---- 上部 ----
box("TopPlate", (0.42, 0.42, 0.035), (0, 0.01, 0.092), "blue", 0.012)
for sx in (-1, 1):
    box(f"TopRail{sx}", (0.04, 0.30, 0.025), (sx * 0.17, 0.02, 0.12), "white", 0.009)
cap1 = box("TopCap1", (0.28, 0.28, 0.05), (0, 0.02, 0.135), "dark", 0.012)
taper(cap1, 0.85)
cap2 = box("TopCap2", (0.17, 0.17, 0.05), (0, 0.02, 0.18), "dark", 0.012)
taper(cap2, 0.8)
box("TopLightBase", (0.07, 0.04, 0.03), (0, -0.04, 0.215), "dark", 0.007)
box("TopLight", (0.03, 0.012, 0.018), (0, -0.062, 0.215), "red", 0.004)

# ---- 結合 & スムーズシェード ----
bpy.ops.object.select_all(action="DESELECT")
for o in parts:
    bpy.context.view_layer.objects.active = o
    for md in list(o.modifiers):
        bpy.ops.object.modifier_apply(modifier=md.name)
for o in parts:
    o.select_set(True)
bpy.context.view_layer.objects.active = parts[0]
bpy.ops.object.join()
drone = bpy.context.object
drone.name = "MiniDrone"
bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
# 30度以上の角は硬く、それ以下（ベベルや球）はなめらかに
bpy.ops.object.shade_smooth_by_angle(angle=math.radians(30), keep_sharp_edges=True)

# ---- ボーン (ふわふわ用) ----
arm_data = bpy.data.armatures.new("MiniDroneRig")
arm = bpy.data.objects.new("MiniDroneRig", arm_data)
scn.collection.objects.link(arm)
bpy.context.view_layer.objects.active = arm
bpy.ops.object.mode_set(mode="EDIT")
root = arm_data.edit_bones.new("Root")
root.head, root.tail = (0, 0, 0), (0, 0, 0.1)
body = arm_data.edit_bones.new("Body")
body.head, body.tail = (0, 0, 0), (0, 0, 0.2)
body.parent = root
bpy.ops.object.mode_set(mode="OBJECT")

drone.parent = arm
vg = drone.vertex_groups.new(name="Body")
vg.add(range(len(drone.data.vertices)), 1.0, "REPLACE")
drone.modifiers.new("Armature", "ARMATURE").object = arm

# ---- ふわふわアニメーション (60フレーム = 2秒でループ) ----
LOOP = 60
arm.animation_data_create()
act = bpy.data.actions.new("Hover")
arm.animation_data.action = act
pb = arm.pose.bones["Body"]
pb.rotation_mode = "XYZ"
for f in range(LOOP + 1):
    t = 2 * math.pi * f / LOOP
    # Bodyボーンは上向きなので、ボーンのY = ワールドの上
    pb.location = (0, 0.025 * math.sin(t), 0)                  # 上下 ±2.5cm
    pb.rotation_euler = (math.radians(2.5) * math.sin(t + 1.2),  # 前後にゆらぎ
                         0,
                         math.radians(2.0) * math.sin(2 * t))    # 左右にゆらぎ
    pb.keyframe_insert("location", frame=f)
    pb.keyframe_insert("rotation_euler", frame=f)
scn.frame_start, scn.frame_end = 0, LOOP - 1   # 60フレーム目は0と同じなので含めない

os.makedirs(out_dir, exist_ok=True)
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(out_dir, "MiniDrone.blend"))
bpy.ops.object.select_all(action="DESELECT")
arm.select_set(True); drone.select_set(True)
bpy.ops.export_scene.fbx(
    filepath=os.path.join(out_dir, "MiniDrone.fbx"),
    use_selection=True, object_types={"MESH", "ARMATURE"},
    bake_space_transform=True, apply_unit_scale=False,
    apply_scale_options="FBX_SCALE_NONE", mesh_smooth_type="OFF",
    add_leaf_bones=False, bake_anim=True, bake_anim_use_all_actions=False,
    bake_anim_use_nla_strips=False, bake_anim_force_startend_keying=True,
    bake_anim_simplify_factor=0.0,
)

# ---- プレビュー ----
scn.frame_set(0)
cam_data = bpy.data.cameras.new("Cam")
cam = bpy.data.objects.new("Cam", cam_data)
scn.collection.objects.link(cam)
cam.location = (-0.75, -1.0, 0.55)
cam.rotation_euler = (mathutils.Vector((0, 0, -0.05)) - cam.location).to_track_quat("-Z", "Y").to_euler()
cam_data.lens = 55
scn.camera = cam
for loc, e in (((-1.5, -1.5, 2.0), 300), ((2.0, 1.0, 1.0), 120), ((0.5, -0.5, -1.5), 60)):
    ld = bpy.data.lights.new("L", "AREA"); ld.energy = e; ld.size = 1.5
    lo = bpy.data.objects.new("L", ld); scn.collection.objects.link(lo)
    lo.location = loc
    lo.rotation_euler = (mathutils.Vector((0, 0, 0)) - lo.location).to_track_quat("-Z", "Y").to_euler()
world = bpy.data.worlds.new("W"); scn.world = world
world.use_nodes = True
world.node_tree.nodes["Background"].inputs[0].default_value = (0.25, 0.27, 0.3, 1)
scn.render.engine = "BLENDER_EEVEE"
scn.view_settings.view_transform = "Standard"
scn.render.resolution_x = scn.render.resolution_y = 800
scn.render.filepath = preview
bpy.ops.render.render(write_still=True)
print("DONE")
