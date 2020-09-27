#pragma once
#include <string>
#include <unordered_map>

#define GET_STRING(key) StringPool::GetString(#key, __FILE__, __LINE__)

class StringPool
{
	typedef const char* C_STR;
public:
	enum class StringLocale : unsigned int
	{
		CHT_TW,
		ENG,
		Tag_StringLocale_End
	};

	/*enum class StringTag : unsigned int
	{
		//WvsApp Common Str Pool
		Common_No_CommandLine_Provided,

		//WvsLogin Str Pool
		Login_Invalid_Socket_Status,

		//WvsGame Str Pool
		GameSrv_Invalid_Socket_Status,
		GameSrv_ItemInfo_Init_ItemName,
		GameSrv_ItemInfo_Init_ItemName_Done,
		GameSrv_ItemInfo_Init_Equip,
		GameSrv_ItemInfo_Init_Equip_Done,
		GameSrv_ItemInfo_Init_Bundle,
		GameSrv_ItemInfo_Init_Bundle_Done,

		//WvsCenter Str Pool
		Center_Invalid_Socket_Status,

		//WvsLib Str Pool
		Lib_WvsBase_Acceptor_Created,
		Lib_WvsBase_Socket_Disconnected,
		
		Tag_String_End
	};*/

private:
	//static C_STR ms_asStringPool[(int)StringLocale::Tag_StringLocale_End][(int)StringTag::Tag_String_End];

	/*template<StringTag STR_TAG, StringLocale LANG_TYPE, typename T, typename... Args>
	static void ConstructString_Impl(const T& t)
	{
		static_assert((int)LANG_TYPE < (int)StringLocale::Tag_StringLocale_End, "Constructing string with unsupported languages, consider to expand StringPool::StringLocale?");
		static_assert((int)STR_TAG < (int)StringTag::Tag_String_End, "Constructing string outside pool index, consider to expand StringPool::StringTag?");

		//Only store strings that fit current locale setting.
		if(LANG_TYPE == ms_strLocale)
			ms_asStringPool[(int)LANG_TYPE][(int)STR_TAG] = t;
	}

	template<StringTag STR_TAG, StringLocale LANG_TYPE, typename T, typename... Args>
	static void ConstructString_Impl(const T& t, Args&&... args)
	{
		static_assert((int)STR_TAG < (int)StringTag::Tag_String_End, "Constructing string outside pool index, consider to expand StringPool::StringTag?");
		
		//Only store strings that fit current locale setting.
		if(LANG_TYPE == ms_strLocale)
			ms_asStringPool[(int)LANG_TYPE][(int)STR_TAG] = t;
		ConstructString_Impl<STR_TAG, (StringLocale)((int)LANG_TYPE + 1), Args...>(args...);
	}

	template<StringTag STR_TAG, typename... Args>
	static void Construct(Args&&... args)
	{
		ConstructString_Impl<STR_TAG, StringLocale::CHT_TW, Args...>(args...);
	}*/

	static StringLocale ms_strLocale;
	static std::unordered_map<std::string, std::string> ms_mStringMap;

	/*static void InitializeCommonStrPool();
	static void InitializeLoginSrvStrPool();
	static void InitializeGameSrvStrPool();
	static void InitializeCenterSrvStrPool();
	static void InitializeDBStrPool();
	static void InitializeLibStrPool();*/
public:
	static void Init(const std::string& sGlobalConfig);
	static C_STR GetString(const char* pKey, const char *pFile = nullptr, int nLine = -1);
	//static C_STR GetString(StringTag STR_TAG);
};