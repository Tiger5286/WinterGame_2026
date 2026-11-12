#include "MutantStateCharge.h"
#include "../Mutant.h"
#include "Components/State/StateMachine.h"
#include "MutantStateOrbit.h"
#include "Game/GameObjects/Character/Player/Player.h"
#include "Components/Transform.h"
#include "Components/Animator/Animator.h"
#include "Game/GameObjects/Attack.h"
#include "Utility/Matrix4x4.h"

namespace
{
	// 突進前の溜めの時間
	constexpr int kWindUpFrame = 60;

	constexpr float kRushSpeed = 20.0f;

	constexpr int kAttackDamage = 200;
	constexpr float kAttackRadius = 200.0f;
	const Vector3 kAttackOffset = { 0.0f,200.0f,-100.0f };

}

MutantStateCharge::MutantStateCharge(Mutant& mutant, int chargeCount) :
	MutantState(mutant),
	m_chargeCount(chargeCount - 1)	// 突進するたびに残り回数を減らす
{
}

MutantStateCharge::~MutantStateCharge()
{
}

void MutantStateCharge::Enter()
{
	m_owner.GetComponent<Animator>()->Play(L"Mutant|Run");
}

void MutantStateCharge::Update()
{
	m_frameCount++;
	auto& thisPos = m_owner.GetComponent<Transform>()->pos;

	if (m_frameCount <= kWindUpFrame)
	{
		// プレイヤーの方を向く
		RotToPlayer();
		// 突進開始の瞬間
		if (m_frameCount == kWindUpFrame)
		{
			// プレイヤーへの方向を求める
			m_toPlayerVec = (GetPlayer()->GetComponent<Transform>()->pos - thisPos);
			m_toPlayerVec.y = 0.0f;
			m_toPlayerVec.Normalize();
			// 攻撃を生成する
			m_pAtk = std::make_shared<Attack>();
			CollisionManager::AttackInfo info = {
				.damage = kAttackDamage
			};
			Attack::Data data = {
				.info = info,
				.colRadius = kAttackRadius,
				.hitObjTag = GameObject::Tag::Player
			};
			m_pAtk->SetData(data);
			m_pAtk->Init();
			// 攻撃の位置を設定する
			auto transform = m_owner.GetComponent<Transform>();
			m_pAtk->GetComponent<Transform>()->pos = kAttackOffset * Matrix4x4::GetRotY(transform->rot.y) + transform->pos;
		}
	}
	else
	{
		// 移動する
		thisPos += m_toPlayerVec * kRushSpeed;
		// 攻撃の位置を更新
		if (m_pAtk)
		{
			auto transform = m_owner.GetComponent<Transform>();
			m_pAtk->GetComponent<Transform>()->pos = kAttackOffset * Matrix4x4::GetRotY(transform->rot.y) + transform->pos;
		}
		// 端についたら終わる、もしくはもう一度
		float centerToThisLenSq = (GetStageCenter() - thisPos).SquaredLength();
		if (centerToThisLenSq > GetStageRadius() * GetStageRadius())
		{
			auto stateMachine = m_owner.GetComponent<StateMachine<Mutant>>();
			if (m_chargeCount <= 0)
			{
				stateMachine->ChangeState(std::make_unique<MutantStateOrbit>(m_owner));
				return;
			}
			else
			{
				stateMachine->ChangeState(std::make_unique<MutantStateCharge>(m_owner, m_chargeCount));
				return;
			}
		}
	}
}

void MutantStateCharge::Exit()
{
}

void MutantStateCharge::Draw()
{
#ifdef _DEBUG
	if (m_pAtk)
	{
		m_pAtk->Draw();
	}
#endif
}
