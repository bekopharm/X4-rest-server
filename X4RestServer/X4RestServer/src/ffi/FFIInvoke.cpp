#include <string>
#include "FFIInvoke.h"
#include "ffirip.h"

// Note: __FUNCTION__ Macro should work with all major compilers.

FFIInvoke::FFIInvoke(const HMODULE x4_module) : x4_module_(x4_module) {}

json FFIInvoke::GetPlayerName()
{
	const auto funcname = std::string(__FUNCTION__).substr(11);
	if (funcs_.count(funcname.c_str()) == 0)
	{
		loadFunction(funcname.c_str());
	}
	return json
		{
			{"player_name", reinterpret_cast<X4FFI::GetPlayerName>(funcs_[funcname.c_str()])()}
		};
}

json FFIInvoke::GetSofttarget()
{
	const auto funcname = std::string(__FUNCTION__).substr(11);
	if (funcs_.count(funcname.c_str()) == 0)
	{
		loadFunction(funcname.c_str());
	}
	X4FFI::SofttargetDetails softTarget = reinterpret_cast<X4FFI::GetSofttarget>(funcs_[funcname.c_str()])();
	return json
	{
		{"universeId", softTarget.softtargetID},
		{"softtargetConnectionName", softTarget.softtargetConnectionName},
	};
}

json FFIInvoke::GetComponentDetails(const X4FFI::UniverseID componentid, const char* const connectionname)
{
	const auto funcname = std::string(__FUNCTION__).substr(11);
	if (funcs_.count(funcname.c_str()) == 0)
	{
		loadFunction(funcname.c_str());
	}
	X4FFI::ComponentDetails compDetails =
		reinterpret_cast<X4FFI::GetComponentDetails>(funcs_[funcname.c_str()])(componentid, connectionname);
	return json
	{
		{"name", compDetails.name},
		{"hull", compDetails.hull},
		{"shield", compDetails.shield},
		{"speed", compDetails.speed},
		{"hasShield", compDetails.hasShield},
	};
}


void FFIInvoke::loadFunction(LPCSTR name)
{
	const auto addr = GetProcAddress(x4_module_, name);
	if (!addr) {
		throw std::exception(std::string(std::string(name) + "not found").c_str());
	}
	funcs_[name] = addr;
}