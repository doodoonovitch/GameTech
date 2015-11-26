#include "stdafx.h"
#include "Introspection.h"


namespace  Introspection
{


TextDeserializer::TextDeserializer(std::wistream & stream)
	: _stream(stream)
{
	//_buffer.reserve(40);
}


TextDeserializer::~TextDeserializer()
{
}

void TextDeserializer::ParseToken_UntilNextToken(Token& tk)
{
	tk._type = TokenType::Undefined_Token;
	bool end = _stream.eof();
	while(!end)
	{
		int ch = _stream.get();
		if (ch == std::wistream::traits_type::eof())
		{
			end = true;
		}
		else
		{
			wchar_t c = (wchar_t)ch;

			switch (c)
			{
			case L' ':
			case L'\n':
			case L'\t':
			case L'\r':
				break;

			case L'{':
			case L'}':
			case L'[':
			case L':':
			case L',':
			case L'"':
				_stream.putback(c);
				end = true;
				break;

			default:
				tk._text.push_back(c);
			}
		}
	} 
}

void TextDeserializer::ParseToken_String(Token& tk)
{
	bool end = _stream.eof();
	while (!end)
	{
		std::wistream::int_type ch = _stream.get();
		if (ch == std::wistream::traits_type::eof())
		{
			LogError();
			end = true;
		}
		else
		{
			wchar_t c = (wchar_t)ch;

			switch (c)
			{
			case L'\\':
			{
				ch = _stream.get();
				if (ch == std::wistream::traits_type::eof())
				{
					LogError();
					end = true;
				}
				else
				{
					c = (wchar_t)ch;
					switch (c)
					{
					case L'"':
					case L'/':
					case L'\\':
						tk._text.push_back(c);
						break;

					case L'b':
						tk._text.push_back(0x08);
						break;

					case L'f':
						tk._text.push_back(0x0c);
						break;

					case L'n':
						tk._text.push_back(0x0a);
						break;

					case L'r':
						tk._text.push_back(0x0d);
						break;

					case L'\t':
						tk._text.push_back(0x0b);
						break;

					case L'u':
					{
						int n = 0;
						wchar_t buff[5];

						bool stop = false;
						while (n < 4 && !stop)
						{
							ch = _stream.get();
							if (ch == std::wistream::traits_type::eof())
							{
								LogError();
								end = true;
								break;
							}
							else
							{
								switch (c)
								{
								case L'0':
								case L'1':
								case L'2':
								case L'3':
								case L'4':
								case L'5':
								case L'6':
								case L'7':
								case L'8':
								case L'9':
								case L'A':
								case L'B':
								case L'C':
								case L'D':
								case L'E':
								case L'F':
								case L'a':
								case L'b':
								case L'c':
								case L'd':
								case L'e':
								case L'f':
									buff[n] = c;
									++n;
									break;

								default:
									LogError();
									_stream.putback(c);
									stop = true;
									break;
								}
							}
						}

						if (!stop)
						{
							buff[n] = 0;
							wchar_t * strend;
							c = (wchar_t)std::wcstoul(buff, &strend, 16);
							tk._text.push_back(c);
						}
					}
					break;

					default:
						LogError();
						break;
					}
				}
			}
			break;

			case L'"':
				end = true;
				break;

			default:
				tk._text.push_back(c);
			}
		}
	}
}

void TextDeserializer::ParseToken_Number(Token& tk)
{
	ParseToken_UntilNextToken(tk);
	tk._type = TokenType::Number;
}

bool TextDeserializer::ParseToken_InitialState(Token & tk)
{
	bool end = false;
	std::wistream::pos_type pos = _stream.tellg();
	std::wistream::int_type ch = _stream.get();
	if (ch == std::wistream::traits_type::eof())
	{
		end = true;
	}
	else
	{
		wchar_t c = (wchar_t)ch;

		switch (c)
		{
		case L'{':
			tk._type = TokenType::LeftBrace;
			end = true;
			break;

		case L'}':
			tk._type = TokenType::RightBracket;
			end = true;
			break;

		case L'[':
			tk._type = TokenType::LeftBracket;
			end = true;
			break;

		case L':':
			tk._type = TokenType::Colon;
			end = true;
			break;

		case L',':
			tk._type = TokenType::Comma;
			end = true;
			break;

		case L'"':
			_lexerState = LexerState::String;
			tk._pos = _stream.tellg();
			tk._type = TokenType::String;
			break;

		case L'f':
			_lexerState = LexerState::Litteral_False;
			break;

		case L't':
			_lexerState = LexerState::Litteral_True;
			break;

		case L'n':
			_lexerState = LexerState::Litteral_Null;
			break;

		case L'-':
		case L'+':
		case L'0':
		case L'1':
		case L'2':
		case L'3':
		case L'4':
		case L'5':
		case L'6':
		case L'7':
		case L'8':
		case L'9':
			_lexerState = LexerState::Number;
			break;


		case L' ':
		case L'\n':
		case L'\t':
		case L'\r':
			break;

		default:
			_lexerState = LexerState::Undefined_Token;
			break;
		}

		if (_lexerState != LexerState::Initial && _lexerState != LexerState::String)
		{
			tk._text.push_back(c);
			tk._pos = pos;
		}
	}

	return end;
}

void TextDeserializer::ParseToken(Token& tk)
{
	tk.Reset();
	
	bool end = _stream.eof();
	while (!end)
	{
		switch (_lexerState)
		{
		case Introspection::TextDeserializer::LexerState::Initial:
		{
			end = ParseToken_InitialState(tk);
		}
		break;

		case Introspection::TextDeserializer::LexerState::String:
		{
			ParseToken_String(tk);
			end = true;
		}
		break;

		case Introspection::TextDeserializer::LexerState::Litteral_False:
		{
			ParseToken_UntilNextToken(tk);
			if (tk._text == _falseToken)
			{
				tk._type = TokenType::FalseLiteral;
			}
			end = true;
		}
		break;

		case Introspection::TextDeserializer::LexerState::Litteral_True:
		{
			ParseToken_UntilNextToken(tk);
			if (tk._text == _trueToken)
			{
				tk._type = TokenType::TrueLiteral;
			}
			end = true;
		}
		break;

		case Introspection::TextDeserializer::LexerState::Litteral_Null:
		{
			ParseToken_UntilNextToken(tk);
			if (tk._text == _nullToken)
			{
				tk._type = TokenType::NullLiteral;
			}
			end = true;
		}
		break;

		case Introspection::TextDeserializer::LexerState::Number:
		{
			ParseToken_Number(tk);
			end = true;
		}
		break;

		case Introspection::TextDeserializer::LexerState::Undefined_Token:
		{
			ParseToken_UntilNextToken(tk);
			end = true;
		}
		break;

		default:
			break;
		}
	}
}


void TextDeserializer::Deserialize(ObjectBase & object)
{
	ObjectBase* obj = &object;
	Deserialize(obj);
}

void TextDeserializer::Deserialize(ObjectBase * & object)
{
	if (_current._type == TokenType::Not_A_Token)
		ParseToken(_current);

	if (_current._type == TokenType::LeftBrace)
	{
		const TypeInfo* ti = object->GetTypeInfo();

		_current.Reset();

		bool end = false;
		while (!end)
		{
			if (_current._type == TokenType::Not_A_Token)
				ParseToken(_current);

			switch (_current._type)
			{
			case TokenType::RightBrace:
				end = true;
				break;

			case TokenType::String:
			{
				bool ok = false;

				Token colonTk;
				Token valueTk;
				
				ParseToken(colonTk);
				if (colonTk._type == TokenType::Colon)
				{
					ParseToken(valueTk);
					if (valueTk.IsAValueToken())
					{
						ok = true;
					}
					else
					{
						LogError();
						_current = valueTk;
					}
				}
				else
				{
					LogError();
					_current = colonTk;
				}

				if (ok)
				{
					const MemberInfo* mi = ti->GetMember(_current._text, true);
					if (mi != nullptr)
					{
						_current = valueTk;
						object->DeserializeMember(*this, mi);
					}

					if (_current._type == TokenType::Not_A_Token)
						ParseToken(_current);

					if (_current._type == TokenType::Comma)
						_current.Reset();
					else
					{
						LogError();
					}

				}
			}
			break;

			default:
				if (_stream.eof())
				{
					LogError();
					end = true;
				}
				else
				{
					LogError();
				}
			}
		}
	}
}


void TextDeserializer::Deserialize(bool & value)
{
	switch (_current._type)
	{
	case TokenType::TrueLiteral:
		value = true;
		_current.Reset();
		break;

	case TokenType::FalseLiteral:
		value = false;
		_current.Reset();
		break;

	default:
		LogError();
		break;
	} 
}

void TextDeserializer::Deserialize(char & value)
{
	if (_current._type == TokenType::String)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string s = converter.to_bytes(_current._text);
		if (!s.empty())
			value = s[0];
		else
			value = 0;
		_current.Reset();
	}
	else
	{
		LogError();
	}
}

void TextDeserializer::Deserialize(wchar_t & value)
{
	if (_current._type == TokenType::String)
	{
		if (_current._text.empty())
			value = _current._text[0];
		else
			value = 0;
		_current.Reset();
	}
	else
	{
		LogError();
	}
}

void TextDeserializer::Deserialize(float & value)
{
	DeserializeFloat<float>(value);
}
void TextDeserializer::Deserialize(double & value)
{
	DeserializeFloat<double>(value);
}

void TextDeserializer::Deserialize(int8_t & value)
{
	DeserializeInteger<int8_t>(value);
}
void TextDeserializer::Deserialize(int16_t & value)
{
	DeserializeInteger<int16_t>(value);
}
void TextDeserializer::Deserialize(int32_t & value)
{
	DeserializeInteger<int32_t>(value);
}
void TextDeserializer::Deserialize(int64_t & value)
{
	DeserializeInteger<int64_t>(value);
}
void TextDeserializer::Deserialize(uint8_t & value)
{
	DeserializeUnsignedInteger<uint8_t>(value);
}
void TextDeserializer::Deserialize(uint16_t & value)
{
	DeserializeUnsignedInteger<uint16_t>(value);
}
void TextDeserializer::Deserialize(uint32_t & value)
{
	DeserializeUnsignedInteger<uint32_t>(value);
}
void TextDeserializer::Deserialize(uint64_t & value)
{
	DeserializeUnsignedInteger<uint64_t>(value);
}

void TextDeserializer::Deserialize(std::string & value)
{
	if (_current._type == TokenType::String)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::swap(value, converter.to_bytes(_current._text));
	}
	else
	{
		LogError();
	}
}
void TextDeserializer::Deserialize(std::wstring & value)
{
	if (_current._type == TokenType::String)
	{
		value = _current._text;
	}
	else
	{
		LogError();
	}
}

void TextDeserializer::Deserialize(char * & value)
{
	if (_current._type == TokenType::String)
	{
		if (value != nullptr)
			free(value);

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string s = converter.to_bytes(_current._text);
		value = _strdup(s.c_str());
	}
	else
	{
		LogError();
	}
}

void TextDeserializer::Deserialize(wchar_t * & value)
{
	if (_current._type == TokenType::String)
	{
		if (value != nullptr)
			free(value);

		value = _wcsdup(_current._text.c_str());
	}
	else
	{
		LogError();
	}
}


void TextDeserializer::Deserialize(bool * & value)
{
	DeserializeBasicTypePointer<bool>(value);
}

void TextDeserializer::Deserialize(float * & value)
{
	DeserializeBasicTypePointer<float>(value);
}

void TextDeserializer::Deserialize(double * & value)
{
	DeserializeBasicTypePointer<double>(value);
}


void TextDeserializer::Deserialize(int8_t * & value)
{
	DeserializeBasicTypePointer<int8_t>(value);
}
void TextDeserializer::Deserialize(int16_t * & value)
{
	DeserializeBasicTypePointer<int16_t>(value);
}
void TextDeserializer::Deserialize(int32_t * & value)
{
	DeserializeBasicTypePointer<int32_t>(value);
}
void TextDeserializer::Deserialize(int64_t * & value)
{
	DeserializeBasicTypePointer<int64_t>(value);
}
void TextDeserializer::Deserialize(uint8_t * & value)
{
	DeserializeBasicTypePointer<uint8_t>(value);
}
void TextDeserializer::Deserialize(uint16_t * & value)
{
	DeserializeBasicTypePointer<uint16_t>(value);
}
void TextDeserializer::Deserialize(uint32_t * & value)
{
	DeserializeBasicTypePointer<uint32_t>(value);
}
void TextDeserializer::Deserialize(uint64_t * & value)
{
	DeserializeBasicTypePointer<uint64_t>(value);
}


void TextDeserializer::Deserialize(std::string * & value) 
{
	DeserializeBasicTypePointer<std::string>(value);
}

void TextDeserializer::Deserialize(std::wstring * & value)
{
	DeserializeBasicTypePointer<std::wstring>(value);
}

void TextDeserializer::DeserializeArray(bool * const values, uint32_t itemCount) 
{
	DeserializeArray<bool>(values, itemCount);
}
void TextDeserializer::DeserializeArray(char * const values, uint32_t itemCount)
{
	DeserializeArray<char>(values, itemCount);
}
void TextDeserializer::DeserializeArray(wchar_t * const values, uint32_t itemCount)
{
	DeserializeArray<wchar_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(float * const values, uint32_t itemCount)
{
	DeserializeArray<float>(values, itemCount);
}
void TextDeserializer::DeserializeArray(double * const values, uint32_t itemCount)
{
	DeserializeArray<double>(values, itemCount);
}

void TextDeserializer::DeserializeArray(int8_t * const values, uint32_t itemCount) 
{
	DeserializeArray<int8_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(int16_t * const values, uint32_t itemCount)
{
	DeserializeArray<int16_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(int32_t * const values, uint32_t itemCount)
{
	DeserializeArray<int32_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(int64_t * const values, uint32_t itemCount)
{
	DeserializeArray<int64_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(uint8_t * const values, uint32_t itemCount)
{
	DeserializeArray<uint8_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(uint16_t * const values, uint32_t itemCount)
{
	DeserializeArray<uint16_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(uint32_t * const values, uint32_t itemCount)
{
	DeserializeArray<uint32_t>(values, itemCount);
}
void TextDeserializer::DeserializeArray(uint64_t * const values, uint32_t itemCount)
{
	DeserializeArray<uint64_t>(values, itemCount);
}

void TextDeserializer::DeserializeArray(std::string * const values, uint32_t itemCount) 
{
	DeserializeArray<std::string>(values, itemCount);
}
void TextDeserializer::DeserializeArray(std::wstring * const values, uint32_t itemCount)
{
	DeserializeArray<std::wstring>(values, itemCount);
}

void TextDeserializer::DeserializeArray(ObjectBase * const values, uint32_t itemCount)
{
	DeserializeArray<ObjectBase>(values, itemCount);
}

} // namespace  Introspection
