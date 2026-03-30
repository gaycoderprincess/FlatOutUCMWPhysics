class ICollisionBody : public IMWInterface {
public:
	ICollisionBody(Car* car) : pCar(car) {}

	static inline const char* _IIDName = "ICollisionBody";

	Car* pCar;
	float fMass = 1000.0;
	UMath::Vector3 vTensorScale = {0.0, 0.0, 0.0};
	UMath::Vector3 vCenterOfGravity = {0.0, 0.0, 0.0};

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

	virtual float GetMass() {
		return fMass;
	}

	virtual void SetCenterOfGravity(UMath::Vector3* cog) {
		vCenterOfGravity = *cog;
	}
	virtual UMath::Vector3* GetCenterOfGravity() {
		return &vCenterOfGravity;
	}

	virtual bool IsInGroundContact() {
		static UMath::Vector3 normal;
		normal = {0,1,0};

		auto origin = pCar->GetMatrix()->p;
		origin.y += 2;
		auto dir = NyaVec3(0,-1,0);

		tLineOfSightIn prop;
		prop.fMaxDistance = 10000;
		tLineOfSightOut out;
		if (CheckLineOfSight(&prop, pGameFlow->pHost->pUnkForLOS, &origin, &dir, &out)) {
			return out.fHitDistance > 2.5;
		}
		return false;
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
		if (CheckLineOfSight(&prop, pGameFlow->pHost->pUnkForLOS, &origin, &dir, &out)) {
			normal.x = out.vHitNormal.x;
			normal.y = out.vHitNormal.y;
			normal.z = out.vHitNormal.z;
		}
		return &normal;
	}
	virtual UMath::Vector3* GetInertiaTensor() {
		if (vTensorScale.x == 0.0f) {
			MWCarTuning tune;
			GetLerpedCarTuning(tune, mCOMObject->Find<IVehicle>()->GetVehicleName(), nullptr);
			vTensorScale.x = tune.TENSOR_SCALE[0];
			vTensorScale.y = tune.TENSOR_SCALE[1];
			vTensorScale.z = tune.TENSOR_SCALE[2];
			fMass = tune.MASS;
		}

		UMath::Vector3 dim;
		dim.x = std::max(std::abs(pCar->vCollisionFullMin.x), std::abs(pCar->vCollisionFullMax.x));
		dim.y = std::max(std::abs(pCar->vCollisionFullMin.y), std::abs(pCar->vCollisionFullMax.y));
		dim.z = std::max(std::abs(pCar->vCollisionFullMin.z), std::abs(pCar->vCollisionFullMax.z));

		static UMath::Vector3 tmp;
		tmp = CalculateInertiaTensor(vTensorScale, fMass, dim);
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