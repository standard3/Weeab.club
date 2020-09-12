/*
Copyright 2018 weeab.club

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once

#include "../../Hooks/Hooks.h"
#include "../../../Hacks/Hacks.h"
#include "../../../Hacks/Features/Visuals/ESP/ESP.h"
#include "../../Entities/Entities.h"
#include "../SDK.h"
#include <deque>
#include <array>
#include "Singleton.hpp"

inline Vector AngleVector(Vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(PI));

	return Vector(cp*cy, cp*sy, -sp);
}

inline float DistPointToLine(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

/*
	Most won't see it here anyways, but question from me to you.
	Should i declare?
*/

struct lbyRecords
{
	int tick_count;
	float lby;
	Vector headPosition;
	int32_t m_nFlags;
};
struct backtrackData
{
	float simtime;
	Vector hitboxPos;
	Vector origin;
	Vector angs;
	int32_t flags;
};
struct urmomlol
{
	int32_t m_nFlags;
};

struct thiccboi
{
	matrix3x4* matrix;
	Vector originX;
};

class BackTrack
{
	int latest_tick;
public:
	lbyRecords records[64];
	bool RunLBYBackTrack(CUserCmd * cmd, Vector & aimPoint);
	bool IsTickValid(int tick);
	void rageBackTrack(CUserCmd * cmd, IClientEntity * pLocal);

	void UpdateRecord(int i);
	void Update(int tick_count);
	void legitBackTrack(CUserCmd* cmd, IClientEntity* pLocal);

};

struct CTickRecord {
	CTickRecord() {}
	CTickRecord(IClientEntity* ent) {
		m_flLowerBodyYawTarget = ent->GetLowerBodyYaw();
		m_angEyeAngles = ent->GetEyeAngles();
		m_flCycle = ent->GetCycle();
		m_flSimulationTime = ent->GetSimulationTime();
		m_nSequence = ent->GetSequence();
		m_vecOrigin = ent->GetOrigin();
		m_vecVelocity = ent->GetVelocity();
		//m_flPoseParameter = ent->getPoseParams();
		m_angAbsAngles = ent->GetAbsAngles();
		m_vecAbsOrigin = ent->GetAbsOrigin();
		m_flDuckSpeed = ent->GetDuckSpeed();
		m_flDuckAmount = ent->GetDuckAmount();
	}

	explicit operator bool() const noexcept {
		return m_flSimulationTime > 0.f;
	}

	bool operator>(const CTickRecord& others) {
		return (m_flSimulationTime > others.m_flSimulationTime);
	}
	bool operator>=(const CTickRecord& others) {
		return (m_flSimulationTime >= others.m_flSimulationTime);
	}
	bool operator<(const CTickRecord& others) {
		return (m_flSimulationTime < others.m_flSimulationTime);
	}
	bool operator<=(const CTickRecord& others) {
		return (m_flSimulationTime <= others.m_flSimulationTime);
	}
	bool operator==(const CTickRecord& others) {
		return (m_flSimulationTime == others.m_flSimulationTime);
	}

	float m_flDuckAmount = 0.f;
	float m_flDuckSpeed = 0.f;
	float m_flLowerBodyYawTarget = 0.f;
	QAngle m_angEyeAngles = QAngle(0, 0, 0);
	float m_flCycle = 0.f;
	float m_flSimulationTime = 0.f;
	int m_nSequence = 0;
	Vector m_vecOrigin = Vector(0, 0, 0);
	Vector m_vecAbsOrigin = Vector(0, 0, 0);
	Vector m_vecVelocity = Vector(0, 0, 0);
	//std::array<float, 24> m_flPoseParameter = {};
	QAngle m_angAbsAngles = QAngle(0, 0, 0);
	int tickcount = 0;
};

struct C_PlayerStored {
	C_BaseCombatWeapon* CurrentWeapon = nullptr;
	int BulletsLeft = 0;
	bool EyeAnglesUpdated = false;
	bool IsFiring = false;
	int TickCount = 0;
	float SimulationTime = 0;
	QAngle EyeAngles = QAngle(0, 0, 0);
	CTickRecord ValidTick = CTickRecord();
};

struct CBacktrackInfo {
	CBacktrackInfo() : shot(false), player(C_PlayerStored()), tick_count(0), temporaryRecord(CTickRecord()) {}

	bool shot = false;
	C_PlayerStored player = C_PlayerStored();
	int tick_count = 0;
	CTickRecord temporaryRecord = CTickRecord();


	IClientEntity* Entity;
	std::deque<CTickRecord> TickRecords;
};

class CBacktracking : public Singleton<CBacktracking> 
{
	friend class CLagcompensation;
	friend class CResolver;


public:
	int ShotBackTrackedTick(IClientEntity* ent);
	void ShotBackTrackStoreFSN(IClientEntity* ent);
	void OverridePosesFsnRenderStart(IClientEntity* ent);
	void ShotBackTrackBeforeAimbot(IClientEntity* ent);
	void ShotBackTrackAimbotStart(IClientEntity* ent);
	void ShotBackTrackAimbotEnd(IClientEntity* ent);
	void ClearRecord(IClientEntity* Entity);

public:
	bool bTickIsValid(int tick);
	void SaveTemporaryRecord(IClientEntity* ent, CTickRecord record = CTickRecord());
	void RestoreTemporaryRecord(IClientEntity* ent);
	std::array<CBacktrackInfo, 32> arr_infos;


private:
	void ApplyTickrecord(IClientEntity* ent, CTickRecord record);
	void ApplyTickRecordFSN(IClientEntity* pEntity, CTickRecord record);
	void ApplyTickRecordCM(IClientEntity* pEntity, CTickRecord record);

private:
};

extern backtrackData headPositions[64][12];
extern thiccboi hdfdunn[64][12];
extern urmomlol commitdie[64][12];

extern BackTrack* backtracking;
