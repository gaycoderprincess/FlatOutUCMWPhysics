class IVehicle : public IMWInterface {
public:
	IVehicle(Car* car) : pCar(car) {}

	static inline const char* _IIDName = "IVehicle";

	Car* pCar;

	virtual const char* GetVehicleName() {
		static std::string str;
		str = std::format("car_{}", GetCarDataID_slow(pCar->pPlayer->nCarId));
		return str.c_str();
	}
	virtual int GetDriverStyle() { return STYLE_RACING; }
	virtual int GetDriverClass() { return DRIVER_HUMAN; }
	virtual float GetSpeed() {
		UMath::Vector3 vel = *pCar->GetVelocity();
		ConvertWorldToLocal(pCar, vel, false);
		return vel.z;
	}
	virtual float GetAbsoluteSpeed() { return std::abs(GetSpeed()); }
	virtual bool IsStaging() { return pGameFlow->nRaceState == RACE_STATE_COUNTDOWN; }
	virtual float GetPerfectLaunch() { return 0.0; }
	virtual bool IsDestroyed() { return pCar->nIsWrecked; }
};