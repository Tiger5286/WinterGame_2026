import bpy, os, math, random
import numpy as np
from mathutils import Vector, Matrix

MODELS = r"C:\Users\SasakiTaiga\Documents\GitHub\WinterGame_2026\Project\data\stages\stage1\models"
KIT = r"C:\Users\SasakiTaiga\Documents\GitHub\WinterGame_2026_MapCreate\Assets\Creepy_Cat\3D Scifi Kit Starter Kit_HD"
OUT_FBX = os.path.join(MODELS, "Stage1VisualDetail.fbx")
OUT_TEX = os.path.join(MODELS, "Stage1VisualDetailTex")
os.makedirs(OUT_TEX, exist_ok=True)
random.seed(7)
rng = np.random.default_rng(7)

bpy.ops.wm.read_factory_settings(use_empty=True)

# ---------------------------------------------------------------- image helpers
def img_to_np(img):
    w, h = img.size
    a = np.empty(w * h * 4, dtype=np.float32)
    img.pixels.foreach_get(a)
    return a.reshape(h, w, 4)

def save_np(arr, name, colorspace='sRGB'):
    h, w = arr.shape[:2]
    img = bpy.data.images.new(name, w, h, alpha=True)
    img.colorspace_settings.name = colorspace
    img.pixels.foreach_set(np.clip(arr, 0, 1).astype(np.float32).ravel())
    path = os.path.join(OUT_TEX, name + ".png")
    img.filepath_raw = path
    img.file_format = 'PNG'
    img.save()
    img.filepath = path
    return img

def blur(x, r, it=3):
    for _ in range(it):
        acc = np.zeros_like(x)
        for d in range(-r, r + 1):
            acc += np.roll(x, d, axis=0)
        x = acc / (2 * r + 1)
        acc = np.zeros_like(x)
        for d in range(-r, r + 1):
            acc += np.roll(x, d, axis=1)
        x = acc / (2 * r + 1)
    return x

def upscale2(a):
    a = np.repeat(np.repeat(a, 2, axis=0), 2, axis=1)
    return blur(a, 1, 1)

def normal_from_height(h, strength):
    dx = (np.roll(h, -1, axis=1) - np.roll(h, 1, axis=1)) * 0.5
    dy = (np.roll(h, -1, axis=0) - np.roll(h, 1, axis=0)) * 0.5   # row index = +V (Blender pixel order)
    n = np.stack([-dx * strength, -dy * strength, np.ones_like(h)], axis=-1)
    n /= np.linalg.norm(n, axis=-1, keepdims=True)
    out = np.ones(h.shape + (4,), dtype=np.float32)
    out[..., :3] = n * 0.5 + 0.5
    return out

def value_noise(shape, cell):
    h, w = shape
    g = rng.random((h // cell + 2, w // cell + 2)).astype(np.float32)
    g = np.repeat(np.repeat(g, cell, 0), cell, 1)[:h, :w]
    return blur(g, max(1, cell // 2), 2)

# ------------------------------------------------- 1. stage textures: grime + normal
STAGE_TEX = {"Wall": ("SciFi_Wall", 12.0), "Floor": ("SciFi_Floor", 10.0),
             "Ceiling": ("SciFi_Ceiling", 8.0), "Top": ("SciFi_Top", 6.0)}
stage_imgs = {}
for mat, (fname, strength) in STAGE_TEX.items():
    src = bpy.data.images.load(os.path.join(MODELS, "Stage1VisualTex", fname + ".png"))
    a = img_to_np(src)
    a = np.concatenate([upscale2(a[..., c])[..., None] for c in range(4)], axis=-1)
    rgb = a[..., :3]
    lum = rgb @ np.array([0.299, 0.587, 0.114], dtype=np.float32)
    # height: bright panels up, seams/slots down, softened so edges read as bevels
    h = blur(lum, 1, 2) * 0.6 + blur(lum, 3, 2) * 0.4
    nrm = normal_from_height(h, strength * a.shape[0] / 512.0)
    # grime: darken around seams (cavity) + soft low-freq dirt + fine speckle
    cavity = np.clip((blur(lum, 6, 2) - lum) * 3.0, 0, 1)
    dirt = value_noise(lum.shape, 64) * 0.6 + value_noise(lum.shape, 16) * 0.4
    speck = blur(rng.random(lum.shape).astype(np.float32), 1, 1)
    shade = (1.0 - 0.35 * cavity) * (0.86 + 0.14 * dirt) * (0.97 + 0.06 * speck)
    rgb = rgb * shade[..., None]
    # slight cool tint in the dirt so it does not look like flat grey
    rgb = rgb * (1.0 - 0.03 * (1 - dirt))[..., None] + np.array([0, 0.004, 0.01], dtype=np.float32) * (1 - dirt)[..., None]
    a[..., :3] = rgb
    a[..., 3] = 1.0
    stage_imgs[mat] = (save_np(a, fname, 'sRGB'), save_np(nrm, fname + "_N", 'Non-Color'))

# ------------------------------------------------- 2. kit textures (1024, AO baked)
def kit_tex(base):
    alb = bpy.data.images.load(os.path.join(KIT, "Textures", base + "_Alb.png"))
    alb.scale(1024, 1024)
    a = img_to_np(alb)
    ao_path = os.path.join(KIT, "Textures", base + "_AO.png")
    if os.path.exists(ao_path):
        ao = bpy.data.images.load(ao_path); ao.scale(1024, 1024)
        a[..., :3] *= (0.25 + 0.75 * img_to_np(ao)[..., :1])
    a[..., 3] = 1.0
    n = bpy.data.images.load(os.path.join(KIT, "Textures", base + "_Norm.png"))
    n.colorspace_settings.name = 'Non-Color'
    n.scale(1024, 1024)
    na = img_to_np(n); na[..., 3] = 1.0
    short = base.replace("_Dif_Style_02", "")
    return save_np(a, short, 'sRGB'), save_np(na, short + "_N", 'Non-Color')

kit_imgs = {"Stuff_01": kit_tex("HD_Stuff_01_Dif_Style_02"),
            "Stuff_04": kit_tex("HD_Stuff_04_Dif_Style_02")}

# ------------------------------------------------- material builder
def make_mat(name, col_img=None, nrm_img=None, color=(0.8, 0.8, 0.8), emission=None, nstrength=1.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    nt = m.node_tree
    bsdf = nt.nodes["Principled BSDF"]
    bsdf.inputs["Roughness"].default_value = 0.55
    bsdf.inputs["Base Color"].default_value = (*color, 1)
    if col_img:
        t = nt.nodes.new("ShaderNodeTexImage"); t.image = col_img
        nt.links.new(t.outputs["Color"], bsdf.inputs["Base Color"])
    if nrm_img:
        t = nt.nodes.new("ShaderNodeTexImage"); t.image = nrm_img
        nm = nt.nodes.new("ShaderNodeNormalMap"); nm.inputs["Strength"].default_value = nstrength
        nt.links.new(t.outputs["Color"], nm.inputs["Color"])
        nt.links.new(nm.outputs["Normal"], bsdf.inputs["Normal"])
    if emission:
        bsdf.inputs["Emission Color"].default_value = (*emission, 1)
        bsdf.inputs["Emission Strength"].default_value = 1.0
    return m

# ------------------------------------------------- 3. import stage visual, re-material
before = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=os.path.join(MODELS, "Stage1Visual.fbx"))
stage_objs = [o for o in set(bpy.data.objects) - before]
root = next(o for o in stage_objs if o.type == 'EMPTY')
for o in stage_objs:
    if o.type != 'MESH':
        continue
    old = o.data.materials[0]
    if old.name in stage_imgs:
        c, n = stage_imgs[old.name]
        o.data.materials[0] = make_mat(old.name, c, n)

# ------------------------------------------------- 4. walkable area from collision
before = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=os.path.join(MODELS, "Stage1StaticCol.fbx"))
col_objs = [o for o in set(bpy.data.objects) - before]
boxes = []
for o in col_objs:
    if o.type != 'MESH':
        continue
    pts = [o.matrix_world @ v.co for v in o.data.vertices]
    mn = Vector([min(p[i] for p in pts) for i in range(3)])
    mx = Vector([max(p[i] for p in pts) for i in range(3)])
    if mx.z - mn.z > 3.0:          # walls only (floor slab is 0..1)
        boxes.append((mn, mx))
for o in col_objs:
    bpy.data.objects.remove(o, do_unlink=True)

CELL = 0.25
X0, Y0, X1, Y1 = -14.0, -12.0, 83.0, 34.5
nx, ny = int((X1 - X0) / CELL), int((Y1 - Y0) / CELL)
blocked = np.zeros((nx, ny), bool)
for mn, mx in boxes:
    i0, i1 = int((mn.x - X0) / CELL), int(math.ceil((mx.x - X0) / CELL))
    j0, j1 = int((mn.y - Y0) / CELL), int(math.ceil((mx.y - Y0) / CELL))
    blocked[max(i0, 0):i1, max(j0, 0):j1] = True
inside = np.zeros_like(blocked)
start = (int((0 - X0) / CELL), int((0 - Y0) / CELL))
stack = [start]
while stack:
    i, j = stack.pop()
    if i < 0 or j < 0 or i >= nx or j >= ny or inside[i, j] or blocked[i, j]:
        continue
    inside[i, j] = True
    stack += [(i + 1, j), (i - 1, j), (i, j + 1), (i, j - 1)]
print("walkable cells", inside.sum(), "touches border", inside[0].any() or inside[-1].any())

def is_inside(p):
    i, j = int((p.x - X0) / CELL), int((p.y - Y0) / CELL)
    return 0 <= i < nx and 0 <= j < ny and inside[i, j]

# wall runs: walk each box side face, keep stretches whose front is walkable
runs = []
for mn, mx in boxes:
    faces = [(Vector((mx.x, 0, 0)), Vector((1, 0, 0)), 'y', mn.y, mx.y),
             (Vector((mn.x, 0, 0)), Vector((-1, 0, 0)), 'y', mn.y, mx.y),
             (Vector((0, mx.y, 0)), Vector((0, 1, 0)), 'x', mn.x, mx.x),
             (Vector((0, mn.y, 0)), Vector((0, -1, 0)), 'x', mn.x, mx.x)]
    for base, n, axis, a0, a1 in faces:
        cur = []
        t = a0 + 0.25
        while t <= a1 - 0.25:
            p = base.copy()
            setattr(p, axis, t)
            if is_inside(p + n * 0.9) and is_inside(p + n * 0.3):
                cur.append(p)
            elif cur:
                runs.append((cur, n)); cur = []
            t += 0.25
        if cur:
            runs.append((cur, n))
runs = [r for r in runs if len(r[0]) >= 8]
print("wall runs", len(runs), "total length", sum(len(r[0]) for r in runs) * 0.25)

# ------------------------------------------------- 5. kit props
KIT_MAT = {
    "Stuff_01": make_mat("Kit_Stuff01", *kit_imgs["Stuff_01"]),
    "Stuff_04": make_mat("Kit_Stuff04", *kit_imgs["Stuff_04"]),
}
KIT_MAT["toto"] = KIT_MAT["Orange"] = KIT_MAT["01 - Default"] = KIT_MAT["Stuff_01"]
KIT_MAT["995 - Light"] = make_mat("Kit_LightGlow", color=(0.9, 0.95, 1.0), emission=(0.85, 0.95, 1.0))

templates = {}
def load_prop(path):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=os.path.join(KIT, "Meshes", path + ".FBX"))
    new = [o for o in set(bpy.data.objects) - before]
    meshes = [o for o in new if o.type == 'MESH']
    for o in meshes:
        o.data.transform(o.matrix_world)
        o.matrix_world = Matrix.Identity(4)
        o.parent = None
    bpy.ops.object.select_all(action='DESELECT')
    for o in meshes:
        o.select_set(True)
    bpy.context.view_layer.objects.active = meshes[0]
    if len(meshes) > 1:
        bpy.ops.object.join()
    ob = bpy.context.view_layer.objects.active
    for o in new:
        if o != ob:
            bpy.data.objects.remove(o, do_unlink=True)
    me = ob.data
    # drop alpha-card faces (DxLib sorting trouble) and remap materials
    drop = {i for i, m in enumerate(me.materials) if m and m.name.startswith("Alpha")}
    if drop:
        import bmesh
        bm = bmesh.new(); bm.from_mesh(me)
        bmesh.ops.delete(bm, geom=[f for f in bm.faces if f.material_index in drop], context='FACES')
        bm.to_mesh(me); bm.free()
    for i, m in enumerate(me.materials):
        key = m.name.split(".")[0] if m else "Stuff_01"
        me.materials[i] = KIT_MAT.get(key, KIT_MAT["Stuff_01"])
    ob.name = "TPL_" + path.split("/")[-1]
    bpy.context.scene.collection.objects.unlink(ob) if ob.name in bpy.context.scene.collection.objects else None
    for c in ob.users_collection:
        c.objects.unlink(ob)
    templates[path] = ob
    return ob

for p in ["Stuff/Beam_03", "Stuff/Pipes_02", "Stuff/Pipes_03", "Stuff/Intercom_01", "Stuff/Intercom_02",
          "Stuff/Air_Grid_01", "Lights/Light_SideLight_01", "Lights/Light_SideLight_02"]:
    load_prop(p)

placed = []
def place(path, p, n, z, scale=(1, 1, 1), along=0.0):
    tpl = templates[path]
    o = bpy.data.objects.new("P", tpl.data)
    bpy.context.scene.collection.objects.link(o)
    ang = math.atan2(n.y, n.x) - math.pi        # kit local -X faces out of the wall
    side = Vector((-n.y, n.x, 0))
    o.location = p + side * along + Vector((0, 0, z))
    o.rotation_euler = (0, 0, ang)
    o.scale = scale
    placed.append(o)

FLOOR = 1.0
for pts, n in runs:
    L = len(pts) * 0.25
    # pillars at both ends of long runs and every ~5m between
    k = max(1, int(L // 5.0))
    pill_idx = sorted({0, len(pts) - 1} | {int(i * (len(pts) - 1) / k) for i in range(k + 1)}) if L >= 4 else []
    pill_set = set()
    for i in pill_idx:
        p = pts[min(max(i, 1), len(pts) - 2)]
        place("Stuff/Beam_03", p + n * 0.0, n, FLOOR, scale=(1, 1, 1.5))
        pill_set.add(i)
    # decorate the bays between pillars
    bays = list(zip(pill_idx[:-1], pill_idx[1:])) if len(pill_idx) > 1 else [(0, len(pts) - 1)]
    for bi, (a, b) in enumerate(bays):
        if b - a < 6:
            continue
        mid = pts[(a + b) // 2]
        choice = random.random()
        if choice < 0.35 and b - a >= 10:
            place(random.choice(["Stuff/Pipes_02", "Stuff/Pipes_03"]), mid, n, FLOOR, scale=(1, 1, 1.15),
                  along=random.uniform(-0.6, 0.6))
        elif choice < 0.65:
            place(random.choice(["Stuff/Intercom_01", "Stuff/Intercom_02"]), mid, n, FLOOR + 1.55,
                  along=random.uniform(-0.8, 0.8))
            place("Lights/Light_SideLight_02", mid, n, FLOOR + 2.3, along=random.uniform(-0.3, 0.3))
        else:
            place("Lights/Light_SideLight_01", mid, n, FLOOR + 1.1, along=-0.5)
            place("Lights/Light_SideLight_01", mid, n, FLOOR + 1.1, along=0.5)
        # long side light strip near the top of each bay
        if random.random() < 0.6:
            place("Lights/Light_SideLight_01", pts[a + (b - a) // 4], n, FLOOR + 3.6)
print("placed props", len(placed))

# bake props into one object per material
bpy.ops.object.select_all(action='DESELECT')
for o in placed:
    o.data = o.data.copy()
    o.select_set(True)
bpy.context.view_layer.objects.active = placed[0]
bpy.ops.object.make_single_user(object=True, obdata=True)
bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
bpy.ops.object.join()
props = bpy.context.view_layer.objects.active
props.name = "Props"
props.parent = root
props.matrix_parent_inverse = root.matrix_world.inverted()
print("ROOT", root.matrix_world)
for tpl in templates.values():
    bpy.data.objects.remove(tpl, do_unlink=True)

# ------------------------------------------------- 6. export (same settings as before)
bpy.ops.object.select_all(action='SELECT')
bpy.ops.export_scene.fbx(filepath=OUT_FBX, use_selection=False, object_types={'EMPTY', 'MESH'},
                         bake_space_transform=True, apply_unit_scale=False, apply_scale_options='FBX_SCALE_NONE',
                         path_mode='RELATIVE', embed_textures=False, use_tspace=True, mesh_smooth_type='FACE',
                         add_leaf_bones=False)
print("EXPORTED", OUT_FBX)
