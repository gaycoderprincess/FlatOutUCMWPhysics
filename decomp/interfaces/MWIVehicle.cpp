class IVehicle : public IMWInterface {
public:
	IVehicle(Car* car) : pCar(car) {}

	static inline const char* _IIDName = "IVehicle";

	Car* pCar;

	virtual const char* GetVehicleName() {
		static std::string str;
		auto carId = GetCarDataID_slow(pCar->pPlayer->nCarId);
		if (carId >= 200 && carId < 300) carId -= 200; // fo2 cars
		str = std::format("car_{}", carId);
		return str.c_str();
	}
	virtual int GetDriverStyle() { return STYLE_RACING; }
	virtual int GetDriverClass() { return DRIVER_HUMAN; }
	virtual float GetSpeed() { // this is so weird and useless
		UMath::Vector3 vel = *pCar->GetVelocity();
		ConvertWorldToLocal(pCar, vel, false);
		return vel.z >= 0.0 ? GetAbsoluteSpeed() : -GetAbsoluteSpeed();
	}
	virtual float GetAbsoluteSpeed() { return pCar->GetVelocity()->length(); }
	virtual bool IsStaging() { return pGameFlow->nRaceState == RACE_STATE_COUNTDOWN; }
	virtual float GetPerfectLaunch() { return 0.0; }
	virtual bool IsDestroyed() { return pCar->nIsWrecked; }
};