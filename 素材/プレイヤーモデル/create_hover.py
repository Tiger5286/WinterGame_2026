import bpy
import math
from pathlib import Path
from mathutils import Quaternion, Vector

folder = Path(bpy.data.filepath).parent
rig = bpy.data.objects['Player']
original_actions = set(bpy.data.actions.keys())
rig.animation_data.action = bpy.data.actions['Fall']
rig.animation_data.action_slot = bpy.data.actions['Fall'].slots[0]
bpy.context.scene.frame_set(15)
base = {b.name: (b.location.copy(), b.rotation_quaternion.copy(), b.scale.copy())
        for b in rig.pose.bones}
action = bpy.data.actions.new('Hover')
action.use_fake_user = True
rig.animation_data.action = action

# 既存の落下姿勢を保ち、ルートを移動させず関節だけを周期的に揺らす。
sway = {
    'Spine': (1.5, 0.0), 'Spine1': (1.0, 0.4),
    'LeftArm': (2.5, 0.3), 'RightArm': (2.5, 0.3),
    'LeftUpLeg': (3.0, 0.0), 'RightUpLeg': (3.0, 0.6),
    'LeftLeg': (4.0, 0.7), 'RightLeg': (4.0, 1.3),
    'LeftFoot': (2.0, 0.9), 'RightFoot': (2.0, 1.5),
}
for frame in range(1, 62):
    phase = 2 * math.pi * (frame - 1) / 60
    for bone in rig.pose.bones:
        loc, rot, scale = base[bone.name]
        bone.location = loc
        bone.rotation_mode = 'QUATERNION'
        bone.rotation_quaternion = rot
        bone.scale = scale
        name = bone.name.split(':')[-1]
        if name in sway:
            amplitude, offset = sway[name]
            bone.rotation_quaternion = rot @ Quaternion(
                (1, 0, 0), math.radians(amplitude) * math.sin(phase + offset))
        for channel in ('location', 'rotation_quaternion', 'scale'):
            bone.keyframe_insert(channel, frame=frame, group=bone.name)

scene = bpy.context.scene
scene.frame_start = 1
scene.frame_end = 60
scene.render.fps = 30
scene.frame_set(1)
for layer in action.layers:
    for strip in layer.strips:
        for bag in strip.channelbags:
            for curve in bag.fcurves:
                for point in curve.keyframe_points:
                    point.interpolation = 'LINEAR'
                assert abs(curve.evaluate(1) - curve.evaluate(61)) < 1e-5
assert original_actions.issubset(set(bpy.data.actions.keys()))
bpy.ops.wm.save_as_mainfile(filepath=str(folder / 'player_hover.blend'))
print('HOVER_VALIDATED', len(original_actions), 'original actions preserved; loop endpoints match')

# 確認用のカメラは保存後に追加し、モデルの納品ファイルには含めない。
points = [o.matrix_world @ Vector(c) for o in bpy.data.objects if o.type == 'MESH' for c in o.bound_box]
center = Vector(tuple((min(p[i] for p in points) + max(p[i] for p in points)) / 2 for i in range(3)))
camera_data = bpy.data.cameras.new('PreviewCamera')
camera = bpy.data.objects.new('PreviewCamera', camera_data)
scene.collection.objects.link(camera)
camera.location = center + Vector((3, -5, 1.5))
camera.rotation_euler = (center - camera.location).to_track_quat('-Z', 'Y').to_euler()
camera_data.type = 'ORTHO'
camera_data.ortho_scale = 2.5
scene.camera = camera
scene.render.engine = 'BLENDER_WORKBENCH'
scene.display.shading.light = 'STUDIO'
scene.display.shading.color_type = 'MATERIAL'
scene.display.shading.show_shadows = True
scene.render.resolution_x = 600
scene.render.resolution_y = 600
scene.render.resolution_percentage = 100
scene.render.filepath = str(folder / 'hover_preview.png')
bpy.ops.render.render(write_still=True)
