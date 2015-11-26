#ifndef INTROSPECTION_TEXTDESERIALIZER_H
#define INTROSPECTION_TEXTDESERIALIZER_H

#pragma once

namespace  Introspection
{
#pragma once

class TextDeserializer :
	public IDeserializer
{
public:
	TextDeserializer(std::wistream & stream);
	virtual ~TextDeserializer();

	virtual void Deserialize(ObjectBase & object) override;
	virtual void Deserialize(ObjectBase * & object) override;

	virtual void Deserialize(bool & value) override;
	virtual void Deserialize(char & value) override;
	virtual void Deserialize(wchar_t & value) override;
	virtual void Deserialize(float & value) override;
	virtual void Deserialize(double & value) override;

	virtual void Deserialize(int8_t & value) override;
	virtual void Deserialize(int16_t & value) override;
	virtual void Deserialize(int32_t & value) override;
	virtual void Deserialize(uint8_t & value) override;
	virtual void Deserialize(uint16_t & value) override;
	virtual void Deserialize(uint32_t & value) override;
	virtual void Deserialize(int64_t & value) override;
	virtual void Deserialize(uint64_t & value) override;

	virtual void Deserialize(std::string & value) override;
	virtual void Deserialize(std::wstring & value) override;

	virtual void Deserialize(char * & value) override;
	virtual void Deserialize(wchar_t * & value) override;

	virtual void Deserialize(bool * & value) override;
	virtual void Deserialize(float * & value) override;
	virtual void Deserialize(double * & value) override;

	virtual void Deserialize(int8_t * & value) override;
	virtual void Deserialize(int16_t * & value) override;
	virtual void Deserialize(int32_t * & value) override;
	virtual void Deserialize(int64_t * & value) override;
	virtual void Deserialize(uint8_t * & value) override;
	virtual void Deserialize(uint16_t * & value) override;
	virtual void Deserialize(uint32_t * & value) override;
	virtual void Deserialize(uint64_t * & value) override;

	virtual void Deserialize(std::string * & value) override;
	virtual void Deserialize(std::wstring * & value) override;

	virtual void DeserializeArray(bool * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(char * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(wchar_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(float * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(double * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(int8_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int16_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int32_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(int64_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint8_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint16_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint32_t * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(uint64_t * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(std::string * const values, uint32_t itemCount) override;
	virtual void DeserializeArray(std::wstring * const values, uint32_t itemCount) override;

	virtual void DeserializeArray(ObjectBase * const values, uint32_t itemCount) override;

protected:

	//bool IgnoreWhiteChars();
	//bool ReadWord(std::wstring& word);
	//bool ReadString(std::wstring& word);
	//bool Read4DigitsHex(wchar_t& value);


	enum class TokenType
	{
		Undefined_Token = -1,
		Not_A_Token = 0,
		LeftBrace,
		RightBrace,
		LeftBracket,
		RightBracket,
		Colon,
		Comma,
		String,
		Number,
		TrueLiteral,
		FalseLiteral,
		NullLiteral,
	};

	class Token
	{
	public:
		Token()
			: _type(TokenType::Not_A_Token)
			, _pos(-1)
		{
		}

		~Token() {}

		void Reset()
		{
			_type = TokenType::Not_A_Token;
			_pos = std::istream::pos_type(-1);
			_text.clear();
		}

		bool IsAValideToken() const
		{
			return _type != TokenType::Not_A_Token && _type != TokenType::Undefined_Token;
		}

		bool IsAValueToken() const
		{
			return _type >= TokenType::String || _type == TokenType::LeftBrace || _type == TokenType::LeftBracket;
		}

		std::istream::pos_type _pos;
		TokenType _type;
		std::wstring _text;
	};

	enum class LexerState
	{
		Initial,
		String,
		Litteral_False,
		Litteral_True,
		Litteral_Null,
		Number,
		Undefined_Token
	};

	void LogError() {}

	void ParseToken(Token& tk);

	void ParseToken_UntilNextToken(Token& tk);
	void ParseToken_String(Token& tk);
	void ParseToken_Number(Token& tk);

	bool ParseToken_InitialState(Token & tk);

	template<typename T>
	void DeserializeInteger(T& value)
	{
		if (_current._type == TokenType::Number)
		{
			value = (T)std::stoll(_current._text);
			_current.Reset();
		}
		else
		{
			LogError();
		}
	}

	template<typename T>
	void DeserializeUnsignedInteger(T& value)
	{
		if (_current._type == TokenType::Number)
		{
			value = (T)std::stoull(_current._text);
			_current.Reset();
		}
		else
		{
			LogError();
		}
	}

	template<typename T>
	void DeserializeFloat(T& value)
	{
		if (_current._type == TokenType::Number)
		{
			value = (T)std::stold(_current._text);
			_current.Reset();
		}
		else
		{
			LogError();
		}
	}


	template<typename T>
	void DeserializeBasicTypePointer(T*& value)
	{
		if (value == nullptr)
			value = new T;
		Deserialize(*value);
	}

	template<typename T>
	void DeserializeArray(T* const values, uint32_t itemCount)
	{
		if (_current._type == TokenType::Not_A_Token)
			ParseToken(_current);

		if (_current._type == TokenType::LeftBracket)
		{
			_current.Reset();

			uint32_t i = 0;
			bool end = i >= itemCount;
			while (!end)
			{
				if (_current._type == TokenType::Not_A_Token)
					ParseToken(_current);

				switch (_current._type)
				{
				case TokenType::RightBrace:
					end = true;
					break;

				case TokenType::Number:
				case TokenType::String:
				case TokenType::FalseLiteral:
				case TokenType::TrueLiteral:
					Deserialize(values[i]);
					++i;
					break;

				case TokenType::Comma:
					_current.Reset();
					++i;
					break;

				default:
					end = true;
					LogError();
					break;
				}

				end = i >= itemCount || _stream.eof();
			}
		}
		else
		{
			LogError();
		}
	}


	template<typename T>
	void DeserializeArrayPointer(T** const values, uint32_t itemCount)
	{
		DeserializeArray<T*>(values, itemCount);
	}

	std::wistream& _stream;

	Token _current;

	LexerState _lexerState = LexerState::Initial;
	const wchar_t* _nullToken = L"null";
	const wchar_t* _falseToken = L"false";
	const wchar_t* _trueToken = L"true";

};


} // namespace  Introspection
#endif // INTROSPECTION_TEXTDESERIALIZER_H