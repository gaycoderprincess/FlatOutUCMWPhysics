#include <windows.h>
#include <format>
#include <toml++/toml.hpp>

#include "nya_dx9_hookbase.h"
#include "nya_commonhooklib.h"
#include "nya_commonmath.h"
#include "fo2versioncheck.h"
#include "fouc.h"

#include "chloemenulib.h"

#include "d3dhook.h"

bool bRevLimiter = true;
bool bSpeedbreakerEnabled = true;
bool bMWWheelPositions = false;
bool bMWNitrous = false;
float fUpgradeLevel = 1.0;
bool bUpgradeJunkman = false;
float fTireYPhysOffset = 0.0;
CNyaTimer gGlobalTimer;

float fOverrideTimescale = 1.0;
void OverrideTimescale(float f) {
	NyaHookLib::Patch<int>(0x5A6071, f * 1000);
}

#include "util.h"

#include "decomp/ConversionUtil.hpp"
#include "decomp/UMathExtras.h"
#include "decomp/HelperTypes.h"

#include "MWCarTuning.h"

#include "decomp/Physics.h"
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

float fPlayerOriginalFudge = 1.0f;
float fPlayerOriginalGlobalFudge = 0.0f;
float fPlayerOriginalGlobalFudgeArcadeRace = 0.0f;
float fPlayerOriginalGlobalFudgeFragDerby = 0.0f;
float Tweak_GameBreakerCollisionMass = 2.0;
void DoGameBreaker(float real_time_delta, IPlayer* player) {
	player->DoGameBreaker(real_time_delta * fOverrideTimescale, real_time_delta);

	// set player mass fudge factor
	static float fFudge = 1.0;
	if (player->InGameBreaker()) {
		fFudge = fPlayerOriginalFudge / Tweak_GameBreakerCollisionMass;
		*(float*)0x849434 = fPlayerOriginalGlobalFudge * Tweak_GameBreakerCollisionMass;
		*(float*)0x849454 = fPlayerOriginalGlobalFudgeArcadeRace * Tweak_GameBreakerCollisionMass;
		*(float*)0x84945C = fPlayerOriginalGlobalFudgeFragDerby * Tweak_GameBreakerCollisionMass;
	}
	else {
		fFudge = fPlayerOriginalFudge;
		*(float*)0x849434 = fPlayerOriginalGlobalFudge;
		*(float*)0x849454 = fPlayerOriginalGlobalFudgeArcadeRace;
		*(float*)0x84945C = fPlayerOriginalGlobalFudgeFragDerby;
	}
	NyaHookLib::Patch<uint16_t>(0x480ABD, 0x9090);
	NyaHookLib::Patch<uint8_t>(0x480AC5, 0xEB);
	NyaHookLib::Patch(0x480ACF + 2, &fFudge);

	float speed = 1.0;
	float target = 1.0;
	if (player->InGameBreaker()) {
		target = 0.25;
		speed = 2.0;
	}
	else {
		target = 1.0;
		speed = 0.5;
	}

	if (target == fOverrideTimescale) return;

	auto modify = speed * real_time_delta;
	if (fOverrideTimescale < target) {
		fOverrideTimescale += modify;
		if (fOverrideTimescale > target) fOverrideTimescale = target;
	}
	else if (fOverrideTimescale > target) {
		fOverrideTimescale -= modify;
		if (fOverrideTimescale > target) fOverrideTimescale = target;
	}
}

void DoReversing(Car* pCar) {
	if (pGameFlow->nRaceState <= RACE_STATE_COUNTDOWN) return;

	auto transmission = GetPlayerInterface(pCar)->Find<ITransmission>();

	bool gameReversing = pCar->mGearbox.nGear == -1;
	if (transmission->IsReversing() && !gameReversing) {
		transmission->Shift(G_FIRST);
	} else {
		if (!transmission->IsReversing() && gameReversing) {
			transmission->Shift(G_REVERSE);
		}
	}
}

void DoShifting(Car* pCar) {
	auto iinput = GetPlayerInterface(pCar)->Find<IInput>();
	auto transmission = GetPlayerInterface(pCar)->Find<ITransmission>();
	auto tiptronic = GetPlayerInterface(pCar)->Find<ITiptronic>();
	if (!GetPlayerInterface(pCar)->Find<IHumanAI>()) return; // ai uses automatic shift

	auto currentUp = pCar->pPlayer->pController->GetInputValue(INPUT_GEAR_UP);
	auto currentDown = pCar->pPlayer->pController->GetInputValue(INPUT_GEAR_DOWN);
	if (pCar->pPlayer->pController->_4[-1] == 0x6F403C) { // xinput controller vtable
		currentDown = IsPadKeyPressed(NYA_PAD_KEY_LB);
		currentUp = IsPadKeyPressed(NYA_PAD_KEY_RB);
	}

	static bool bLastUp = false;
	if (currentUp && !bLastUp && transmission->GetGear() != transmission->GetTopGear()) {
		auto nextGear = transmission->GetGear()+1;
		if (iinput->IsAutomaticShift()) {
			if (nextGear == G_NEUTRAL) nextGear = G_FIRST;
			if (nextGear == G_FIRST) {
				transmission->Shift((GearID)nextGear);
			}
			else {
				tiptronic->SportShift((GearID)nextGear);
			}
		}
		else {
			transmission->Shift((GearID)nextGear);
		}
	}
	bLastUp = currentUp;

	static bool bLastDown = false;
	if (currentDown && !bLastDown && transmission->GetGear() != G_REVERSE) {
		auto nextGear = transmission->GetGear()-1;
		if (iinput->IsAutomaticShift()) {
			if (nextGear == G_NEUTRAL) nextGear = G_REVERSE;
			if (nextGear == G_REVERSE) {
				transmission->Shift((GearID)nextGear);
			}
			else {
				tiptronic->SportShift((GearID)nextGear);
			}
		}
		else {
			transmission->Shift((GearID)nextGear);
		}
	}
	bLastDown = currentDown;
}

auto GetPlayerStartPosition(Car* pCar) {
	return &pEnvironment->aStartpoints[((pCar->pPlayer->nStartPosition-1)%pEnvironment->nNumStartpoints)];
}

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
	if (!pEngine || !pSuspension) return;

	if (pCar->pPlayer->nAutoTransmission) {
		DoReversing(pCar);
	}
	DoShifting(pCar);
	OverrideTimescale(fOverrideTimescale);

	auto ivehicle = pEngine->GetVehicle();

	// hack to fix start positions
	if (ivehicle->IsStaging()) {
		auto start = GetPlayerStartPosition(pCar);
		pCar->GetMatrix()->p.x = start->fPosition[0];
		pCar->GetMatrix()->p.z = start->fPosition[2];

		pCar->GetVelocity()->x = 0.0;
		pCar->GetVelocity()->z = 0.0;
	}

	ivehicle->OnTaskSimulate(gGlobalTimer.fDeltaTime);

	if (!bMWNitrous) {
		pEngine->mNOSCapacity = pCar->fNitro / pCar->fMaxNitro;
	}

	pEngine->OnTaskSimulate(gGlobalTimer.fDeltaTime);
	pSuspension->OnTaskSimulate(gGlobalTimer.fDeltaTime);

	for (int i = 0; i < 4; i++) {
		int mwTireId = GetMWWheelID(i);
		auto tire = &pCar->aTires[i];
		tire->GetMatrix()->p = *pSuspension->GetWheelLocalPos(mwTireId);
		tire->GetMatrix()->p.y += (tire->fRadius * 0.5) + (tire->fRadius * 0.1);
		float skid = 1.0 - pSuspension->GetWheelTraction(mwTireId);
		if (ivehicle->IsStaging()) skid = 0.0;
		tire->fTireSmokeX = pSuspension->mTires[mwTireId]->GetLateralSpeed();
		tire->fTireSmokeZ = skid * 100;
		tire->fSkidSound1 = skid * 15; // todo this doesn't work?
		tire->fSkidSound2 = skid * 50; // todo this doesn't work?
		tire->fTurnSpeed = pSuspension->GetWheelAngularVelocity(mwTireId);
		tire->pGroundSurface = nullptr;
		if (tire->bOnGround = pSuspension->IsWheelOnGround(mwTireId)) {
			auto surface = &pEnvironment->aSurfaces[pSuspension->GetWheelRoadSurface(mwTireId)];
			tire->pGroundSurface = &pCar->TireDynamics[surface->nDynamics];
			pCar->nGroundSurfaces[i] = pSuspension->GetWheelRoadSurface(mwTireId);
			//pCar->pPlayer->nTimeInAir = 0.0;
			//pCar->pPlayer->fTimeInAirForBonus = 0.0;
		}
	}

	if (bMWNitrous) {
		pGameFlow->PreRace.fNitroMultiplier = 0.0;
		pCar->fNitro = pEngine->GetNOSCapacity() * pCar->fMaxNitro;
	}
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

	if (**(float**)(0x480ACF + 2) != fPlayerOriginalFudge) {
		fPlayerOriginalFudge = **(float**)(0x480ACF + 2);
	}
	if (fPlayerOriginalGlobalFudge == 0.0f) {
		fPlayerOriginalGlobalFudge = *(float*)0x849434;
		fPlayerOriginalGlobalFudgeArcadeRace = *(float*)0x849454;
		fPlayerOriginalGlobalFudgeFragDerby = *(float*)0x84945C;
	}

	for (int i = 0; i < pPlayerHost->GetNumPlayers(); i++) {
		auto ply = pPlayerHost->aPlayers[i]->pCar;

		auto start = GetPlayerStartPosition(ply);
		*ply->GetMatrix() = *(NyaMat4x4*)start->fMatrix;
		ply->GetMatrix()->p = *(NyaVec3*)start->fPosition;
		ply->GetMatrix()->p.y += 0.33;

		aPlayerInterfaces[i].aInterfaces.clear();
		aPlayerInterfaces[i].pCar = ply;
		aPlayerInterfaces[i].Add(new IVehicle(ply));
		aPlayerInterfaces[i].Add(new IRigidBody(ply));
		aPlayerInterfaces[i].Add(new ICollisionBody(ply));
		aPlayerInterfaces[i].Add(new IInput(ply));
		if (i == 0) {
			aPlayerInterfaces[i].Add(new IPlayer(ply));
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

	DrawMenuOption(std::format("Race State {}", pGameFlow->nRaceState));

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
			DrawMenuOption(std::format("nGear {}", pPlayerHost->aPlayers[0]->pCar->mGearbox.nGear));
			//QuickValueEditor("fTireYVisOffset", fTireYVisOffset);

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
	CNyaTimer gRealTimer;
	gRealTimer.Process();

	if (pLoadingScreen || pGameFlow->nGameState != GAME_STATE_RACE) {
		fOverrideTimescale = 1.0;
		OverrideTimescale(1.0);

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

	if (bSpeedbreakerEnabled) {
		if (pGameFlow->nIsPauseMenuUp) {
			OverrideTimescale(1.0);
		}
		else {
			if (auto pPlayer = GetPlayerInterface(pPlayerHost->aPlayers[0]->pCar)->GetPlayer()) {
				if (IsKeyJustPressed('X') || IsPadKeyJustPressed(NYA_PAD_KEY_R3, -1)) {
					pPlayer->ToggleGameBreaker();
				}
				DoGameBreaker(gRealTimer.fDeltaTime, pPlayer);
			}
			OverrideTimescale(fOverrideTimescale);
		}
	}

	auto ivehicle = aEngines[0]->GetVehicle();

	static double speedoAlpha = 255.0;
	speedoAlpha = ivehicle->IsStaging() && ivehicle->IsInPerfectLaunchRange() ? 127.0 : 255.0;
	NyaHookLib::Patch(0x4D9E93 + 2, &speedoAlpha);

	float perfectLaunchPopupTimer = ivehicle->mPerfectLaunch.Time - 3.0; // 1.5 seconds
	if (perfectLaunchPopupTimer > 0 && !pGameFlow->nIsPauseMenuUp) {
		tNyaStringData data;
		data.x = 0.5;
		data.y = 0.3;
		data.size = 0.04;
		data.XCenterAlign = true;
		data.outlinea = 255;
		if (perfectLaunchPopupTimer < 0.5) {
			data.a = data.outlinea = perfectLaunchPopupTimer * 2 * 255;
		}
		DrawString(data, "PERFECT LAUNCH!", &DrawStringFO2);
	}
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FOUC_GFWL);

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
			NyaFO2Hooks::aEndSceneFuncs.push_back(D3DHookMain);

			ChloeMenuLib::RegisterMenu("MW Physics Debug Menu", &DebugMenu);

			if (std::filesystem::exists("FlatOutUCMWPhysics_gcp.toml")) {
				auto config = toml::parse_file("FlatOutUCMWPhysics_gcp.toml");
				bSpeedbreakerEnabled = config["speedbreaker"].value_or(bSpeedbreakerEnabled);
				bRevLimiter = config["rev_limiter"].value_or(bRevLimiter);
				fUpgradeLevel = config["upgrade_level"].value_or(fUpgradeLevel);
				bUpgradeJunkman = config["upgrade_junkman"].value_or(bUpgradeJunkman);
				fTireYPhysOffset = config["tire_y_offset"].value_or(fTireYPhysOffset);
				bMWNitrous = config["mw_nitrous"].value_or(bMWNitrous);
				bMWWheelPositions = config["mw_wheel_positions"].value_or(bMWWheelPositions);
			}

			ApplyMWPhysicsHooks();

			WriteLog("Mod initialized");
		} break;
		default:
			break;
	}
	return TRUE;
}