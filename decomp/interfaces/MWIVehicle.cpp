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
	virtual float GetSpeedometer() { return mCOMObject->Find<ITransmission>()->GetSpeedometer(); }
	virtual float GetSpeed() { // this is so weird and useless
		UMath::Vector3 vel = *pCar->GetVelocity();
		ConvertWorldToLocal(pCar, vel, false);
		return vel.z >= 0.0 ? GetAbsoluteSpeed() : -GetAbsoluteSpeed();
	}
	virtual float GetAbsoluteSpeed() { return pCar->GetVelocity()->length(); }
	virtual bool IsStaging() { return pGameFlow->nRaceState <= RACE_STATE_COUNTDOWN; }
	virtual float GetPerfectLaunch() { return 0.0; }
	virtual bool IsDestroyed() { return pCar->nIsWrecked; }

	struct LaunchState {
		float Time = 0;
		float Amount = 0;
	} mPerfectLaunch;

	virtual void DoStaging() {
		if (mPerfectLaunch.Time > 0.0) return;

		mPerfectLaunch.Amount = 0.0;

		auto engine = mCOMObject->Find<IEngine>();
		if (!engine) return;
		auto raceEngine = mCOMObject->Find<IRaceEngine>();
		if (!raceEngine) return;

		float launchRange = 0.0;
		auto v6 = raceEngine->GetPerfectLaunchRange(&launchRange);
		if (launchRange > 0.0 && v6 > 0.0) {
			auto v8 = engine->GetRPM();
			auto v9 = v8 - v6;
			if (v9 < launchRange && v9 > 0.0) {
				mPerfectLaunch.Amount = (((v8 - v6) / launchRange) + 1.0) * 0.5;
			}
		}
	}

	virtual void Launch() {
		auto engine = mCOMObject->Find<IEngine>();
		if (!engine) return;

		if (mPerfectLaunch.Time > 0.0) return;

		if (GetDriverClass() != DRIVER_HUMAN) {
			mPerfectLaunch.Amount = 0.0;
			mPerfectLaunch.Time = 0.0;
		}
		else if (mPerfectLaunch.Amount > 0.0) {
			mPerfectLaunch.Time = 4.5;
		}
	}

	virtual void OnTaskSimulate(float dT) {
		static bool bLastStaging = false;
		if (bLastStaging && !IsStaging()) {
			Launch();
		}
		bLastStaging = IsStaging();

		if (IsStaging()) {
			DoStaging();
		}
		else if (mPerfectLaunch.Time > 0.0) {
			if (auto transmission = mCOMObject->Find<ITransmission>()) {
				if (!transmission->IsGearChanging()) {
					mPerfectLaunch.Time = mPerfectLaunch.Time - dT;
					if (mPerfectLaunch.Time <= 0.0) {
						mPerfectLaunch.Amount = 0.0;
						mPerfectLaunch.Time = 0.0;
					}
				}
				if (GetSpeed() > 26.8218) {
					mPerfectLaunch.Time = 0.0;
					mPerfectLaunch.Amount = 0.0;
				}
			}
		}
	}
};