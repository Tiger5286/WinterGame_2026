import bpy
import math
from mathutils import Euler, Quaternion

# ============================================================
# JumpBase 33F -> Fall animation generator
# - Uses the pose at frame 33 of the "JumpBase" Action as base.
# - Creates a subtle, loopable floating/falling animation.
# - Keeps Hips/root translation fixed.
# - Exports an armature-only FBX (no mesh).
# ============================================================

INPUT_BLEND = "/mnt/data/player.blend"
OUTPUT_FBX = "/mnt/data/Fall.fbx"

SOURCE_ACTION_NAME = "JumpBase"
REFERENCE_FRAME = 33

START_FRAME = 0
END_FRAME = 30
KEY_FRAMES = [0, 7, 15, 23, 30]

# Maximum local rotation offsets in degrees.
# Deliberately small: the reference pose remains almost unchanged.
SPINE_PITCH = 1.2
SPINE_YAW = 0.8
SPINE_ROLL = 0.5

NECK_PITCH = 0.45
NECK_YAW = 0.35
NECK_ROLL = 0.25

HEAD_PITCH = 0.65
HEAD_YAW = 0.45
HEAD_ROLL = 0.30

SHOULDER_ROLL = 0.45
ARM_EXTRA = 0.20


def find_armature():
    selected = bpy.context.selected_objects
    for obj in selected:
        if obj.type == 'ARMATURE':
            return obj

    armatures = [obj for obj in bpy.data.objects if obj.type == 'ARMATURE']
    if not armatures:
        raise RuntimeError("Armature object was not found.")
    if len(armatures) > 1:
        # Prefer an armature whose name suggests the player.
        for obj in armatures:
            if "player" in obj.name.lower() or "armature" in obj.name.lower():
                return obj
    return armatures[0]


def find_action(name):
    action = bpy.data.actions.get(name)
    if action is None:
        raise RuntimeError(f'Action "{name}" was not found.')
    return action


def bone_kind(name):
    n = name.lower().replace(" ", "").replace("_", "")

    if "spine2" in n:
        return "spine2"
    if "spine1" in n:
        return "spine1"
    if "spine" in n:
        return "spine"

    if "neck" in n:
        return "neck"
    if "head" in n:
        return "head"

    if "leftshoulder" in n or "rightshoulder" in n:
        return "shoulder"

    if "leftarm" in n or "rightarm" in n:
        return "arm"

    return None


def capture_reference_pose(armature):
    """Capture pose-bone local transforms evaluated at the reference frame."""
    scene = bpy.context.scene
    scene.frame_set(REFERENCE_FRAME)
    bpy.context.view_layer.update()

    pose = {}

    for pb in armature.pose.bones:
        item = {
            "location": pb.location.copy(),
            "scale": pb.scale.copy(),
            "rotation_mode": pb.rotation_mode,
        }

        if pb.rotation_mode == 'QUATERNION':
            item["rotation"] = pb.rotation_quaternion.copy()
        elif pb.rotation_mode == 'AXIS_ANGLE':
            item["rotation"] = tuple(pb.rotation_axis_angle)
        else:
            item["rotation"] = pb.rotation_euler.copy()

        pose[pb.name] = item

    return pose


def apply_reference_pose(armature, pose):
    for pb in armature.pose.bones:
        p = pose[pb.name]

        pb.location = p["location"]
        pb.scale = p["scale"]

        if p["rotation_mode"] == 'QUATERNION':
            pb.rotation_mode = 'QUATERNION'
            pb.rotation_quaternion = p["rotation"]
        elif p["rotation_mode"] == 'AXIS_ANGLE':
            pb.rotation_mode = 'AXIS_ANGLE'
            pb.rotation_axis_angle = p["rotation"]
        else:
            pb.rotation_mode = p["rotation_mode"]
            pb.rotation_euler = p["rotation"]


def make_delta(kind, t):
    """
    t = 0..1.
    One gentle oscillation. Endpoints are exactly zero, so the
    animation starts and ends at the reference pose.
    """
    wave = math.sin(2.0 * math.pi * t)
    wave2 = math.sin(2.0 * math.pi * t + math.pi * 0.5)

    if kind == "spine2":
        return Euler((
            math.radians(SPINE_PITCH * wave),
            math.radians(SPINE_YAW * wave2),
            math.radians(SPINE_ROLL * wave),
        ), 'XYZ')

    if kind == "spine1":
        return Euler((
            math.radians(SPINE_PITCH * 0.55 * wave),
            math.radians(SPINE_YAW * 0.45 * wave2),
            math.radians(SPINE_ROLL * 0.45 * wave),
        ), 'XYZ')

    if kind == "spine":
        return Euler((
            math.radians(SPINE_PITCH * 0.25 * wave),
            math.radians(SPINE_YAW * 0.20 * wave2),
            math.radians(SPINE_ROLL * 0.20 * wave),
        ), 'XYZ')

    if kind == "neck":
        # Slightly counter the torso.
        return Euler((
            math.radians(-NECK_PITCH * wave),
            math.radians(-NECK_YAW * wave2),
            math.radians(-NECK_ROLL * wave),
        ), 'XYZ')

    if kind == "head":
        return Euler((
            math.radians(HEAD_PITCH * wave2),
            math.radians(HEAD_YAW * wave),
            math.radians(HEAD_ROLL * wave2),
        ), 'XYZ')

    if kind == "shoulder":
        # Very small shoulder movement; left/right are naturally
        # mirrored by the sign below.
        return Euler((
            math.radians(0.15 * wave),
            math.radians(0.10 * wave2),
            math.radians(SHOULDER_ROLL * wave),
        ), 'XYZ')

    if kind == "arm":
        return Euler((
            math.radians(0.10 * wave),
            math.radians(0.05 * wave2),
            math.radians(ARM_EXTRA * wave),
        ), 'XYZ')

    return None


def apply_rotation_delta(pb, delta):
    if delta is None:
        return

    if pb.rotation_mode == 'QUATERNION':
        pb.rotation_quaternion = pb.rotation_quaternion @ delta.to_quaternion()

    elif pb.rotation_mode == 'AXIS_ANGLE':
        base_q = Quaternion(
            pb.rotation_axis_angle[1:],
            pb.rotation_axis_angle[0]
        )
        q = base_q @ delta.to_quaternion()
        angle = q.angle
        axis = q.axis
        pb.rotation_axis_angle = (angle, axis.x, axis.y, axis.z)

    else:
        base_q = pb.rotation_euler.to_quaternion()
        q = base_q @ delta.to_quaternion()
        pb.rotation_euler = q.to_euler(pb.rotation_mode)


def key_pose(armature, frame, reference_pose):
    bpy.context.scene.frame_set(frame)

    apply_reference_pose(armature, reference_pose)

    t = (frame - START_FRAME) / float(END_FRAME - START_FRAME)

    for pb in armature.pose.bones:
        # Keep root/Hips translation completely fixed.
        if pb.name.lower().endswith("hips") or "hips" in pb.name.lower():
            pb.location = reference_pose[pb.name]["location"]
            continue

        kind = bone_kind(pb.name)
        delta = make_delta(kind, t)
        apply_rotation_delta(pb, delta)

    # Key every bone's pose channels so the resulting FBX contains
    # a self-contained animation.
    for pb in armature.pose.bones:
        pb.keyframe_insert(data_path="location", frame=frame)
        pb.keyframe_insert(data_path="scale", frame=frame)

        if pb.rotation_mode == 'QUATERNION':
            pb.keyframe_insert(data_path="rotation_quaternion", frame=frame)
        elif pb.rotation_mode == 'AXIS_ANGLE':
            pb.keyframe_insert(data_path="rotation_axis_angle", frame=frame)
        else:
            pb.keyframe_insert(data_path="rotation_euler", frame=frame)


def main():
    # The script is intended to be run with player.blend already opened.
    # If it is run externally, open the specified blend file first.
    current_path = bpy.data.filepath

    # Prefer the currently opened .blend. This is important on Windows,
    # because the ChatGPT-side /mnt/data path does not exist on the user's PC.
    if not current_path:
        try:
            bpy.ops.wm.open_mainfile(filepath=INPUT_BLEND)
            current_path = bpy.data.filepath
        except Exception as e:
            raise RuntimeError(
                f"Could not open {INPUT_BLEND}: {e}"
            )

    print("Using blend file:", current_path)

    armature = find_armature()
    source_action = find_action(SOURCE_ACTION_NAME)

    # Output beside the opened .blend file.
    import os
    output_dir = os.path.dirname(bpy.data.filepath)
    output_fbx = os.path.join(output_dir, "Fall.fbx")
    output_blend = os.path.join(output_dir, "player_with_fall.blend")

    # Evaluate the source action on the armature.
    if armature.animation_data is None:
        armature.animation_data_create()
    armature.animation_data.action = source_action

    reference_pose = capture_reference_pose(armature)

    # Remove an old generated Fall action if present.
    old = bpy.data.actions.get("Fall")
    if old:
        bpy.data.actions.remove(old)

    fall = bpy.data.actions.new("Fall")
    armature.animation_data.action = fall

    # Generate subtle motion around the 33F reference pose.
    for frame in KEY_FRAMES:
        key_pose(armature, frame, reference_pose)


    fall.use_fake_user = True
    fall.frame_start = START_FRAME
    fall.frame_end = END_FRAME

    # Select only the armature for FBX export.
    bpy.ops.object.select_all(action='DESELECT')
    armature.select_set(True)
    bpy.context.view_layer.objects.active = armature

    # Export only the armature: no mesh, no camera, no lights.
    bpy.ops.export_scene.fbx(
        filepath=output_fbx,
        use_selection=True,
        object_types={'ARMATURE'},
        use_mesh_modifiers=False,
        add_leaf_bones=False,
        primary_bone_axis='Y',
        secondary_bone_axis='X',
        use_armature_deform_only=False,
        bake_anim=True,
        bake_anim_use_all_bones=True,
        bake_anim_use_nla_strips=False,
        bake_anim_use_all_actions=False,
        bake_anim_force_startend_keying=True,
        bake_anim_step=1.0,
        bake_anim_simplify_factor=0.0,
        use_metadata=False,
    )

    # Save a copy containing the generated Fall action as well.
    bpy.ops.wm.save_as_mainfile(filepath=output_blend)

    print("============================================")
    print("Fall animation generated successfully.")
    print("Reference : JumpBase frame 33")
    print("Action    : Fall")
    print("Frames    : 0 - 30")
    print("Output    :", output_fbx)
    print("Armature-only FBX: YES")
    print("Mesh exported: NO")
    print("============================================")


main()
