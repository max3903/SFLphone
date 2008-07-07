/*********************************************************************
* Base64 - a simple base64 encoder and decoder.
*
* Copyright (c) 1999, Bob Withers - bwit@pobox.com
*
* This code may be freely used for any purpose, either personal
* or commercial, provided the authors copyright notice remains
* intact.
*********************************************************************/
/*********************************************************************
* file altered by : Christophe Tournayre
*********************************************************************/

#ifndef __LIBCONNECT_PROJECT__BASE64_HPP__
#define __LIBCONNECT_PROJECT__BASE64_HPP__

#include <string>

using namespace std;

/// coding namespace
//namespace coding
//{
	/// Filling character
	const char fillchar = '=';

	/// Base64 alphabet
	// 00000000001111111111222222
	// 01234567890123456789012345
	static std::string cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

	// 22223333333333444444444455
	// 67890123456789012345678901
	"abcdefghijklmnopqrstuvwxyz"

	// 555555556666
	// 234567890123
	"0123456789+/";

	/*! \class Base64
	\brief This class allow encoding and decoding of a string in base 64

	The goal of base64 coding is to use 4 printables characters
	(US-ASCII formated) to code a block of 3 unspecified Bytes
	(3*8 bits = 24 bits). By traversing the binary data from left to right,
	blocks of 24 bits are created by concatenating blocks of 3 data of 8 bits.
	Then, every 24 bits blocks are divided in 4 groups of 6 bits corresponding
	to 4 characters of the base64 alphabet.\n

	For example, the string "ahg" is represented by : \n
	a : 97 &nbsp; : 01100001\n
	h : 104 : 01101000\n
	g : 103 : 01100111\n
	Concatenated and separated in group of 6 bit it gives : 011000 010110
	100001 100111.\n
	So 24 22 33 39 correspond to YWhn (see correspondance in that file).
	*/
	class Base64
	{
		public:
			/*! \brief Function that encode a string in base64

			\param data : the string to encode
			\return string : the encoded string
			*/
			static std::string encode(const std::string & data);

			/*! \brief Function that decode a string from base64 encoding

			\param data : the string to decode
			\return string : the decoded string
			*/
			static std::string decode(const std::string & data);

		private:

	};
//}
#endif
