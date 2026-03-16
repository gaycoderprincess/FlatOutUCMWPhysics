namespace MWIChassis {
	// this is horrible but vtables don't work with the way i have this set up
	ChassisMW* GetChassis(uintptr_t ptr) {
		ptr -= offsetof(ChassisMW, tmpChassis);
		return (ChassisMW*)ptr;
	}

	bool IsChassisSimple(uintptr_t ptr) {
		return !strcmp(GetChassis(ptr)->mChassisType, "SuspensionSimple");
	}

	SuspensionSimpleMW* GetSuspensionSimple(uintptr_t ptr) {
		if (strcmp(GetChassis(ptr)->mChassisType, "SuspensionSimple")) {
			MessageBoxA(nullptr, std::format("GetSuspensionSimple() wrong type! Called from {:X}", (uintptr_t)__builtin_return_address(0)).c_str(), "nya?!~", MB_ICONERROR);
			__debugbreak();
		}
		static_assert(offsetof(SuspensionSimpleMW, tmpChassis) == offsetof(ChassisMW, tmpChassis));
		ptr -= offsetof(SuspensionSimpleMW, tmpChassis);
		return (SuspensionSimpleMW*)ptr;
	}

	SuspensionRacerMW* GetSuspensionRacer(uintptr_t ptr) {
		if (strcmp(GetChassis(ptr)->mChassisType, "SuspensionRacer")) {
			MessageBoxA(nullptr, std::format("GetSuspensionRacer() wrong type! Called from {:X}", (uintptr_t)__builtin_return_address(0)).c_str(), "nya?!~", MB_ICONERROR);
			__debugbreak();
		}
		static_assert(offsetof(SuspensionRacerMW, tmpChassis) == offsetof(ChassisMW, tmpChassis));
		ptr -= offsetof(SuspensionRacerMW, tmpChassis);
		return (SuspensionRacerMW*)ptr;
	}

	float __thiscall GetWheelTraction(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelDynamicSlipAngle(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelSlipRatio(uintptr_t ptr, unsigned int);
	float __thiscall GetDragBoost(uintptr_t ptr);
	unsigned int __thiscall GetNumWheels(uintptr_t ptr);
	const UMath::Vector3 *__thiscall GetWheelPos(uintptr_t ptr, unsigned int);
	const UMath::Vector3 *__thiscall GetWheelLocalPos(uintptr_t ptr, unsigned int);
	UMath::Vector3 *__thiscall GetWheelCenterPos(uintptr_t ptr, UMath::Vector3 *result, unsigned int);
	float __thiscall GetWheelTorque(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelBrakeTorque(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelLoad(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelRoadHeight(uintptr_t ptr, unsigned int);
	bool __thiscall IsWheelOnGround(uintptr_t ptr, unsigned int);
	float __thiscall GetCompression(uintptr_t ptr, unsigned int);
	float __thiscall GuessCompression(uintptr_t ptr, unsigned int, float);
	void __thiscall ForceCompression(uintptr_t ptr, unsigned int, float);
	float __thiscall GetWheelSlip(uintptr_t ptr, unsigned int);
	float __thiscall GetToleratedSlip(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelSkid(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelSlipAngle(uintptr_t ptr, unsigned int);
	const UMath::Vector4 *__thiscall GetWheelRoadNormal(uintptr_t ptr, unsigned int);
	const SimSurface *__thiscall GetWheelRoadSurface(uintptr_t ptr, unsigned int);
	const UMath::Vector3 *__thiscall GetWheelVelocity(uintptr_t ptr, unsigned int);
	int __thiscall GetNumWheelsOnGround(uintptr_t ptr);
	unsigned int __thiscall GetNumWheelsOnBand(uintptr_t ptr);
	float __thiscall GetWheelAngularVelocity(uintptr_t ptr, int);
	void __thiscall SetWheelAngularVelocity(uintptr_t ptr, int, float);
	void __thiscall SetWheelMaxAngularVelocity(uintptr_t ptr, int, float);
	float __thiscall GetWheelSteer(uintptr_t ptr, unsigned int);
	bool __thiscall CalculateFFBSteeringState(uintptr_t ptr, FFBSteeringState *);
	float __thiscall GetSuspensionDigression(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelLateralForce(uintptr_t ptr, unsigned int);
	float __thiscall CalculateUndersteerFactor(uintptr_t ptr);
	float __thiscall CalculateOversteerFactor(uintptr_t ptr);
	float __thiscall GetRideHeight(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelRadius(uintptr_t ptr, unsigned int);
	float __thiscall GetMaxSteering(uintptr_t ptr);
	void __thiscall MatchSpeed(uintptr_t ptr, float);
	float __thiscall GetDriveshaftTorqueEffect(uintptr_t ptr);
	float __thiscall GetRenderMotion(uintptr_t ptr);
	ISteeringWheel::SteeringType __thiscall GetSteeringType(uintptr_t ptr);
	float __thiscall GetWheelTorqueRatio(uintptr_t ptr, unsigned int);
	float __thiscall GetWheelIdealTorque(uintptr_t ptr, unsigned int);
	void __thiscall SetWheelRemoved(uintptr_t ptr, unsigned int, bool);
	void __thiscall SetWheelMaximumTorqueRatio(uintptr_t ptr, unsigned int, float);
	bool __thiscall IsAntiBrakeLockOn(uintptr_t ptr);
	int __thiscall GetAntiBrakeLockLevel(uintptr_t ptr);
	void __thiscall SetAntiBrakeLockLevel(uintptr_t ptr, int);
	bool __thiscall IsStabilityManagementOn(uintptr_t ptr);
	int __thiscall GetStabilityManagementLevel(uintptr_t ptr);
	void __thiscall SetStabilityManagementLevel(uintptr_t ptr, int);
	bool __thiscall IsDriftAsssistOn(uintptr_t ptr);
	int __thiscall GetDriftAssistLevel(uintptr_t ptr);
	void __thiscall SetDriftAssistLevel(uintptr_t ptr, int);
	bool __thiscall IsDriftGlueToRoadOn(uintptr_t ptr);
	int __thiscall GetDriftGlueToRoadLevel(uintptr_t ptr);
	void __thiscall SetDriftGlueToRoadLevel(uintptr_t ptr, int);
	bool __thiscall IsDriftDynamicBrakeOn(uintptr_t ptr);
	int __thiscall GetDriftDynamicBrakeLevel(uintptr_t ptr);
	void __thiscall SetDriftDynamicBrakeLevel(uintptr_t ptr, int);
	bool __thiscall IsDriftSpeedControlOn(uintptr_t ptr);
	int __thiscall GetDriftSpeedControlLevel(uintptr_t ptr);
	void __thiscall SetDriftSpeedControlLevel(uintptr_t ptr, int);
	bool __thiscall IsRacelineAssistOn(uintptr_t ptr);
	int __thiscall GetRacelineAssistLevel(uintptr_t ptr);
	void __thiscall SetRacelineAssistLevel(uintptr_t ptr, int);
	bool __thiscall IsBrakingAssistOn(uintptr_t ptr);
	int __thiscall GetBrakingAssistLevel(uintptr_t ptr);
	void __thiscall SetBrakingAssistLevel(uintptr_t ptr, int);
	float __thiscall GetWheelEdgeOfControlSlipAngleLow(uintptr_t ptr, int);
	float __thiscall GetWheelEdgeOfControlSlipAngleHigh(uintptr_t ptr, int);
	float __thiscall GetWheelToleratedSlipRatio(uintptr_t ptr, int);
	float __thiscall GetWheelPeakLongForceSlipRatio(uintptr_t ptr, int);
	float __thiscall GetDragCoefficient(uintptr_t ptr);
	float __thiscall GetDownCoefficient(uintptr_t ptr);
	float __thiscall GetStaticGripForSpeed(uintptr_t ptr, float);
	void __thiscall SetClutchKickExtraTireSpin(uintptr_t ptr, float);
	float __thiscall GetHydraulicsCompression(uintptr_t ptr, unsigned int);
	void __thiscall ChangeSlot(uintptr_t ptr, bool);
	bool __thiscall IsSlotCar(uintptr_t ptr);
	float __thiscall GetWheelieAngle(uintptr_t ptr);
	bool __thiscall IsStaticResetCondition(uintptr_t ptr);
	void __thiscall SetWindCondition(uintptr_t ptr, float, float, float);
	void __thiscall SetAICatchOverride(uintptr_t ptr, bool);

	float __thiscall GetWheelTraction(uintptr_t ptr, unsigned int index) {
		ICHASSIS_FUNCTION_LOG("GetWheelTraction");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[index]->GetTraction();
	}
	float __thiscall GetWheelDynamicSlipAngle(uintptr_t ptr, unsigned int index) { // todo what is this
		ICHASSIS_FUNCTION_LOG("GetWheelDynamicSlipAngle");
		auto pThis = GetSuspensionRacer(ptr);
		//return pThis->mTires[index]->GetSlipAngle();
		return 1.0 - pThis->mTires[index]->GetTraction();
	}
	float __thiscall GetWheelSlipRatio(uintptr_t ptr, unsigned int index) { // todo this is weird
		ICHASSIS_FUNCTION_LOG("GetWheelSlipRatio");
		auto pThis = GetSuspensionRacer(ptr);
		return 1.0 - pThis->mTires[index]->GetTraction();
		//return pThis->mTires[index]->GetCurrentSlip();
	}
	float __thiscall GetWheelToleratedSlipRatio(uintptr_t ptr, int index) {
		ICHASSIS_FUNCTION_LOG("GetWheelToleratedSlipRatio");
		auto pThis = GetSuspensionRacer(ptr);
		//return pThis->mTires[index]->GetToleratedSlip();
		return 1.0;
	}
	float __thiscall GetWheelPeakLongForceSlipRatio(uintptr_t ptr, int index) { // ??????
		ICHASSIS_FUNCTION_LOG("GetWheelPeakLongForceSlipRatio");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[index]->GetToleratedSlip();
	}
	float __thiscall GetDragBoost(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetDragBoost");
		return 0.0;
	}
	unsigned int __thiscall GetNumWheels(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetNumWheels");
		return 4;
	}
	const UMath::Vector3 *__thiscall GetWheelPos(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelPos");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelPos(i);
	}
	const UMath::Vector3 *__thiscall GetWheelLocalPos(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelLocalPos");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelLocalPos(i);
	}
	UMath::Vector3 *__thiscall GetWheelCenterPos(uintptr_t ptr, UMath::Vector3 *result, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelCenterPos");
		auto pThis = GetSuspensionRacer(ptr);
		*result = pThis->mTires[i]->GetPosition();
		if (!pThis->mRB) {
			return result;
		} else {
			UMath::Vector3 tmp;
			pThis->mRB->GetUpVector(&tmp);
			UMath::ScaleAdd(tmp, pThis->mTires[i]->GetRadius(), *result, *result);
			return result;
		}
	}
	float __thiscall GetWheelTorque(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelTorque");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[i]->GetTotalTorque();
	}
	float __thiscall GetWheelBrakeTorque(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelBrakeTorque");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[i]->mBrakeTorque;
	}
	float __thiscall GetWheelLoad(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelLoad");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[i]->GetLoad();
	}
	float __thiscall GetWheelRoadHeight(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelRoadHeight");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelRoadHeight(i);
	}
	bool __thiscall IsWheelOnGround(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("IsWheelOnGround");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->IsWheelOnGround(i);
	}
	float __thiscall GetCompression(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetCompression");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetCompression(i);
	}
	float __thiscall GuessCompression(uintptr_t ptr, unsigned int id, float downforce) {
		ICHASSIS_FUNCTION_LOG("GuessCompression");
		auto pThis = GetSuspensionRacer(ptr);
		float compression = 0.0f;
		if (downforce < 0.0f) {
			unsigned int axle = id / 2;
			float spring_weight = LBIN2NM(pThis->mMWAttributes->SPRING_STIFFNESS.At(axle));
			downforce *= 0.25f;
			compression = -downforce / spring_weight;
		}
		return compression;
	}
	void __thiscall ForceCompression(uintptr_t ptr, unsigned int id, float f) { // todo does this break stuff due to uc using it
		ICHASSIS_FUNCTION_LOG("ForceCompression");
		auto pThis = GetSuspensionRacer(ptr);
		pThis->mTires[id]->SetCompression(f);
	}
	float __thiscall GetWheelSlip(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelSlip");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[id]->GetCurrentSlip();
	}
	float __thiscall GetToleratedSlip(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetToleratedSlip");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[id]->GetToleratedSlip();
	}
	float __thiscall GetWheelSkid(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelSkid");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[id]->GetLateralSpeed();
	}
	float __thiscall GetWheelSlipAngle(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelSlipAngle");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelSlipAngle(id);
	}
	float __thiscall GetWheelEdgeOfControlSlipAngleLow(uintptr_t ptr, int id) { // ????
		ICHASSIS_FUNCTION_LOG("GetWheelEdgeOfControlSlipAngleLow");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelSlipAngle(id);
	}
	float __thiscall GetWheelEdgeOfControlSlipAngleHigh(uintptr_t ptr, int id) { // ????
		ICHASSIS_FUNCTION_LOG("GetWheelEdgeOfControlSlipAngleHigh");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelSlipAngle(id);
	}
	const UMath::Vector4* __thiscall GetWheelRoadNormal(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelRoadNormal");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelRoadNormal(id);
	}
	const SimSurface* __thiscall GetWheelRoadSurface(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelRoadSurface");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelRoadSurface(id);
	}
	const UMath::Vector3* __thiscall GetWheelVelocity(uintptr_t ptr, unsigned int id) {
		ICHASSIS_FUNCTION_LOG("GetWheelVelocity");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelVelocity(id);
	}
	int __thiscall GetNumWheelsOnGround(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetNumWheelsOnGround");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetNumWheelsOnGround();
	}
	unsigned int __thiscall GetNumWheelsOnBand(uintptr_t ptr) { // todo what is this?
		ICHASSIS_FUNCTION_LOG("GetNumWheelsOnBand");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mNumWheelsOnGround;
	}
	float __thiscall GetWheelAngularVelocity(uintptr_t ptr, int index) {
		ICHASSIS_FUNCTION_LOG("GetWheelAngularVelocity");
		auto pThis = GetSuspensionRacer(ptr);
		auto tire = pThis->mTires[index];
		if (tire->IsBrakeLocked()) {
			return 0.0f;
		}
		if (!tire->IsOnGround() || !tire->IsSlipping()) {
			return tire->GetAngularVelocity();
		}
		return tire->GetRoadSpeed() / tire->GetRadius();
	}
	void __thiscall SetWheelAngularVelocity(uintptr_t ptr, int i, float f) {
		ICHASSIS_FUNCTION_LOG("SetWheelAngularVelocity");
		auto pThis = GetSuspensionRacer(ptr);
		pThis->mTires[i]->SetAngularVelocity(f);
	}
	void __thiscall SetWheelMaxAngularVelocity(uintptr_t ptr, int i, float f) { // todo this doesn't exist
		ICHASSIS_FUNCTION_LOG("SetWheelMaxAngularVelocity");
		auto pThis = GetSuspensionRacer(ptr);
	}
	float __thiscall GetWheelSteer(uintptr_t ptr, unsigned int wheel) {
		ICHASSIS_FUNCTION_LOG("GetWheelSteer");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetWheelSteer(wheel);
	}
	bool __thiscall CalculateFFBSteeringState(uintptr_t ptr, FFBSteeringState*) {
		ICHASSIS_FUNCTION_LOG("CalculateFFBSteeringState");
		auto pThis = GetSuspensionRacer(ptr);
		return false;
	}
	float __thiscall GetSuspensionDigression(uintptr_t ptr, unsigned int i) { // todo is this correct
		ICHASSIS_FUNCTION_LOG("GetSuspensionDigression");
		auto pThis = GetSuspensionRacer(ptr);
		return 1.0f - pThis->mMWAttributes->SHOCK_DIGRESSION.At(IsRear(i));
	}
	float __thiscall GetWheelLateralForce(uintptr_t ptr, unsigned int i) {
		ICHASSIS_FUNCTION_LOG("GetWheelLateralForce");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[i]->mLateralForce;
	}
	float __thiscall GetRideHeight(uintptr_t ptr, unsigned int idx) {
		ICHASSIS_FUNCTION_LOG("GetRideHeight");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetRideHeight(idx);
	}
	float __thiscall GetWheelRadius(uintptr_t ptr, unsigned int idx) {
		ICHASSIS_FUNCTION_LOG("GetWheelRadius");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[idx]->GetRadius();
	}
	float __thiscall GetMaxSteering(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetMaxSteering");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->GetMaxSteering();
	}
	void __thiscall MatchSpeed(uintptr_t ptr, float speed) {
		ICHASSIS_FUNCTION_LOG("MatchSpeed");
		auto pThis = GetSuspensionRacer(ptr);
		pThis->MatchSpeed(speed);
	}
	float __thiscall GetDriveshaftTorqueEffect(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetDriveshaftTorqueEffect");
		auto pThis = GetSuspensionRacer(ptr);
		return 0.0;
	}
	float __thiscall GetRenderMotion(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetRenderMotion");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mMWAttributes->RENDER_MOTION;
	}
	ISteeringWheel::SteeringType __thiscall GetSteeringType(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetSteeringType");
		auto pThis = GetSuspensionRacer(ptr);

		ISteeringWheel::SteeringType steer_type = ISteeringWheel::kGamePad;

		IPlayer *player = pThis->GetOwner()->GetPlayer();
		if (player) {
			ISteeringWheel *device = player->GetSteeringDevice();

			if (device && device->IsConnected()) {
				steer_type = device->GetSteeringType();
			}
		}

		return steer_type;
	}
	float __thiscall GetWheelTorqueRatio(uintptr_t ptr, unsigned int i) { // todo what is this
		ICHASSIS_FUNCTION_LOG("GetWheelTorqueRatio");
		auto pThis = GetSuspensionRacer(ptr);
		auto tire = pThis->mTires[i];
		auto v70 = tire->mGripBoost;
		if (v70 <= 1.0) v70 = 1.0;
		float v63 = 1.0; // todo Curve::GetValueLinear((v19->mVehicleInfo->mLayoutPtr + 48 * v19->mAxleIndex + 0x1F0), (v21 * 2.23699)) Mu0[AxleIndex]
		auto v69 = std::sqrt(((tire->mLongitudeForce * tire->mLongitudeForce) + (tire->mLateralForce * tire->mLateralForce)));
		auto v72 = (v69 / (((((tire->mBrake + 1.0) * tire->mTractionBoost) * tire->mLoad) * v70) * v63));
		return UMath::Clamp(v72, -3.0f, 3.0f);
	}
	float __thiscall GetWheelIdealTorque(uintptr_t ptr, unsigned int i) { // todo what is this
		ICHASSIS_FUNCTION_LOG("GetWheelIdealTorque");
		auto pThis = GetSuspensionRacer(ptr);
		auto tire = pThis->mTires[i];
		return std::min(tire->GetTotalTorque() / tire->mRadius, 0.0f);
	}
	void __thiscall SetWheelRemoved(uintptr_t ptr, unsigned int i, bool b) { // todo
		ICHASSIS_FUNCTION_LOG("SetWheelRemoved");
		auto pThis = GetSuspensionRacer(ptr);
	}
	void __thiscall SetWheelMaximumTorqueRatio(uintptr_t ptr, unsigned int i, float f) { // todo
		ICHASSIS_FUNCTION_LOG("SetWheelMaximumTorqueRatio");
		auto pThis = GetSuspensionRacer(ptr);
	}
	bool __thiscall IsAntiBrakeLockOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("1"); return false; }
	int __thiscall GetAntiBrakeLockLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("2"); return 0; }
	void __thiscall SetAntiBrakeLockLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("3"); }
	bool __thiscall IsStabilityManagementOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("4"); return false; }
	int __thiscall GetStabilityManagementLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("5"); return 0; }
	void __thiscall SetStabilityManagementLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("6"); }
	bool __thiscall IsDriftAsssistOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("7"); return false; }
	int __thiscall GetDriftAssistLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("8"); return 0; }
	void __thiscall SetDriftAssistLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("9"); }
	bool __thiscall IsDriftGlueToRoadOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("10"); return false; }
	int __thiscall GetDriftGlueToRoadLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("11"); return 0; }
	void __thiscall SetDriftGlueToRoadLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("12"); }
	bool __thiscall IsDriftDynamicBrakeOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("13"); return false; }
	int __thiscall GetDriftDynamicBrakeLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("14"); return 0; }
	void __thiscall SetDriftDynamicBrakeLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("15"); }
	bool __thiscall IsDriftSpeedControlOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("16"); return false; }
	int __thiscall GetDriftSpeedControlLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("17"); return 0; }
	void __thiscall SetDriftSpeedControlLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("18"); }
	bool __thiscall IsRacelineAssistOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("19"); return false; }
	int __thiscall GetRacelineAssistLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("20"); return 0; }
	void __thiscall SetRacelineAssistLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("21"); }
	bool __thiscall IsBrakingAssistOn(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("22"); return false; }
	int __thiscall GetBrakingAssistLevel(uintptr_t ptr) { ICHASSIS_FUNCTION_LOG("23"); return 0; }
	void __thiscall SetBrakingAssistLevel(uintptr_t ptr, int i) { ICHASSIS_FUNCTION_LOG("24"); }
	float __thiscall GetDragCoefficient(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetDragCoefficient");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mMWAttributes->DRAG_COEFFICIENT;
	}
	float __thiscall GetDownCoefficient(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("GetDownCoefficient");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mMWAttributes->AERO_COEFFICIENT;
	}
	float __thiscall GetStaticGripForSpeed(uintptr_t ptr, float f) { // todo
		ICHASSIS_FUNCTION_LOG("GetStaticGripForSpeed");
		auto pThis = GetSuspensionRacer(ptr);
		return 0.8;
	}
	void __thiscall SetClutchKickExtraTireSpin(uintptr_t ptr, float f) { // todo
		ICHASSIS_FUNCTION_LOG("SetClutchKickExtraTireSpin");
		auto pThis = GetSuspensionRacer(ptr);
	}
	float __thiscall GetWheelieAngle(uintptr_t ptr) { // todo
		ICHASSIS_FUNCTION_LOG("GetWheelieAngle");
		auto pThis = GetSuspensionRacer(ptr);
		return 0.0;
	}
	bool __thiscall IsStaticResetCondition(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("IsStaticResetCondition");
		auto pThis = GetSuspensionRacer(ptr);
		return false;
	}
	void __thiscall SetAICatchOverride(uintptr_t ptr, bool) {
		ICHASSIS_FUNCTION_LOG("SetAICatchOverride");
		auto pThis = GetSuspensionRacer(ptr);
	}
	float __thiscall CalculateUndersteerFactor(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("CalculateUndersteerFactor");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->CalculateUndersteerFactor();
	}
	float __thiscall CalculateOversteerFactor(uintptr_t ptr) {
		ICHASSIS_FUNCTION_LOG("CalculateOversteerFactor");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->CalculateOversteerFactor();
	}
	void __thiscall SetWindCondition(uintptr_t ptr, float, float, float) {
		ICHASSIS_FUNCTION_LOG("SetWindCondition");
		auto pThis = GetSuspensionRacer(ptr);
	}
	bool __thiscall IsSlotCar(uintptr_t ptr) { // todo
		ICHASSIS_FUNCTION_LOG("IsSlotCar");
		auto pThis = GetSuspensionRacer(ptr);
		return false;
	}
	void __thiscall ChangeSlot(uintptr_t ptr, bool) { // todo
		ICHASSIS_FUNCTION_LOG("ChangeSlot");
		auto pThis = GetSuspensionRacer(ptr);
	}
	float __thiscall GetHydraulicsCompression(uintptr_t ptr, unsigned int i) { // todo
		ICHASSIS_FUNCTION_LOG("GetHydraulicsCompression");
		auto pThis = GetSuspensionRacer(ptr);
		return pThis->mTires[i]->GetCompression();
	}

	void* NewVTable[] = {
			(void*)0xDEADBEEF, // dtor
			(void*)&GetWheelTraction,
			(void*)&GetWheelDynamicSlipAngle,
			(void*)&GetWheelSlipRatio,
			(void*)&GetDragBoost,
			(void*)&GetNumWheels,
			(void*)&GetWheelPos,
			(void*)&GetWheelLocalPos,
			(void*)&GetWheelCenterPos,
			(void*)&GetWheelTorque,
			(void*)&GetWheelBrakeTorque,
			(void*)&GetWheelLoad,
			(void*)&GetWheelRoadHeight,
			(void*)&IsWheelOnGround,
			(void*)&GetCompression,
			(void*)&GuessCompression,
			(void*)&ForceCompression,
			(void*)&GetWheelSlip,
			(void*)&GetToleratedSlip,
			(void*)&GetWheelSkid,
			(void*)&GetWheelSlipAngle,
			(void*)&GetWheelRoadNormal,
			(void*)&GetWheelRoadSurface,
			(void*)&GetWheelVelocity,
			(void*)&GetNumWheelsOnGround,
			(void*)&GetNumWheelsOnBand,
			(void*)&GetWheelAngularVelocity,
			(void*)&SetWheelAngularVelocity,
			(void*)&SetWheelMaxAngularVelocity,
			(void*)&GetWheelSteer,
			(void*)&CalculateFFBSteeringState,
			(void*)&GetSuspensionDigression,
			(void*)&GetWheelLateralForce,
			(void*)&CalculateUndersteerFactor,
			(void*)&CalculateOversteerFactor,
			(void*)&GetRideHeight,
			(void*)&GetWheelRadius,
			(void*)&GetMaxSteering,
			(void*)&MatchSpeed,
			(void*)&GetDriveshaftTorqueEffect,
			(void*)&GetRenderMotion,
			(void*)&GetSteeringType,
			(void*)&GetWheelTorqueRatio,
			(void*)&GetWheelIdealTorque,
			(void*)&SetWheelRemoved,
			(void*)&SetWheelMaximumTorqueRatio,
			(void*)&IsAntiBrakeLockOn,
			(void*)&GetAntiBrakeLockLevel,
			(void*)&SetAntiBrakeLockLevel,
			(void*)&IsStabilityManagementOn,
			(void*)&GetStabilityManagementLevel,
			(void*)&SetStabilityManagementLevel,
			(void*)&IsDriftAsssistOn,
			(void*)&GetDriftAssistLevel,
			(void*)&SetDriftAssistLevel,
			(void*)&IsDriftGlueToRoadOn,
			(void*)&GetDriftGlueToRoadLevel,
			(void*)&SetDriftGlueToRoadLevel,
			(void*)&IsDriftDynamicBrakeOn,
			(void*)&GetDriftDynamicBrakeLevel,
			(void*)&SetDriftDynamicBrakeLevel,
			(void*)&IsDriftSpeedControlOn,
			(void*)&GetDriftSpeedControlLevel,
			(void*)&SetDriftSpeedControlLevel,
			(void*)&IsRacelineAssistOn,
			(void*)&GetRacelineAssistLevel,
			(void*)&SetRacelineAssistLevel,
			(void*)&IsBrakingAssistOn,
			(void*)&GetBrakingAssistLevel,
			(void*)&SetBrakingAssistLevel,
			(void*)&GetWheelEdgeOfControlSlipAngleLow,
			(void*)&GetWheelEdgeOfControlSlipAngleHigh,
			(void*)&GetWheelToleratedSlipRatio,
			(void*)&GetWheelPeakLongForceSlipRatio,
			(void*)&GetDragCoefficient,
			(void*)&GetDownCoefficient,
			(void*)&GetStaticGripForSpeed,
			(void*)&SetClutchKickExtraTireSpin,
			(void*)&GetHydraulicsCompression,
			(void*)&ChangeSlot,
			(void*)&IsSlotCar,
			(void*)&GetWheelieAngle,
			(void*)&IsStaticResetCondition,
			(void*)&SetWindCondition,
			(void*)&SetAICatchOverride,
	};
}