#ifndef INTROSPECTION_BASICTYPECONVERTERS_H
#define INTROSPECTION_BASICTYPECONVERTERS_H

#pragma once

namespace  Introspection
{



	template<typename T>
	struct IntConverter
	{
		static bool ToString(std::string& out, T in)
		{
			out = std::to_string(in);
			return true;
		}

		static bool FromString(T& out, const char* in)
		{
			char *end;
			out = (T)strtol(in, &end, 10);
			return true;
		}

		static bool FromString(T& out, const std::string& in)
		{
			return FromString(out, in.c_str());
		}
	};

	template<typename T>
	struct UIntConverter
	{
		static bool ToString(std::string& out, T in)
		{
			out = std::to_string(in);
			return true;
		}


		static bool FromString(T& out, const char* in)
		{
			char *end;
			out = (T)strtoul(in, &end, 10);
			return true;
		}

		static bool FromString(T& out, const std::string& in)
		{
			return FromString(out, in.c_str());
		}
	};

	template<typename T>
	struct LongLongConverter
	{
		static bool ToString(std::string& out, T in)
		{
			out = std::to_string(in);
			return true;
		}


		static bool FromString(T& out, const char* in)
		{
			char *end;
			out = (T)strtoll(in, &end, 10);
			return true;
		}

		static bool FromString(T& out, const std::string& in)
		{
			return FromString(out, in.c_str());
		}
	};

	template<typename T>
	struct ULongLongConverter
	{
		static bool ToString(std::string& out, T in)
		{
			out = std::to_string(in);
			return true;
		}

		static bool FromString(T& out, const char* in)
		{
			char *end;
			out = (T)strtoull(in, &end, 10);
			return true;
		}

		static bool FromString(T& out, const std::string& in)
		{
			return FromString(out, in.c_str());
		}
	};



template<typename T>
struct Converter
{
	static bool ToString(std::string& out, T in)
	{
		return false;
	}

	bool FromString(T& out, const char* in)
	{
		return false;
	}

	bool FromString(T& out, const std::string& in)
	{
		return false;
	}
};

template<>
struct Converter<bool>
{
	static bool ToString(std::string& out, bool in)
	{
		out = in ? "true" : "false";
		return true;
	}
	static bool FromString(bool& out, const char* in)
	{
		out = _stricmp(in, "true") == 0;
		return true;
	}

	static bool FromString(bool& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<char>
{
	static bool ToString(std::string& out, char in)
	{
		out.clear();
		out.push_back(in);
		return true;
	}
	static bool FromString(char& out, const char* in)
	{
		out = *in;
		return true;
	}

	static bool FromString(char& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<char*>
{
	static bool ToString(std::string& out, char* in)
	{
		out = in;
		return true;
	}
	static bool FromString(char*& out, const char* in)
	{
		out = _strdup(in);
		return true;
	}

	static bool FromString(char*& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<wchar_t>
{
	static bool ToString(std::string& out, wchar_t in)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string s = converter.to_bytes(in);

		out.clear();
		if (!s.empty())
			out.push_back(s[0]);

		return true;
	}
	static bool FromString(wchar_t& out, const char* in)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring s = converter.from_bytes(in);

		out = !s.empty() ? s[0] : 0;

		return true;
	}

	static bool FromString(wchar_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<wchar_t*>
{
	static bool ToString(std::string& out, wchar_t* in)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		out = converter.to_bytes(in);

		return true;
	}
	static bool FromString(wchar_t*& out, const char* in)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		out = _wcsdup(converter.from_bytes(in).c_str());

		return true;
	}

	static bool FromString(wchar_t*& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<int8_t>
{
	static bool ToString(std::string& out, int8_t in)
	{
		return IntConverter<int8_t>::ToString(out, in);
	}

	static bool FromString(int8_t& out, const char* in)
	{
		return IntConverter<int8_t>::FromString(out, in);
	}

	static bool FromString(int8_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<int16_t>
{
	static bool ToString(std::string& out, int16_t in)
	{
		return IntConverter<int16_t>::ToString(out, in);
	}

	static bool FromString(int16_t& out, const char* in)
	{
		return IntConverter<int16_t>::FromString(out, in);
	}

	static bool FromString(int16_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<int32_t>
{
	static bool ToString(std::string& out, int32_t in)
	{
		return IntConverter<int32_t>::ToString(out, in);
	}

	static bool FromString(int32_t& out, const char* in)
	{
		return IntConverter<int32_t>::FromString(out, in);
	}

	static bool FromString(int32_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<int64_t>
{
	static bool ToString(std::string& out, int64_t in)
	{
		return LongLongConverter<int64_t>::ToString(out, in);
	}

	static bool FromString(int64_t& out, const char* in)
	{
		return LongLongConverter<int64_t>::FromString(out, in);
	}

	static bool FromString(int64_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<uint8_t>
{
	static bool ToString(std::string& out, uint8_t in)
	{
		return UIntConverter<uint8_t>::ToString(out, in);
	}

	static bool FromString(uint8_t& out, const char* in)
	{
		return UIntConverter<uint8_t>::FromString(out, in);
	}

	static bool FromString(uint8_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<uint16_t>
{
	static bool ToString(std::string& out, uint16_t in)
	{
		return UIntConverter<uint16_t>::ToString(out, in);
	}

	static bool FromString(uint16_t& out, const char* in)
	{
		return UIntConverter<uint16_t>::FromString(out, in);
	}

	static bool FromString(uint16_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<uint32_t>
{
	static bool ToString(std::string& out, uint32_t in)
	{
		return UIntConverter<uint32_t>::ToString(out, in);
	}

	static bool FromString(uint32_t& out, const char* in)
	{
		return UIntConverter<uint32_t>::FromString(out, in);
	}

	static bool FromString(uint32_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<uint64_t>
{
	static bool ToString(std::string& out, uint64_t in)
	{
		return ULongLongConverter<uint64_t>::ToString(out, in);
	}

	static bool FromString(uint64_t& out, const char* in)
	{
		return ULongLongConverter<uint64_t>::FromString(out, in);
	}

	static bool FromString(uint64_t& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<float>
{
	static bool ToString(std::string& out, float in)
	{
		out = std::to_string(in);
		return true;
	}

	static bool FromString(float& out, const char* in)
	{
		char* end;
		out = std::strtof(in, &end);
		return true;
	}

	static bool FromString(float& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<double>
{
	static bool ToString(std::string& out, double in)
	{
		const char* fmt = "%.17g";

		int len = ::_scprintf(fmt, in);
		out.resize(len + 1, '\0');
		::sprintf_s(&out[0], len + 1, fmt, in);
		return true;
	}

	static bool FromString(double& out, const char* in)
	{
		char* end;
		out = std::strtod(in, &end);
		return true;
	}

	static bool FromString(double& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};

template<>
struct Converter<long double>
{
	static bool ToString(std::string& out, long double in)
	{
		out = std::to_string(in);
		return true;
	}

	static bool FromString(long double& out, const char* in)
	{
		char* end;
		out = std::strtold(in, &end);
		return true;
	}

	static bool FromString(long double& out, const std::string& in)
	{
		return FromString(out, in.c_str());
	}
};




} // namespace  Introspection
#endif // INTROSPECTION_BASICTYPECONVERTERS_H