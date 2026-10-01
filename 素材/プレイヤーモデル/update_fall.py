import bpy
import math
import shutil
from pathlib import Path
from mathutils import Quaternion, Vector

path = Path(bpy.data.filepath)
backup = path.with_name('player_hover_before_fall_edit.blend')
if not backup.exists():
    shutil.copy2(path, backup)
rig = bpy.data.objects['Player']
rig.animation_data.action = bpy.data.actions['Idle']
rig.animation_data.action_slot = bpy.data.actions['Idle'].slots[0]
bpy.context.scene.frame_set(1)
base = {b.name: (b.location.copy(), b.rotation_quaternion.copy(), b.scale.copy()) for b in rig.pose.bones}
rig.animation_data.action = None
bpy.data.actions.remove(bpy.data.actions['Fall'])
action = bpy.data.actions.new('Fall')
action.use_fake_user = True
rig.animation_data.action = action

# Hoverの屈伸姿勢と区別するため、脚を伸ばし腕を横へ開く。
for frame in range(31):
    phase = 2 * math.pi * frame / 30
    for bone in rig.pose.bones:
        name = bone.name.split(':')[-1]
        loc, rot, scale = base[bone.name]
        bone.location, bone.rotation_quaternion, bone.scale = loc, rot, scale
        bone.rotation_mode = 'QUATERNION'
        if name == 'Hips':
            bone.rotation_quaternion = Quaternion((1, 0, 0), math.radians(-5))
        elif name in ('LeftArm', 'RightArm'):
            sign = 1 if name.startswith('Left') else -1
            bone.rotation_quaternion = Quaternion((0, 0, 1), math.radians(sign * (25 + 3 * math.sin(phase))))
        elif name in ('LeftForeArm', 'RightForeArm'):
            bone.rotation_quaternion = Quaternion((1, 0, 0), math.radians(-12))
        elif name in ('LeftUpLeg', 'RightUpLeg'):
            sign = 1 if name.startswith('Left') else -1
            bone.rotation_quaternion = Quaternion((0, 0, 1), math.radians(sign * 7)) @ Quaternion((1, 0, 0), math.radians(3 * math.sin(phase + sign * 0.5)))
        elif name in ('LeftLeg', 'RightLeg'):
            bone.rotation_quaternion = Quaternion((1, 0, 0), math.radians(-7))
        for channel in ('location', 'rotation_quaternion', 'scale'):
            bone.keyframe_insert(channel, frame=frame, group=bone.name)
for layer in action.layers:
    for strip in layer.strips:
        for bag in strip.channelbags:
            for curve in bag.fcurves:
                for point in curve.keyframe_points:
                    point.interpolation = 'LINEAR'
                assert abs(curve.evaluate(0) - curve.evaluate(30)) < 1e-5
scene = bpy.context.scene
scene.frame_start, scene.frame_end = 0, 29
scene.frame_set(0)
bpy.ops.wm.save_as_mainfile(filepath=str(path))

# 同じカメラでFallとHoverの姿勢を比較する。
cam_data = bpy.data.cameras.new('PreviewCamera')
camera = bpy.data.objects.new('PreviewCamera', cam_data)
scene.collection.objects.link(camera)
center = Vector((0, 0, 1))
camera.location = center + Vector((3, -5, 1.5))
camera.rotation_euler = (center - camera.location).to_track_quat('-Z', 'Y').to_euler()
cam_data.type, cam_data.ortho_scale = 'ORTHO', 2.7
scene.camera = camera
scene.render.engine = 'BLENDER_WORKBENCH'
scene.display.shading.light = 'STUDIO'
scene.display.shading.color_type = 'MATERIAL'
scene.render.resolution_x = scene.render.resolution_y = 600
scene.render.resolution_percentage = 100
for name in ('Fall', 'Hover'):
    rig.animation_data.action = bpy.data.actions[name]
    rig.animation_data.action_slot = bpy.data.actions[name].slots[0]
    scene.frame_set(1)
    scene.render.filepath = str(path.parent / (name.lower() + '_comparison.png'))
    bpy.ops.render.render(write_still=True)
print('Fall loop verified; original file backed up:', backup)
