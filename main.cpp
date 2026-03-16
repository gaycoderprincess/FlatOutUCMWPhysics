#include <windows.h>
#include <format>
#include <cmath>
#include <numbers>
#include <toml++/toml.hpp>

#include "nya_commonhooklib.h"
#include "nya_commonmath.h"
#include "nfsps.h"

#include "inputs.h"

#include "include/chloemenulib.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("NFSPSMWPhysics_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

wchar_t gDLLDir[MAX_PATH];
class DLLDirSetter {
public:
	wchar_t backup[MAX_PATH];

	DLLDirSetter() {
		GetCurrentDirectoryW(MAX_PATH, backup);
		SetCurrentDirectoryW(gDLLDir);
	}
	~DLLDirSetter() {
		SetCurrentDirectoryW(backup);
	}
};

//#define FUNCTION_LOG(name) WriteLog(std::format("{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
#define WHEEL_FUNCTION_LOG(name) WriteLog(std::format("Wheel::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
#define CHASSIS_FUNCTION_LOG(name) WriteLog(std::format("Chassis::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
#define SUSPENSIONSIMPLE_FUNCTION_LOG(name) WriteLog(std::format("SuspensionSimple::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
#define SUSPENSIONRACER_FUNCTION_LOG(name) WriteLog(std::format("SuspensionRacer::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
#define ENGINERACER_FUNCTION_LOG(name) WriteLog(std::format("EngineRacer::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)));
//#define ICHASSIS_FUNCTION_LOG(name) WriteLog(std::format("IChassis::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define ITIPTRONIC_FUNCTION_LOG(name) WriteLog(std::format("ITiptronic::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define IRACEENGINE_FUNCTION_LOG(name) WriteLog(std::format("IRaceEngine::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define IENGINEDAMAGE_FUNCTION_LOG(name) WriteLog(std::format("IEngineDamage::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define IINDUCTABLE_FUNCTION_LOG(name) WriteLog(std::format("IInductable::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define ITRANSMISSION_FUNCTION_LOG(name) WriteLog(std::format("ITransmission::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
//#define IENGINE_FUNCTION_LOG(name) WriteLog(std::format("IEngine::{} called from {:X}", name, (uintptr_t)__builtin_return_address(0)))
#define ICHASSIS_FUNCTION_LOG(name) {}
#define ITIPTRONIC_FUNCTION_LOG(name) {}
#define IRACEENGINE_FUNCTION_LOG(name) {}
#define IENGINEDAMAGE_FUNCTION_LOG(name) {}
#define IINDUCTABLE_FUNCTION_LOG(name) {}
#define ITRANSMISSION_FUNCTION_LOG(name) {}
#define IENGINE_FUNCTION_LOG(name) {}

bool bAffectOpponents = false;
bool bRevLimiter = true;
float fOpponentRubberband = 0.0;

auto cartuning_LookupKey = (uint32_t(__thiscall*)(const ISimable*))0x49D000;
auto ctor_cartuning = (void(__stdcall*)(Attrib::Gen::vehicle*, uint32_t))0x49CD50;
//auto dtor_simobject = (void(__thiscall*)(void*))0x7BC8A0;

#define GET_FAKE_INTERFACE(base, type, var) { auto ptr = (uintptr_t)this; ptr += offsetof(base, var); return (type*)ptr; }

void UpdatePerfectLaunchText() {
	auto& list = VEHICLE_LIST::GetList(VEHICLE_PLAYERS);
	if (list.empty()) return;

	auto ply = VEHICLE_LIST::GetList(VEHICLE_PLAYERS)[0];
	if (!ply) return;

	const char* text = ply->GetPerfectLaunch() > 0.0 ? "PERFECT LAUNCH!" : "GO!";

	for (int i = 0; i < EA::Localizer::LocalizerManager::mMaxCategories; i++) {
		auto category = &EA::Localizer::LocalizerManager::mCategories[i];
		for (int j = 0; j < category->mNumberStringRecords; j++) {
			auto record = &category->mRecordTables[j];
			if (record->mHash == 0x61223AB5) {
				record->mPackedString = text;
			}
		}
	}
}

#include "decomp/ConversionUtil.hpp"
#include "decomp/UMathExtras.h"
#include "decomp/AverageWindow.h"

#include "MWCarTuning.h"

#include "decomp/behaviors/MWWheel.h"
#include "decomp/behaviors/MWChassisBase.h"
#include "decomp/behaviors/SuspensionRacer.h"
#include "decomp/behaviors/SuspensionSimple.h"
#include "decomp/behaviors/EngineRacer.h"
#include "decomp/interfaces/MWIChassis.cpp"
#include "decomp/interfaces/MWIRaceEngine.cpp"
#include "decomp/interfaces/MWITiptronic.cpp"
#include "decomp/interfaces/MWIEngineDamage.cpp"
#include "decomp/interfaces/MWIInductable.cpp"
#include "decomp/interfaces/MWITransmission.cpp"
#include "decomp/interfaces/MWIEngine.cpp"
#include "decomp/behaviors/MWWheel.cpp"
#include "decomp/behaviors/MWChassisBase.cpp"
#include "decomp/behaviors/SuspensionRacer.cpp"
#include "decomp/behaviors/SuspensionSimple.cpp"
#include "decomp/behaviors/EngineRacer.cpp"

void ValueEditorMenu(float& value) {
	ChloeMenuLib::BeginMenu();

	static char inputString[1024] = {};
	ChloeMenuLib::AddTextInputToString(inputString, 1024, true);
	ChloeMenuLib::SetEnterHint("Apply");

	if (DrawMenuOption(inputString + (std::string)"...", "", false, false) && inputString[0]) {
		value = std::stof(inputString);
		memset(inputString,0,sizeof(inputString));
		ChloeMenuLib::BackOut();
	}

	ChloeMenuLib::EndMenu();
}

void QuickValueEditor(const char* name, float& value) {
	if (DrawMenuOption(std::format("{} - {}", name, value))) { ValueEditorMenu(value); }
}

void DebugMenu() {
	ChloeMenuLib::BeginMenu();

	if (bAffectOpponents) {
		QuickValueEditor("AI Rubberband Factor", fOpponentRubberband);
	}

	if (DrawMenuOption("Toggle Speedbreaker")) {
		PLAYER_LIST::GetList(PLAYER_LOCAL)[0]->ToggleGameBreaker();
	}

	if (DrawMenuOption("EngineRacer")) {
		ChloeMenuLib::BeginMenu();

		if (pEngine) {
			auto ply = VEHICLE_LIST::GetList(VEHICLE_PLAYERS)[0];

			if (auto cust = ply->GetCustomizations()) {
				for (int i = 0; i < 32; i++) {
					DrawMenuOption(std::format("tune {} {:.2f}", i, ply->GetCustomizations()->PhysicsTuning[i]));
				}

				//DrawMenuOption(std::format("CARSLOTID_BRAKE_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_BRAKE_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_DRIVETRAIN_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_DRIVETRAIN_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_ENGINE_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_ENGINE_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_FORCED_INDUCTION_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_FORCED_INDUCTION_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_NITROUS_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_NITROUS_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_SUSPENSION_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_SUSPENSION_PACKAGE]));
				DrawMenuOption(std::format("CARSLOTID_TIRE_PACKAGE {}", ply->GetCustomizations()->InstalledParts[CARSLOTID_TIRE_PACKAGE]));
			}

			DrawMenuOption(std::format("pEngine {:X}", (uintptr_t)pEngine));
			DrawMenuOption(std::format("ITransmission {:X}", (uintptr_t)pEngine->GetITransmission()));
			DrawMenuOption(std::format("mGear {}", pEngine->mGear));
			DrawMenuOption(std::format("GetTopGear {}", (int)pEngine->GetTopGear()));
			DrawMenuOption(std::format("mTransmissionVelocity {:.2f}", pEngine->mTransmissionVelocity));
			DrawMenuOption(std::format("GetDriveTorque {:.2f}", pEngine->GetDriveTorque()));
			DrawMenuOption(std::format("GetSpeedometer {:.2f}", pEngine->GetSpeedometer()));
			DrawMenuOption(std::format("CalcSpeedometer {:.2f}", pEngine->CalcSpeedometer(RPS2RPM(pEngine->mTransmissionVelocity), pEngine->mGear)));
			DrawMenuOption(std::format("GetMaxSpeedometer {:.2f}", pEngine->GetMaxSpeedometer()));
			DrawMenuOption(std::format("IsGearChanging {}", pEngine->IsGearChanging()));
			DrawMenuOption(std::format("mEngineBraking {}", pEngine->mEngineBraking));
			DrawMenuOption(std::format("IDLE {:.2f}", pEngine->mCarInfo.GetLayout()->IDLE));
			DrawMenuOption(std::format("TORQUE.size() {}", pEngine->mMWInfo->TORQUE.size()));
			DrawMenuOption(std::format("mSuspension {:X}", (uintptr_t)pEngine->mSuspension));
			DrawMenuOption(std::format("mIInput {:X}", (uintptr_t)pEngine->mIInput));
			DrawMenuOption(std::format("actual mSuspension {:X}", (uintptr_t)pEngine->mVehicle->mCOMObject->Find<IChassis>()));
			DrawMenuOption(std::format("actual mIInput {:X}", (uintptr_t)pEngine->mVehicle->mCOMObject->Find<IInput>()));
			DrawMenuOption(std::format("pSuspension {:X}", (uintptr_t)pSuspension));
			DrawMenuOption(std::format("GetControlGas {}", pEngine->mIInput->GetControlGas()));
		}
		else {
			DrawMenuOption("woof?");
		}

		ChloeMenuLib::EndMenu();
	}
	if (DrawMenuOption("SuspensionRacer")) {
		ChloeMenuLib::BeginMenu();

	if (pSuspension) {
		ISteeringWheel::SteeringType steer_type = ISteeringWheel::kGamePad;

		if (IPlayer *player = PLAYER_LIST::GetList(PLAYER_LOCAL)[0]) {
			ISteeringWheel *device = player->GetSteeringDevice();

			if (device && device->IsConnected()) {
				steer_type = device->GetSteeringType();
			}
		}
		DrawMenuOption(std::format("steer_type {}", (int)steer_type));

		DrawMenuOption(std::format("RIM_SIZE.Front {:.2f}", pSuspension->mAttributes.GetLayout()->RIM_SIZE.Front));
		DrawMenuOption(std::format("RIM_SIZE.Rear {:.2f}", pSuspension->mAttributes.GetLayout()->RIM_SIZE.Rear));
		DrawMenuOption(std::format("WHEEL_BASE {:.2f}", pSuspension->mAttributes.GetLayout()->WHEEL_BASE));
		DrawMenuOption(std::format("TRACK_WIDTH.Front {:.2f}", pSuspension->mAttributes.GetLayout()->TRACK_WIDTH.Front));
		DrawMenuOption(std::format("TRACK_WIDTH.Rear {:.2f}", pSuspension->mAttributes.GetLayout()->TRACK_WIDTH.Rear));
		DrawMenuOption(std::format("SECTION_WIDTH.Front {:.2f}", pSuspension->mAttributes.GetLayout()->SECTION_WIDTH.Front));
		DrawMenuOption(std::format("SECTION_WIDTH.Rear {:.2f}", pSuspension->mAttributes.GetLayout()->SECTION_WIDTH.Rear));
		DrawMenuOption(std::format("FRONT_AXLE {:.2f}", pSuspension->mAttributes.GetLayout()->FRONT_AXLE));
		DrawMenuOption(std::format("Layout {:X}", (uintptr_t)pSuspension->mAttributes.GetLayout()));

		if (auto veh = VEHICLE_LIST::GetList(VEHICLE_PLAYERS)[0]) {
			DrawMenuOption(std::format("state.speed {:.2f}", LastChassisState.speed));
			DrawMenuOption(std::format("GetSpeedometer() {:.2f}", veh->mCOMObject->Find<ITransmission>()->GetSpeedometer()));
			DrawMenuOption(std::format("GetMaxSpeedometer() {:.2f}", veh->mCOMObject->Find<ITransmission>()->GetMaxSpeedometer()));
			DrawMenuOption(std::format("GetPerfectLaunch() {:.2f}", veh->GetPerfectLaunch()));
		}

		//QuickValueEditor("BrakesAtValue", UNDERCOVER_BrakesAtValue);
		//QuickValueEditor("StaticGripAtValue", UNDERCOVER_StaticGripAtValue);
		//QuickValueEditor("RollCenterAtValue", UNDERCOVER_RollCenterAtValue);
		//QuickValueEditor("AeroCGAtValue", UNDERCOVER_AeroCGAtValue);
		//QuickValueEditor("AeroCoeffAtValue", UNDERCOVER_AeroCoeffAtValue);
		//QuickValueEditor("SuspensionAtValue", UNDERCOVER_SuspensionAtValue);
		//QuickValueEditor("SteeringAtValue", UNDERCOVER_SteeringAtValue);

		DrawMenuOption(std::format("state.inertia {:.2f} {:.2f} {:.2f}", LastChassisState.inertia.x, LastChassisState.inertia.y, LastChassisState.inertia.z));
		DrawMenuOption(std::format("state.dimension {:.2f} {:.2f} {:.2f}", LastChassisState.dimension.x, LastChassisState.dimension.y, LastChassisState.dimension.z));
		DrawMenuOption(std::format("state.cog {:.2f} {:.2f} {:.2f}", LastChassisState.cog.x, LastChassisState.cog.y, LastChassisState.cog.z));
		DrawMenuOption(std::format("state.speed {:.2f}", LastChassisState.speed));
		DrawMenuOption(std::format("state.slipangle {:.2f}", LastChassisState.slipangle));
		DrawMenuOption(std::format("state.ground_effect {:.2f}", LastChassisState.ground_effect));
		DrawMenuOption(std::format("state.time {:.2f}", LastChassisState.time));

		//DrawMenuOption(std::format("state.flags - {}", LastChassisState.flags));
		//DrawMenuOption(std::format("state.time - {}", LastChassisState.time));
		//DrawMenuOption(std::format("state.mass - {}", LastChassisState.mass));
		//DrawMenuOption(std::format("state.local_vel - {:.2f} {:.2f} {:.2f}", LastChassisState.local_vel.x, LastChassisState.local_vel.y, LastChassisState.local_vel.z));
		//DrawMenuOption(std::format("state.linear_vel - {:.2f} {:.2f} {:.2f}", LastChassisState.linear_vel.x, LastChassisState.linear_vel.y, LastChassisState.linear_vel.z));
		//DrawMenuOption(std::format("state.speed - {:.2f}", LastChassisState.speed));
		DrawMenuOption(std::format("state.steer_input - {:.2f}", LastChassisState.steer_input));
		DrawMenuOption(std::format("state.nos_boost - {:.2f}", LastChassisState.nos_boost));
		DrawMenuOption(std::format("state.shift_boost - {:.2f}", LastChassisState.shift_boost));
		DrawMenuOption(std::format("mDrift.State - {}", (int)pSuspension->mDrift.State));
		DrawMenuOption(std::format("mDrift.Value - {:.2f}", pSuspension->mDrift.Value));
		DrawMenuOption(std::format("mBurnOut.Traction - {:.2f}", pSuspension->mBurnOut.GetTraction()));
		DrawMenuOption(std::format("mBurnOut.State - {}", pSuspension->mBurnOut.GetState()));
		//DrawMenuOption(std::format("MaxSlip - {:.2f}", pSuspension->ComputeMaxSlip(LastChassisState)));
		//DrawMenuOption(std::format("MaxSteering - {:.2f}", pSuspension->CalculateMaxSteering(LastChassisState, ISteeringWheel::kGamePad)));
		DrawMenuOption(std::format("LateralGripScale - {:.2f}", pSuspension->ComputeLateralGripScale(LastChassisState)));
		//DrawMenuOption(std::format("TractionScale - {:.2f}", pSuspension->ComputeTractionScale(LastChassisState)));
		DrawMenuOption(std::format("Wheels - {:.2f} {:.2f}", pSuspension->mSteering.Wheels[0], pSuspension->mSteering.Wheels[1]));
		DrawMenuOption(std::format("LastMaximum - {:.2f}", pSuspension->mSteering.LastMaximum));
		//DrawMenuOption(std::format("mGameBreaker - {:.2f}", pSuspension->mGameBreaker));

		for (int i = 0; i < 4; i++) {
			auto tire = pSuspension->mTires[i];
			DrawMenuOption(std::format("Tire {}", i+1));
			DrawMenuOption(std::format("fNormal - {:.2f} {:.2f} {:.2f} {:.2f}", tire->mNormal.x, tire->mNormal.y, tire->mNormal.z, tire->mNormal.w));
			DrawMenuOption(std::format("mCompression - {:.2f}", tire->mCompression));
			DrawMenuOption(std::format("mLateralSpeed - {:.2f}", tire->mLateralSpeed));
			DrawMenuOption(std::format("mForce - {:.2f} {:.2f} {:.2f}", tire->mForce.x, tire->mForce.y, tire->mForce.z));
			DrawMenuOption(std::format("mLongitudeForce - {:.2f}", tire->mLongitudeForce));
			DrawMenuOption(std::format("mTractionBoost - {:.2f}", tire->mTractionBoost));
			DrawMenuOption(std::format("mLateralBoost - {:.2f}", tire->mLateralBoost));
			DrawMenuOption(std::format("mDriftFriction - {:.2f}", tire->mDriftFriction));
			DrawMenuOption(std::format("mGripBoost - {:.2f}", tire->mGripBoost));
			DrawMenuOption(std::format("mTraction - {:.2f}", tire->mTraction));
			DrawMenuOption(std::format("mSlip - {:.2f}", tire->mSlip));
			DrawMenuOption(std::format("mRadius - {:.2f}", tire->mRadius));
			DrawMenuOption(std::format("mRoadSpeed - {:.2f}", tire->mRoadSpeed));
			DrawMenuOption(std::format("mAV - {:.2f}", tire->mAV));
		}
	}
	else {
		DrawMenuOption("woof?");
	}

		ChloeMenuLib::EndMenu();
	}

	ChloeMenuLib::EndMenu();
}

auto oldctorbase = (void*(__thiscall*)(void*, BehaviorParams*, int))0x71CE50;
SuspensionSimpleMW* SuspensionSimpleConstruct(BehaviorParams* bp) {
	auto data = (SuspensionSimpleMW*)gFastMem.Alloc(sizeof(SuspensionSimpleMW), nullptr);
	memset(data,0,sizeof(SuspensionSimpleMW));
	oldctorbase(data, bp, 0);
	data->Create(*bp);
	return data;
}

SuspensionRacerMW* SuspensionRacerConstruct(BehaviorParams* bp) {
	auto data = pSuspension = (SuspensionRacerMW*)gFastMem.Alloc(sizeof(SuspensionRacerMW), nullptr);
	memset(data,0,sizeof(SuspensionRacerMW));
	oldctorbase(data, bp, 0);
	data->Create(*bp);
	return data;
}

EngineRacer* EngineRacerConstruct(BehaviorParams* bp) {
	auto data = pEngine = (EngineRacer*)gFastMem.Alloc(sizeof(EngineRacer), nullptr);
	memset(data,0,sizeof(EngineRacer));
	oldctorbase(data, bp, 0);
	data->Create(*bp);
	return data;
}

class FactoryEntry {
public:
	UCrc32 mSignature;
	void* mConstructor;
	FactoryEntry *mTail;

	static inline auto& mHead = *(FactoryEntry**)0xACDE7C;

	FactoryEntry(const char* name, void* function) {
		mSignature.mCRC = Attrib::StringHash32(name);
		mConstructor = function;
		mTail = FactoryEntry::mHead;
		FactoryEntry::mHead = this;
	}
};
FactoryEntry __EngineRacerMW("EngineRacerMW", (void*)&EngineRacerConstruct);
FactoryEntry __SuspensionSimpleMW("SuspensionSimpleMW", (void*)&SuspensionSimpleConstruct);
FactoryEntry __SuspensionRacerMW("SuspensionRacerMW", (void*)&SuspensionRacerConstruct);

bool bEnabledGrip = true;
bool bEnabledSpeed = true;
bool bEnabledDrag = false;
bool bEnabledDrift = false;
bool GetIsVanillaEvent() {
	if (GRaceStatus::fObj && GRaceStatus::fObj->mRaceParms) {
		auto raceType = GRaceParameters::GetRaceType(GRaceStatus::fObj->mRaceParms);
		if (raceType >= GRace::kRaceType_Grip_Min && raceType < GRace::kRaceType_Grip_Max) return !bEnabledGrip;
		if (raceType >= GRace::kRaceType_P2P_Min && raceType < GRace::kRaceType_P2P_Max) return !bEnabledSpeed;
		if (raceType >= GRace::kRaceType_Drag_Min && raceType < GRace::kRaceType_Drag_Max) return !bEnabledDrag;
		if (raceType >= GRace::kRaceType_Drift_Min && raceType < GRace::kRaceType_Drift_Max) return !bEnabledDrift;
	}
	return false;
}

bool bSpeedbreakerEnabled = false;
void SpeedbreakerLoop() {
	if (GetIsVanillaEvent()) nNOSState = 0;

	if (!bSpeedbreakerEnabled) return;
	if (DALPauseStates::mPauseRequest) return;
	if (!Sim::Exists()) return;
	if (Sim::GetState() != Sim::STATE_ACTIVE) return;
	if (INIS::mInstance) return;
	if (!GRaceStatus::fObj) return;
	if (!GRaceStatus::fObj->mRaceParms) return;

	auto list = PLAYER_LIST::GetList(PLAYER_LOCAL);
	if (list.empty()) return;

	auto ply = list[0];
	if (IsKeyJustPressed('X') || IsPadKeyJustPressed(NYA_PAD_KEY_X, -1)) {
		ply->ToggleGameBreaker();
	}

	RefreshInputs();
}

void AssistLoop() {
	auto list = VEHICLE_LIST::GetList(VEHICLE_PLAYERS);
	if (list.empty()) return;

	auto ply = list[0];
	for (int i = 0; i < NUM_DRIVER_AIDS; i++) {
		if (ply->GetDriverAidLevel((DriverAidType)i) == 0) continue;
		ply->SetDriverAidLevel((DriverAidType)i, 0, true);
	}
}

// it's supposed to be 2085.71, 8257.07, 511.35
// from inertia tensor 1.0, 3.5, 1.0, mass 1280

std::vector<Attrib::Collection*> FindCollectionAndAllChildren(const char* className, const char* name) {
	std::vector<Attrib::Collection*> out;

	auto parent = Attrib::FindCollection(Attrib::StringHash32(className), Attrib::StringHash32(name));
	if (!parent) return out;
	out.push_back(parent);

	auto pClass = Attrib::Database::sThis->GetClass(Attrib::StringHash32(className));
	auto collHash = pClass->GetFirstCollection();
	while (collHash) {
		auto childCollection = Attrib::FindCollection(Attrib::StringHash32(className), collHash);
		if (childCollection->mParent == parent) {
			out.push_back(childCollection);
		}
		collHash = pClass->GetNextCollection(collHash);
	}

	return out;
}

UCrc32* __thiscall LookupBehaviorSignatureHooked(PVehicle* pThis, UCrc32* result, const Attrib::StringKey* mechanic) {
	bool isCorrectDriverClass = pThis->mDriverClass == DRIVER_HUMAN;
	if (bAffectOpponents && pThis->mDriverClass == DRIVER_RACER) isCorrectDriverClass = true;
	if (isCorrectDriverClass && !GetIsVanillaEvent()) {
		if (mechanic == &BEHAVIOR_MECHANIC_ENGINE) {
			*result = __EngineRacerMW.mSignature;
			return result;
		}
		if (mechanic == &BEHAVIOR_MECHANIC_SUSPENSION) {
			*result = __SuspensionRacerMW.mSignature;
			return result;
		}
	}
	return pThis->LookupBehaviorSignature(result, mechanic);
}

uintptr_t StickyNOSASM_jmp = 0x41AD8F;
void __attribute__((naked)) __fastcall StickyNOSASM() {
	__asm__ (
		"mov edx, [esi]\n\t"
		"mov eax, [edx+8]\n\t"
		"mov ecx, %1\n\t"
		"mov [esp+0x18], cl\n\t"
		"mov ecx, esi\n\t"
		"mov [esp+0x20], ebx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (StickyNOSASM_jmp), "m" (nNOSState)
	);
}

void PreProcessCarTuning(const std::string& filename) {
	if (!filename.ends_with(".conf")) return;

	auto tuning = LoadCarTuningFromFile(filename);
	if (!tuning) {
		WriteLog(std::format("Failed to load {}", filename));
		return;
	}

	auto collections = FindCollectionAndAllChildren("vehicle", tuning->carName.c_str());
	for (auto collection : collections) {
		auto f = (float*)Attrib::Collection::GetData(collection, Attrib::StringHash32("TENSOR_SCALE"), 0);
		f[0] = tuning->TENSOR_SCALE[0];
		f[1] = tuning->TENSOR_SCALE[1];
		f[2] = tuning->TENSOR_SCALE[2];
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x16AA080 && NyaHookLib::GetEntryPoint() != 0x428C25) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.1 (.exe size of 3765248 or 28739656 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			GetCurrentDirectoryW(MAX_PATH, gDLLDir);

			NyaHooks::LateInitHook::Init();
			NyaHooks::LateInitHook::aFunctions.push_back([](){
				DLLDirSetter _setdir;

				for (const auto& entry : std::filesystem::directory_iterator("CarDataDump")) {
					if (entry.is_directory()) continue;
					PreProcessCarTuning(entry.path().filename().string());
				}
				for (const auto& entry : std::filesystem::directory_iterator("CarDataDump/orig_mw_full")) {
					if (entry.is_directory()) continue;
					PreProcessCarTuning(entry.path().filename().string());
				}
				for (const auto& entry : std::filesystem::directory_iterator("CarDataDump/orig_cb_full")) {
					if (entry.is_directory()) continue;
					PreProcessCarTuning(entry.path().filename().string());
				}
			});
			NyaHooks::WorldServiceHook::Init();
			NyaHooks::WorldServiceHook::aPreFunctions.push_back(AssistLoop);
			NyaHooks::WorldServiceHook::aPreFunctions.push_back(SpeedbreakerLoop);
			NyaHooks::WorldServiceHook::aPreFunctions.push_back(UpdatePerfectLaunchText);

			ChloeMenuLib::RegisterMenu("MW Physics Debug Menu", &DebugMenu);

			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x72BD95, &LookupBehaviorSignatureHooked);
			NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x72BD3E, &LookupBehaviorSignatureHooked);

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x41AD80, &StickyNOSASM);
			NyaHookLib::Patch<uint16_t>(0x55C9CE, 0x9090); // proper nos display

			if (std::filesystem::exists("NFSPSMWPhysics_gcp.toml")) {
				auto config = toml::parse_file("NFSPSMWPhysics_gcp.toml");
				bEnabledGrip = config["mw_physics_grip"].value_or(bEnabledGrip);
				bEnabledSpeed = config["mw_physics_speed"].value_or(bEnabledSpeed);
				bEnabledDrag = config["mw_physics_drag"].value_or(bEnabledDrag);
				bEnabledDrift = config["mw_physics_drift"].value_or(bEnabledDrift);
				bAffectOpponents = config["mw_physics_opponents"].value_or(bAffectOpponents);
				fOpponentRubberband = config["mw_opponent_rubberband"].value_or(0.0);
				bSpeedbreakerEnabled = config["speedbreaker"].value_or(bSpeedbreakerEnabled);
				bRevLimiter = config["rev_limiter"].value_or(bRevLimiter);
			}

			//NyaHooks::SkipFEFixes::Init();
			//SkipFE = true;
			//SkipFEForever = true;
			//SkipFEPlayerCar = "rx7";
			//SkipFETrackNumber = 6000;

			WriteLog("Mod initialized");
		} break;
		default:
			break;
	}
	return TRUE;
}