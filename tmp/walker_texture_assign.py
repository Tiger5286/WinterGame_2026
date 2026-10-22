import bpy, json, pathlib, hashlib
from mathutils import Vector
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath='C:/Users/SasakiTaiga/Downloads/Walker.fbx')
print('INSPECT',json.dumps({'objects':[(o.name,o.type,len(o.data.vertices) if o.type=='MESH' else len(o.data.bones) if o.type=='ARMATURE' else 0) for o in bpy.context.scene.objects],'materials':[(m.name,[(n.name,n.image.name,n.image.filepath) for n in m.node_tree.nodes if n.type=='TEX_IMAGE' and n.image]) for m in bpy.data.materials if m.node_tree],'actions':[(a.name,list(a.frame_range)) for a in bpy.data.actions]},ensure_ascii=False))
out=pathlib.Path('C:/Users/SasakiTaiga/Documents/GitHub/WinterGame_2026/素材/敵モデル/Walker/Walker_Cyber')
out.mkdir(exist_ok=True)
textures=out/'textures'
textures.mkdir(exist_ok=True)
texture=pathlib.Path('C:/Users/SasakiTaiga/Documents/GitHub/WinterGame_2026/素材/敵モデル/Walker/Walker.fbm/Ch36_1001_Diffuse_Cyber.png')
new_image=bpy.data.images.load(str(texture),check_existing=False)
material=bpy.data.materials['Ch36_Body']
bsdf=next(n for n in material.node_tree.nodes if n.type=='BSDF_PRINCIPLED')
base=bsdf.inputs['Base Color']
assert len(base.links)==1 and base.links[0].from_node.type=='TEX_IMAGE'
base.links[0].from_node.image=new_image
# ノーマル等は元の画像を保持し、埋め込み画像を実ファイル化してFBXへ再埋め込みする。
for node in material.node_tree.nodes:
    if node.type!='TEX_IMAGE' or not node.image: continue
    img=node.image
    name=pathlib.PureWindowsPath(img.filepath).name
    dest=textures/name
    if img.packed_file:
        dest.write_bytes(bytes(img.packed_file.data))
    elif pathlib.Path(img.filepath).exists():
        dest.write_bytes(pathlib.Path(img.filepath).read_bytes())
    else:
        raise RuntimeError('Missing image: '+img.filepath)
    img.filepath=str(dest)
bpy.context.scene.frame_set(1)
bpy.ops.export_scene.fbx(filepath=str(out/'Walker_Cyber.fbx'),use_selection=False,object_types={'ARMATURE','MESH'},add_leaf_bones=False,path_mode='COPY',embed_textures=True,bake_anim=True,bake_anim_use_all_actions=True,bake_anim_use_nla_strips=False,bake_anim_simplify_factor=0.0)
# 書き出したFBXを再読み込みし、画像・メッシュ・ボーン・アニメーションを確認する。
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(out/'Walker_Cyber.fbx'))
meshes=[o for o in bpy.context.scene.objects if o.type=='MESH']
arms=[o for o in bpy.context.scene.objects if o.type=='ARMATURE']
assert sum(len(o.data.vertices) for o in meshes)==14442
assert len(arms)==1 and len(arms[0].data.bones)==65
assert len(bpy.data.actions)==1
assert abs(bpy.data.actions[0].frame_range[1]-bpy.data.actions[0].frame_range[0]-250)<0.01
mat=meshes[0].data.materials[0]
bsdf=next(n for n in mat.node_tree.nodes if n.type=='BSDF_PRINCIPLED')
img=bsdf.inputs['Base Color'].links[0].from_node.image
assert img.packed_file and hashlib.sha256(bytes(img.packed_file.data)).digest()==hashlib.sha256(texture.read_bytes()).digest()
print('VERIFIED: texture bytes, 14442 vertices, 65 bones, 250-frame animation duration')
bpy.context.scene.frame_set(1)
bpy.context.view_layer.update()
points=[o.matrix_world@Vector(p) for o in meshes for p in o.bound_box]
low=Vector(tuple(min(v[i] for v in points) for i in range(3)))
high=Vector(tuple(max(v[i] for v in points) for i in range(3)))
center=(low+high)/2
height=high.z-low.z
def aim(obj): obj.rotation_euler=(center-obj.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=center+Vector((height*.65,-height*2.4,height*.25)))
camera=bpy.context.object
aim(camera)
camera.data.type='ORTHO'
camera.data.ortho_scale=max(height*1.2,(high.x-low.x)*1.4)
scene=bpy.context.scene
scene.camera=camera
for offset,power,size in [((1,-2,2),1000,2),((-2,-1,1),600,2),((0,2,2),1100,1.5)]:
    bpy.ops.object.light_add(type='AREA',location=center+Vector(offset)*height)
    lamp=bpy.context.object
    lamp.data.energy=power*height*height
    lamp.data.shape='DISK'
    lamp.data.size=size*height
    aim(lamp)
scene.world=bpy.data.worlds.new('PreviewWorld')
scene.world.color=(.16,.16,.16)
scene.render.engine='CYCLES'
scene.cycles.samples=24
scene.render.resolution_x=850
scene.render.resolution_y=1000
scene.render.resolution_percentage=100
scene.render.filepath=str(out/'preview.png')
bpy.ops.render.render(write_still=True)
