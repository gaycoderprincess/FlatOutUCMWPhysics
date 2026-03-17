#include <windows.h>
#include <format>
#include <toml++/toml.hpp>

#include "nya_commonhooklib.h"
#include "nya_commonmath.h"
#include "fo2versioncheck.h"
#include "fouc.h"

#include "chloemenulib.h"

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutUCMWPhysics_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

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

#define GET_FAKE_INTERFACE(base, type, var) { auto ptr = (uintptr_t)this; ptr += offsetof(base, var); return (type*)ptr; }

#include "nya_commontimer.cpp"

bool bRevLimiter = true;
float fUpgradeLevel = 1.0;
CNyaTimer gGlobalTimer;

#include "decomp/ConversionUtil.hpp"
#include "decomp/UMathExtras.h"
#include "decomp/HelperTypes.h"

#include "MWCarTuning.h"
#include "decomp/interfaces/MWInterface.h"
#include "decomp/interfaces/MWIChassis.h"
#include "decomp/interfaces/MWIRaceEngine.h"
#include "decomp/interfaces/MWITiptronic.h"
#include "decomp/interfaces/MWIEngineDamage.h"
//#include "decomp/interfaces/MWIInductable.cpp"
#include "decomp/interfaces/MWITransmission.h"
#include "decomp/interfaces/MWIEngine.h"
#include "decomp/interfaces/MWIVehicle.cpp"
#include "decomp/interfaces/MWIPlayer.cpp"
#include "decomp/interfaces/MWICollisionBody.cpp"
#include "decomp/interfaces/MWIRigidBody.cpp"
#include "decomp/interfaces/MWIInput.cpp"
#include "decomp/interfaces/MWISpikeable.cpp"
#include "decomp/interfaces/MWICheater.cpp"
#include "decomp/interfaces/MWIHumanAI.cpp"

#include "decomp/behaviors/MWWheel.h"
#include "decomp/behaviors/MWChassisBase.h"
#include "decomp/behaviors/SuspensionRacer.h"
#include "decomp/behaviors/EngineRacer.h"
#include "decomp/behaviors/MWWheel.cpp"
#include "decomp/behaviors/MWChassisBase.cpp"
#include "decomp/behaviors/SuspensionRacer.cpp"
#include "decomp/behaviors/EngineRacer.cpp"

float fTireRadiusMult = 0.5;
float fTireYOffset = 0.03;
std::vector<EngineRacer*> aEngines;
std::vector<SuspensionRacerMW*> aSuspensions;
void __fastcall DoFO2Downforce(Car* pCar) {
	if (aEngines.empty() && aSuspensions.empty()) return;

	gGlobalTimer.fDeltaTime = 0.01;
	//gGlobalTimer.Process();

	EngineRacer* pEngine = nullptr;
	SuspensionRacerMW* pSuspension = nullptr;
	for (auto& engine : aEngines) {
		if (engine->pCar != pCar) continue;
		pEngine = engine;
	}
	for (auto& susp : aSuspensions) {
		if (susp->pCar != pCar) continue;
		pSuspension = susp;
	}
	pEngine->OnTaskSimulate(gGlobalTimer.fDeltaTime);
	pSuspension->OnTaskSimulate(gGlobalTimer.fDeltaTime);

	for (int i = 0; i < 4; i++) {
		auto tire = &pCar->aTires[i];
		//tire->GetMatrix()->p.y = pSuspension->GetWheelLocalPos(i)->y + pSuspension->GetWheelRadius(i);
		tire->GetMatrix()->p.y = pSuspension->GetWheelLocalPos(i)->y + (tire->fRadius * fTireRadiusMult) + fTireYOffset;
		float skid = 1.0 - pSuspension->GetWheelTraction(i);
		if (pSuspension->GetVehicle()->IsStaging()) skid = 0.0;
		tire->fTireSmokeX = 0; // todo
		tire->fTireSmokeZ = skid * 100;
		tire->fSkidSound1 = skid * 15; // todo this doesn't work?
		tire->fSkidSound2 = skid * 50; // todo this doesn't work?
		tire->fTurnSpeed = pSuspension->GetWheelAngularVelocity(i);
		tire->pGroundSurface = nullptr;
		if (tire->bOnGround = pSuspension->IsWheelOnGround(i)) {
			auto surface = &pEnvironment->aSurfaces[pSuspension->GetWheelRoadSurface(i)];
			tire->pGroundSurface = &pCar->TireDynamics[surface->nDynamics];
			pCar->nGroundSurfaces[i] = pSuspension->GetWheelRoadSurface(i);
			//pCar->pPlayer->nTimeInAir = 0.0;
			//pCar->pPlayer->fTimeInAirForBonus = 0.0;
		}
	}

	pGameFlow->PreRace.fNitroMultiplier = 0.0;
	pCar->fNitro = pEngine->GetNOSCapacity() * pCar->fMaxNitro;
	if (pCar == pPlayerHost->aPlayers[0]->pCar) {
		pCar->pPlayer->pIngameHUD->fRPMFraction = pEngine->GetRPM() / 10000.0;
		pCar->pPlayer->pIngameHUD->nGear = pEngine->GetGear();
		//pCar->mGearbox.nGear = pEngineRacer->GetGear();
	}
}

uintptr_t FO2SlideControlWrappedASM_jmp = 0x42AFBF;
void __attribute__((naked)) FO2DownforceASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, ebp\n\t"
		"call %1\n\t"
		"popad\n\t"
		"fld dword ptr [ebp+0x290]\n\t"
		"jmp %0\n\t"
			:
			: "m" (FO2SlideControlWrappedASM_jmp), "i" (DoFO2Downforce)
	);
}

uintptr_t NoSlideControlASM_jmp = 0x42B4AE;
void __attribute__((naked)) __fastcall NoSlideControlASM() {
	__asm__ (
		"push ebp\n\t"
		"mov ebp, esp\n\t"
		"and esp, 0xFFFFFFF8\n\t"
		"sub esp, 0x98\n\t"
		"fldz\n\t"
		"jmp %0\n\t"
			:
			: "m" (NoSlideControlASM_jmp)
	);
}

auto EngineRPMSoundHooked_orig = (void(__stdcall*)(Car*, int))nullptr;
void __stdcall EngineRPMSoundHooked(Car* a1, int a2) {
	if (auto ply = GetPlayerInterface(a1)) {
		auto engine = ply->Find<IEngine>();
		auto bak = a1->fRPM;
		a1->fRPM = engine->GetRPM();
		EngineRPMSoundHooked_orig(a1, a2);
		a1->fRPM = bak;
	}
	else {
		EngineRPMSoundHooked_orig(a1, a2);
	}
}

void ApplyMWPhysicsHooks() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B480, 0x42B494); // remove vanilla tire behavior
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42D650, 0x42EA38); // remove vanilla suspension behavior
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4E5310, 0x4E536E); // remove hud rpm updater
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4E548E, 0x4E54C0); // remove hud gear updater
	NyaHookLib::Patch<uint8_t>(0x42FD60, 0xC3); // remove rubber walls and some tire stuff
	NyaHookLib::Patch<uint8_t>(0x42AF43, 0xEB); // remove turbo and nos
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42D21C, 0x42D634); // remove tire forces

	// disable slidecontrol stuff in the fouc code
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B6B6, 0x42BCF7);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42C02C, 0x42C26D);

	// sub_42D1B0 also processes tires

	// remove vanilla tire drive forces
	//NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4554E0, 0x4556BB); // this also breaks angular velocity
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4554FE, 0x455578);

	// remove tire velocity falloff
	NyaHookLib::Fill(0x45559B, 0x90, 3);
	NyaHookLib::Fill(0x4555CA, 0x90, 2);
	NyaHookLib::Fill(0x4555D0, 0x90, 3);

	EngineRPMSoundHooked_orig = (void(__stdcall*)(Car*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480A17, &EngineRPMSoundHooked);
}

void SwitchToMWPhysics() {
	if (!aEngines.empty() || !aSuspensions.empty()) return;

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = pPlayerHost->aPlayers[i]->pCar;

		aPlayerInterfaces[i].aInterfaces.clear();
		aPlayerInterfaces[i].pCar = ply;
		aPlayerInterfaces[i].Add(new IVehicle(ply));
		aPlayerInterfaces[i].Add(new IRigidBody(ply));
		aPlayerInterfaces[i].Add(new ICollisionBody(ply));
		aPlayerInterfaces[i].Add(new IInput(ply));
		if (i == 0) {
			aPlayerInterfaces[i].Add(new IHumanAI());
		}

		auto engine = new EngineRacer(ply);
		auto susp = new SuspensionRacerMW(ply);
		engine->OnBehaviorChange();
		susp->OnBehaviorChange();
		aEngines.push_back(engine);
		aSuspensions.push_back(susp);
	}

	NyaHookLib::Patch<uint64_t>(0x42B11C, 0x86D990909090D8DD); // downforce x
	NyaHookLib::Patch<uint64_t>(0x42B132, 0x44D990909090D8DD); // downforce y
	NyaHookLib::Patch<uint64_t>(0x42B144, 0x44D990909090D8DD); // downforce z
	NyaHookLib::Patch<uint64_t>(0x42B18D, 0x44D990909090D8DD); // downforce rx
	NyaHookLib::Patch<uint64_t>(0x42B1B5, 0xCAD990909090D8DD); // downforce ry
	NyaHookLib::Patch<uint64_t>(0x42B1D3, 0x1DD890909090D8DD); // downforce rz
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42AFB9, &FO2DownforceASM);

	NyaHookLib::Patch<uint8_t>(0x42B4A0, 0xC3); // remove vanilla sliding behavior
	NyaHookLib::Fill(0x42F9CE, 0x90, 5); // remove vanilla sliding behavior
	NyaHookLib::Fill(0x480D2C, 0x90, 5); // remove vanilla sliding behavior
	NyaHookLib::Fill(0x51460E, 0x90, 5); // remove vanilla sliding behavior
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x42F9CE, &NoSlideControlASM);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x480D2C, &NoSlideControlASM);
	//NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x51460E, &NoSlideControlASM);

	// disable slidecontrol stuff in the fouc code
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42B6B6, 0x42BCF7);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42C02C, 0x42C26D);
}

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

	if (DrawMenuOption("SwitchToMWPhysics")) {
		SwitchToMWPhysics();
	}

	if (!aEngines.empty()) {
		if (DrawMenuOption("EngineRacer")) {
			ChloeMenuLib::BeginMenu();



			ChloeMenuLib::EndMenu();
		}
	}
	if (!aSuspensions.empty()) {
		auto susp = aSuspensions[0];
		if (DrawMenuOption("SuspensionRacer")) {
			ChloeMenuLib::BeginMenu();

			QuickValueEditor("Mass", pPlayerHost->aPlayers[0]->pCar->fMass);
			QuickValueEditor("fTireRadiusMult", fTireRadiusMult);
			QuickValueEditor("fTireYOffset", fTireYOffset);

			auto vGroundNormal = *aPlayerInterfaces[0].Find<ICollisionBody>()->GetGroundNormal();
			DrawMenuOption(std::format("vGroundNormal {:.2f} {:.2f} {:.2f}", vGroundNormal.x, vGroundNormal.y, vGroundNormal.z));

			auto vCenterOfMass = *(UMath::Vector3*)pPlayerHost->aPlayers[0]->pCar->vCenterOfMass;
			auto vCenterOfMassAbsolute = *(UMath::Vector3*)pPlayerHost->aPlayers[0]->pCar->vCenterOfMassAbsolute;
			DrawMenuOption(std::format("vCenterOfMass {:.2f} {:.2f} {:.2f}", vCenterOfMass.x, vCenterOfMass.y, vCenterOfMass.z));
			DrawMenuOption(std::format("vCenterOfMassAbsolute {:.2f} {:.2f} {:.2f}", vCenterOfMassAbsolute.x, vCenterOfMassAbsolute.y, vCenterOfMassAbsolute.z));
			DrawMenuOption(std::format("state.inertia {:.2f} {:.2f} {:.2f}", LastChassisState.inertia.x, LastChassisState.inertia.y, LastChassisState.inertia.z));
			DrawMenuOption(std::format("state.linear_vel {:.2f} {:.2f} {:.2f}", LastChassisState.linear_vel.x, LastChassisState.linear_vel.y, LastChassisState.linear_vel.z));
			DrawMenuOption(std::format("state.angular_vel {:.2f} {:.2f} {:.2f}", LastChassisState.angular_vel.x, LastChassisState.angular_vel.y, LastChassisState.angular_vel.z));
			DrawMenuOption(std::format("state.local_vel {:.2f} {:.2f} {:.2f}", LastChassisState.local_vel.x, LastChassisState.local_vel.y, LastChassisState.local_vel.z));
			DrawMenuOption(std::format("state.local_angular_vel {:.2f} {:.2f} {:.2f}", LastChassisState.local_angular_vel.x, LastChassisState.local_angular_vel.y, LastChassisState.local_angular_vel.z));

			DrawMenuOption(std::format("mNumWheelsOnGround {}", susp->mNumWheelsOnGround));
			DrawMenuOption(std::format("mTires[0]->mCompression {:.2f}", susp->mTires[0]->mCompression));
			DrawMenuOption(std::format("mTires[0]->mNormal {:.2f} {:.2f} {:.2f} {:.2f}", susp->mTires[0]->mNormal.x, susp->mTires[0]->mNormal.y, susp->mTires[0]->mNormal.z, susp->mTires[0]->mNormal.w));
			DrawMenuOption(std::format("mTires[0]->fYOffset {:.2f}", susp->mTires[0]->mWorldPos.fYOffset));

			ChloeMenuLib::EndMenu();
		}
	}

	ChloeMenuLib::EndMenu();
}

void MainLoop() {
	if (pLoadingScreen || pGameFlow->nGameState != GAME_STATE_RACE) {
		for (auto& susp : aEngines) {
			delete susp;
		}
		for (auto& susp : aSuspensions) {
			delete susp;
		}
		aEngines.clear();
		aSuspensions.clear();

		for (auto& ply : aPlayerInterfaces) {
			// delete all other interfaces too
			for (auto& ptr : ply.aInterfaces) {
				delete ptr.pInterface;
			}
			ply.aInterfaces.clear();
			ply.pCar = nullptr;
		}
		return;
	}

	if (aEngines.empty() && aSuspensions.empty()) {
		SwitchToMWPhysics();
	}
	//NyaHookLib::Patch<uint8_t>(0x43D69E, 0xEB); // disable auto reset
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FOUC_GFWL);

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aEndSceneFuncs.push_back(MainLoop);

			ChloeMenuLib::RegisterMenu("MW Physics Debug Menu", &DebugMenu);

			ApplyMWPhysicsHooks();

			WriteLog("Mod initialized");
		} break;
		default:
			break;
	}
	return TRUE;
}