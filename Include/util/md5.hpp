#pragma once
#include <string>
#include <cstring>
#include "../common/define.hpp"

/**
* @The implement of md5.
* @author Jiewei Wei
* @mail weijieweijerry@163.com
* @github https://github.com/JieweiWei
* @data Oct 19 2014
* @Alter by lianglin 08.25 2020 
*/

/* padding for calculate. */
static const shine::uint8 PADDING[64] = { 0x80 };

/* Hex numbers. */
static const shine::int8 HEX_NUMBERS[16] =
{
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

namespace shine
{
    class md5
    {
    public:
        /* Construct a MD5 object with a string. */
        md5(const std::string& message)
        {
            finished = false;
            /* Reset number of bits. */
            count[0] = count[1] = 0;
            /* Initialization constants. */
            state[0] = 0x67452301;
            state[1] = 0xefcdab89;
            state[2] = 0x98badcfe;
            state[3] = 0x10325476;

            /* Initialization the object according to message. */
            init((const uint8*)message.c_str(), message.length());
        }

        /* Convert digest to string value */
        std::string to_string_16()
        {
            return to_string_32().substr(8, 16);
        }

        std::string to_string_32()
        {
            const uint8* digest_ = getDigest();
            std::string str;
            str.reserve(16 << 1);
            for (size_t i = 0; i < 16; ++i)
            {
                int t = digest_[i];
                int a = t / 16;
                int b = t % 16;
                str.append(1, HEX_NUMBERS[a]);
                str.append(1, HEX_NUMBERS[b]);
            }
            return str;
        }

    private:
        /* Generate md5 digest. */
        const uint8* getDigest()
        {
            if (!finished)
            {
                finished = true;

                uint8 bits[8];
                uint32 oldState[4];
                uint32 oldCount[2];
                uint32 index, padLen;

                /* Save current state and count. */
                memcpy(oldState, state, 16);
                memcpy(oldCount, count, 8);

                /* Save number of bits */
                encode(count, bits, 8);

                /* Pad out to 56 mod 64. */
                index = (uint32)((count[0] >> 3) & 0x3f);
                padLen = (index < 56) ? (56 - index) : (120 - index);
                init((const uint8*)PADDING, padLen);

                /* Append length (before padding) */
                init(bits, 8);

                /* Store state in digest */
                encode(state, digest, 16);

                /* Restore current state and count. */
                memcpy(state, oldState, 16);
                memcpy(count, oldCount, 8);
            }
            return digest;
        }



        /* Initialization the md5 object, processing another message block,
        * and updating the context.*/
        void init(const uint8* input, size_t len)
        {

            uint32 i, index, partLen;

            finished = false;

            /* Compute number of uint8s mod 64 */
            index = (uint32)((count[0] >> 3) & 0x3f);

            /* update number of bits */
            if ((count[0] += ((uint32)len << 3)) < ((uint32)len << 3)) {
                ++count[1];
            }
            count[1] += ((uint32)len >> 29);

            partLen = 64 - index;

            /* transform as many times as possible. */
            if (len >= partLen)
            {
                memcpy(&buffer[index], input, partLen);
                transform(buffer);

                for (i = partLen; i + 63 < len; i += 64)
                {
                    transform(&input[i]);
                }
                index = 0;

            }
            else {
                i = 0;
            }

            /* Buffer remaining input */
            memcpy(&buffer[index], &input[i], len - i);
        }

        /* MD5 basic transformation. Transforms state based on block. */
        void transform(const uint8 block[64])
        {
            uint32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

            decode(block, x, 64);

#define lin_s11 7
#define lin_s12 12
#define lin_s13 17
#define lin_s14 22
#define lin_s21 5
#define lin_s22 9
#define lin_s23 14
#define lin_s24 20
#define lin_s31 4
#define lin_s32 11
#define lin_s33 16
#define lin_s34 23
#define lin_s41 6
#define lin_s42 10
#define lin_s43 15
#define lin_s44 21


#define lin_ROTATELEFT(num, n) (((num) << (n)) | ((num) >> (32-(n))))
#define lin_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define lin_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define lin_H(x, y, z) ((x) ^ (y) ^ (z))
#define lin_I(x, y, z) ((y) ^ ((x) | (~z)))

#define lin_FF(a, b, c, d, x, s, ac) { \
    (a) += lin_F((b), (c), (d)) + (x)+ac; \
    (a) = lin_ROTATELEFT((a), (s)); \
    (a) += (b); \
            }
            /* Round 1 */
            lin_FF(a, b, c, d, x[0], lin_s11, 0xd76aa478);
            lin_FF(d, a, b, c, x[1], lin_s12, 0xe8c7b756);
            lin_FF(c, d, a, b, x[2], lin_s13, 0x242070db);
            lin_FF(b, c, d, a, x[3], lin_s14, 0xc1bdceee);
            lin_FF(a, b, c, d, x[4], lin_s11, 0xf57c0faf);
            lin_FF(d, a, b, c, x[5], lin_s12, 0x4787c62a);
            lin_FF(c, d, a, b, x[6], lin_s13, 0xa8304613);
            lin_FF(b, c, d, a, x[7], lin_s14, 0xfd469501);
            lin_FF(a, b, c, d, x[8], lin_s11, 0x698098d8);
            lin_FF(d, a, b, c, x[9], lin_s12, 0x8b44f7af);
            lin_FF(c, d, a, b, x[10], lin_s13, 0xffff5bb1);
            lin_FF(b, c, d, a, x[11], lin_s14, 0x895cd7be);
            lin_FF(a, b, c, d, x[12], lin_s11, 0x6b901122);
            lin_FF(d, a, b, c, x[13], lin_s12, 0xfd987193);
            lin_FF(c, d, a, b, x[14], lin_s13, 0xa679438e);
            lin_FF(b, c, d, a, x[15], lin_s14, 0x49b40821);


#define lin_GG(a, b, c, d, x, s, ac) { \
    (a) += lin_G((b), (c), (d)) + (x)+ac; \
    (a) = lin_ROTATELEFT((a), (s)); \
    (a) += (b); \
            }
            /* Round 2 */
            lin_GG(a, b, c, d, x[1], lin_s21, 0xf61e2562);
            lin_GG(d, a, b, c, x[6], lin_s22, 0xc040b340);
            lin_GG(c, d, a, b, x[11], lin_s23, 0x265e5a51);
            lin_GG(b, c, d, a, x[0], lin_s24, 0xe9b6c7aa);
            lin_GG(a, b, c, d, x[5], lin_s21, 0xd62f105d);
            lin_GG(d, a, b, c, x[10], lin_s22, 0x2441453);
            lin_GG(c, d, a, b, x[15], lin_s23, 0xd8a1e681);
            lin_GG(b, c, d, a, x[4], lin_s24, 0xe7d3fbc8);
            lin_GG(a, b, c, d, x[9], lin_s21, 0x21e1cde6);
            lin_GG(d, a, b, c, x[14], lin_s22, 0xc33707d6);
            lin_GG(c, d, a, b, x[3], lin_s23, 0xf4d50d87);
            lin_GG(b, c, d, a, x[8], lin_s24, 0x455a14ed);
            lin_GG(a, b, c, d, x[13], lin_s21, 0xa9e3e905);
            lin_GG(d, a, b, c, x[2], lin_s22, 0xfcefa3f8);
            lin_GG(c, d, a, b, x[7], lin_s23, 0x676f02d9);
            lin_GG(b, c, d, a, x[12], lin_s24, 0x8d2a4c8a);


#define lin_HH(a, b, c, d, x, s, ac) { \
    (a) += lin_H((b), (c), (d)) + (x)+ac; \
    (a) = lin_ROTATELEFT((a), (s)); \
    (a) += (b); \
            }
            /* Round 3 */
            lin_HH(a, b, c, d, x[5], lin_s31, 0xfffa3942);
            lin_HH(d, a, b, c, x[8], lin_s32, 0x8771f681);
            lin_HH(c, d, a, b, x[11], lin_s33, 0x6d9d6122);
            lin_HH(b, c, d, a, x[14], lin_s34, 0xfde5380c);
            lin_HH(a, b, c, d, x[1], lin_s31, 0xa4beea44);
            lin_HH(d, a, b, c, x[4], lin_s32, 0x4bdecfa9);
            lin_HH(c, d, a, b, x[7], lin_s33, 0xf6bb4b60);
            lin_HH(b, c, d, a, x[10], lin_s34, 0xbebfbc70);
            lin_HH(a, b, c, d, x[13], lin_s31, 0x289b7ec6);
            lin_HH(d, a, b, c, x[0], lin_s32, 0xeaa127fa);
            lin_HH(c, d, a, b, x[3], lin_s33, 0xd4ef3085);
            lin_HH(b, c, d, a, x[6], lin_s34, 0x4881d05);
            lin_HH(a, b, c, d, x[9], lin_s31, 0xd9d4d039);
            lin_HH(d, a, b, c, x[12], lin_s32, 0xe6db99e5);
            lin_HH(c, d, a, b, x[15], lin_s33, 0x1fa27cf8);
            lin_HH(b, c, d, a, x[2], lin_s34, 0xc4ac5665);


#define lin_II(a, b, c, d, x, s, ac) { \
    (a) += lin_I((b), (c), (d)) + (x)+ac; \
    (a) = lin_ROTATELEFT((a), (s)); \
    (a) += (b); \
            }
            /* Round 4 */
            lin_II(a, b, c, d, x[0], lin_s41, 0xf4292244);
            lin_II(d, a, b, c, x[7], lin_s42, 0x432aff97);
            lin_II(c, d, a, b, x[14], lin_s43, 0xab9423a7);
            lin_II(b, c, d, a, x[5], lin_s44, 0xfc93a039);
            lin_II(a, b, c, d, x[12], lin_s41, 0x655b59c3);
            lin_II(d, a, b, c, x[3], lin_s42, 0x8f0ccc92);
            lin_II(c, d, a, b, x[10], lin_s43, 0xffeff47d);
            lin_II(b, c, d, a, x[1], lin_s44, 0x85845dd1);
            lin_II(a, b, c, d, x[8], lin_s41, 0x6fa87e4f);
            lin_II(d, a, b, c, x[15], lin_s42, 0xfe2ce6e0);
            lin_II(c, d, a, b, x[6], lin_s43, 0xa3014314);
            lin_II(b, c, d, a, x[13], lin_s44, 0x4e0811a1);
            lin_II(a, b, c, d, x[4], lin_s41, 0xf7537e82);
            lin_II(d, a, b, c, x[11], lin_s42, 0xbd3af235);
            lin_II(c, d, a, b, x[2], lin_s43, 0x2ad7d2bb);
            lin_II(b, c, d, a, x[9], lin_s44, 0xeb86d391);

            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
        }

        /* Encodes input (usigned long) into output (uint8). */
        void encode(const uint32* input, uint8* output, size_t length)
        {

            for (size_t i = 0, j = 0; j < length; ++i, j += 4)
            {
                output[j] = (uint8)(input[i] & 0xff);
                output[j + 1] = (uint8)((input[i] >> 8) & 0xff);
                output[j + 2] = (uint8)((input[i] >> 16) & 0xff);
                output[j + 3] = (uint8)((input[i] >> 24) & 0xff);
            }
        }

        /* Decodes input (uint8) into output (usigned long). */
        void decode(const uint8* input, uint32* output, size_t length)
        {
            for (size_t i = 0, j = 0; j < length; ++i, j += 4)
            {
                output[i] = ((uint32)input[j]) | (((uint32)input[j + 1]) << 8) |
                    (((uint32)input[j + 2]) << 16) | (((uint32)input[j + 3]) << 24);
            }
        }

    private:
        /* Flag for mark whether calculate finished. */
        bool finished;

        /* state (ABCD). */
        uint32 state[4];

        /* number of bits, low-order word first. */
        uint32 count[2];

        /* input buffer. */
        uint8 buffer[64];

        /* message digest. */
        uint8 digest[16];

    };
}

