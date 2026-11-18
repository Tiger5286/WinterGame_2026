"""Original sci-fi hover drone v2 - detailed + metal, baked to texture.
Z-up metres, front = -Y.  blender -b --factory-startup --python make_drone2.py -- <outdir> <previewdir>
"""
import bpy, sys, os, math
from mathutils import Vector, Matrix

argv = sys.argv[sys.argv.index("--") + 1:]
OUT, PREV = argv[0], argv[1]
LITE = "--lite" in argv          # low-poly variant for many on-screen enemies
NAME = "Drone_Lite" if LITE else "Drone"
os.makedirs(OUT, exist_ok=True); os.makedirs(PREV, exist_ok=True)
TEX_RES = 1024 if LITE else 2048

def V(n):
    """round-part segment count (halved in lite mode, min 6)"""
    return max(6, n // 2) if LITE else n

bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene

# =====================================================================
# procedural metal materials
# =====================================================================
def metal_mat(name, base, wear, metal, rough, wear_amt=1.0, grime=0.7, streak=True, chip_scale=18.0):
    m = bpy.data.materials.new(name)
    nt = m.node_tree if m.node_tree else None
    if nt is None:
        m.use_nodes = True; nt = m.node_tree
    N, L = nt.nodes, nt.links
    bsdf = N.get("Principled BSDF")
    tc = N.new("ShaderNodeTexCoord")

    # large-scale colour variation
    n1 = N.new("ShaderNodeTexNoise"); n1.inputs["Scale"].default_value = 6; n1.inputs["Detail"].default_value = 8
    L.new(tc.outputs["Object"], n1.inputs["Vector"])
    # brushed streaks (stretched noise)
    mp = N.new("ShaderNodeMapping"); mp.inputs["Scale"].default_value = (2, 2, 60) if streak else (1, 1, 1)
    L.new(tc.outputs["Object"], mp.inputs["Vector"])
    n2 = N.new("ShaderNodeTexNoise"); n2.inputs["Scale"].default_value = 20; n2.inputs["Detail"].default_value = 4
    L.new(mp.outputs["Vector"], n2.inputs["Vector"])
    var = N.new("ShaderNodeMath"); var.operation = 'MULTIPLY'
    L.new(n1.outputs["Fac"], var.inputs[0]); L.new(n2.outputs["Fac"], var.inputs[1])
    vr = N.new("ShaderNodeMapRange"); vr.inputs["From Min"].default_value = 0.1; vr.inputs["From Max"].default_value = 0.5
    vr.inputs["To Min"].default_value = 0.78; vr.inputs["To Max"].default_value = 1.12
    L.new(var.outputs[0], vr.inputs["Value"])
    col = N.new("ShaderNodeMix"); col.data_type = 'RGBA'; col.blend_type = 'MULTIPLY'
    col.inputs["Factor"].default_value = 1.0
    col.inputs[6].default_value = (*base, 1)
    L.new(vr.outputs["Result"], col.inputs[7])

    # edge mask: bevel-normal vs real normal
    bev = N.new("ShaderNodeBevel"); bev.inputs["Radius"].default_value = 0.008; bev.samples = 8
    geo = N.new("ShaderNodeNewGeometry")
    dot = N.new("ShaderNodeVectorMath"); dot.operation = 'DOT_PRODUCT'
    L.new(bev.outputs["Normal"], dot.inputs[0]); L.new(geo.outputs["Normal"], dot.inputs[1])
    edge = N.new("ShaderNodeMapRange"); edge.inputs["From Min"].default_value = 0.995; edge.inputs["From Max"].default_value = 0.93
    L.new(dot.outputs["Value"], edge.inputs["Value"])
    chip = N.new("ShaderNodeTexNoise"); chip.inputs["Scale"].default_value = chip_scale; chip.inputs["Detail"].default_value = 10
    chip.inputs["Roughness"].default_value = 0.7
    L.new(tc.outputs["Object"], chip.inputs["Vector"])
    cm = N.new("ShaderNodeMath"); cm.operation = 'MULTIPLY'
    L.new(edge.outputs["Result"], cm.inputs[0]); L.new(chip.outputs["Fac"], cm.inputs[1])
    wr = N.new("ShaderNodeMapRange"); wr.inputs["From Min"].default_value = 0.22; wr.inputs["From Max"].default_value = 0.32
    L.new(cm.outputs[0], wr.inputs["Value"])
    wm = N.new("ShaderNodeMath"); wm.operation = 'MULTIPLY'; wm.inputs[1].default_value = wear_amt
    L.new(wr.outputs["Result"], wm.inputs[0])
    worn = N.new("ShaderNodeMix"); worn.data_type = 'RGBA'
    L.new(wm.outputs[0], worn.inputs["Factor"])
    L.new(col.outputs[2], worn.inputs[6]); worn.inputs[7].default_value = (*wear, 1)

    # grime: AO + bottom darkening
    ao = N.new("ShaderNodeAmbientOcclusion"); ao.samples = 16; ao.inputs["Distance"].default_value = 0.08
    aor = N.new("ShaderNodeMapRange"); aor.inputs["To Min"].default_value = 1 - grime
    L.new(ao.outputs["AO"], aor.inputs["Value"])
    sep = N.new("ShaderNodeSeparateXYZ"); L.new(tc.outputs["Object"], sep.inputs[0])
    bot = N.new("ShaderNodeMapRange"); bot.inputs["From Min"].default_value = -0.25; bot.inputs["From Max"].default_value = 0.1
    bot.inputs["To Min"].default_value = 0.7; bot.inputs["To Max"].default_value = 1.0
    L.new(sep.outputs["Z"], bot.inputs["Value"])
    gm = N.new("ShaderNodeMath"); gm.operation = 'MULTIPLY'
    L.new(aor.outputs["Result"], gm.inputs[0]); L.new(bot.outputs["Result"], gm.inputs[1])
    dirty = N.new("ShaderNodeMix"); dirty.data_type = 'RGBA'; dirty.blend_type = 'MULTIPLY'
    dirty.inputs["Factor"].default_value = 1.0
    L.new(worn.outputs[2], dirty.inputs[6]); L.new(gm.outputs[0], dirty.inputs[7])
    L.new(dirty.outputs[2], bsdf.inputs["Base Color"])

    # metal / rough also vary with wear (preview only)
    mm = N.new("ShaderNodeMapRange"); mm.inputs["To Min"].default_value = metal; mm.inputs["To Max"].default_value = 1.0
    L.new(wm.outputs[0], mm.inputs["Value"]); L.new(mm.outputs["Result"], bsdf.inputs["Metallic"])
    rr = N.new("ShaderNodeMapRange"); rr.inputs["To Min"].default_value = rough * 0.7; rr.inputs["To Max"].default_value = rough * 1.3
    L.new(n2.outputs["Fac"], rr.inputs["Value"]); L.new(rr.outputs["Result"], bsdf.inputs["Roughness"])
    m.diffuse_color = (*base, 1)
    m["metal"] = metal; m["rough"] = rough
    return m

def glow_mat(name, col, strength):
    m = bpy.data.materials.new(name)
    if not m.node_tree: m.use_nodes = True
    b = m.node_tree.nodes.get("Principled BSDF")
    b.inputs["Base Color"].default_value = (*col, 1)
    b.inputs["Emission Color"].default_value = (*col, 1)
    b.inputs["Emission Strength"].default_value = strength
    m.diffuse_color = (*col, 1)
    return m

M_PAINT  = metal_mat("Paint",  (0.50, 0.53, 0.56), (0.80, 0.80, 0.82), 0.35, 0.38)
M_GUN    = metal_mat("Gunmetal", (0.07, 0.075, 0.085), (0.42, 0.43, 0.45), 0.9, 0.32, wear_amt=0.8, grime=0.5)
M_ORANGE = metal_mat("Orange", (0.85, 0.30, 0.03), (0.75, 0.72, 0.70), 0.25, 0.4, wear_amt=1.2)
M_BRASS  = metal_mat("Brass",  (0.62, 0.42, 0.18), (0.95, 0.80, 0.55), 1.0, 0.28, wear_amt=0.6, grime=0.8)
M_EYE    = glow_mat("Eye", (1.0, 0.06, 0.03), 8.0)
M_JET    = glow_mat("Jet", (0.25, 0.75, 1.0), 6.0)

# =====================================================================
# primitive helpers
# =====================================================================
parts = {"body": [], "turret": [], "fanL": [], "fanR": []}
FAN_Y = -0.255
RY = (math.radians(90), 0, 0)

def finish(o, m, group, bevel=0.0, seg=2):
    o.data.materials.append(m)
    if LITE:  # only the big parts keep a single-segment bevel
        seg = 1
        if bevel < 0.01: bevel = 0
    if bevel > 0:
        md = o.modifiers.new("Bevel", "BEVEL")
        md.width = bevel; md.segments = seg
        md.limit_method = 'ANGLE'; md.angle_limit = math.radians(40)
    parts[group].append(o)
    return o

def box(size, loc, m, group="body", rot=(0, 0, 0), bevel=0.006):
    bpy.ops.mesh.primitive_cube_add(size=1, location=loc, rotation=rot)
    o = bpy.context.active_object
    o.scale = size; bpy.ops.object.transform_apply(scale=True)
    return finish(o, m, group, min(bevel, min(size) * 0.45) if bevel else 0)

def cyl(r, depth, loc, m, group="body", rot=(0, 0, 0), verts=24, bevel=0.0, r2=None, squash=None):
    verts = V(verts)
    if r2 is None:
        bpy.ops.mesh.primitive_cylinder_add(vertices=verts, radius=r, depth=depth, location=loc, rotation=rot)
    else:
        bpy.ops.mesh.primitive_cone_add(vertices=verts, radius1=r, radius2=r2, depth=depth, location=loc, rotation=rot)
    o = bpy.context.active_object
    if squash: o.data.transform(Matrix.Diagonal((*squash, 1.0)))
    return finish(o, m, group, bevel)

def tube(r_out, r_in, depth, loc, m, group="body", rot=(0, 0, 0), verts=24, squash=None):
    verts = V(verts)
    bpy.ops.mesh.primitive_cylinder_add(vertices=verts, radius=r_out, depth=depth, location=loc, rotation=rot)
    o = bpy.context.active_object
    scene.tool_settings.mesh_select_mode = (False, False, True)
    bpy.ops.object.mode_set(mode='EDIT'); bpy.ops.mesh.select_all(action='DESELECT')
    bpy.ops.object.mode_set(mode='OBJECT')
    # delete caps, inset to make a ring
    for p in o.data.polygons:
        p.select = len(p.vertices) > 4
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.inset(thickness=r_out - r_in, depth=0)
    bpy.ops.mesh.delete(type='FACE')
    bpy.ops.object.mode_set(mode='OBJECT')
    sol = o.modifiers.new("Solid", "SOLIDIFY"); sol.thickness = 0.004
    if squash: o.data.transform(Matrix.Diagonal((*squash, 1.0)))
    return finish(o, m, group, 0.002)

def sphere(r, loc, m, group="body", scale=(1, 1, 1)):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=10 if LITE else 20, ring_count=6 if LITE else 12, radius=r, location=loc)
    o = bpy.context.active_object
    o.scale = scale; bpy.ops.object.transform_apply(scale=True)
    return finish(o, m, group)

def bolt(loc, axis="Z", group="body", r=0.007):
    if LITE:  # bolts are too small to read at game distance
        return
    rot = {"Z": (0, 0, 0), "Y": RY, "X": (0, math.radians(90), 0)}[axis]
    cyl(r, 0.006, loc, M_GUN, group, rot=rot, verts=6)

def pipe(points, r, m, group="body"):
    cu = bpy.data.curves.new("pipe", 'CURVE'); cu.dimensions = '3D'
    cu.bevel_depth = r; cu.use_fill_caps = True
    cu.bevel_resolution = 0 if LITE else 2
    cu.resolution_u = 4 if LITE else 8
    sp = cu.splines.new('BEZIER'); sp.bezier_points.add(len(points) - 1)
    for bp, p in zip(sp.bezier_points, points):
        bp.co = p; bp.handle_left_type = bp.handle_right_type = 'AUTO'
    o = bpy.data.objects.new("pipe", cu); scene.collection.objects.link(o)
    bpy.ops.object.select_all(action='DESELECT'); o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.convert(target='MESH')
    o = bpy.context.active_object
    return finish(o, m, group)

def hazard(center, length, height, axis, normal_off, group="body", n=6):
    """diagonal orange/dark stripes on a dark backing plate. axis: 'X' or 'Y' run direction, plate faces +Z."""
    cx, cy, cz = center
    step = length / n
    for i in range(n):
        off = -length / 2 + step * (i + 0.5)
        loc = (cx, cy + off, cz) if axis == 'Y' else (cx + off, cy, cz)
        rz = math.radians(45)
        box((step * 0.45, height * 1.1, 0.006) if axis == 'X' else (height * 1.1, step * 0.45, 0.006),
            loc, M_ORANGE if i % 2 == 0 else M_GUN, group, rot=(0, 0, rz), bevel=0)

# =====================================================================
# BODY
# =====================================================================
# --- core frame
box((0.32, 0.56, 0.24), (0, 0.02, 0), M_GUN, bevel=0.025)
box((0.26, 0.12, 0.19), (0, -0.30, 0.0), M_GUN, bevel=0.02)
# segmented top armour plates with seams
for y, ln in ((-0.17, 0.14), (0.0, 0.17), (0.18, 0.15)):
    box((0.29, ln, 0.04), (0, y + 0.03, 0.138), M_PAINT, bevel=0.01)
    for bx in (-0.125, 0.125):
        for by in (-ln / 2 + 0.02, ln / 2 - 0.02):
            bolt((bx, y + 0.03 + by, 0.159))
# recessed vent on mid plate
box((0.14, 0.13, 0.012), (0, 0.03, 0.16), M_GUN, bevel=0.003)
for i in range(6):
    box((0.12, 0.008, 0.016), (0, -0.025 + i * 0.022, 0.166), M_ORANGE, rot=(math.radians(30), 0, 0), bevel=0.0)
# core side panels + louvres
for sx in (-1, 1):
    box((0.012, 0.24, 0.13), (0.164 * sx, 0.06, 0.0), M_PAINT, bevel=0.004)
    for i in range(7):
        box((0.01, 0.006, 0.10), (0.172 * sx, -0.035 + i * 0.03, 0.0), M_GUN, bevel=0.0)
# underside plate + cable bundle
box((0.24, 0.40, 0.02), (0, 0.04, -0.125), M_PAINT, bevel=0.006)
for i in range(3):
    pipe([(-0.05 + i * 0.05, 0.22, -0.14), (-0.04 + i * 0.04, 0.10, -0.15), (-0.03 + i * 0.03, -0.02, -0.14)], 0.008, M_GUN)

# --- rear radiator block
box((0.24, 0.10, 0.16), (0, 0.33, -0.01), M_PAINT, bevel=0.015)
for i in range(7):
    box((0.006, 0.03, 0.12), (-0.09 + i * 0.03, 0.39, -0.01), M_GUN, bevel=0.0)
box((0.20, 0.012, 0.03), (0, 0.39, 0.065), M_JET, bevel=0.0)
hazard((0, 0.33, 0.072), 0.20, 0.03, 'X', 0)

# --- nose: mono-eye
tube(0.088, 0.072, 0.03, (0, -0.37, 0.01), M_ORANGE, rot=RY, verts=32)
cyl(0.075, 0.05, (0, -0.365, 0.01), M_GUN, rot=RY, verts=32, bevel=0.006)
cyl(0.058, 0.01, (0, -0.392, 0.01), M_GUN, rot=RY, verts=32)
sphere(0.045, (0, -0.395, 0.01), M_EYE, scale=(1, 0.55, 1))
for i in range(8):  # iris shutters
    a = i * math.pi / 4
    box((0.012, 0.008, 0.018), (math.cos(a) * 0.064, -0.398, 0.01 + math.sin(a) * 0.064), M_PAINT,
        rot=(0, -a + math.pi / 2, 0), bevel=0.0)
box((0.22, 0.10, 0.03), (0, -0.33, 0.115), M_PAINT, rot=(math.radians(-12), 0, 0), bevel=0.008)
for bx in (-0.09, 0.09):
    bolt((bx, -0.36, 0.128))
# twin aux sensors beside the eye
for sx in (-1, 1):
    cyl(0.016, 0.03, (0.105 * sx, -0.37, -0.05), M_GUN, rot=RY, verts=12)
    cyl(0.01, 0.005, (0.105 * sx, -0.386, -0.05), M_EYE, rot=RY, verts=12)

# --- side pods
for sx in (-1, 1):
    x = 0.33 * sx
    SQ = (1.0, 0.8, 1.0)
    cyl(0.13, 0.50, (x, 0.02, 0.0), M_PAINT, rot=RY, verts=32, bevel=0.015, squash=SQ)
    for y in (-0.16, 0.08, 0.20):  # armour bands
        cyl(0.136, 0.025, (x, y, 0.0), M_GUN, rot=RY, verts=32, bevel=0.004, squash=SQ)
    # front intake with fan
    tube(0.115, 0.095, 0.05, (x, -0.255, 0.0), M_GUN, rot=RY, verts=32, squash=SQ)
    cyl(0.097, 0.02, (x, -0.235, 0.0), M_GUN, rot=RY, verts=32, squash=SQ)
    FG = "fanL" if sx < 0 else "fanR"
    cyl(0.025, 0.04, (x, FAN_Y, 0.0), M_BRASS, FG, rot=RY, verts=16)
    cyl(0.012, 0.012, (x, FAN_Y - 0.022, 0.0), M_GUN, FG, rot=RY, verts=12)
    # 9 blades (40 deg spacing) + one orange marker blade, so the spin stays readable
    # even when a viewer skips several animation frames per redraw (strobe / wagon-wheel effect).
    NB = 9
    for i in range(NB):  # circular blade layout (spins around Y)
        a = i * 2 * math.pi / NB
        box((0.07, 0.004, 0.022), (x + math.cos(a) * 0.055, -0.252, math.sin(a) * 0.055),
            M_ORANGE if i == 0 else M_PAINT, FG,
            rot=(math.radians(25), -a, 0), bevel=0.0)
    # rear nozzle + heat fins
    cyl(0.105, 0.13, (x, 0.33, 0.0), M_GUN, rot=RY, verts=32, r2=0.08)
    tube(0.083, 0.068, 0.03, (x, 0.40, 0.0), M_BRASS, rot=RY, verts=32)
    cyl(0.068, 0.01, (x, 0.395, 0.0), M_JET, rot=RY, verts=24)
    for i in range(12):
        a = i * math.pi / 6
        box((0.004, 0.10, 0.02), (x + math.cos(a) * 0.1, 0.33, math.sin(a) * 0.1), M_GUN,
            rot=(0, -a, 0), bevel=0.0)
    # outer armour plate, bolts, stripe, hazard
    box((0.03, 0.36, 0.11), (x + 0.132 * sx, 0.02, 0.0), M_PAINT, bevel=0.008)
    box((0.006, 0.30, 0.02), (x + 0.149 * sx, 0.02, 0.03), M_ORANGE, bevel=0.0)
    for by in (-0.15, 0.0, 0.15):
        for bz in (-0.035, 0.035):
            bolt((x + 0.149 * sx, 0.02 + by, bz), axis="X")
    box((0.006, 0.10, 0.025), (x + 0.149 * sx, 0.12, -0.03), M_GUN, bevel=0)
    # nav light
    box((0.02, 0.02, 0.012), (x + 0.12 * sx, -0.2, 0.04), M_EYE if sx < 0 else M_JET, bevel=0)
    # top stripe on pod
    box((0.05, 0.22, 0.008), (x, -0.02, 0.105), M_ORANGE, bevel=0.0)
    # struts + hydraulic pistons
    box((0.13, 0.15, 0.07), (0.2 * sx, -0.06, 0.02), M_GUN, bevel=0.01)
    box((0.13, 0.09, 0.055), (0.2 * sx, 0.20, 0.0), M_GUN, bevel=0.01)
    cyl(0.012, 0.12, (0.21 * sx, 0.07, -0.05), M_BRASS, rot=(0, math.radians(90), 0), verts=12)
    cyl(0.018, 0.06, (0.24 * sx, 0.07, -0.05), M_PAINT, rot=(0, math.radians(90), 0), verts=12)
    # coolant pipes core -> pod
    pipe([(0.13 * sx, 0.22, 0.12), (0.2 * sx, 0.24, 0.17), (0.29 * sx, 0.22, 0.12)], 0.011, M_BRASS)
    pipe([(0.13 * sx, -0.10, 0.12), (0.21 * sx, -0.12, 0.165), (0.29 * sx, -0.10, 0.115)], 0.009, M_GUN)
    # hover thrusters under pods
    for y in (-0.14, 0.20):
        cyl(0.05, 0.05, (x, y, -0.115), M_GUN, verts=24, bevel=0.004)
        tube(0.055, 0.044, 0.02, (x, y, -0.142), M_BRASS, verts=24)
        cyl(0.04, 0.006, (x, y, -0.143), M_JET, verts=24)
        for i in range(4):
            a = i * math.pi / 4
            box((0.07, 0.004, 0.012), (x, y, -0.148), M_GUN, rot=(0, 0, a), bevel=0.0)

# --- top: grab-rail + antenna mast
for y in (-0.10, 0.18):
    box((0.03, 0.03, 0.02), (0.11, y, 0.165), M_GUN, bevel=0.004)
    box((0.018, 0.018, 0.08), (0.11, y, 0.20), M_ORANGE, bevel=0.003)
box((0.018, 0.30, 0.018), (0.11, 0.04, 0.24), M_ORANGE, bevel=0.003)
box((0.04, 0.04, 0.02), (-0.10, 0.20, 0.165), M_GUN, bevel=0.004)
cyl(0.014, 0.10, (-0.10, 0.20, 0.22), M_GUN, verts=10)
cyl(0.009, 0.10, (-0.10, 0.20, 0.31), M_PAINT, verts=10)
cyl(0.018, 0.012, (-0.10, 0.20, 0.27), M_BRASS, verts=10)
cyl(0.05, 0.012, (-0.10, 0.19, 0.37), M_PAINT, verts=24, rot=(math.radians(25), 0, 0), bevel=0.003)
cyl(0.02, 0.014, (-0.10, 0.188, 0.375), M_GUN, verts=16, rot=(math.radians(25), 0, 0))
sphere(0.01, (-0.10, 0.183, 0.385), M_EYE)

# =====================================================================
# TURRET (separate object, pivot at mount)
# =====================================================================
TZ = -0.16
T = "turret"
cyl(0.095, 0.04, (0, -0.04, TZ + 0.005), M_GUN, T, verts=32, bevel=0.005)
tube(0.1, 0.085, 0.015, (0, -0.04, TZ - 0.012), M_ORANGE, T, verts=32)
for i in range(8):
    a = i * math.pi / 4
    bolt((math.cos(a) * 0.075, -0.04 + math.sin(a) * 0.075, TZ + 0.026), group=T, r=0.006)
box((0.14, 0.26, 0.09), (0, -0.08, TZ - 0.07), M_PAINT, T, bevel=0.015)
for sx in (-1, 1):
    box((0.012, 0.18, 0.06), (0.074 * sx, -0.06, TZ - 0.07), M_GUN, T, bevel=0.004)
    for i in range(4):
        box((0.006, 0.006, 0.05), (0.082 * sx, -0.12 + i * 0.04, TZ - 0.07), M_PAINT, T, bevel=0)
# ammo drum on the side
cyl(0.04, 0.05, (0.1, -0.0, TZ - 0.075), M_GUN, T, rot=(0, math.radians(90), 0), verts=24, bevel=0.004)
pipe([(0.1, -0.04, TZ - 0.04), (0.06, -0.08, TZ - 0.02), (0.02, -0.12, TZ - 0.03)], 0.008, M_BRASS, T)
hazard((0, -0.07, TZ - 0.023), 0.12, 0.05, 'Y', 0, T, n=5)
for sx in (-1, 1):
    bx = 0.034 * sx
    cyl(0.02, 0.22, (bx, -0.30, TZ - 0.07), M_GUN, T, rot=RY, verts=16)
    for y in (-0.24, -0.32):
        cyl(0.024, 0.012, (bx, y, TZ - 0.07), M_PAINT, T, rot=RY, verts=16)
    tube(0.028, 0.016, 0.035, (bx, -0.415, TZ - 0.07), M_GUN, T, rot=RY, verts=16)
    for zz in (-0.012, 0.012):  # muzzle brake slots
        box((0.06, 0.006, 0.004), (bx, -0.41, TZ - 0.07 + zz), M_GUN, T, bevel=0)
box((0.03, 0.04, 0.025), (0, -0.20, TZ - 0.02), M_GUN, T, bevel=0.004)
sphere(0.012, (0, -0.222, TZ - 0.02), M_EYE, T)

# =====================================================================
# apply, join, smooth
# =====================================================================
def join(objs, name):
    for o in objs:
        bpy.context.view_layer.objects.active = o
        for md in list(o.modifiers):
            bpy.ops.object.modifier_apply(modifier=md.name)
    bpy.ops.object.select_all(action='DESELECT')
    for o in objs: o.select_set(True)
    bpy.context.view_layer.objects.active = objs[0]
    bpy.ops.object.join()
    o = bpy.context.active_object
    o.name = o.data.name = name
    # joined object inherits the first part's rotation (e.g. the fan hub cylinder's 90deg X).
    # Apply it so keyframed rotations start from identity.
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    o.data.shade_smooth()
    o.data.set_sharp_from_angle(angle=math.radians(35))
    return o

scene.cursor.location = (0, 0, 0)
body = join(parts["body"], "Drone_Body")
bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
turret = join(parts["turret"], "Drone_Turret")
scene.cursor.location = (0, -0.04, TZ)
bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
scene.cursor.location = (0, 0, 0)
turret.parent = body
fans = []
for key, name, sx in (("fanL", "Drone_FanL", -1), ("fanR", "Drone_FanR", 1)):
    f = join(parts[key], name)
    scene.cursor.location = (0.33 * sx, FAN_Y, 0.0)
    bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    scene.cursor.location = (0, 0, 0)
    f.parent = body
    fans.append(f)
ALL = [body, turret] + fans

tris = sum(len(p.vertices) - 2 for o in ALL for p in o.data.polygons)
print("TRIS", tris, "DIMS", tuple(round(d, 3) for d in body.dimensions))

# =====================================================================
# UV + bake base colour (incl. wear / AO grime) into one atlas
# =====================================================================
bpy.ops.object.select_all(action='DESELECT')
for o in ALL: o.select_set(True)
bpy.context.view_layer.objects.active = body
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.select_all(action='SELECT')
bpy.ops.uv.smart_project(angle_limit=math.radians(60), island_margin=0.004, scale_to_bounds=True)
bpy.ops.object.mode_set(mode='OBJECT')

img = bpy.data.images.new(NAME + "_BaseColor", TEX_RES, TEX_RES, alpha=False)
all_mats = [M_PAINT, M_GUN, M_ORANGE, M_BRASS, M_EYE, M_JET]
for m in all_mats:
    n = m.node_tree.nodes.new("ShaderNodeTexImage"); n.image = img
    m.node_tree.nodes.active = n

scene.render.engine = 'CYCLES'
scene.cycles.samples = 24
scene.cycles.device = 'CPU'
bpy.ops.object.bake(type='DIFFUSE', pass_filter={'COLOR'}, margin=6, use_clear=True)
img.filepath_raw = os.path.join(OUT, NAME + "_BaseColor.png"); img.file_format = 'PNG'
img.save()
print("BAKED")

# replace non-glow materials with one textured material (DxLib friendly)
baked = bpy.data.materials.new("Drone_Metal")
if not baked.node_tree: baked.use_nodes = True
bn = baked.node_tree.nodes; b = bn.get("Principled BSDF")
t = bn.new("ShaderNodeTexImage"); t.image = img
baked.node_tree.links.new(t.outputs["Color"], b.inputs["Base Color"])
b.inputs["Metallic"].default_value = 0.75
b.inputs["Roughness"].default_value = 0.33
for m in (M_EYE, M_JET):
    for n in [n for n in m.node_tree.nodes if n.type == 'TEX_IMAGE']:
        m.node_tree.nodes.remove(n)
for o in ALL:
    for i, slot in enumerate(o.material_slots):
        if slot.material not in (M_EYE, M_JET):
            o.material_slots[i].material = baked
    # collapse duplicate slots
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.select_all(action='DESELECT'); o.select_set(True)
    bpy.ops.object.material_slot_remove_unused()

# =====================================================================
# animation: fans spin + gentle hover bob (seamless 60-frame loop @30fps)
# =====================================================================
LOOP = 60
FAN_TURNS = 2   # avg 12 deg/frame @30fps: below half the 40-deg blade spacing, so no wagon-wheel effect
FAN_PULSE = math.radians(30)  # speed varies about 9..15 deg/frame
scene.render.fps = 30
scene.frame_start, scene.frame_end = 0, LOOP
for fr in range(LOOP + 1):
    t = fr / LOOP * 2 * math.pi
    body.location = (0, 0, 0.025 * math.sin(t))
    body.rotation_euler = (math.radians(2.0) * math.sin(t + math.pi / 2),   # pitch lags the bob
                           math.radians(1.5) * math.sin(t + math.pi / 3),   # small roll
                           0)
    body.keyframe_insert("location", frame=fr)
    body.keyframe_insert("rotation_euler", frame=fr)
    # Fan spin is deliberately NOT constant-speed: DxLib drops keys that lie on a straight
    # line, leaving one key per 180 deg, and then the fan holds still and flips 180 deg.
    # A sine term (zero at both loop ends) makes the speed pulse with the bob, so every key is kept.
    spin = FAN_TURNS * 2 * math.pi * fr / LOOP + FAN_PULSE * math.sin(t)
    for f, d in zip(fans, (-1, 1)):
        f.rotation_euler = (0, d * spin, 0)
        f.keyframe_insert("rotation_euler", frame=fr)

def set_linear(o):
    ad = o.animation_data
    if ad and ad.action:
        ad.action.name = "Hover"
        fcs = []
        if hasattr(ad.action, "fcurves"):
            fcs = ad.action.fcurves
        else:  # 5.x layered actions
            for layer in ad.action.layers:
                for strip in layer.strips:
                    for cb in strip.channelbags:
                        fcs.extend(cb.fcurves)
        for fc in fcs:
            for kp in fc.keyframe_points:
                kp.interpolation = 'LINEAR'
for o in [body] + fans:
    set_linear(o)
scene.frame_set(0)

bpy.ops.wm.save_as_mainfile(filepath=os.path.join(OUT, NAME + ".blend"))

bpy.ops.object.select_all(action='DESELECT')
for o in ALL: o.select_set(True)
# Same convention as the stage FBX: raw Y-up, 1 Blender m -> 100 raw units (cm), no axis node.
bpy.ops.export_scene.fbx(filepath=os.path.join(OUT, NAME + ".fbx"), use_selection=True, global_scale=1.0,
                         apply_unit_scale=False, apply_scale_options='FBX_SCALE_NONE',
                         bake_space_transform=True, axis_forward='-Z', axis_up='Y',
                         use_mesh_modifiers=True, mesh_smooth_type='FACE', path_mode='RELATIVE',
                         object_types={'MESH'}, add_leaf_bones=False,
                         bake_anim=True, bake_anim_use_all_actions=False, bake_anim_use_nla_strips=False,
                         bake_anim_step=1.0, bake_anim_simplify_factor=0.0, bake_anim_force_startend_keying=True)
print("EXPORTED")

# =====================================================================
# preview video (EEVEE)
# =====================================================================
world = bpy.data.worlds.new("W"); scene.world = world
if not world.node_tree: world.use_nodes = True
wn, wl = world.node_tree.nodes, world.node_tree.links
bg = wn.get("Background")
wtc = wn.new("ShaderNodeTexCoord"); wsep = wn.new("ShaderNodeSeparateXYZ")
wl.new(wtc.outputs["Generated"], wsep.inputs[0])
ramp = wn.new("ShaderNodeValToRGB")
ramp.color_ramp.elements[0].position = 0.35; ramp.color_ramp.elements[0].color = (0.01, 0.015, 0.02, 1)
ramp.color_ramp.elements[1].position = 0.8; ramp.color_ramp.elements[1].color = (0.35, 0.45, 0.55, 1)
wmr = wn.new("ShaderNodeMapRange"); wmr.inputs["From Min"].default_value = -1
wl.new(wsep.outputs["Z"], wmr.inputs["Value"]); wl.new(wmr.outputs["Result"], ramp.inputs["Fac"])
wl.new(ramp.outputs["Color"], bg.inputs["Color"]); bg.inputs["Strength"].default_value = 0.8
for name, loc, e, c in (("Key", (2, -2, 3), 700, (1, 0.95, 0.9)), ("Rim", (-1.5, 2.5, 1.5), 500, (0.6, 0.8, 1)),
                        ("Fill", (-2.5, -1.5, 0.3), 120, (1, 1, 1)), ("Top", (0, 0, 3), 200, (1, 1, 1))):
    l = bpy.data.lights.new(name, "AREA"); l.energy = e; l.size = 1.5; l.color = c
    lo = bpy.data.objects.new(name, l); scene.collection.objects.link(lo)
    lo.location = loc
    lo.rotation_euler = (-Vector(loc)).to_track_quat('-Z', 'Y').to_euler()
cam = bpy.data.objects.new("Cam", bpy.data.cameras.new("Cam")); scene.collection.objects.link(cam)
cam.data.lens = 55
scene.camera = cam
pos = (-1.25, -1.75, 0.6)
cam.location = pos
cam.rotation_euler = (Vector((0, 0.0, 0)) - Vector(pos)).to_track_quat('-Z', 'Y').to_euler()
for eng in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
    try:
        scene.render.engine = eng; break
    except TypeError:
        pass
try: scene.view_settings.look = 'AgX - Medium High Contrast'
except Exception: pass
scene.render.resolution_x, scene.render.resolution_y = 800, 600
scene.frame_start, scene.frame_end = 0, LOOP * 2 - 1   # show two loops
# loop the action for the preview
for o in [body] + fans:
    o.animation_data.action_extrapolation = 'HOLD'
for o in [body] + fans:
    ad = o.animation_data
    act = ad.action
    trk = ad.nla_tracks.new()
    st = trk.strips.new("Hover", 0, act)
    st.repeat = 2.0
    ad.action = None
try:
    scene.render.image_settings.media_type = 'VIDEO'
except Exception:
    pass
scene.render.image_settings.file_format = 'FFMPEG'
scene.render.ffmpeg.format = 'MPEG4'
scene.render.ffmpeg.codec = 'H264'
scene.render.ffmpeg.constant_rate_factor = 'HIGH'
scene.render.filepath = os.path.join(PREV, NAME + "_hover.mp4")
bpy.ops.render.render(animation=True)
print("DONE")

