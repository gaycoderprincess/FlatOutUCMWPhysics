class ChassisMW;
class IChassis {
public:
	static inline const char* _IIDName = "IChassis";

	ChassisMW* pChassis;

	virtual float GetWheelSkid(int i) { return 0.0; }
	virtual float GetWheelSteer(int i) { return 0.0; }
	virtual float GetWheelSlip(int i) { return 0.0; }
	virtual float GetWheelAngularVelocity(int i) { return 0.0; }
	virtual bool IsWheelOnGround(int i) { return true; }
	virtual int GetNumWheels() { return 4; }
	virtual int GetNumWheelsOnGround() { return 4; }
	virtual void SetWheelAngularVelocity(int i, float f) {}
};

class IVehicle {
public:
	static inline const char* _IIDName = "IVehicle";

	Car* pCar;

	virtual const char* GetVehicleName() {
		static std::string str;
		str = std::format("car_{}", pCar->pPlayer->nCarId);
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
	virtual bool IsStaging() { return false; } // todo
	virtual float GetPerfectLaunch() { return 0.0; }
	virtual bool IsDestroyed() { return pCar->nIsWrecked; }
};

UMath::Vector3 vInertiaTensor = {1.0, 2.0, 1.0};
double fDeltaTime = 1.0 / 60.0; // todo!
class IRigidBody {
public:
	static inline const char* _IIDName = "IRigidBody";

	Car* pCar;

	virtual float GetSpeed() {
		return pCar->GetVelocity()->length();
	}
	virtual float GetMass() {
		return pCar->fMass;
	}
	virtual void GetMatrix4(UMath::Matrix4* out) {
		*out = *pCar->GetMatrix();
	}
	virtual UMath::Vector3* GetPosition() {
		return (UMath::Vector3*)&pCar->GetMatrix()->p;
	}
	virtual void GetRightVector(UMath::Vector3* out) {
		*out = pCar->GetMatrix()->x;
	}
	virtual void GetUpVector(UMath::Vector3* out) {
		*out = pCar->GetMatrix()->y;
	}
	virtual void GetForwardVector(UMath::Vector3* out) {
		*out = pCar->GetMatrix()->z;
	}
	virtual void GetDimension(UMath::Vector3 *out) {
		*out = pCar->vCollisionFullMax;
	}
	virtual void ConvertWorldToLocal(UMath::Vector3 *val, bool translate) {
		::ConvertWorldToLocal(pCar, *val, translate);
	}
	virtual UMath::Vector3* GetLinearVelocity() {
		return (UMath::Vector3*)pCar->GetVelocity();
	}
	virtual UMath::Vector3* GetAngularVelocity() {
		return (UMath::Vector3*)pCar->GetAngVelocity();
	}
	virtual void SetLinearVelocity(UMath::Vector3 *val) {
		*pCar->GetVelocity() = *val;
	}
	virtual void SetAngularVelocity(UMath::Vector3 *val) {
		*pCar->GetAngVelocity() = *val;
	}
	virtual void Resolve(const UMath::Vector3* _force, const UMath::Vector3* _torque) {
		auto force = *_force;
		auto torque = *_torque;

		float oom = 1.0 / pCar->fMass;
		auto dT = fDeltaTime;

		auto bodyMatrix = *pCar->GetMatrix();
		bodyMatrix.p = {0,0,0}; // hmm...
		auto mInvWorldTensor = GetInverseWorldTensor(CalculateInertiaTensor(vInertiaTensor, pCar->fMass, pCar->vCollisionFullMax), bodyMatrix);

		UMath::Vector3 mCOG = {0,0,0};

		NyaVec3 v22;
		v22.x = ((bodyMatrix.x.x * mCOG.x) + (bodyMatrix.z.x * mCOG.z) + (bodyMatrix.y.x * mCOG.y));
		v22.y = ((bodyMatrix.x.y * mCOG.x) + (bodyMatrix.z.y * mCOG.z) + (bodyMatrix.y.y * mCOG.y));
		v22.z = ((bodyMatrix.x.z * mCOG.x) + (bodyMatrix.z.z * mCOG.z) + (bodyMatrix.y.z * mCOG.y));

		*pCar->GetVelocity() += (force * oom * dT);

		pCar->GetAngVelocity()->x += (mInvWorldTensor.x.x * torque.x * dT) + (mInvWorldTensor.z.x * torque.z * dT) + (mInvWorldTensor.y.x * torque.y * dT);
		pCar->GetAngVelocity()->y += (mInvWorldTensor.x.y * torque.x * dT) + (mInvWorldTensor.z.y * torque.z * dT) + (mInvWorldTensor.y.y * torque.y * dT);
		pCar->GetAngVelocity()->z += (mInvWorldTensor.x.z * torque.x * dT) + (mInvWorldTensor.z.z * torque.z * dT) + (mInvWorldTensor.y.z * torque.y * dT);

		bodyMatrix = *pCar->GetMatrix();
		bodyMatrix.p = {0,0,0}; // hmm...

		mInvWorldTensor = GetInverseWorldTensor(CalculateInertiaTensor(vInertiaTensor, pCar->fMass, pCar->vCollisionFullMax), bodyMatrix);
	}
	virtual void ResolveForce(UMath::Vector3* force) {
		UMath::Vector3 tmp = {0,0,0};
		Resolve(force, &tmp);
	}
	virtual void ResolveTorque(UMath::Vector3* torque) {
		UMath::Vector3 tmp = {0,0,0};
		Resolve(&tmp, torque);
	}
	virtual void ResolveTorque(const UMath::Vector3 *force, const UMath::Vector3 *p) {
		auto bodyMatrix = *pCar->GetMatrix();
		auto position = pCar->GetMatrix()->p;

		UMath::Vector3 mCOG = {0,0,0};

		UMath::Vector3 cg;
		UMath::Vector3 torque;
		UMath::Vector3 r;
		UMath::Rotate(mCOG, bodyMatrix, cg);
		UMath::Add(cg, position, cg);
		UMath::Sub(*p, cg, r);
		UMath::Cross(r, *force, torque);
		UMath::Add(torque, torque, torque);

		UMath::Vector3 tmp = {0,0,0};
		Resolve(&tmp, &torque);
	}
	virtual void ResolveForce(const UMath::Vector3 *force, const UMath::Vector3 *p) {
		auto bodyMatrix = *pCar->GetMatrix();
		auto position = pCar->GetMatrix()->p;

		UMath::Vector3 mCOG = {0,0,0};

		UMath::Vector3 cg;
		UMath::Vector3 torque;
		UMath::Vector3 r;
		UMath::Rotate(mCOG, bodyMatrix, cg);
		UMath::Add(cg, position, cg);
		UMath::Sub(*p, cg, r);
		UMath::Cross(r, *force, torque);
		Resolve(force, &torque);
	}
	virtual void ModifyXPos(float offset) {
		pCar->GetMatrix()->p.x += offset;
	}
	virtual void ModifyYPos(float offset) {
		pCar->GetMatrix()->p.y += offset;
	}
	virtual void ModifyZPos(float offset) {
		pCar->GetMatrix()->p.z += offset;
	}
};

class ICollisionBody {
public:
	static inline const char* _IIDName = "ICollisionBody";

	Car* pCar;

	// todo
	virtual UMath::Vector3* GetForce() {
		static UMath::Vector3 tmp;
		tmp = {0,0,0};
		return &tmp;
	}
	virtual UMath::Vector3* GetTorque() {
		static UMath::Vector3 tmp;
		tmp = {0,0,0};
		return &tmp;
	}

	// todo important!!
	virtual void SetCenterOfGravity(UMath::Vector3* cog) {

	}
	virtual UMath::Vector3* GetCenterOfGravity() {
		static UMath::Vector3 tmp;
		tmp = {0,0,0};
		return &tmp;
	}

	virtual bool IsInGroundContact() {
		return true; // todo
	}
	virtual UMath::Vector3* GetGroundNormal() {
		static UMath::Vector3 normal;
		normal = {0,1,0};

		auto origin = pCar->GetMatrix()->p;
		origin.y += 2;
		auto dir = NyaVec3(0,-1,0);

		tLineOfSightIn prop;
		prop.fMaxDistance = 10000;
		tLineOfSightOut out;
		if (!CheckLineOfSight(&prop, pGameFlow->pHost->pUnkForLOS, &origin, &dir, &out)) {
			normal.x = out.vHitNormal.x;
			normal.y = out.vHitNormal.y;
			normal.z = out.vHitNormal.z;
		}
		return &normal;
	}
	virtual UMath::Vector3* GetInertiaTensor() {
		static UMath::Vector3 tmp;
		tmp = CalculateInertiaTensor(vInertiaTensor, pCar->fMass, pCar->vCollisionFullMax);
		return &tmp;
	}
	virtual void Damp(float amount) {
		UMath::Vector3& linearVel = *(UMath::Vector3*)pCar->GetVelocity();
		UMath::Vector3& angularVel = *(UMath::Vector3*)pCar->GetAngVelocity();

		float scale = 1.0f - amount;
		UMath::Scale(linearVel, scale, linearVel);
		UMath::Scale(angularVel, scale, angularVel);
	}
	virtual bool HasHadCollision() { return false; }
};

class IInput {
public:
	static inline const char* _IIDName = "IInput";

	Car* pCar;

	virtual float GetControlGas() { return pCar->fGasPedal; }
	virtual float GetControlBrake() { return pCar->fBrakePedal; }
	virtual float GetControlHandBrake() { return pCar->fHandbrake; }
	virtual float GetControlSteering() {
		auto ply = pCar->pPlayer;
		if (ply->nIsUsingKeyboard) {
			float steeringInput = 0;
			if (ply->nSteeringKeyboardLeft) steeringInput = -1;
			if (ply->nSteeringKeyboardRight) steeringInput += 1;
			return steeringInput;
		}
		else {
			auto xinput = (XInputController*)ply->pController;
			return xinput->fLeftStickX;
		}
	}
	virtual bool GetControlNOS() { return pCar->fNitroButton > 0.0; }
	virtual bool IsAutomaticShift() { return true; }
};

class IEngineDamage {
public:
	static inline const char* _IIDName = "IEngineDamage";

	Car* pCar;

	virtual bool IsBlown() { return pCar->nIsWrecked; }
};

class IHumanAI {
public:
	static inline const char* _IIDName = "IHumanAI";

	virtual bool IsPlayerSteering() { return true; }
};

class IEngine {
public:
	static inline const char* _IIDName = "IEngine";

	Car* pCar;

	virtual float GetNOSBoost() { return 0.0; }
};

class ITransmission {
public:
	static inline const char* _IIDName = "ITransmission";

	Car* pCar;

	// todo is this correct
	virtual int GetGear() {
		return pCar->mGearbox.nGear;
	}
	virtual float GetDriveTorque() { return 0.0; }
	virtual float GetSpeedometer() { return 0.0; }
	virtual float GetMaxSpeedometer() { return 0.0; }
};

class ChassisMW {
public:
	struct State {
		UMath::Matrix4 matrix;
		UMath::Vector3 local_vel;
		float gas_input;
		UMath::Vector3 linear_vel;
		float brake_input;
		UMath::Vector3 angular_vel;
		float ground_effect;
		UMath::Vector3 cog;
		float ebrake_input;
		UMath::Vector3 dimension;
		Angle steer_input;
		UMath::Vector3 local_angular_vel;
		Angle slipangle;
		UMath::Vector3 inertia;
		float mass;
		UMath::Vector3 world_cog;
		float speed;
		float time;
		int flags;
		short driver_style;
		short driver_class;
		short gear;
		short blown_tires;
		float nos_boost;
		float shift_boost;

		enum Flags { IS_STAGING = 1, IS_DESTROYED };

		const UMath::Vector3 &GetRightVector() const {
			return *(UMath::Vector3*)&matrix.x;
		}
		const UMath::Vector3 &GetUpVector() const {
			return *(UMath::Vector3*)&matrix.y;
		}
		const UMath::Vector3 &GetForwardVector() const {
			return *(UMath::Vector3*)&matrix.z;
		}
		const UMath::Vector3 &GetPosition() const {
			return *(UMath::Vector3*)&matrix.p;
		}
	};

	enum SleepState {
		SS_LATERAL = 2,
		SS_ALL = 1,
		SS_NONE = 0,
	};

	void Create(Car* car);
	void Destroy(char a2);

	Mps ComputeMaxSlip(const State &state);
	void DoTireHeat(const State &state);
	float ComputeLateralGripScale(const State &state);
	float ComputeTractionScale(const State &state);
	SleepState DoSleep(const State &state);
	void ComputeAckerman(const float steering, const State &state, UMath::Vector4 *left, UMath::Vector4 *right);
	void SetCOG(float extra_bias, float extra_ride);
	void ComputeState(float dT, State &state);
	void DoAerodynamics(const State &state, float drag_pct, float aero_pct, float aero_front_z, float aero_rear_z,
						const Physics::Tunings *tunings);
	void DoJumpStabilizer(const State &state);

	Meters GuessCompression(unsigned int id, float downforce);
	float GetRenderMotion();
	Meters GetRideHeight(unsigned int idx);
	float CalculateUndersteerFactor();
	float CalculateOversteerFactor();
	void OnTaskSimulate(float dT);

	Car* pCar;
	MWCarTuning* mMWAttributes;
	float mJumpTime;
	float mJumpAlititude;
	float mTireHeat;
	IVehicle tmpIVehicle;
	IChassis* pIChassis;

	IRigidBody* mRB;
	ICollisionBody* mRBComplex;
	IInput* mInput;
	IEngineDamage* mEngineDamage;
	IHumanAI* mHumanAI;
	IEngine* mEngine;
	ITransmission* mTransmission;

	IVehicle* GetVehicle() {
		tmpIVehicle.pCar = pCar;
		return &tmpIVehicle;
	}

	IChassis* GetIChassis() {
		pIChassis->pChassis = this;
		return pIChassis;
	}

	const char* mChassisType = "Chassis";
};