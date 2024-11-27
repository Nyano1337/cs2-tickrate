#pragma once

#include <cstdint>
#include <tier1/convar.h>
#include <sdk/common.h>

template<typename T>
constexpr EConVarType TranslateConVarType();

template<>
constexpr EConVarType TranslateConVarType<bool>(void) {
	return EConVarType_Bool;
}

template<>
constexpr EConVarType TranslateConVarType<int16_t>(void) {
	return EConVarType_Int16;
}

template<>
constexpr EConVarType TranslateConVarType<uint16_t>(void) {
	return EConVarType_UInt16;
}

template<>
constexpr EConVarType TranslateConVarType<int32_t>(void) {
	return EConVarType_Int32;
}

template<>
constexpr EConVarType TranslateConVarType<uint32_t>(void) {
	return EConVarType_UInt32;
}

template<>
constexpr EConVarType TranslateConVarType<int64_t>(void) {
	return EConVarType_Int64;
}

template<>
constexpr EConVarType TranslateConVarType<uint64_t>(void) {
	return EConVarType_UInt64;
}

template<>
constexpr EConVarType TranslateConVarType<float>(void) {
	return EConVarType_Float32;
}

template<>
constexpr EConVarType TranslateConVarType<double>(void) {
	return EConVarType_Float64;
}

template<>
constexpr EConVarType TranslateConVarType<const char*>(void) {
	return EConVarType_String;
}

template<>
constexpr EConVarType TranslateConVarType<Color>(void) {
	return EConVarType_Color;
}

template<>
constexpr EConVarType TranslateConVarType<Vector2D>(void) {
	return EConVarType_Vector2;
}

template<>
constexpr EConVarType TranslateConVarType<Vector>(void) {
	return EConVarType_Vector3;
}

template<>
constexpr EConVarType TranslateConVarType<Vector4D>(void) {
	return EConVarType_Vector4;
}

template<>
constexpr EConVarType TranslateConVarType<QAngle>(void) {
	return EConVarType_Qangle;
}

template<>
constexpr EConVarType TranslateConVarType<void*>(void) {
	return EConVarType_Invalid;
}

class ConVarHandleS2 {
public:
	ConVarHandleS2(uint16_t index = -1, uint32_t handle = -1) : m_convarIndex(index), m_handleIndex(handle) {}

	bool IsValid() const {
		return m_convarIndex != 0xFFFF;
	}

	void Invalidate() {
		m_convarIndex = 0xFFFF;
		m_handleIndex = 0x0;
	}

	uint16_t GetConVarIndex() const {
		return m_convarIndex;
	}

	uint32_t GetIndex() const {
		return m_handleIndex;
	}

private:
	uint16_t m_convarIndex;
	uint32_t m_handleIndex;
};

class BaseConVar;
using FnGenericChangeCallback_t = void (*)(BaseConVar* ref, CSplitScreenSlot nSlot, CVValue_t* pNewValue, CVValue_t* pOldValue);

struct CVarCreationBase_t {
	CVarCreationBase_t() : m_pszName(nullptr), m_pszHelpString(nullptr), m_nFlags(0) {}

	bool IsFlagSet(int64_t flag) const;
	void AddFlags(int64_t flags);
	void RemoveFlags(int64_t flags);
	int64_t GetFlags() const;

	const char* m_pszName;
	const char* m_pszHelpString;
	int64_t m_nFlags;
};

template<typename T>
class CConVarData;

class CConVarBaseData {
public:
	template<typename T>
	inline const CConVarData<T>* Cast() const {
		if (this->m_eVarType == TranslateConVarType<T>()) {
			return reinterpret_cast<const CConVarData<T>*>(this);
		}
		return nullptr;
	}

	template<typename T>
	inline CConVarData<T>* Cast() {
		if (this->m_eVarType == TranslateConVarType<T>()) {
			return reinterpret_cast<CConVarData<T>*>(this);
		}
		return nullptr;
	}

	CConVarBaseData()
		: m_pszName("<undefined>"), m_defaultValue(nullptr), m_minValue(nullptr), m_maxValue(nullptr),
		  m_pszHelpString("This convar is being accessed prior to ConVar_Register being called"), m_eVarType(EConVarType_Invalid) {}

	inline const char* GetName(void) const {
		return m_pszName;
	}

	inline const char* GetDescription(void) const {
		return m_pszHelpString;
	}

	inline EConVarType GetType(void) const {
		return m_eVarType;
	}

	inline bool HasDefaultValue() const {
		return m_defaultValue != nullptr;
	}

	inline bool HasMinValue() const {
		return m_minValue != nullptr;
	}

	inline bool HasMaxValue() const {
		return m_maxValue != nullptr;
	}

	inline int GetTimesChanged(void) const {
		return m_iTimesChanged;
	}

	inline void SetTimesChanged(int val) {
		m_iTimesChanged = val;
	}

	inline bool IsFlagSet(int64_t flag) const {
		return (flag & m_nFlags) ? true : false;
	}

	inline void AddFlags(int64_t flags) {
		m_nFlags |= flags;
	}

	inline void RemoveFlags(int64_t flags) {
		m_nFlags &= ~flags;
	}

	inline int64_t GetFlags(void) const {
		return m_nFlags;
	}

	inline int GetCallbackIndex(void) const {
		return m_iCallbackIndex;
	}

protected:
	template<typename T>
	static inline void ValueToString(const T& val, char* dst, size_t length);

	template<typename T>
	static T ValueFromString(const char* val);

	const char* m_pszName;

	void* m_defaultValue;
	void* m_minValue;
	void* m_maxValue;
	const char* m_pszHelpString;
	EConVarType m_eVarType;

	// This gets copied from the ConVarDesc_t on creation
	short unk1;

	unsigned int m_iTimesChanged;
	int64 m_nFlags;
	unsigned int m_iCallbackIndex;

	// Used when setting default, max, min values from the ConVarDesc_t
	// although that's not the only place of usage
	// flags seems to be:
	// (1 << 0) Skip setting value to split screen slots and also something keyvalues related
	// (1 << 1) Skip setting default value
	// (1 << 2) Skip setting min/max values
	int m_nUnknownAllocFlags;
};

struct ConVarCreation_t : CVarCreationBase_t {
	ConVarCreation_t() : m_pHandle(nullptr), m_pConVarData(nullptr) {}

#pragma pack(push, 1)

	struct ConVarValueInfo_t {
		ConVarValueInfo_t() : m_bHasDefault(false), m_bHasMin(false), m_bHasMax(false) {}

		template<typename T>
		T& DefaultValue() {
			return *reinterpret_cast<T*>(m_defaultValue);
		}

		template<typename T>
		T& MinValue() {
			return *reinterpret_cast<T*>(m_minValue);
		}

		template<typename T>
		T& MaxValue() {
			return *reinterpret_cast<T*>(m_maxValue);
		}

		int32_t m_unknown1; // 0x18

		bool m_bHasDefault; // 0x22
		bool m_bHasMin;     // 0x23
		bool m_bHasMax;     // 0x24

	private:
		// Don't use CVValue_t directly, to avoid initialising memory
		uint8_t m_defaultValue[sizeof(CVValue_t)]; // 0x25
		uint8_t m_minValue[sizeof(CVValue_t)];     // 0x35
		uint8_t m_maxValue[sizeof(CVValue_t)];     // 0x45
	} m_valueInfo;                                 // 0x22

#pragma pack(pop)

	FnGenericChangeCallback_t m_fnCallBack; // 0x56
	EConVarType m_eVarType;                 // 0x58

	ConVarHandleS2* m_pHandle;       // 0x60
	CConVarBaseData** m_pConVarData; // 0x68
};

static_assert(sizeof(ConVarCreation_t) == 0x70, "ConVarCreation_t wrong size!");
static_assert(sizeof(ConVarCreation_t) % 8 == 0x0, "ConVarCreation_t isn't 8 bytes aligned!");
static_assert(sizeof(CVValue_t) == 0x10, "CVValue_t wrong size!");

template<typename T>
class ConVarS2;

template<typename T>
class CConVarData : public CConVarBaseData {
public:
	friend class ConVarS2<T>;

	CConVarData() {
		m_defaultValue = new T();
		m_minValue = new T();
		m_maxValue = new T();
		m_eVarType = TranslateConVarType<T>();
	}

	~CConVarData() {
		delete m_defaultValue;
		delete m_minValue;
		delete m_eVarType;
	}

	inline const T& GetDefaultValue() const {
		return *reinterpret_cast<T*>(m_defaultValue);
	}

	inline const T& GetMinValue() const {
		return *reinterpret_cast<T*>(m_minValue);
	}

	inline const T& GetMaxValue() const {
		return *reinterpret_cast<T*>(m_maxValue);
	}

	inline void SetDefaultValue(const T& value) {
		if (!m_defaultValue) {
			m_defaultValue = new T();
		}
		*reinterpret_cast<T*>(m_defaultValue) = value;
	}

	inline void SetMinValue(const T& value) {
		if (!m_minValue) {
			m_minValue = new T();
		}
		*reinterpret_cast<T*>(m_minValue) = value;
	}

	inline void SetMaxValue(const T& value) {
		if (!m_maxValue) {
			m_maxValue = new T();
		}
		*reinterpret_cast<T*>(m_maxValue) = value;
	}

	inline void RemoveDefaultValue() {
		if (m_defaultValue) {
			delete reinterpret_cast<T*>(m_defaultValue);
		}
		m_defaultValue = nullptr;
	}

	inline void RemoveMinValue() {
		if (m_minValue) {
			delete reinterpret_cast<T*>(m_minValue);
		}
		m_minValue = nullptr;
	}

	inline void RemoveMaxValue() {
		if (m_maxValue) {
			delete reinterpret_cast<T*>(m_maxValue);
		}
		m_maxValue = nullptr;
	}

	inline const T& Clamp(const T& value) const {
		if (HasMinValue() && value < GetMinValue()) {
			return GetMinValue();
		}
		if (HasMaxValue() && value > GetMaxValue()) {
			return GetMaxValue();
		}
		return value;
	}

	inline const T& GetValue(const CSplitScreenSlot& index = 0) const {
		return m_value[index];
	}

	inline void SetValue(const T& value, const CSplitScreenSlot& index = 0) {
		m_value[index] = value;
	}

	inline void GetStringValue(char* dst, size_t len, const CSplitScreenSlot& index = 0) const {
		ValueToString(GetValue(index), dst, len);
	}

	inline void GetStringDefaultValue(char* dst, size_t len) const {
		ValueToString(GetDefaultValue(), dst, len);
	}

	inline void GetStringMinValue(char* dst, size_t len) const {
		ValueToString(GetMinValue(), dst, len);
	}

	inline void GetStringMaxValue(char* dst, size_t len) const {
		ValueToString(GetMaxValue(), dst, len);
	}

	inline void SetStringValue(const char* src, const CSplitScreenSlot& index = 0) {
		SetValue(ValueFromString(src), index);
	}

	inline void SetStringDefaultValue(const char* src) {
		SetDefaultValue(ValueFromString(src));
	}

	inline void SetStringMinValue(const char* src) {
		SetMinValue(ValueFromString(src));
	}

	inline void SetStringMaxValue(const char* src) {
		SetMaxValue(ValueFromString(src));
	}

protected:
	static inline void ValueToString(const T& value, char* dst, size_t length) {
		CConVarBaseData::ValueToString<T>(value, dst, length);
	};

	static T ValueFromString(const char* val) {
		return CConVarBaseData::ValueFromString<T>(val);
	};

	T m_value[MAX_SPLITSCREEN_CLIENTS];
};

class BaseConVar {
public:
	inline const char* GetName() const {
		return m_ConVarData->GetName();
	}

	inline const char* GetDescription() const {
		return m_ConVarData->GetDescription();
	}

	inline EConVarType GetType() const {
		return m_ConVarData->GetType();
	}

	inline bool HasDefaultValue() const {
		return m_ConVarData->HasDefaultValue();
	}

	inline bool HasMinValue() const {
		return m_ConVarData->HasMinValue();
	}

	inline bool HasMaxValue() const {
		return m_ConVarData->HasMaxValue();
	}

	inline bool IsFlagSet(int64_t flag) const {
		return m_ConVarData->IsFlagSet(flag);
	}

	inline void AddFlags(int64_t flags) {
		m_ConVarData->AddFlags(flags);
	}

	inline void RemoveFlags(int64_t flags) {
		return m_ConVarData->RemoveFlags(flags);
	}

	inline int64_t GetFlags(void) const {
		return m_ConVarData->GetFlags();
	}

	inline ConVarHandleS2 GetHandle() const {
		return m_Handle;
	};

	inline CConVarBaseData* GetConVarData() const {
		return m_ConVarData;
	};

protected:
	// High-speed method to read convar data
	ConVarHandleS2 m_Handle;
	CConVarBaseData* m_ConVarData;
};

static const ConVarHandleS2 INVALID_CONVAR_HANDLE = ConVarHandleS2();

// same with EConVarType but added EConVarTypeS2_MAX
enum EConVarTypeS2 : int16_t {
	EConVarTypeS2_Invalid = -1,
	EConVarTypeS2_Bool,
	EConVarTypeS2_Int16,
	EConVarTypeS2_UInt16,
	EConVarTypeS2_Int32,
	EConVarTypeS2_UInt32,
	EConVarTypeS2_Int64,
	EConVarTypeS2_UInt64,
	EConVarTypeS2_Float32,
	EConVarTypeS2_Float64,
	EConVarTypeS2_String,
	EConVarTypeS2_Color,
	EConVarTypeS2_Vector2,
	EConVarTypeS2_Vector3,
	EConVarTypeS2_Vector4,
	EConVarTypeS2_Qangle,
	EConVarTypeS2_MAX
};

static CConVarBaseData* GetInvalidConVar(EConVarType type) {
	static CConVarBaseData* invalid_convar[EConVarTypeS2_MAX + 1] = {
		new CConVarData<bool>(),     new CConVarData<int16_t>(),     new CConVarData<uint16_t>(), new CConVarData<int32_t>(),
		new CConVarData<uint32_t>(), new CConVarData<int64_t>(),     new CConVarData<uint64_t>(), new CConVarData<float>(),
		new CConVarData<double>(),   new CConVarData<const char*>(), new CConVarData<Color>(),    new CConVarData<Vector2D>(),
		new CConVarData<Vector>(),   new CConVarData<Vector4D>(),    new CConVarData<QAngle>(),
		new CConVarData<void*>() // EConVarTypeS2_MAX
	};

	if (type == EConVarType_Invalid) {
		return invalid_convar[EConVarTypeS2_MAX];
	}
	return invalid_convar[type];
}

void RegisterConVarS2(ConVarCreation_t& cvar);
void UnRegisterConVar(ConVarHandleS2& cvar);

template<typename T>
class ConVarS2 : public BaseConVar {
public:
	using FnChangeCallback_t = void (*)(ConVarS2<T>* ref, const CSplitScreenSlot nSlot, const T* pNewValue, const T* pOldValue);

	ConVarS2(const char* name, int32_t flags, const char* description, const T& value, FnChangeCallback_t cb = nullptr) {
		this->Init(INVALID_CONVAR_HANDLE, TranslateConVarType<T>());

		ConVarCreation_t setup;
		setup.m_valueInfo.m_bHasDefault = true;
		setup.m_valueInfo.DefaultValue<T>() = value;
		setup.m_eVarType = TranslateConVarType<T>();
		setup.m_fnCallBack = reinterpret_cast<FnGenericChangeCallback_t>(cb);

		this->Register(name, flags & ~FCVAR_DEVELOPMENTONLY, description, setup);
	}

	ConVarS2(const char* name, int32_t flags, const char* description, const T& value, bool min, const T& minValue, bool max, const T& maxValue,
			 FnChangeCallback_t cb = nullptr) {
		this->Init(INVALID_CONVAR_HANDLE, TranslateConVarType<T>());

		ConVarCreation_t setup;
		setup.m_valueInfo.m_bHasDefault = true;
		setup.m_valueInfo.DefaultValue<T>() = value;

		setup.m_valueInfo.m_bHasMin = min;
		setup.m_valueInfo.m_bHasMax = max;
		setup.m_valueInfo.MinValue<T>() = minValue;
		setup.m_valueInfo.MaxValue<T>() = maxValue;

		setup.m_eVarType = TranslateConVarType<T>();
		setup.m_fnCallBack = reinterpret_cast<FnGenericChangeCallback_t>(cb);

		this->Register(name, flags & ~FCVAR_DEVELOPMENTONLY, description, setup);
	}

	~ConVarS2() {
		UnRegisterConVar(this->m_Handle);
	}

	inline const CConVarData<T>* GetConVarData() const {
		return reinterpret_cast<const CConVarData<T>*>(m_ConVarData);
	}

	inline CConVarData<T>* GetConVarData() {
		return reinterpret_cast<CConVarData<T>*>(m_ConVarData);
	}

	inline const T& GetDefaultValue() const {
		return GetConVarData()->GetDefaultValue();
	}

	inline const T& GetMinValue() const {
		return GetConVarData()->GetMinValue();
	}

	inline const T& GetMaxValue() const {
		return GetConVarData()->GetMaxValue();
	}

	inline void SetDefaultValue(const T& value) {
		GetConVarData()->SetDefaultValue(value);
	}

	inline void SetMinValue(const T& value) {
		GetConVarData()->SetMinValue(value);
	}

	inline void SetMaxValue(const T& value) {
		GetConVarData()->SetMaxValue(value);
	}

	inline void RemoveDefaultValue() {
		GetConVarData()->RemoveDefaultValue();
	}

	inline void RemoveMinValue() {
		GetConVarData()->RemoveMinValue();
	}

	inline void RemoveMaxValue() {
		GetConVarData()->RemoveMaxValue();
	}

	inline const T& Clamp(const T& value) const {
		return GetConVarData()->Clamp(value);
	}

	inline const T& GetValue(const CSplitScreenSlot& index = CSplitScreenSlot(0)) const {
		return GetConVarData()->GetValue(index);
	}

	inline void SetValue(const T& val, const CSplitScreenSlot& index = CSplitScreenSlot(0)) {
		auto newValue = this->Clamp(val);

		char szNewValue[256], szOldValue[256];
		CConVarData<T>::ValueToString(newValue, szNewValue, sizeof(szNewValue));
		GetConVarData()->GetStringValue(szOldValue, sizeof(szOldValue), index);

		// Deep copy
		T oldValue = this->GetValue();
		this->UpdateValue(newValue, index, (const CVValue_t*)&newValue, (const CVValue_t*)&oldValue, szNewValue, szOldValue);
	}

	inline void GetStringValue(char* dst, size_t len, const CSplitScreenSlot& index = 0) const {
		GetConVarData()->GetStringValue(dst, len, index);
	}

	inline void GetStringDefaultValue(char* dst, size_t len) const {
		GetConVarData()->GetStringDefaultValue(dst, len);
	}

	inline void GetStringMinValue(char* dst, size_t len) const {
		GetConVarData()->GetStringMinValue(dst, len);
	}

	inline void GetStringMaxValue(char* dst, size_t len) const {
		GetConVarData()->GetStringMaxValue(dst, len);
	}

private:
	void Init(ConVarHandleS2 defaultHandle, EConVarType type) {
		this->m_Handle.Invalidate();
		this->m_ConVarData = nullptr;

		if (g_pCVar) {
			char* adsf = new char(sizeof(ConVarHandleS2));
			ConVarHandle* legacyHandle = (ConVarHandle*)adsf;
			memcpy(legacyHandle, &defaultHandle, sizeof(ConVarHandleS2));
			auto cvar = (CConVarBaseData*)g_pCVar->GetConVar(*legacyHandle);
			this->m_ConVarData = (cvar && cvar->Cast<T>()) ? cvar : nullptr;
			if (!this->m_ConVarData) {
				this->m_ConVarData = GetInvalidConVar(TranslateConVarType<T>());
			}
			// technically this
			// result = *(char ***)(sub_10B7760((unsigned int)a3) + 80);
		}
		this->m_Handle = defaultHandle;
	}

	void Register(const char* name, int32_t flags, const char* description, ConVarCreation_t& cvar) {
		this->m_ConVarData = GetInvalidConVar(cvar.m_eVarType);
		this->m_Handle.Invalidate();

		/*if (!CommandLine()->HasParm("-tools")
			&& (flags
				& (FCVAR_DEVELOPMENTONLY | FCVAR_ARCHIVE | FCVAR_USERINFO | FCVAR_CHEAT | FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE
				   | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_CLIENTCMD_CAN_EXECUTE))
				   == 0) {
			flags |= FCVAR_DEVELOPMENTONLY;
		}*/

		cvar.m_pszName = name;
		cvar.m_pszHelpString = description;
		cvar.m_nFlags = flags;

		cvar.m_pHandle = &this->m_Handle;
		cvar.m_pConVarData = &this->m_ConVarData;

		RegisterConVarS2(cvar);
	}

	inline void UpdateValue(const T& value, const CSplitScreenSlot& index, const CVValue_t* newValue, const CVValue_t* oldValue,
							const char* szNewValue, const char* szOldValue) {
		SURF_DEBUG_BREAK();
		// GetConVarData()->SetValue(value, index);
		//
		// GetConVarData()->SetTimesChanged(GetConVarData()->GetTimesChanged() + 1);
		// g_pCVar->CallChangeCallback(this->m_Handle, index, newValue, oldValue);
		// g_pCVar->CallGlobalChangeCallbacks(this, index, szNewValue, szOldValue);
	}
};

static_assert(sizeof(ConVarS2<int>) == 0x10, "ConVarS2 is of the wrong size!");
static_assert(sizeof(ConVarS2<int>) == sizeof(ConVarS2<Vector>), "Templated ConVarS2 size varies!");
