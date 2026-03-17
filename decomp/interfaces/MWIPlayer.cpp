class IPlayer : public IMWInterface {
public:
	IPlayer(Car* car) : pCar(car) {}

	static inline const char* _IIDName = "IPlayer";

	Car* pCar;

	virtual bool CanRechargeNOS() { return true; }
	virtual bool InGameBreaker() { return false; }
	virtual void ChargeGameBreaker(float f) {}
};