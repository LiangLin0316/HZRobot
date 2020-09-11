#pragma once
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <vector>
#include <set>
#include <algorithm>
#include "../common/define.hpp"
#include "tool.hpp"
#include "md5.hpp"
#if !(defined SHINE_OS_ANDROID)
#if (defined SHINE_OS_LINUX || defined SHINE_OS_APPLE)
#include <iconv.h>
#endif
#endif

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static const char* shine_url_encode_table[256] = {
	"%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
	"%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F",
	"%20", "!", "%22", "%23", "$", "%25", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "%3C", "=", "%3E", "?",
	"@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
	"P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "%5B", "%5C", "%5D", "%5E", "_",
	"%60", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
	"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "%7B", "%7C", "%7D", "%7E", "%7F",
	"%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F",
	"%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F",
	"%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF",
	"%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF",
	"%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF",
	"%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF",
	"%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF",
	"%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF"
};

static unsigned char shine_url_decode_table[256] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
	0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0, 10, 11, 12, 13, 14, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

namespace shine
{
    class string : public std::string
    {
    public:
        string(){}
        string(const int8* str) : std::string(str == 0 ? "" : str) {}
        string(const std::string &other) : std::string(other){}
        string(std::string &&other) : std::string(other){}
        string(const string &other) : std::string(other){}
        string(string &&other) : std::string(other){}

        string (int16 v){
            std::string::assign(std::move(number_to_string("%d", v)));
        }

        string (uint16 v){
            std::string::assign(std::move(number_to_string("%u", v)));
        }

        string(int32 v){
            std::string::assign(std::move(number_to_string("%d", v)));
        }

        string(uint32 v){
            std::string::assign(std::move(number_to_string("%u", v)));
        }

        string(Long v){
            std::string::assign(std::move(number_to_string("%ld", v)));
        }

        string(uLong v){
            std::string::assign(std::move(number_to_string("%lu", v)));
        }

        string (int64 v){
            std::string::assign(std::move(number_to_string("%lld", v)));
        }

        string (uint64 v){
            std::string::assign(std::move(number_to_string("%lu", v)));
        }

//         string(size_type v){
//             std::string::assign(std::to_string(v));
//         }

        string (Float v){
            std::string::assign(std::move(number_to_string("%f", v)));
        }

        string(Double v){
            std::string::assign(std::move(number_to_string("%f", v)));
        }

        string(LDouble v){
            std::string::assign(std::move(number_to_string("%Lf", v)));
        }

        template<class T>
        std::string number_to_string(const int8 *fmt, const T &num) const {
            int8 buf[64];
            size_t len = SHINE_SNPRINTF(buf, sizeof(buf), fmt, num);
            char *pos = strstr(buf, ".");
            if (pos != NULL)
            {
                pos += 2;
                while (len > (size_t)(pos - buf) && buf[--len] == '0')
                    buf[len] = '\0';
            }

            return buf;
        }

        int16 to_int16(){
            return (int16)::atoi(this->c_str());
        }

        uint16 to_uint16() const {
            return (uint16)::atoi(this->c_str());
        }

        int32 to_int32() const{
            return ::atoi(this->c_str());
        }

        uint32 to_uint32() const {
            return (uint32)::atoi(this->c_str());
        }

        Long to_long() const {
            return (Long)::atol(this->c_str());
        }

        uLong to_ulong() const {
            return (uLong)::atol(this->c_str());
        }

        int64 to_int64() const {
            return ::atoll(this->c_str());
        }

        uint64 to_uint64() const {
            return (uint64)::atoll(this->c_str());
        }

        Float to_float() const {
            return (Float)::atof(this->c_str());
        }

        Double to_double() const {
            return ::atof(this->c_str());
        }

        LDouble to_long_double() const {
            return (LDouble)::atof(this->c_str());
        }

		operator const char*() const {
			return c_str();
		}

		operator char*() const {
			return (char*)data();
		}

		operator void*() const {
			return (void*)data();
		}

		char & operator [] (int32 pos) {
			return at(pos);
		}

		const char& operator [] (int32 pos) const {
			return at(pos);
		}

        string &operator=(const string &v){
            std::string::assign(v.data(), v.size());
            return *this;
        }

        string &operator=(int8 v){
            std::string::assign(1, v);
            return *this;
        }

        string &operator=(uint8 v){
            std::string::assign(1, v);
            return *this;
        }

        string &operator=(int16 v){
            std::string::assign(number_to_string("%d", v));
            return *this;
        }

        string &operator=(uint16 v){
            std::string::assign(number_to_string("%u", v));
            return *this;
        }

        string &operator=(int32 v){

            std::string::assign(number_to_string("%d", v));
            return *this;
        }

        string &operator=(uint32 v){
            std::string::assign(number_to_string("%u", v));
            return *this;
        }

        string &operator=(Long v){

            std::string::assign(number_to_string("%ld", v));
            return *this;
        }

        string &operator=(uLong v){
            std::string::assign(number_to_string("%lu", v));
            return *this;
        }

        string &operator=(int64 v){
            std::string::assign(number_to_string("%lld", v));
            return *this;
        }

        string &operator=(uint64 v){
            std::string::assign(number_to_string("%lu", v));
            return *this;
        }

        string &operator=(Float v){
            std::string::assign(number_to_string("%f", v));
            return *this;
        }

        string &operator=(Double v){
            std::string::assign(number_to_string("%f", v));
            return *this;
        }

        string &operator=(LDouble v){
            std::string::assign(number_to_string("%Lf", v));
            return *this;
        }

        string &operator<<(const int8 *v){
            std::string::append(v);
            return *this;
        }

        string &operator<<(int8 v){
            std::string::append(1, v);
            return *this;
        }

        string &operator<<(uint8 v){
            std::string::append(1, v);
            return *this;
        }

        string &operator<<(int16 v){
            std::string::append(number_to_string("%d", v));
            return *this;
        }

        string &operator<<(uint16 v){
            std::string::append(number_to_string("%u", v));
            return *this;
        }

        string &operator<<(int32 v){
            std::string::append(number_to_string("%d", v));
            return *this;
        }

        string &operator<<(uint32 v){
            std::string::append(number_to_string("%u", v));
            return *this;
        }

        string &operator<<(Long v){
            std::string::append(number_to_string("%ld", v));
            return *this;
        }

        string &operator<<(uLong v){
            std::string::append(number_to_string("%lu", v));
            return *this;
        }

        string &operator<<(int64 v){
            std::string::append(number_to_string("%lld", v));
            return *this;
        }

        string &operator<<(uint64 v){
            std::string::append(number_to_string("%lu", v));
            return *this;
        }

        string &operator<<(Float v){
            std::string::append(number_to_string("%f", v));
            return *this;
        }

        string &operator<<(Double v){
            std::string::append(number_to_string("%f", v));
            return *this;
        }

        string &operator<<(LDouble v){
            std::string::append(number_to_string("%Lf", v));
            return *this;
        }

        string &operator+=(int8 v){
            std::string::append(1, v);
            return *this;
        }

        string &operator+=(uint8 v){
            std::string::append(1, v);
            return *this;
        }

        string &operator+=(int16 v){
            std::string::append(number_to_string("%d", v));
            return *this;
        }

        string &operator+=(uint16 v){
            std::string::append(number_to_string("%u", v));
            return *this;
        }

        string &operator+=(int32 v){
            std::string::append(number_to_string("%d", v));
            return *this;
        }

        string &operator+=(uint32 v){
            std::string::append(number_to_string("%u", v));
            return *this;
        }

        string &operator+=(Long v){
            std::string::append(number_to_string("%ld", v));
            return *this;
        }

        string &operator+=(uLong v){
            std::string::append(number_to_string("%lu", v));
            return *this;
        }

        string &operator+=(int64 v){
            std::string::append(number_to_string("%lld", v));
            return *this;
        }

        string &operator+=(uint64 v){
            std::string::append(number_to_string("%lu", v));
            return *this;
        }

        string &operator+=(Float v){
            std::string::append(number_to_string("%f", v));
            return *this;
        }

        string &operator+=(Double v){
            std::string::append(number_to_string("%f", v));
            return *this;
        }

        string &operator+=(LDouble v){
            std::string::append(number_to_string("%Lf", v));
            return *this;
        }

        string &operator+=(const int8 *v){
            std::string::append(v);
            return *this;
        }

        string &operator+=(const string &v){
            std::string::append(v.data(), v.size());
            return *this;
        }

		static std::vector<string> split(const string &str, const std::string &des) {
			std::vector<string> ret;
			std::string::size_type des_len = des.size();
			std::string::size_type begin = 0;
			std::string::size_type end = str.find(des, begin);

			while (end != std::string::npos)
			{
				ret.push_back(str.substr(begin, end - begin));
				begin = end + des_len;
				end = str.find(des, begin);
			}

			ret.push_back(str.substr(begin, end));

			return ret;
		}

		std::vector<string> split(const std::string &des) const {
			return std::move(split(*this, des));
		}


		static std::set<string> split2(const string &str, const std::string &des) {
			std::set<string> ret;
			std::string::size_type des_len = des.size();
			std::string::size_type begin = 0;
			std::string::size_type end = str.find(des, begin);

			while (end != std::string::npos)
			{
				ret.emplace(str.substr(begin, end - begin));
				begin = end + des_len;
				end = str.find(des, begin);
			}

			ret.emplace(str.substr(begin, end));

			return ret;
		}

		std::set<string> split2(const std::string &des) const {
			return std::move(split2(*this, des));
		}

		static size_type assert_size(const int8 *fmt, va_list args)
        {
#if (defined SHINE_OS_WINDOWS)
#pragma warning(disable:4996)
#endif
            return vsnprintf(0, 0, fmt, args);
        }

        static void format(string &str, const int8 *fmt, va_list args)
        {
            SHINE_VSNPRINTF((char*)str.c_str(), str.size(), fmt, args);
        }

        static string format_create(const int8 *fmt, ...){
            string ret;
            va_list args;
            va_start(args, fmt);
            size_type size = assert_size(fmt, args);
            va_end(args);

            va_start(args, fmt);
            ret.resize(size + 1);
            format(ret, fmt, args);
            va_end(args);

            ret.resize(size);
            return std::move(ret);
        }

        string& format(const int8 *fmt, ...){
            va_list args;
            va_start(args, fmt);
            size_type size = assert_size(fmt, args);
            va_end(args);

            va_start(args, fmt);
            resize(size + 1);
            format(*this, fmt, args);
            va_end(args);

            resize(size);

            return *this;
        }


        string& format_append(const int8 *fmt, ...){
            string tmp;
            va_list args;
            va_start(args, fmt);
            size_type size = assert_size(fmt, args);
            va_end(args);

            va_start(args, fmt);
            tmp.resize(size + 1);
            format(tmp, fmt, args);
            va_end(args);
            tmp.resize(size);

            append(tmp);
            return *this;

        }

        static void replace_all(string &str, const std::string &src, const std::string &des) {
            string::size_type pos = 0;
            string::size_type src_len = src.size();
            string::size_type des_len = des.size();
            pos = str.find(src, pos);
            while ((pos != string::npos))
            {
                str.replace(pos, src_len, des);
                pos = str.find(src, (pos + des_len));
            }
        }

        string& replace_all(const std::string &src, const std::string &des) {
            replace_all(*this, src, des);
            return *this;
        }

        string& to_upper(){
            std::transform(begin(), end(), begin(), ::toupper);
            return *this;
        }

        string& to_lower(){
            std::transform(begin(), end(), begin(), ::tolower);
            return *this;
        }

        string& trim_left() {
            if (!empty())
                erase(0, find_first_not_of(" \n\r\t"));

            return *this;
        }

        string& trim_right() {
            if (!empty())
                erase(find_last_not_of(" \n\r\t") + 1);

            return *this;
        }

        string& trim() {
            return trim_left().trim_right();
        }

        bool contains(const char* text) const {
            return find(text) != std::string::npos;
        }

		static bool is_utf8(const char* str, std::size_t length)
		{
			std::size_t i = 0;
			unsigned char bytes = 0;
			unsigned char chr;
			bool all_ascii = true; 

			for (i = 0; i < length; i++)
			{
				chr = *(str + i);

				if ((chr & 0x80) != 0)
					all_ascii = false;

				if (bytes == 0) 
				{
					if (chr >= 0x80)
					{
						if (chr >= 0xFC && chr <= 0xFD)
							bytes = 6;
						else if (chr >= 0xF8)
							bytes = 5;
						else if (chr >= 0xF0)
							bytes = 4;
						else if (chr >= 0xE0)
							bytes = 3;
						else if (chr >= 0xC0)
							bytes = 2;
						else
						{
							return false;
						}
						bytes--;
					}
				}
				else
				{
					if ((chr & 0xC0) != 0x80)
					{
						return false;
					}
					bytes--;
				}
			}

			if (bytes > 0)
			{
				return false;
			}

			if (all_ascii) 
			{
				return false;
			}

			return true;
		}


#if !(defined SHINE_OS_ANDROID)
#ifdef SHINE_OS_WINDOWS
		static void gbk_to_utf8(const string& gbk, string &utf8)
		{
			WCHAR * str1;
			int n = MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);
			str1 = new WCHAR[n];
			MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, str1, n);
			n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
			char * str2 = new char[n];
			WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
			utf8 = str2;
			delete[]str1;
			str1 = NULL;
			delete[]str2;
			str2 = NULL;
		}

		static void utf8_to_gbk(const string& utf8, string &gbk)
		{
			int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
			unsigned short * wsz_gbk = new unsigned short[len + 1];
			memset(wsz_gbk, 0, len * 2 + 2);
			MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)utf8.c_str(), -1, (LPWSTR)wsz_gbk, len);

			len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wsz_gbk, -1, NULL, 0, NULL, NULL);
			char *sz_gbk = new char[len + 1];
			memset(sz_gbk, 0, len + 1);
			WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wsz_gbk, -1, sz_gbk, len, NULL, NULL);

			gbk = sz_gbk;
			delete[]sz_gbk;
			delete[]wsz_gbk;
		}
#elif (defined SHINE_OS_LINUX || defined SHINE_OS_APPLE)
		static void convert(const char* from, const char *to, const string &input, string &output) {
			output.clear();
			size_t input_len = input.length();
			if (input_len == 0)
				return;

			size_t output_len = 2 * input_len + 1;
			char *p_buf = new char[output_len];
			memset(p_buf, 0, output_len);
			iconv_t cd;
			char *p_input = (char *)input.c_str();
			char *p_out = p_buf;
			cd = iconv_open(to, from);
			iconv(cd, &p_input, &input_len, &p_buf, &output_len);
			iconv_close(cd);
			output = p_out;
			delete []p_out;
		}

		static void gbk_to_utf8(const string& gbk, string& utf8)
		{
			convert("GBK", "utf8", gbk, utf8);
		}

		static void utf8_to_gbk(const string& utf8, string& gbk)
		{
			convert("utf8", "GBK", utf8, gbk);
		}

#endif
		string gbk_to_utf8() const {
			string utf8;
			gbk_to_utf8(*this, utf8);
			return std::move(utf8);
		}

		string utf8_to_gbk() const {
			string gbk;
			utf8_to_gbk(*this, gbk);
			return std::move(gbk);
		}
#endif
		static string print_hex_string(const string &str) {
            static const uint8 buf[] = "0123456789ABCDEF";
            string ret;
            for (size_t i = 0; i < str.size(); i++)
            {
                if (i > 0 && i % 8 == 0)
                    ret += ' ';

                uint32 index = (uint8)str[i];
                ret += buf[index >> 4];
                ret += buf[index & 0xF];
                ret += ' ';

                if (i > 0 && i % 32 == 0)
                    ret += "\n";
            }

            return std::move(ret);
        }

        string md5_16() const {
            return md5_16(*this);
        }

        static string md5_16(const string &str){
            return md5(str).to_string_16();
        }

        string md5_32() const {
            return md5_32(*this);
        }

        static string md5_32(const string &str){
            return md5(str).to_string_32();
        }

        string encode_base64(){
            return encode_base64(*this);
        }

        static string encode_base64(const string &data) {
            const uint8* bytes_to_encode = (const uint8*)data.data();
            size_t in_len = data.size();

            string ret;
            int32 i = 0;
            int32 j = 0;
            uint8 char_array_3[3];
            uint8 char_array_4[4];

            while (in_len--) {
                char_array_3[i++] = *(bytes_to_encode++);
                if (i == 3) {
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;

                    for (i = 0; (i < 4); i++)
                        ret += base64_chars[char_array_4[i]];
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++)
                    char_array_3[j] = '\0';

                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (j = 0; (j < i + 1); j++)
                    ret += base64_chars[char_array_4[j]];

                while ((i++ < 3))
                    ret += '=';

            }

            return std::move(ret);

        }


        string decode_base64(){
            return decode_base64(*this);
        }

        static string decode_base64(const string &data) {
            size_type in_len = data.size();
            int32 i = 0;
            int32 j = 0;
            int32 in_ = 0;
            uint8 char_array_4[4], char_array_3[3];
            string ret;

            while (in_len-- && (data[in_] != '=') && is_base64(data[in_])) {
                char_array_4[i++] = data[in_]; in_++;
                if (i == 4) {
                    for (i = 0; i < 4; i++)
                        char_array_4[i] = static_cast<uint8>(base64_chars.find(char_array_4[i]));

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (i = 0; (i < 3); i++)
                        ret += char_array_3[i];
                    i = 0;
                }
            }

            if (i) {
                for (j = i; j < 4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j < 4; j++)
                    char_array_4[j] = static_cast<uint8>(base64_chars.find(char_array_4[j]));

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
            }

            return std::move(ret);
        }

		static string encode_url(const char *url) {
			string ret;

			unsigned char *data = (unsigned char *)url;
			while (*data)
			{
				ret.append(shine_url_encode_table[*data++]);
			}

			return std::move(ret);
		}

		static bool decode_url(string &ret, const char *url) {
			ret.clear();
			int len = strlen(url);
			int pos = 0;
			unsigned char *data = (unsigned char *)url;
			while (data[pos])
			{
				if (data[pos] == '%')
				{
					if (len >= pos + 2 && isalnum(data[pos + 1]) && isalnum(data[pos + 2]))
					{
						ret += (unsigned char)((shine_url_decode_table[(unsigned char)data[pos + 1]] << 4)
							+ shine_url_decode_table[(unsigned char)data[pos + 2]]);
						pos += 3;
					}
					else {
						return false;
					}
				}
				else {
					ret += data[pos++];
				}
			}

			return true;
		}

    private:
        static inline bool is_base64(uint8 c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }
    };

    struct cmp_string
    {
        bool operator()(const string &rc1, const string &rc2) const
        {
            return rc1 == rc2;
        }
    };

}

/*

namespace std{

// TEMPLATE STRUCT SPECIALIZATION hash
template<>
struct hash<shine::string>
    : public unary_function<std::string, size_t>
{	// hash functor for basic_string
    typedef shine::string _Kty;

    size_t operator()(const _Kty& _Keyval) const
    {	// hash _Keyval to size_t value by pseudorandomizing transform
        return (_Hash_seq((const unsigned char *)_Keyval.c_str(),
            _Keyval.size() * sizeof (char)));
    }
};

}*/
