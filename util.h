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

void WriteLog(const std::string& str) {
	static auto file = std::ofstream("FlatOutUCMWPhysics_gcp.log");

	file << str;
	file << "\n";
	file.flush();
}

int GetMWWheelID(int acWheel) {
	// mw has inverted rear tire positions?
	switch (acWheel) {
		case 0:
			return 0;
		case 1:
			return 1;
		case 2:
			return 3;
		case 3:
			return 2;
	}
}

bool IsPlayerCar(Car* car) {
	return car->pPlayer->nPlayerType == PLAYERTYPE_LOCAL;
}

float GetCarUpgradeLevel(Car* car) {
	if (pGameFlow->PreRace.nMode == GM_CAREER) {
		return fUpgradeLevel;
	}
	return pGameFlow->PreRace.fUpgradeLevel;
}

bool GetCarJunkman(Car* car) {
	if (pGameFlow->PreRace.nMode == GM_CAREER) {
		return bUpgradeJunkman;
	}
	return pGameFlow->PreRace.fUpgradeLevel > 1.0;
}