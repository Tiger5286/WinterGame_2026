#include "DroneStateShoot.h"
#include "DroneStateIdle.h"
#include "../Drone.h"
#include "Components/State/StateMachine.h"
#include "Utility/MyLib.h"
#include "DxLib.h"
#include <algorithm>

namespace
{
	// 構える時間
	constexpr int kReadyFrame = 30;
	// 構えている間、1フレームに残りの角度の何割だけ近づくか
	constexpr float kReadyTurnRate = 0.2f;

	// ゆっくり追う時間
	constexpr int kTrackFrame = 180;
	// 追っている間に1フレームで回れる最大の角度
	const float kTrackTurnSpeed = MyLib::DegreeToRadian(0.2f);
	// 追っている間の点滅の間隔(追い始め → 追い終わり にかけてだんだん短くする)
	constexpr int kTrackBlinkStartInterval = 20;
	constexpr int kTrackBlinkEndInterval = 1;

	// 光ってから撃つまでの時間
	constexpr int kLockFrame = 30;
	// 光っている時間(Lockの最初の数フレーム)
	constexpr int kFlashFrame = 8;

	// 撃った後の待ち時間
	constexpr int kCooldownFrame = 180;

	// レーザーの色
	constexpr unsigned int kLaserColor = 0xff0000;
	// 光った時のレーザーの色と太さ
	constexpr unsigned int kFlashColor = 0xffcccc;
	constexpr float kFlashRadius = 3.0f;
	constexpr int kFlashDivNum = 8;
}

DroneStateShoot::DroneStateShoot(Drone& drone) :
	DroneState(drone)
{
}

DroneStateShoot::~DroneStateShoot()
{
}

void DroneStateShoot::Enter()
{
	StartReady();
}

void DroneStateShoot::Update()
{
	const float ts = m_owner.GetTimeScale();
	m_frame += ts;

	switch (m_phase)
	{
	case Phase::Ready:
		// 素早くプレイヤーの方に向く
		TurnToPlayerLerp(kReadyTurnRate);
		if (m_frame >= kReadyFrame)
		{
			ChangePhase(Phase::Track);
		}
		break;

	case Phase::Track:
		// ゆっくりプレイヤーの方に向いていく
		TurnToPlayerLimited(kTrackTurnSpeed);
		// 残りフレームが0になったら表示を切り替え、その時点の間隔で数え直す
		m_blinkTimer -= ts;
		if (m_blinkTimer <= 0)
		{
			m_isLaserOn = !m_isLaserOn;
			m_blinkTimer = GetTrackBlinkInterval();
		}
		if (m_frame >= kTrackFrame)
		{
			// ここで向きを固定する(この後は回らない)
			m_lockedDir = GetMuzzleDir();
			ChangePhase(Phase::Lock);
		}
		break;

	case Phase::Lock:
		if (m_frame >= kLockFrame)
		{
			// 固定した向きに撃つ
			Fire(GetMuzzlePos(), m_lockedDir);
			ChangePhase(Phase::Cooldown);
		}
		break;

	case Phase::Cooldown:
		if (m_frame >= kCooldownFrame)
		{
			// まだ見えていればもう一度構える、見えていなければIdleに戻る
			if (CanSeePlayer())
			{
				StartReady();
			}
			else
			{
				m_owner.GetComponent<StateMachine<Drone>>()->ChangeState(std::make_unique<DroneStateIdle>(m_owner));
				return;
			}
		}
		break;
	}
}

void DroneStateShoot::Exit()
{
}

void DroneStateShoot::Draw()
{
	// 待っている間はレーザーを消す
	if (m_phase == Phase::Cooldown) return;
	// 追っている間は点滅の消えているタイミングなら描かない
	if (m_phase == Phase::Track && !m_isLaserOn) return;

	// 固定した後は固定した向き、それまでは銃が今向いている向き
	const Vector3 start = GetMuzzlePos();
	const Vector3 dir = m_phase == Phase::Lock ? m_lockedDir : GetMuzzleDir();
	const Vector3 end = GetLaserEnd(start, dir);

	// 固定した直後の数フレームだけ、白っぽく太くして光らせる
	if (m_phase == Phase::Lock && m_frame < kFlashFrame)
	{
		DrawCapsule3D(start, end, kFlashRadius, kFlashDivNum, kFlashColor, kFlashColor, true);
		return;
	}
	DrawLine3D(start, end, kLaserColor);
}

void DroneStateShoot::StartReady()
{
	ChangePhase(Phase::Ready);
}

void DroneStateShoot::ChangePhase(Phase phase)
{
	m_phase = phase;
	m_frame = 0.0f;
	// 点滅は「点いている」状態から始める
	m_isLaserOn = true;
	m_blinkTimer = GetTrackBlinkInterval();
}

float DroneStateShoot::GetTrackBlinkInterval() const
{
	// 追っている時間の進み具合(0～1)で、間隔を最初の値から最後の値へ近づける
	const float t = m_frame / kTrackFrame;
	const float interval = kTrackBlinkStartInterval + (kTrackBlinkEndInterval - kTrackBlinkStartInterval) * t;
	return (std::max)(1.0f, (interval));
}
