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

bool bRevLimiter = true;
CNyaRaceTimer gGlobalTimer;

#include "decomp/ConversionUtil.hpp"
#include "decomp/UMathExtras.h"
#include "decomp/HelperTypes.h"
#include "decomp/interfaces/MWInterface.h"
#include "decomp/interfaces/MWIChassis.cpp"
#include "decomp/interfaces/MWIRaceEngine.cpp"
#include "decomp/interfaces/MWITiptronic.cpp"
#include "decomp/interfaces/MWIEngineDamage.cpp"
//#include "decomp/interfaces/MWIInductable.cpp"
#include "decomp/interfaces/MWITransmission.cpp"
#include "decomp/interfaces/MWIEngine.cpp"
#include "decomp/interfaces/MWIVehicle.cpp"
#include "decomp/interfaces/MWICollisionBody.cpp"
#include "decomp/interfaces/MWIRigidBody.cpp"
#include "decomp/interfaces/MWIInput.cpp"
#include "decomp/interfaces/MWIHumanAI.cpp"

#include "MWCarTuning.h"

#include "decomp/behaviors/MWWheel.h"
#include "decomp/behaviors/MWChassisBase.h"
#include "decomp/behaviors/SuspensionRacer.h"
#include "decomp/behaviors/EngineRacer.h"
#include "decomp/behaviors/MWWheel.cpp"
#include "decomp/behaviors/MWChassisBase.cpp"
#include "decomp/behaviors/SuspensionRacer.cpp"
#include "decomp/behaviors/EngineRacer.cpp"

EngineRacer* pEngineRacer = nullptr;
SuspensionRacerMW* pSuspensionRacer = nullptr;
void SwitchToMWPhysics() {
	if (pEngineRacer || pSuspensionRacer) return;

	auto ply = pPlayerHost->aPlayers[0]->pCar;

	gPlayerInterfaces.Add(new IVehicle(ply));
	gPlayerInterfaces.Add(new IRigidBody(ply));
	gPlayerInterfaces.Add(new ICollisionBody(ply));
	gPlayerInterfaces.Add(new IInput(ply));
	gPlayerInterfaces.Add(new IHumanAI());

	pEngineRacer = new EngineRacer(ply);
	pSuspensionRacer = new SuspensionRacerMW(ply);
	pEngineRacer->OnBehaviorChange();
	pSuspensionRacer->OnBehaviorChange();
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

	if (pEngineRacer) {
		if (DrawMenuOption("EngineRacer")) {
			ChloeMenuLib::BeginMenu();



			ChloeMenuLib::EndMenu();
		}
	}
	if (pSuspensionRacer) {
		if (DrawMenuOption("SuspensionRacer")) {
			ChloeMenuLib::BeginMenu();

			auto vGroundNormal = *gPlayerInterfaces.Find<ICollisionBody>()->GetGroundNormal();
			DrawMenuOption(std::format("vGroundNormal {:.2f} {:.2f} {:.2f}", vGroundNormal.x, vGroundNormal.y, vGroundNormal.z));

			ChloeMenuLib::EndMenu();
		}
	}

	ChloeMenuLib::EndMenu();
}

void __fastcall DoFO2Downforce(Car* pCar) {
	if (!pEngineRacer || !pSuspensionRacer) return;

	gGlobalTimer.Process();
	pEngineRacer->OnTaskSimulate(gGlobalTimer.fDeltaTime);
	pSuspensionRacer->OnTaskSimulate(gGlobalTimer.fDeltaTime);
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

void MainLoop() {
	NyaHookLib::Patch<uint64_t>(0x42B11C, 0x86D990909090D8DD); // downforce x
	NyaHookLib::Patch<uint64_t>(0x42B132, 0x44D990909090D8DD); // downforce y
	NyaHookLib::Patch<uint64_t>(0x42B144, 0x44D990909090D8DD); // downforce z
	NyaHookLib::Patch<uint64_t>(0x42B18D, 0x44D990909090D8DD); // downforce rx
	NyaHookLib::Patch<uint64_t>(0x42B1B5, 0xCAD990909090D8DD); // downforce ry
	NyaHookLib::Patch<uint64_t>(0x42B1D3, 0x1DD890909090D8DD); // downforce rz
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x42AFB9, &FO2DownforceASM);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			DoFlatOutVersionCheck(FO2Version::FOUC_GFWL);

			NyaFO2Hooks::PlaceD3DHooks();
			NyaFO2Hooks::aEndSceneFuncs.push_back(MainLoop);

			ChloeMenuLib::RegisterMenu("MW Physics Debug Menu", &DebugMenu);

			WriteLog("Mod initialized");
		} break;
		default:
			break;
	}
	return TRUE;
}