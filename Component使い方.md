# Components フォルダーの使い方

この文書は Project/Source/Components 内のクラスを、現在のコードに沿って説明します。

## 共通の使い方

GameObject のコンストラクターが Transform を自動で追加します。それ以外のコンポーネントは、GameObject の派生クラスのコンストラクターで AddComponent<T>() を呼んで追加します。GetComponent<T>() はコンポーネントへのポインターを返し、見つからない場合は nullptr です。

**例:**
    AddComponent<Physics>();
    AddComponent<CapsuleCollider>(*GetComponent<Transform>(), 30.0f, 180.0f);

    // GameObject::Init() 内で依存先を設定する
    GetComponent<Physics>()->Init(
        GetComponent<Transform>(), GetComponent<CapsuleCollider>());

コンポーネントの Update() は自動で呼ばれません。各 GameObject の Update() から、必要な順番で呼びます。アニメーション後にボーン位置を使うヒットボックスを更新するなど、更新順も重要です。RemoveComponent<T>() は最初に見つかった1個を破棄するため、破棄後に以前取得したポインターを使わないでください。

## 1. Component.h — Component
全コンポーネントの共通基底クラスです。仮想デストラクターを持つだけで、更新や描画の仕組みは提供しません。派生クラス固有の処理は、GameObject 側から明示的に呼びます。

## 2. Transform.h — Transform
GameObject の位置・回転・拡大率を保持します。GameObject 生成時に自動追加されます。

    auto* transform = GetComponent<Transform>();
    transform->pos = Vector3(0.0f, 0.0f, 0.0f);
    transform->rot.y = DX_PI_F;    // 回転はラジアン
    transform->scale = Vector3::One();

モデル描画や Collider がこの Transform を参照します。Transform 自体に Update() はありません。

## 3. Physics.h / Physics.cpp — Physics
速度・加速度・重力・水平減速・最高速度と、CollisionManager を使った押し戻しを扱います。Transform と任意の Collider を Init() に渡し、GameObject の Update() から毎フレーム Update() を呼びます。

    auto* physics = GetComponent<Physics>();
    physics->Init(GetComponent<Transform>(), GetComponent<CapsuleCollider>());
    physics->m_accel = Vector3(1.0f, 0.0f, 0.0f);
    physics->Update();

m_vel と m_accel は毎フレームの速度・加速度として公開されています。SetDrag() は水平速度の減衰率 (0～1)、SetGravity() はY方向に加える重力、SetMaxSpeed() はXZ方向の上限速度を指定します。IsGrounded() で接地状態を確認できます。Collider を nullptr にすると Physics 内の衝突判定と押し戻しは行われません。

現在の実装では接地中の短い段差追従も行います。床扱いの角度は水平から45度以内、吸着距離は12です。
> **注意:** Physics が押し戻しに使う CollisionManager::CheckCollision() は、移動側コライダーとして CapsuleCollider だけを受け付けます。SphereCollider などを渡してもこの経路では衝突解決されません。また、相手側の球コライダーも現在は押し戻し判定の対象外です。

## 4. Animator 関連

### Animator.h / Animator.cpp — Animator
モデルのアニメーションを登録・再生します。モデルを Init() に渡してから AddAnimation() でモデル内のアニメーション名を登録し、Play() で再生します。Update() は GameObject の Update() から毎フレーム呼んでください。

    auto* animator = GetComponent<Animator>();
    animator->Init(model);
    animator->AddAnimation(L"Player|Idle", 0.5f, true);
    animator->Play(L"Player|Idle");
    animator->Update();

レイヤーは FullBody、UpperBody、LowerBody です。SetSplitBody(true) で全身レイヤーから上下半身レイヤーへ徐々に切り替え、PlaySynced() で再生時間を合わせて別レイヤーへ同じアニメーションを設定できます。Stop() で指定レイヤーを止め、IsEnd() で非ループアニメーションの終了を確認します。SetAimAngle() は上半身レイヤーの照準角を設定します。角度はラジアンで渡します。

Animator は Mixamo 系のボーン名 (Hips、LeftUpLeg、RightUpLeg、Spine2 など) を検索します。モデルに必要な名前がない場合、Init() の assert に失敗することがあります。アニメーション名もモデル内の名前と完全一致させます。

### Animation.h / Animation.cpp — Animation
Animator に登録されるアニメーション情報です。モデル、アニメーション番号、再生速度、ループ有無、全長を保持します。通常は直接生成せず、Animator::AddAnimation() 経由で登録します。

### AnimationLayer.h / AnimationLayer.cpp — AnimationLayer
Animator 内部の再生レイヤーです。現在と次のアニメーションを保持し、切り替えを10フレームでブレンドします。通常は Animator の API を使い、直接操作しません。

## 5. Collider 関連

### Collider.h / Collider.cpp — Collider
当たり判定の共通基底クラスです。Transform への参照、形状種別、有効/無効、オフセットを持ちます。SetEnable(false) で無効化できます。Collider の派生クラスは対応する Draw() を実装します。

> **重要:** 現在の Collider::SetOffset() の値は球・カプセルのデバッグ描画には反映されますが、CollisionManager の判定位置には反映されません。判定にもオフセットを使いたい場合は CollisionManager 側の対応が必要です。

### SphereCollider.h / .cpp — SphereCollider
Transform の位置を中心とする球です。コンストラクターに Transform と半径を渡します。SetRadius() で半径を変更できます。

    AddComponent<SphereCollider>(*GetComponent<Transform>(), 40.0f);

### CapsuleCollider.h / .cpp — CapsuleCollider
Y軸に沿ったカプセルです。コンストラクターの height は球部分を含むカプセル全体の高さで、半径の2倍未満にはなりません。Transform の位置はカプセル底面側の基準として使われます。SetRadius()、SetHeight() で寸法を変更し、Draw() でデバッグ表示できます。

    AddComponent<CapsuleCollider>(*GetComponent<Transform>(), 30.0f, 180.0f);

> **注意:** GetBottom() と GetTop() は現在 const Vector3& を返しますが、実装は計算結果の一時オブジェクトを返しています。返された参照を保存して使うのは危険です。利用する場合は呼び出し元で Vector3 の値として受け取る形が安全です。根本対応は戻り値を Vector3 に変更することです。

### PolygonCollider.h / .cpp — PolygonCollider
モデルのポリゴンを使う Collider です。Transform と Model* を受け取ります。Model の所有権は持たないため、Model は PolygonCollider より長く生存させてください。SetModel() で参照先を変更できます。

## 6. Hitbox 関連

### Hitbox.h / Hitbox.cpp — Hitbox
射撃などの対象判定用に、複数のボーン追従コライダーをまとめます。AddCollider() に HitboxSphere::Info または HitboxCapsule::Info を渡します。Update() は登録した全判定を更新し、Draw() は _DEBUG 時に表示します。

    HitboxSphere::Info head;
    head.frameName = L"mixamorig:Head";
    head.pModel = model;
    head.radius = 20.0f;
    head.localOffset = Vector3::Zero();
    head.isWeakPoint = true;
    GetComponent<Hitbox>()->AddCollider(head);

アニメーションとモデル姿勢を更新した後に Hitbox::Update() を呼び、当たり判定のボーン位置を最新にします。

### HitboxCollider.h / .cpp — HitboxCollider
Hitbox 内で使う球・カプセルの抽象基底クラスです。Update() と Draw() を派生クラスに要求し、Type で形状を識別します。GameObject の Component ではありません。

### HitboxSphere.h / .cpp — HitboxSphere
指定したモデルボーンのワールド位置に球を置きます。Info に frameName、pModel、radius、localOffset、isWeakPoint を設定します。frameName はモデルのボーン名と一致させます。pModel は所有せず、ローカルオフセットはボーン座標系の平行移動です。

### HitboxCapsule.h / .cpp — HitboxCapsule
bottomFrameName と topFrameName で指定した2ボーンを結ぶカプセルです。Info に pModel、radius、bottomLocalOffset、topLocalOffset、isWeakPoint を設定します。2つのボーン名はモデル内に存在する必要があります。pModel の寿命は Hitbox より長くしてください。

## 7. Hackable.h / Hackable.cpp — Hackable
ゲームオブジェクトにハッキング可能なパズル状態とハッキング残り時間を持たせます。AddComponent<Hackable>() で追加し、Init() で盤面を初期化します。SetInfo() の func には、パズルを解いたときに呼ぶ処理を設定します。

    Hackable::Info info;
    info.func = [this](Hackable::HackedData data) {
        OnHacked(data);
    };
    info.hackLocalPos = Vector3(0.0f, 100.0f, 0.0f);
    auto* hackable = GetComponent<Hackable>();
    hackable->SetInfo(info);
    hackable->Init();

Update() は残り時間とパズル完了を更新します。ハッキング入力を受け取る間は UpdateHacking() を呼びます。IsHacked() はハッキング残り時間があるかを返し、GetHackLocalPos() はUI配置用オフセットを返します。func を設定せずにパズルを解くと空の関数を呼ぶことになるため、Init()/Update() の前にコールバックを設定してください。

盤面サイズや制限時間は現在 Hackable.cpp 内の定数で固定されています。NodeType は盤面内部のマス状態です。

## 8. State 関連

### State.h — `State<Owner>`
状態ごとの基底クラスです。Owner への参照を保持します。各派生ステートで Enter()、Update()、Exit() を実装します。Draw() は必要な場合だけオーバーライドします。

### StateMachine.h — StateMachine<Owner>
Owner ごとの現在の State を1つ管理する Component です。追加時に Owner を渡します。

    AddComponent<StateMachine<Player>>(*this);
    GetComponent<StateMachine<Player>>()->ChangeState(
        std::make_unique<PlayerStateIdle>(*this));

ChangeState() は古い状態の Exit() を呼び、新しい状態を所有して Enter() を呼びます。以後、GameObject の Update()/Draw() から StateMachine の Update()/Draw() を呼びます。ステートは unique_ptr で所有されるため、切り替え前後で所有権を共有しません。ステートが保持する Owner 参照は、StateMachine より長く生存する必要があります。

## 現在のコードで特に気をつける点
- GameObject の派生クラスが各コンポーネントの初期化・更新・描画を呼びます。追加しただけでは動きません。
- Init() に必要な Transform、Collider、Model、ボーン名を先に準備してください。
- GetComponent<T>() の戻り値は nullptr の可能性があります。存在しない場合があるコンポーネントは確認してから使います。
- Collider と Hitbox の `Model*` / `Transform&` は所有権を持たない参照です。参照先を先に破棄しないでください。
