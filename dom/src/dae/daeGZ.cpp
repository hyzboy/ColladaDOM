/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the MIT Open Source License, for details please see license.txt or the website
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#include <ColladaDOM.inl> //PCH

#ifdef __COLLADA_DOM__ZAE

COLLADA_(namespace)
{//-.
//<-'


////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE 
////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE 
////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE ////INFLATE 


//https://github.com/uroni/miniz/blob/master/miniz_tinfl.c
//(changes made for readability/maintainability)
//REMINDER: MAY NEED TO SUPPORT CRC-32 CHECKSUMS
/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/
namespace //SCHEDULED FOR REMOVAL?
{
//Reminder: This is how Miniz defines these types ca. 2017.
typedef unsigned char /*mz_*/uint8_t;
typedef signed short /*mz_*/int16_t;
typedef unsigned short /*mz_*/uint16_t;
typedef unsigned int /*mz_*/uint32_t;
typedef long long /*mz_*/int64_t;
typedef unsigned long long /*mz_*/uint64_t;
static daeCTC<CHAR_BIT==8> _uint8_check;
static daeCTC<sizeof(uint16_t)*CHAR_BIT==16> _uint16_check;
static daeCTC<sizeof(uint32_t)*CHAR_BIT==32> _uint32_check;
#if COLLADA_UPTR_MAX==0xFFffFFffFFffFFffULL
#define TINFL_BITBUF_SIZE 64
#else
#define TINFL_BITBUF_SIZE 32
#endif
typedef uint32_t tinfl_bit_buf_t; //uint64_t
}
enum // Internal/private bits
{
  TINFL_MAX_HUFF_TABLES = 3, 
  TINFL_MAX_HUFF_SYMBOLS_0 = 288,
  TINFL_MAX_HUFF_SYMBOLS_1 = 32,
  TINFL_MAX_HUFF_SYMBOLS_2 = 19,
  TINFL_FAST_LOOKUP_BITS = 10, 
  TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS,
};
typedef struct _tinfl_huff_table //compiler
{
	uint8_t m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
	int16_t m_look_up[TINFL_FAST_LOOKUP_SIZE], m_tree[TINFL_MAX_HUFF_SYMBOLS_0*2];

}tinfl_huff_table;
typedef struct _tinfl_decompressor //compiler
{
	uint32_t /*m_state,*/ m_num_bits, m_zhdr0, m_zhdr1, m_z_adler32, m_final, m_type,
	m_check_adler32, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];

	tinfl_bit_buf_t m_bit_buf;
	size_t m_dist_from_out_buf_start;
	tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
	uint8_t m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0+TINFL_MAX_HUFF_SYMBOLS_1+137];

	uint32_t m_state; //HACK: VERY LAST for get_tinfl_decompressor_state_etc.
	//daeGZ
	//Miniz houses this and the dictionary separate from the decompressor, but I can't see
	//how the structures are separate when using TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF
	//mode. If all input is available and output fills the dictionary flush, then it can
	//be independent. But COLLADA-DOM doesn't have that luxury.
	struct
	{
		unsigned short wrapping_output_buf;

	}ext;

}tinfl_decompressor;
enum //UNUSED //UNUSED //UNUSED //UNUSED 
{
	//If set, the input has a valid zlib header and ends with an adler32 checksum (it's a valid zlib stream). Otherwise, the input is a raw deflate stream.	
	//TINFL_FLAG_PARSE_ZLIB_HEADER = 1,
	//If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input.
	TINFL_FLAG_HAS_MORE_INPUT = 2,
	//If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB).
	TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,	
	//TINFL_FLAG_COMPUTE_ADLER32 = 8,	
	//Force adler-32 checksum computation of the decompressed bytes.
};
typedef enum _tinfl_status //compiler
{
	TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS = -4,
	TINFL_STATUS_BAD_PARAM = -3,
	TINFL_STATUS_ADLER32_MISMATCH = -2,
	TINFL_STATUS_FAILED = -1,
	TINFL_STATUS_DONE = 0,
	TINFL_STATUS_NEEDS_MORE_INPUT = 1,
	TINFL_STATUS_HAS_MORE_OUTPUT = 2

}tinfl_status;
static tinfl_status tinfl_decompress(tinfl_decompressor*r, 
const uint8_t*pIn_buf_next, size_t*pIn_buf_size, uint8_t*pOut_buf_start, uint8_t*pOut_buf_next, size_t*pOut_buf_size
/*,const uint32_t decomp_flags*/)
{	enum{ decomp_flags=0 };

	tinfl_status status = TINFL_STATUS_FAILED; //out

	static const int s_length_base[31] = 
	{ 3,4,5,6,7,8,9,10,11,13, 15,17,19,23,27,31,35,43,51,59, 67,83,99,115,131,163,195,227,258,0,0 };
	static const int s_length_extra[31] = 
	{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };
	static const int s_dist_base[32] = 
	{ 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193, 257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};
	static const int s_dist_extra[32] = 
	{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
	static const uint8_t s_length_dezigzag[19] = 
	{ 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
	static const int s_min_table_sizes[3] = { 257,1,4 };
		
	uint32_t num_bits, dist, counter, num_extra; tinfl_bit_buf_t bit_buf;	
	const uint8_t *pIn_buf_cur = pIn_buf_next, *const pIn_buf_end = pIn_buf_next+*pIn_buf_size;
	uint8_t *pOut_buf_cur = pOut_buf_next, *const pOut_buf_end = pOut_buf_next+*pOut_buf_size; 
	size_t out_buf_size_mask = decomp_flags&TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF?size_t(-1):((pOut_buf_next-pOut_buf_start)+*pOut_buf_size)-1;
	size_t dist_from_out_buf_start;

	//Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter).
	if((out_buf_size_mask+1)&out_buf_size_mask||pOut_buf_next<pOut_buf_start){ *pIn_buf_size = *pOut_buf_size = 0; return TINFL_STATUS_BAD_PARAM; }

	num_bits = r->m_num_bits; bit_buf = r->m_bit_buf; dist = r->m_dist; counter = r->m_counter; num_extra = r->m_num_extra; 
	dist_from_out_buf_start = r->m_dist_from_out_buf_start;
	
	#define TINFL_CR_BEGIN switch(r->m_state){ default: assert(0); case 0:
	#define TINFL_CR_RETURN(state_index,result) \
	{ status = result; r->m_state = state_index; goto common_exit; case state_index:; }
	#define TINFL_CR_RETURN_FOREVER(state_index,result) \
	for(;;){ TINFL_CR_RETURN(state_index,result); }
	#define TINFL_CR_FINISH } 	
	#define TINFL_GET_BYTE(state_index,c) \
	{\
		while(pIn_buf_cur>=pIn_buf_end)\
		TINFL_CR_RETURN(state_index,decomp_flags&TINFL_FLAG_HAS_MORE_INPUT?TINFL_STATUS_NEEDS_MORE_INPUT:TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS)\
		c = *pIn_buf_cur++;\
	}
	#define TINFL_NEED_BITS(state_index,n) \
	do{ unsigned c; TINFL_GET_BYTE(state_index,c); bit_buf|=tinfl_bit_buf_t(c)<<num_bits; num_bits+=8;\
	}while(num_bits<unsigned(n));
	#define TINFL_SKIP_BITS(state_index,n) \
	{ if(num_bits<unsigned(n)) TINFL_NEED_BITS(state_index,n) bit_buf>>=(n); num_bits-=(n); }
	#define TINFL_GET_BITS(state_index,b,n) \
	{ if(num_bits<unsigned(n)) TINFL_NEED_BITS(state_index,n) b = bit_buf&((1<<(n))-1); bit_buf>>=(n); num_bits-=(n); } 
	//TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2.
	//It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a
	//Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the
	//bit buffer contains >=15 bits (deflate's max. Huffman code size).
	#define TINFL_HUFF_BITBUF_FILL(state_index,pHuff) \
	do{\
		temp = (pHuff)->m_look_up[bit_buf&(TINFL_FAST_LOOKUP_SIZE-1)]; \
		if(temp>=0)\
		{\
			code_len = temp>>9;\
			if(code_len&&num_bits>=code_len) break; \
		}\
		else if(num_bits>TINFL_FAST_LOOKUP_BITS)\
		{\
			code_len = TINFL_FAST_LOOKUP_BITS;\
			do{ temp = (pHuff)->m_tree[~temp+((bit_buf>>code_len++)&1)];\
			}while(temp<0&&num_bits>=code_len+1);\
			if(temp>=0) break;\
		}\
		TINFL_GET_BYTE(state_index,c)\
		bit_buf|=tinfl_bit_buf_t(c)<<num_bits;\
		num_bits+=8;\
	}while(num_bits<15);
	//TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read
	//beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully
	//decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32.
	//The slow path is only executed at the very end of the input buffer.
	#define TINFL_HUFF_DECODE(state_index,sym,pHuff) \
	{\
		int temp; unsigned code_len, c;\
		if(num_bits<15)\
		if(pIn_buf_end-pIn_buf_cur>=2)\
		{\
			bit_buf|=((tinfl_bit_buf_t)pIn_buf_cur[0]<<num_bits)|((tinfl_bit_buf_t)pIn_buf_cur[1]<<num_bits+8);\
			pIn_buf_cur+=2; num_bits+=16;\
		}\
		else TINFL_HUFF_BITBUF_FILL(state_index,pHuff)\
		temp = (pHuff)->m_look_up[bit_buf&(TINFL_FAST_LOOKUP_SIZE-1)];\
		if(temp<0)\
		{\
			code_len = TINFL_FAST_LOOKUP_BITS;\
			do{ temp = (pHuff)->m_tree[~temp+((bit_buf>>code_len++)&1)];\
			}while(temp<0);\
		}\
		else{ code_len = temp>>9; temp&=511; }\
		sym = temp;\
		bit_buf>>=code_len;	num_bits-=code_len;\
	}
	#define TINFL_ZERO_MEM(obj) memset(&(obj),0,sizeof(obj));
	#if 0 //MINIZ_USE_UNALIGNED_LOADS_AND_STORES && MINIZ_LITTLE_ENDIAN
	#define TINFL_READ_LE16(p) *((const uint16_t*)(p))
	#define TINFL_READ_LE32(p) *((const uint32_t*)(p))
	#else
	#define TINFL_READ_LE16(p) ((uint32_t)(((const uint8_t*)(p))[0])|((uint32_t)(((const uint8_t*)(p))[1])<<8U))
	#define TINFL_READ_LE32(p) ((uint32_t)(((const uint8_t*)(p))[0])|((uint32_t)(((const uint8_t*)(p))[1])<<8U)|\
					           ((uint32_t)(((const uint8_t*)(p))[2])<<16U)|\
							   ((uint32_t)(((const uint8_t*)(p))[3])<<24U))
	#endif
	TINFL_CR_BEGIN //switch(r->m_state){ case 0:
	//
	bit_buf = num_bits = dist = counter = 0;
	num_extra = r->m_zhdr0 = r->m_zhdr1 = 0; 
	r->m_z_adler32 = r->m_check_adler32 = 1;
	//
	/*if(0!=(decomp_flags&TINFL_FLAG_PARSE_ZLIB_HEADER))
	{
		TINFL_GET_BYTE(1,r->m_zhdr0); TINFL_GET_BYTE(2,r->m_zhdr1)
		counter = (r->m_zhdr0*256+r->m_zhdr1)%31!=0||(r->m_zhdr1&32)!=0||(r->m_zhdr0&15)!=8?1:0;
		if(0==(decomp_flags&TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) 
		counter|=1U<<8U+(r->m_zhdr0>>4)>32768U||out_buf_size_mask+1<size_t(1U<<8U+(r->m_zhdr0>>4));
		if(counter) TINFL_CR_RETURN_FOREVER(36,TINFL_STATUS_FAILED)
	}*/
	do
	{	TINFL_GET_BITS(3,r->m_final,3); 
		r->m_type = r->m_final>>1;
		if(r->m_type==0)
		{
			TINFL_SKIP_BITS(5,num_bits&7)
			for(counter=0;counter<4;++counter) 
			{
				if(num_bits) 
					TINFL_GET_BITS(6,r->m_raw_header[counter],8) 
				else TINFL_GET_BYTE(7,r->m_raw_header[counter]) 
			}

			counter = r->m_raw_header[0]|(r->m_raw_header[1]<<8);
			if(counter!=(unsigned)(0xFFFF^(r->m_raw_header[2]|(r->m_raw_header[3]<<8)))) 
			TINFL_CR_RETURN_FOREVER(39,TINFL_STATUS_FAILED)

			while(counter&&num_bits)
			{
				TINFL_GET_BITS(51,dist,8)
				while(pOut_buf_cur>=pOut_buf_end)
				TINFL_CR_RETURN(52,TINFL_STATUS_HAS_MORE_OUTPUT)
				*pOut_buf_cur++ = (uint8_t)dist;
				counter--;
			}

			while(counter)
			{
				while(pOut_buf_cur>=pOut_buf_end)
				TINFL_CR_RETURN(9,TINFL_STATUS_HAS_MORE_OUTPUT)

				while(pIn_buf_cur>=pIn_buf_end)
				TINFL_CR_RETURN(38,decomp_flags&TINFL_FLAG_HAS_MORE_INPUT?TINFL_STATUS_NEEDS_MORE_INPUT:TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS)				

				size_t n = std::min(counter,std::min<size_t>(pOut_buf_end-pOut_buf_cur,pIn_buf_end-pIn_buf_cur));
				memcpy(pOut_buf_cur,pIn_buf_cur,n);				
				pIn_buf_cur+=n; pOut_buf_cur+=n; 
				counter-=(unsigned)n;
			}
		}
		else if(r->m_type==3)
		{
			TINFL_CR_RETURN_FOREVER(10,TINFL_STATUS_FAILED)
		}
		else
		{
			if(r->m_type==1)
			{
				uint8_t *p = r->m_tables[0].m_code_size; 
				r->m_table_sizes[0] = 288; r->m_table_sizes[1] = 32; 
				memset(r->m_tables[1].m_code_size,5,32); 
				unsigned i = 0;
				for(;i<=143;++i) *p++ = 8; 
				for(;i<=255;++i) *p++ = 9;
				for(;i<=279;++i) *p++ = 7; 
				for(;i<=287;++i) *p++ = 8;
			}
			else
			{
				for(counter=0;counter<3;counter++)
				{ 
					TINFL_GET_BITS(11,r->m_table_sizes[counter],"\05\05\04"[counter])
					r->m_table_sizes[counter]+=s_min_table_sizes[counter]; 
				}
				TINFL_ZERO_MEM(r->m_tables[2].m_code_size)
				for(counter=0;counter<r->m_table_sizes[2];counter++) 
				{
					unsigned s; TINFL_GET_BITS(14,s,3)
					r->m_tables[2].m_code_size[s_length_dezigzag[counter]] = (uint8_t)s; 
				}
				r->m_table_sizes[2] = 19;
			}

			for(;(int)r->m_type>=0;r->m_type--)
			{					
				tinfl_huff_table *pTable;
				pTable = &r->m_tables[r->m_type]; //C2360
				TINFL_ZERO_MEM(pTable->m_look_up) TINFL_ZERO_MEM(pTable->m_tree)
				unsigned i,used_syms,total,sym_index,next_code[17],total_syms[16];
				TINFL_ZERO_MEM(total_syms)
				for(i=0;i<r->m_table_sizes[r->m_type];i++) 
				total_syms[pTable->m_code_size[i]]++;
				used_syms = total = next_code[0] = next_code[1] = 0;
				for(i=1;i<=15;used_syms+=total_syms[i++]) 
				next_code[i+1] = total = total+total_syms[i]<<1; 
				if(65536!=total&&used_syms>1)
				TINFL_CR_RETURN_FOREVER(35,TINFL_STATUS_FAILED)
				
				int tree_next, tree_cur; 
				for(tree_next=-1,sym_index=0;sym_index<r->m_table_sizes[r->m_type];sym_index++)
				{
					unsigned rev_code = 0, l, cur_code;
					unsigned code_size = pTable->m_code_size[sym_index]; 
					if(!code_size) continue;
					cur_code = next_code[code_size]++; 
					for(l=code_size;l>0;l--,cur_code>>=1) 
					rev_code = (rev_code<<1)|(cur_code&1);
					if(code_size<=TINFL_FAST_LOOKUP_BITS)
					{
						int16_t k = int16_t(code_size<<9|sym_index); 
						while(rev_code<TINFL_FAST_LOOKUP_SIZE)
						{
							pTable->m_look_up[rev_code] = k; rev_code+=1<<code_size; 
						} 
						continue; 
					}
					tree_cur = pTable->m_look_up[rev_code&(TINFL_FAST_LOOKUP_SIZE-1)];
					if(tree_cur==0)
					{
						pTable->m_look_up[rev_code&(TINFL_FAST_LOOKUP_SIZE-1)] = (int16_t)tree_next;
						tree_cur = tree_next; tree_next-=2; 
					}
					rev_code>>=(TINFL_FAST_LOOKUP_BITS-1);
					for(unsigned j=code_size;j>(TINFL_FAST_LOOKUP_BITS+1);j--)
					{
						tree_cur-=((rev_code>>=1)&1);
						if(!pTable->m_tree[-tree_cur-1])
						{ 
							pTable->m_tree[-tree_cur-1] = (int16_t)tree_next;
							tree_cur = tree_next; tree_next-=2;
						}
						else tree_cur = pTable->m_tree[-tree_cur-1];
					}
					tree_cur-=(rev_code>>=1)&1;
					pTable->m_tree[-tree_cur-1] = (int16_t)sym_index;
				}
				if(r->m_type==2)
				{
					for(counter=0;counter<r->m_table_sizes[0]+r->m_table_sizes[1];)
					{
						TINFL_HUFF_DECODE(16,dist,&r->m_tables[2])
						if(dist<16)
						{
							r->m_len_codes[counter++] = (uint8_t)dist; 
							continue; 
						}
						if(dist==16&&0==counter)
						TINFL_CR_RETURN_FOREVER(17,TINFL_STATUS_FAILED)
						unsigned s; 
						num_extra = "\02\03\07"[dist-16]; 
						TINFL_GET_BITS(18,s,num_extra) s+="\03\03\013"[dist-16];
						memset(r->m_len_codes+counter,(dist==16)?r->m_len_codes[counter-1]:0,s); 
						counter+=s;
					}
					if((r->m_table_sizes[0]+r->m_table_sizes[1])!=counter)
					{
						TINFL_CR_RETURN_FOREVER(21,TINFL_STATUS_FAILED)
					}
					memcpy(r->m_tables[0].m_code_size,r->m_len_codes,r->m_table_sizes[0]);
					memcpy(r->m_tables[1].m_code_size,r->m_len_codes+r->m_table_sizes[0],r->m_table_sizes[1]);
				}
			}

			for(uint8_t *pSrc;;)
			{
				for(;;)
				{
					if(pIn_buf_end-pIn_buf_cur<4||pOut_buf_end-pOut_buf_cur<2)
					{
						TINFL_HUFF_DECODE(23,counter,&r->m_tables[0])
						if(counter>=256) break;
						while(pOut_buf_cur>=pOut_buf_end)
						TINFL_CR_RETURN(24,TINFL_STATUS_HAS_MORE_OUTPUT)
						*pOut_buf_cur++ = (uint8_t)counter;
					}
					else
					{							
						#if 64==TINFL_BITBUF_SIZE //TINFL_USE_64BIT_BITBUF
						if(num_bits<30){ bit_buf|=(((tinfl_bit_buf_t)TINFL_READ_LE32(pIn_buf_cur))<<num_bits); pIn_buf_cur+=4; num_bits+=32; }
						#else
						if(num_bits<15){ bit_buf|=(((tinfl_bit_buf_t)TINFL_READ_LE16(pIn_buf_cur))<<num_bits); pIn_buf_cur+=2; num_bits+=16; }
						#endif

						unsigned code_len;
						int sym2 = r->m_tables[0].m_look_up[bit_buf&(TINFL_FAST_LOOKUP_SIZE-1)];
						if(sym2<0) 
						{
							code_len = TINFL_FAST_LOOKUP_BITS; 							
							do{ sym2 = r->m_tables[0].m_tree[~sym2+((bit_buf>>code_len++)&1)]; 							
							}while(sym2<0);
						}
						else code_len = sym2>>9;
						counter = sym2; bit_buf>>=code_len; num_bits-=code_len;
						if(counter&256) break;

						#if 64!=TINFL_BITBUF_SIZE //!TINFL_USE_64BIT_BITBUF
						if(num_bits<15){ bit_buf|=(((tinfl_bit_buf_t)TINFL_READ_LE16(pIn_buf_cur))<<num_bits); pIn_buf_cur+=2; num_bits+=16; }
						#endif

						sym2 = 
						r->m_tables[0].m_look_up[bit_buf&(TINFL_FAST_LOOKUP_SIZE-1)];
						if(sym2<0)
						{
							code_len = TINFL_FAST_LOOKUP_BITS; 
							do{ sym2 = r->m_tables[0].m_tree[~sym2+((bit_buf>>code_len++)&1)];
							}while(sym2<0);
						}
						else code_len = sym2>>9;
						bit_buf>>=code_len; num_bits-=code_len;
						pOut_buf_cur[0] = (uint8_t)counter;
						if(sym2&256)
						{
							pOut_buf_cur++;	counter = sym2;
							break;
						}
						pOut_buf_cur[1] = (uint8_t)sym2;
						pOut_buf_cur+=2;
					}
				}
				counter&=511;
				if(counter==256) break;
				num_extra = s_length_extra[counter-257];
				counter = s_length_base[counter-257];
				if(num_extra)
				{
					unsigned extra_bits; 
					TINFL_GET_BITS(25,extra_bits,num_extra)
					counter+=extra_bits; 
				}
				TINFL_HUFF_DECODE(26,dist,&r->m_tables[1])
				num_extra = s_dist_extra[dist]; 
				dist = s_dist_base[dist];
				if(num_extra)
				{
					unsigned extra_bits;
					TINFL_GET_BITS(27,extra_bits,num_extra)
					dist+=extra_bits; 
				}
				dist_from_out_buf_start = pOut_buf_cur-pOut_buf_start;
				if(dist>dist_from_out_buf_start&&decomp_flags&TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)				
				TINFL_CR_RETURN_FOREVER(37,TINFL_STATUS_FAILED)

				pSrc = pOut_buf_start+((dist_from_out_buf_start-dist)&out_buf_size_mask);

				if(std::max(pOut_buf_cur,pSrc)+counter>pOut_buf_end)
				{
					while(counter--)
					{
						while(pOut_buf_cur>=pOut_buf_end)
						TINFL_CR_RETURN(53,TINFL_STATUS_HAS_MORE_OUTPUT)						
						*pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start-dist)&out_buf_size_mask];
						dist_from_out_buf_start++;
					}
					continue;
				}
				/*#if 0 //MINIZ_USE_UNALIGNED_LOADS_AND_STORES
				else if(counter>=9&&counter<=dist)
				{
					const uint8_t *pSrc_end = pSrc+(counter&~7);

					do
					{
						((uint32_t*)pOut_buf_cur)[0] = ((const uint32_t*)pSrc)[0];
						((uint32_t*)pOut_buf_cur)[1] = ((const uint32_t*)pSrc)[1];
						pSrc+=8; pOut_buf_cur+=8;

					}while(pSrc<pSrc_end);

					counter&=7;
					if(counter<3)
					{
						if(counter!=0)
						{
							pOut_buf_cur[0] = pSrc[0];
							if(counter>1)
							pOut_buf_cur[1] = pSrc[1];
							pOut_buf_cur+=counter;
						}
						continue;
					}
				}
				#endif*/
				do
				{
					pOut_buf_cur[0] = pSrc[0];
					pOut_buf_cur[1] = pSrc[1];
					pOut_buf_cur[2] = pSrc[2];
					pOut_buf_cur+=3; pSrc+=3;
					counter-=3;

				}while((int)counter>2);

				if((int)counter>0)
				{
					pOut_buf_cur[0] = pSrc[0];
					if((int)counter>1)
					pOut_buf_cur[1] = pSrc[1];
					pOut_buf_cur+=counter;
				}
			}
		}
	}while(0==(r->m_final&1));

	/* Ensure byte alignment and put back any bytes from the bitbuf if we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
	/* I'm being super conservative here. A number of simplifications can be made to the byte alignment part, and the Adler32 check shouldn't ever need to worry about reading from the bitbuf now. */
    TINFL_SKIP_BITS(32,num_bits&7);
    while(pIn_buf_cur>pIn_buf_next&&num_bits>=8)
    {
        pIn_buf_cur--; num_bits-=8;
    }
    bit_buf&=tinfl_bit_buf_t((uint64_t(1)<<num_bits)-uint64_t(1));
    assert(0==num_bits); /* if this assert fires then we've read beyond the end of non-deflate/zlib streams with following data (such as gzip streams). */

	/*if(0!=(decomp_flags&TINFL_FLAG_PARSE_ZLIB_HEADER))
	{
		for(counter=0;counter<4;counter++) 
		{
			unsigned s; 
			if(0!=num_bits) TINFL_GET_BITS(41,s,8) else TINFL_GET_BYTE(42,s)
			r->m_z_adler32 = (r->m_z_adler32<<8)|s; 
		}
	}*/
	TINFL_CR_RETURN_FOREVER(34,TINFL_STATUS_DONE)
	TINFL_CR_FINISH //}

common_exit: /////////////////////////////////////////////////////////////////

	/* As long as we aren't telling the caller that we NEED more input to make forward progress: */
    /* Put back any bytes from the bitbuf in case we've looked ahead too far on gzip, or other Deflate streams followed by arbitrary data. */
    /* We need to be very careful here to NOT push back any bytes we definitely know we need to make forward progress, though, or we'll lock the caller up into an inf loop. */
    if(status!=TINFL_STATUS_NEEDS_MORE_INPUT
	 &&status!=TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS)
    {
        while(pIn_buf_cur>pIn_buf_next&&num_bits>=8)
        {
            pIn_buf_cur--; num_bits-=8;
        }
    }

	r->m_num_bits = num_bits; 
	r->m_bit_buf = bit_buf&tinfl_bit_buf_t((uint64_t(1)<<num_bits)-uint64_t(1));
	r->m_dist = dist; 
	r->m_counter = counter; 
	r->m_num_extra = num_extra;
	r->m_dist_from_out_buf_start = dist_from_out_buf_start;

	*pIn_buf_size = pIn_buf_cur-pIn_buf_next; 
	*pOut_buf_size = pOut_buf_cur-pOut_buf_next;

	/*if(status>=0&&0!=(decomp_flags&(TINFL_FLAG_PARSE_ZLIB_HEADER|TINFL_FLAG_COMPUTE_ADLER32)))
	{			
		extern int tdefl_adler32(const unsigned char*,size_t,unsigned=1);
		r->m_check_adler32 = tdefl_adler32(pOut_buf_next,*pOut_buf_size,tdefl_adler32);		
		if(status==TINFL_STATUS_DONE
		&&decomp_flags&TINFL_FLAG_PARSE_ZLIB_HEADER&&r->m_check_adler32!=r->m_z_adler32)
		status = TINFL_STATUS_ADLER32_MISMATCH;
	}*/
	return status;
}



////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE
////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE
////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE ////DEFLATE



//https://github.com/uroni/miniz/blob/master/miniz_tdef.c
//(changes made for readability/maintainability)
/**************************************************************************
 *
 * Copyright 2013-2014 RAD Game Tools and Valve Software
 * Copyright 2010-2014 Rich Geldreich and Tenacious Software LLC
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/
extern int tdefl_adler32(const unsigned char *ptr, size_t buf_len, unsigned adler=1)
{
	unsigned s1 = adler&0xffff, s2 = adler>>16;
	size_t block_len = buf_len%5552;
	//if(ptr==nullptr) return 1; //MZ_ADLER32_INIT
	for(size_t i;0!=buf_len;block_len=5552,buf_len-=block_len)
	{
		for(i=0;i+7<block_len;i+=8,ptr+=8)
		{
			s2+=s1+=ptr[0]; s2+=s1+=ptr[1]; s2+=s1+=ptr[2]; s2+=s1+=ptr[3];
			s2+=s1+=ptr[4]; s2+=s1+=ptr[5]; s2+=s1+=ptr[6]; s2+=s1+=ptr[7];
		}
		for(;i<block_len;i++) s2+=s1+=*ptr++; s1%=65521U; s2%=65521U; 
	}
	return (s2<<16)+s1;
}
extern int tdefl_crc32(const unsigned char *ptr, size_t buf_len, unsigned crc=0)
{
    static const uint32_t s_crc32[16] = 
	{
			 0,	0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
	0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c 
	};
    //if(!ptr) return MZ_CRC32_INIT; //0
    for(crc=~crc;buf_len-->0;)
    {
        unsigned char b = *ptr++;
        crc = (crc>>4)^s_crc32[(crc&0xF)^(b&0xF)]; crc = (crc>>4)^s_crc32[(crc&0xF)^(b>>4)];
    }
    return ~crc;
}
#define TDEFL_FORCEINLINE inline 	
enum
{
    TDEFL_WRITE_ZLIB_HEADER = 0x01000,
    TDEFL_COMPUTE_ADLER32 = 0x02000,
    TDEFL_GREEDY_PARSING_FLAG = 0x04000,
    TDEFL_NONDETERMINISTIC_PARSING_FLAG = 0x08000,
    TDEFL_RLE_MATCHES = 0x10000,
    TDEFL_FILTER_MATCHES = 0x20000,
    TDEFL_FORCE_ALL_STATIC_BLOCKS = 0x40000,
    TDEFL_FORCE_ALL_RAW_BLOCKS = 0x80000
};
enum
{
    TDEFL_MAX_HUFF_TABLES = 3,
    TDEFL_MAX_HUFF_SYMBOLS_0 = 288,
    TDEFL_MAX_HUFF_SYMBOLS_1 = 32,
    TDEFL_MAX_HUFF_SYMBOLS_2 = 19,
    TDEFL_LZ_DICT_SIZE = 32768,
    TDEFL_LZ_DICT_SIZE_MASK = TDEFL_LZ_DICT_SIZE-1,
    TDEFL_MIN_MATCH_LEN = 3,
    TDEFL_MAX_MATCH_LEN = 258
};
/*enum //#define TDEFL_LESS_MEMORY 0
{
    TDEFL_LZ_CODE_BUF_SIZE = 64*1024,
    TDEFL_OUT_BUF_SIZE = TDEFL_LZ_CODE_BUF_SIZE*13/10,
    TDEFL_MAX_HUFF_SYMBOLS = 288,
    TDEFL_LZ_HASH_BITS = 15,
    TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
    TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS+2)/3,
    TDEFL_LZ_HASH_SIZE = 1<<TDEFL_LZ_HASH_BITS
}; */
enum //#define TDEFL_LESS_MEMORY 1
{
    TDEFL_LZ_CODE_BUF_SIZE = 24*1024,
    TDEFL_OUT_BUF_SIZE = (TDEFL_LZ_CODE_BUF_SIZE*13)/10,
    TDEFL_MAX_HUFF_SYMBOLS = 288,
    TDEFL_LZ_HASH_BITS = 12,
    TDEFL_LEVEL1_HASH_SIZE_MASK = 4095,
    TDEFL_LZ_HASH_SHIFT = (TDEFL_LZ_HASH_BITS+2)/3,
    TDEFL_LZ_HASH_SIZE = 1<<TDEFL_LZ_HASH_BITS
};
typedef enum 
{
    TDEFL_STATUS_BAD_PARAM = -2,
    TDEFL_STATUS_PUT_BUF_FAILED = -1,
    TDEFL_STATUS_OKAY = 0,
    TDEFL_STATUS_DONE = 1
}tdefl_status;
typedef enum 
{
    TDEFL_NO_FLUSH = 0,
    TDEFL_SYNC_FLUSH = 2,
    TDEFL_FULL_FLUSH = 3,
    TDEFL_FINISH = 4
}tdefl_flush;
typedef bool(*tdefl_put_buf_func_ptr)(const void*,size_t,void*);
typedef struct
{
    tdefl_put_buf_func_ptr m_pPut_buf_func;
    void *m_pPut_buf_user;
    unsigned m_flags, m_max_probes[2];
    int m_greedy_parsing;
    unsigned m_adler32, m_lookahead_pos, m_lookahead_size, m_dict_size;
    uint8_t *m_pLZ_code_buf, *m_pLZ_flags, *m_pOutput_buf, *m_pOutput_buf_end;
    unsigned m_num_flags_left, m_total_lz_bytes, m_lz_code_buf_dict_pos, m_bits_in, m_bit_buffer;
    unsigned m_saved_match_dist, m_saved_match_len, m_saved_lit, m_output_flush_ofs, m_output_flush_remaining, m_finished, m_block_index, m_wants_to_finish;
    tdefl_status m_prev_return_status;
    const void *m_pIn_buf;
    void *m_pOut_buf;
    size_t *m_pIn_buf_size, *m_pOut_buf_size;
    tdefl_flush m_flush;
    const uint8_t *m_pSrc;
    size_t m_src_buf_left, m_out_buf_ofs;
    uint8_t m_dict[TDEFL_LZ_DICT_SIZE+TDEFL_MAX_MATCH_LEN-1];
    uint16_t m_huff_count[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint16_t m_huff_codes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint8_t m_huff_code_sizes[TDEFL_MAX_HUFF_TABLES][TDEFL_MAX_HUFF_SYMBOLS];
    uint8_t m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE];
    uint16_t m_next[TDEFL_LZ_DICT_SIZE];
    uint16_t m_hash[TDEFL_LZ_HASH_SIZE];
    uint8_t m_output_buf[TDEFL_OUT_BUF_SIZE];
}tdefl_compressor;
static const uint16_t s_tdefl_len_sym[256] =
{
	257,258,259,260,261,262,263,264,265,265,266,266,267,267,268,268,269,269,269,269,270,270,270,270,271,271,271,271,272,272,272,272,
	273,273,273,273,273,273,273,273,274,274,274,274,274,274,274,274,275,275,275,275,275,275,275,275,276,276,276,276,276,276,276,276,
	277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,
	279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,
	281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,
	282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,
	283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,283,
	284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,284,285
};
static const uint8_t s_tdefl_len_extra[256] =
{
	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0
};
static const uint8_t s_tdefl_small_dist_sym[512] =
{
	0,1,2,3,4,4,5,5,6,6,6,6,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,
	11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,
	14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
	14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,
	17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17
};
static const uint8_t s_tdefl_small_dist_extra[512] =
{
	0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7
};
static const uint8_t s_tdefl_large_dist_sym[128] =
{
	0,0,18,19,20,20,21,21,22,22,22,22,23,23,23,23,24,24,24,24,24,24,24,24,25,25,25,25,25,25,25,25,26,26,26,26,26,26,26,26,26,26,26,26,
	26,26,26,26,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,
	28,28,28,28,28,28,28,28,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29
};
static const uint8_t s_tdefl_large_dist_extra[128] =
{
	0,0,8,8,9,9,9,9,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13
};
typedef struct /* Radix sorts tdefl_sym_freq[] array by 16-bit key m_key. Returns ptr to sorted values. */
{
	uint16_t m_key,m_sym_index;

}tdefl_sym_freq;
static tdefl_sym_freq *tdefl_radix_sort_syms(unsigned num_syms, tdefl_sym_freq *pSyms0, tdefl_sym_freq *pSyms1)
{
	uint32_t total_passes = 2,hist[256*2];
	tdefl_sym_freq *pCur_syms = pSyms0,*pNew_syms = pSyms1;
	TINFL_ZERO_MEM(hist)
	for(unsigned i=0;i<num_syms;i++)
	{
		unsigned freq = pSyms0[i].m_key;
		hist[freq&0xFF]++;
		hist[256+(freq>>8&0xFF)]++;
	}
	while(total_passes>1&&num_syms==hist[(total_passes-1)*256])
	total_passes--;
	for(uint32_t pass_shift=0,pass=0;pass<total_passes;pass++,pass_shift+=8)
	{
		const uint32_t *pHist = &hist[pass<<8];
		unsigned offsets[256],cur_ofs = 0;
		for(unsigned i=0;i<256;i++)
		{
			offsets[i] = cur_ofs;
			cur_ofs+=pHist[i];
		}
		for(unsigned j,i=0;i<num_syms;i++)
		{
			j = pCur_syms[i].m_key>>pass_shift&0xFF;			
			pNew_syms[offsets[j]] = pCur_syms[i];
			offsets[j]++;
		}
		tdefl_sym_freq *swap = pCur_syms;
		pCur_syms = pNew_syms;
		pNew_syms = swap;
	}
	return pCur_syms;
}/* tdefl_calculate_minimum_redundancy() originally written by:
 Alistair Moffat, alistair@cs.mu.oz.au, Jyrki Katajainen, jyrki@diku.dk, November 1996. */
static void tdefl_calculate_minimum_redundancy(tdefl_sym_freq *A, int n)
{
	switch(n){ case 1: A[0].m_key = 1; case 0: return; }

	int root,leaf,next,avbl,used,dpth;

	A[0].m_key+=A[1].m_key;
	root = 0;
	leaf = 2;
	for(next=1;next<n-1;next++)
	{
		if(leaf>=n||A[root].m_key<A[leaf].m_key)
		{
			A[next].m_key = A[root].m_key;
			A[root++].m_key = (uint16_t)next;
		}
		else A[next].m_key = A[leaf++].m_key;

		if(leaf>=n||root<next&&A[root].m_key<A[leaf].m_key)
		{
			A[next].m_key = uint16_t(A[next].m_key+A[root].m_key);
			A[root++].m_key = (uint16_t)next;
		}
		else A[next].m_key = uint16_t(A[next].m_key+A[leaf++].m_key);
	}
	A[n-2].m_key = 0;
	for(next = n-3;next>=0;next--)
	A[next].m_key = A[A[next].m_key].m_key+1;
	avbl = 1;
	used = dpth = 0;
	root = n-2; next = n-1;
	while(avbl>0)
	{
		while(root>=0&&(int)A[root].m_key==dpth)
		{
			used++; root--;
		}
		while(avbl>used)
		{
			avbl--; A[next--].m_key = (uint16_t)dpth;
		}
		avbl = 2*used; dpth++; used = 0;
	}
}/* Limits canonical Huffman code table's max code size. */
enum{ TDEFL_MAX_SUPPORTED_HUFF_CODESIZE=32 };
static void tdefl_huffman_enforce_max_code_size(int *pNum_codes, int code_list_len, int max_code_size)
{
	uint32_t total = 0;
	if(code_list_len<=1)
	return;
	for(int i=max_code_size+1;i<=TDEFL_MAX_SUPPORTED_HUFF_CODESIZE;i++)
	pNum_codes[max_code_size]+=pNum_codes[i];
	for(int i=max_code_size;i>0;i--)
	total+=(((uint32_t)pNum_codes[i])<<(max_code_size-i));
	while(total!=(1UL<<max_code_size))
	{
		pNum_codes[max_code_size]--;
		for(int i=max_code_size-1;i>0;i--) if(pNum_codes[i])
		{
			pNum_codes[i]--; pNum_codes[i+1]+=2;
			break;
		}
		total--;
	}
}
static void tdefl_optimize_huffman_table(tdefl_compressor *d, int table_num, int table_len, int code_size_limit, bool static_table)
{
	int num_codes[1+TDEFL_MAX_SUPPORTED_HUFF_CODESIZE];
	unsigned next_code[TDEFL_MAX_SUPPORTED_HUFF_CODESIZE+1];
	TINFL_ZERO_MEM(num_codes)
	if(static_table)
	{
		for(int i=0;i<table_len;i++)
		num_codes[d->m_huff_code_sizes[table_num][i]]++;
	}
	else
	{
		tdefl_sym_freq *pSyms,syms0[TDEFL_MAX_HUFF_SYMBOLS],syms1[TDEFL_MAX_HUFF_SYMBOLS];
		int num_used_syms = 0;
		const uint16_t *pSym_count = &d->m_huff_count[table_num][0];
		for(int i=0;i<table_len;i++) if(0!=pSym_count[i])
		{
			syms0[num_used_syms].m_key = (uint16_t)pSym_count[i];
			syms0[num_used_syms++].m_sym_index = (uint16_t)i;
		}

		pSyms = tdefl_radix_sort_syms(num_used_syms,syms0,syms1);
		tdefl_calculate_minimum_redundancy(pSyms,num_used_syms);

		for(int i=0;i<num_used_syms;i++)
		num_codes[pSyms[i].m_key]++;

		tdefl_huffman_enforce_max_code_size(num_codes,num_used_syms,code_size_limit);

		TINFL_ZERO_MEM(d->m_huff_code_sizes[table_num])
		TINFL_ZERO_MEM(d->m_huff_codes[table_num])
		for(int i=1,j=num_used_syms;i<=code_size_limit;i++)
		for(int k=num_codes[i];k>0;k--)
		d->m_huff_code_sizes[table_num][pSyms[--j].m_sym_index] = uint8_t(i);
	}

	next_code[1] = 0;
	for(int i=2,j=0;i<=code_size_limit;i++)
	next_code[i] = j = j+num_codes[i-1]<<1;

	for(int i=0;i<table_len;i++)
	{
		unsigned rev_code = 0,code,code_size;
		code_size = d->m_huff_code_sizes[table_num][i];
		if(0==code_size) continue;

		code = next_code[code_size]++;
		for(int k=code_size;k>0;k--,code>>=1)
		rev_code = rev_code<<1|code&1;
		d->m_huff_codes[table_num][i] = (uint16_t)rev_code;
	}
}
#define TDEFL_PUT_BITS(b,l) \
{\
	unsigned bits = b, len = l;\
	assert(bits<=(1U<<len)-1U);\
	d->m_bit_buffer|=bits<<d->m_bits_in;\
	d->m_bits_in+=len;\
	while(d->m_bits_in>=8)\
	{\
		if(d->m_pOutput_buf<d->m_pOutput_buf_end)\
		*d->m_pOutput_buf++ = uint8_t(d->m_bit_buffer);\
		d->m_bit_buffer>>=8; d->m_bits_in-=8;\
	}\
}
#define TDEFL_RLE_PREV_CODE_SIZE() \
if(0!=rle_repeat_count)\
{\
	if(rle_repeat_count<3)\
	{\
		d->m_huff_count[2][prev_code_size] = \
		uint16_t(d->m_huff_count[2][prev_code_size]+rle_repeat_count);\
		while(rle_repeat_count-->0)\
		packed_code_sizes[num_packed_code_sizes++] = prev_code_size;\
	}\
	else\
	{\
		d->m_huff_count[2][16] = uint16_t(d->m_huff_count[2][16]+1);\
		packed_code_sizes[num_packed_code_sizes++] = 16;\
		packed_code_sizes[num_packed_code_sizes++] = uint8_t(rle_repeat_count-3);\
	}\
	rle_repeat_count = 0;\
}
#define TDEFL_RLE_ZERO_CODE_SIZE() \
if(0!=rle_z_count)\
{\
	if(rle_z_count<3)\
	{\
		d->m_huff_count[2][0] = uint16_t(d->m_huff_count[2][0]+rle_z_count);\
		while(rle_z_count-->0)\
		packed_code_sizes[num_packed_code_sizes++] = 0;\
	}\
	else if(rle_z_count<=10)\
	{\
		d->m_huff_count[2][17] = uint16_t(d->m_huff_count[2][17]+1);\
		packed_code_sizes[num_packed_code_sizes++] = 17;\
		packed_code_sizes[num_packed_code_sizes++] = uint8_t(rle_z_count-3);\
	}\
	else\
	{\
		d->m_huff_count[2][18] = uint16_t(d->m_huff_count[2][18]+1);\
		packed_code_sizes[num_packed_code_sizes++] = 18;\
		packed_code_sizes[num_packed_code_sizes++] = uint8_t(rle_z_count-11);\
	}\
	rle_z_count = 0;\
}
static uint8_t s_tdefl_packed_code_size_syms_swizzle[] = 
{
	16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15
};
static void tdefl_start_dynamic_block(tdefl_compressor *d)
{
	int num_lit_codes,num_dist_codes,num_bit_lengths;
	unsigned i,total_code_sizes_to_pack,num_packed_code_sizes,rle_z_count,rle_repeat_count,packed_code_sizes_index;
	uint8_t code_sizes_to_pack[TDEFL_MAX_HUFF_SYMBOLS_0+TDEFL_MAX_HUFF_SYMBOLS_1],packed_code_sizes[TDEFL_MAX_HUFF_SYMBOLS_0+TDEFL_MAX_HUFF_SYMBOLS_1],prev_code_size = 0xFF;

	d->m_huff_count[0][256] = 1;

	tdefl_optimize_huffman_table(d,0,TDEFL_MAX_HUFF_SYMBOLS_0,15,false);
	tdefl_optimize_huffman_table(d,1,TDEFL_MAX_HUFF_SYMBOLS_1,15,false);

	for(num_lit_codes = 286;num_lit_codes>257;num_lit_codes--)
	if(d->m_huff_code_sizes[0][num_lit_codes-1])
	break;
	for(num_dist_codes = 30;num_dist_codes>1;num_dist_codes--)
	if(d->m_huff_code_sizes[1][num_dist_codes-1])
	break;

	memcpy(code_sizes_to_pack,&d->m_huff_code_sizes[0][0],num_lit_codes);
	memcpy(code_sizes_to_pack+num_lit_codes,&d->m_huff_code_sizes[1][0],num_dist_codes);
	total_code_sizes_to_pack = num_lit_codes+num_dist_codes;
	num_packed_code_sizes = 0;
	rle_z_count = 0;
	rle_repeat_count = 0;

	memset(&d->m_huff_count[2][0],0,sizeof(d->m_huff_count[2][0])*TDEFL_MAX_HUFF_SYMBOLS_2);
	for(i=0;i<total_code_sizes_to_pack;i++)
	{
		uint8_t code_size = code_sizes_to_pack[i];
		if(!code_size)
		{
			TDEFL_RLE_PREV_CODE_SIZE()
			if(++rle_z_count==138)
			TDEFL_RLE_ZERO_CODE_SIZE()			
		}
		else
		{
			TDEFL_RLE_ZERO_CODE_SIZE()
			if(code_size!=prev_code_size)
			{
				TDEFL_RLE_PREV_CODE_SIZE()
				d->m_huff_count[2][code_size] = (uint16_t)(d->m_huff_count[2][code_size]+1);
				packed_code_sizes[num_packed_code_sizes++] = code_size;
			}
			else if(++rle_repeat_count==6)
			{
				TDEFL_RLE_PREV_CODE_SIZE()
			}
		}
		prev_code_size = code_size;
	}
	if(0!=rle_repeat_count)
	{
		TDEFL_RLE_PREV_CODE_SIZE()
	}
	else TDEFL_RLE_ZERO_CODE_SIZE()

	tdefl_optimize_huffman_table(d,2,TDEFL_MAX_HUFF_SYMBOLS_2,7,false);

	TDEFL_PUT_BITS(2,2)

	TDEFL_PUT_BITS(num_lit_codes-257,5)
	TDEFL_PUT_BITS(num_dist_codes-1,5)

	for(num_bit_lengths=18;num_bit_lengths>=0;num_bit_lengths--)
	if(d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[num_bit_lengths]])
	break;
	num_bit_lengths = std::max(4,num_bit_lengths+1);
	TDEFL_PUT_BITS(num_bit_lengths-4,4)
	for(i=0;(int)i<num_bit_lengths;i++)
	TDEFL_PUT_BITS(d->m_huff_code_sizes[2][s_tdefl_packed_code_size_syms_swizzle[i]],3)

	for(packed_code_sizes_index=0;packed_code_sizes_index<num_packed_code_sizes;)
	{
		unsigned code = packed_code_sizes[packed_code_sizes_index++];
		assert(code<TDEFL_MAX_HUFF_SYMBOLS_2);
		TDEFL_PUT_BITS(d->m_huff_codes[2][code],d->m_huff_code_sizes[2][code])
		if(code>=16)
		TDEFL_PUT_BITS(packed_code_sizes[packed_code_sizes_index++],"\02\03\07"[code-16])
	}
}
static void tdefl_start_static_block(tdefl_compressor *d)
{
	uint8_t *p = &d->m_huff_code_sizes[0][0];

	unsigned i = 0;
	for(;i<=143;i++) *p++ = 8;
	for(;i<=255;i++) *p++ = 9;
	for(;i<=279;i++) *p++ = 7;
	for(;i<=287;i++) *p++ = 8;

	memset(d->m_huff_code_sizes[1],5,32);

	tdefl_optimize_huffman_table(d,0,288,15,true);
	tdefl_optimize_huffman_table(d,1,32,15,true);

	TDEFL_PUT_BITS(1,2)
}
static const unsigned mz_bitmasks[17] = 
{
	0x0000,0x0001,0x0003,0x0007,0x000F,0x001F,0x003F,0x007F,0x00FF,0x01FF,0x03FF,0x07FF,0x0FFF,0x1FFF,0x3FFF,0x7FFF,0xFFFF
};
static bool tdefl_compress_lz_codes(tdefl_compressor *d)
{	
	unsigned flags = 1;
	for(uint8_t*pLZ_codes=d->m_lz_code_buf;pLZ_codes<d->m_pLZ_code_buf;flags>>=1)
	{
		if(flags==1)
		flags = *pLZ_codes++|0x100;
		if((flags&1)!=0)
		{
			unsigned sym,num_extra_bits;
			unsigned match_len = pLZ_codes[0],match_dist = pLZ_codes[1]|pLZ_codes[2]<<8;
			pLZ_codes+=3;

			assert(d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]]);
			TDEFL_PUT_BITS(d->m_huff_codes[0][s_tdefl_len_sym[match_len]],d->m_huff_code_sizes[0][s_tdefl_len_sym[match_len]])
			TDEFL_PUT_BITS(match_len&mz_bitmasks[s_tdefl_len_extra[match_len]],s_tdefl_len_extra[match_len])

			if(match_dist<512)
			{
				sym = s_tdefl_small_dist_sym[match_dist];
				num_extra_bits = s_tdefl_small_dist_extra[match_dist];
			}
			else
			{
				sym = s_tdefl_large_dist_sym[match_dist>>8];
				num_extra_bits = s_tdefl_large_dist_extra[match_dist>>8];
			}
			assert(d->m_huff_code_sizes[1][sym]);
			TDEFL_PUT_BITS(d->m_huff_codes[1][sym],d->m_huff_code_sizes[1][sym])
			TDEFL_PUT_BITS(match_dist&mz_bitmasks[num_extra_bits],num_extra_bits)
		}
		else
		{
			unsigned lit = *pLZ_codes++;
			assert(d->m_huff_code_sizes[0][lit]);
			TDEFL_PUT_BITS(d->m_huff_codes[0][lit],d->m_huff_code_sizes[0][lit])
		}
	}

	TDEFL_PUT_BITS(d->m_huff_codes[0][256],d->m_huff_code_sizes[0][256])

	return d->m_pOutput_buf<d->m_pOutput_buf_end;
}
static bool tdefl_compress_block(tdefl_compressor *d, bool static_block)
{
	if(static_block) tdefl_start_static_block(d);
	else tdefl_start_dynamic_block(d);
	return tdefl_compress_lz_codes(d);
}
static int tdefl_flush_block(tdefl_compressor *d, int flush)
{
	uint8_t *pSaved_output_buf;
	bool comp_block_succeeded = false;
	bool use_raw_block = 0!=(d->m_flags&TDEFL_FORCE_ALL_RAW_BLOCKS)
	&&d->m_lookahead_pos-d->m_lz_code_buf_dict_pos<=d->m_dict_size;
	uint8_t *pOutput_buf_start;
	if(d->m_pPut_buf_func==nullptr&&*d->m_pOut_buf_size-d->m_out_buf_ofs>=TDEFL_OUT_BUF_SIZE)
	pOutput_buf_start = (uint8_t*)d->m_pOut_buf+d->m_out_buf_ofs;
	else pOutput_buf_start = d->m_output_buf;

	d->m_pOutput_buf = pOutput_buf_start;
	d->m_pOutput_buf_end = d->m_pOutput_buf+TDEFL_OUT_BUF_SIZE-16;

	assert(!d->m_output_flush_remaining);
	d->m_output_flush_ofs = 0;
	d->m_output_flush_remaining = 0;

	*d->m_pLZ_flags = uint8_t(*d->m_pLZ_flags>>d->m_num_flags_left);
	if(8==d->m_num_flags_left)
	d->m_pLZ_code_buf--;

	if(0!=(d->m_flags&TDEFL_WRITE_ZLIB_HEADER)&&0==d->m_block_index)
	{
		TDEFL_PUT_BITS(0x78,8)
		TDEFL_PUT_BITS(0x01,8)
	}

	TDEFL_PUT_BITS(flush==TDEFL_FINISH,1)

	pSaved_output_buf = d->m_pOutput_buf;
	unsigned saved_bit_buf = d->m_bit_buffer;
	unsigned saved_bits_in = d->m_bits_in;

	if(!use_raw_block) comp_block_succeeded = 
	tdefl_compress_block(d,0!=(d->m_flags&TDEFL_FORCE_ALL_STATIC_BLOCKS)||d->m_total_lz_bytes<48);

	/* If the block gets expanded, forget the current contents of the output buffer and send a raw block instead. */
	if((use_raw_block||0!=d->m_total_lz_bytes&&d->m_pOutput_buf-pSaved_output_buf+1U>=d->m_total_lz_bytes) 
	&&d->m_lookahead_pos-d->m_lz_code_buf_dict_pos<=d->m_dict_size)
	{
		unsigned i;
		d->m_pOutput_buf = pSaved_output_buf;
		d->m_bit_buffer = saved_bit_buf;
		d->m_bits_in = saved_bits_in;
		TDEFL_PUT_BITS(0,2)
		if(d->m_bits_in)
		{
			TDEFL_PUT_BITS(0,8-d->m_bits_in)
		}
		for(i=2;i;--i,d->m_total_lz_bytes^=0xFFFF)
		{
			TDEFL_PUT_BITS(d->m_total_lz_bytes&0xFFFF,16)
		}
		for(i=0;i<d->m_total_lz_bytes;++i)
		{
			TDEFL_PUT_BITS(d->m_dict[(d->m_lz_code_buf_dict_pos+i)&TDEFL_LZ_DICT_SIZE_MASK],8)
		}
	}		
	else
	{
		/* Check for the extremely unlikely (if not impossible) case of the compressed block not fitting into the output buffer when using dynamic codes. */
		if(!comp_block_succeeded)
		{
			d->m_pOutput_buf = pSaved_output_buf;
			d->m_bit_buffer = saved_bit_buf,d->m_bits_in = saved_bits_in;
			tdefl_compress_block(d,true);
		}
	}

	if(flush==TDEFL_FINISH)
	{
		if(d->m_bits_in)
		{
			TDEFL_PUT_BITS(0,8-d->m_bits_in)
		}
		/*if(0!=(d->m_flags&TDEFL_WRITE_ZLIB_HEADER))
		{
			for(unsigned i=0,a=d->m_adler32;i<4;i++)
			{
				TDEFL_PUT_BITS(a>>24&0xFF,8)
				a<<=8;
			}
		}*/
	}
	else if(0!=flush)
	{
		TDEFL_PUT_BITS(0,3)
		if(0!=d->m_bits_in)
		{
			TDEFL_PUT_BITS(0,8-d->m_bits_in)
		}
		for(unsigned i=2,z=0;i;i--,z^=0xFFFF)
		{
			TDEFL_PUT_BITS(z&0xFFFF,16)
		}
	}

	assert(d->m_pOutput_buf<d->m_pOutput_buf_end);

	memset(&d->m_huff_count[0][0],0,sizeof(d->m_huff_count[0][0])*TDEFL_MAX_HUFF_SYMBOLS_0);
	memset(&d->m_huff_count[1][0],0,sizeof(d->m_huff_count[1][0])*TDEFL_MAX_HUFF_SYMBOLS_1);

	d->m_pLZ_code_buf = d->m_lz_code_buf+1;
	d->m_pLZ_flags = d->m_lz_code_buf;
	d->m_num_flags_left = 8;
	d->m_lz_code_buf_dict_pos+=d->m_total_lz_bytes;
	d->m_total_lz_bytes = 0;
	d->m_block_index++;

	int n = int(d->m_pOutput_buf-pOutput_buf_start); 
	if(n!=0) if(d->m_pPut_buf_func!=nullptr)
	{
		*d->m_pIn_buf_size = d->m_pSrc-(const uint8_t*)d->m_pIn_buf;
		if(!(*d->m_pPut_buf_func)(d->m_output_buf,n,d->m_pPut_buf_user))
		return d->m_prev_return_status = TDEFL_STATUS_PUT_BUF_FAILED;
	}
	else if(pOutput_buf_start==d->m_output_buf)
	{
		int bytes_to_copy = (int)std::min<size_t>(n,*d->m_pOut_buf_size-d->m_out_buf_ofs);
		memcpy((uint8_t*)d->m_pOut_buf+d->m_out_buf_ofs,d->m_output_buf,bytes_to_copy);
		d->m_out_buf_ofs+=bytes_to_copy;
		n-=bytes_to_copy;
		if(0!=n)
		{
			d->m_output_flush_ofs = bytes_to_copy;
			d->m_output_flush_remaining = n;
		}
	}
	else d->m_out_buf_ofs+=n; return d->m_output_flush_remaining;
}
static TDEFL_FORCEINLINE void tdefl_find_match(tdefl_compressor *d, 
unsigned lookahead_pos, unsigned max_dist, unsigned max_match_len, unsigned*pMatch_dist, unsigned*pMatch_len)
{
	unsigned pos = lookahead_pos&TDEFL_LZ_DICT_SIZE_MASK;
	unsigned match_len = *pMatch_len;
	unsigned num_probes_left = d->m_max_probes[match_len>=32];
	const uint8_t *s = d->m_dict+pos,*p,*q;
	uint8_t c0 = d->m_dict[pos+match_len],c1 = d->m_dict[pos+match_len-1];
	assert(max_match_len<=TDEFL_MAX_MATCH_LEN);
	if(max_match_len<=match_len)
	return;
	for(unsigned probe_pos=pos,dist,next_probe_pos,probe_len;;)
	{
		for(;;)
		{
			if(0==--num_probes_left)
			return;
			#define TDEFL_PROBE\
			next_probe_pos = d->m_next[probe_pos];\
			if(0==next_probe_pos||(dist=uint16_t(lookahead_pos-next_probe_pos))>max_dist)\
			return;\
			probe_pos = next_probe_pos&TDEFL_LZ_DICT_SIZE_MASK;\
			if(c0==d->m_dict[probe_pos+match_len]&&c1==d->m_dict[probe_pos+match_len-1])\
			break;
			TDEFL_PROBE
			TDEFL_PROBE
			TDEFL_PROBE
		}
		if(0==dist) break;
		p = s;
		q = d->m_dict+probe_pos;
		for(probe_len = 0;probe_len<max_match_len;probe_len++)
		if(*p++!=*q++)
		break;
		if(probe_len>match_len)
		{
			*pMatch_dist = dist;
			*pMatch_len = match_len = probe_len;
			if(match_len==max_match_len)
			return;
			c0 = d->m_dict[pos+match_len];
			c1 = d->m_dict[pos+match_len-1];
		}
	}
}
static TDEFL_FORCEINLINE void tdefl_record_literal(tdefl_compressor *d,uint8_t lit)
{
	d->m_total_lz_bytes++;
	*d->m_pLZ_code_buf++ = lit;
	*d->m_pLZ_flags = uint8_t(*d->m_pLZ_flags>>1);
	if(0==--d->m_num_flags_left)
	{
		d->m_num_flags_left = 8;
		d->m_pLZ_flags = d->m_pLZ_code_buf++;
	}
	d->m_huff_count[0][lit]++;
}

static TDEFL_FORCEINLINE void tdefl_record_match(tdefl_compressor *d,unsigned match_len,unsigned match_dist)
{
	uint32_t s0,s1;

	assert(match_len>=TDEFL_MIN_MATCH_LEN&&match_dist>=1&&match_dist<=TDEFL_LZ_DICT_SIZE);

	d->m_total_lz_bytes+=match_len;

	d->m_pLZ_code_buf[0] = uint8_t(match_len-TDEFL_MIN_MATCH_LEN);

	match_dist-=1;
	d->m_pLZ_code_buf[1] = uint8_t(match_dist&0xFF);
	d->m_pLZ_code_buf[2] = uint8_t(match_dist>>8);
	d->m_pLZ_code_buf+=3;

	*d->m_pLZ_flags = uint8_t(*d->m_pLZ_flags>>1|0x80);
	if(0==--d->m_num_flags_left)
	{
		d->m_num_flags_left = 8;
		d->m_pLZ_flags = d->m_pLZ_code_buf++;
	}

	s0 = s_tdefl_small_dist_sym[match_dist&511];
	s1 = s_tdefl_large_dist_sym[match_dist>>8&0x7F];
	d->m_huff_count[1][match_dist<512?s0:s1]++;

	if(match_len>=TDEFL_MIN_MATCH_LEN)
	d->m_huff_count[0][s_tdefl_len_sym[match_len-TDEFL_MIN_MATCH_LEN]]++;
}
static bool tdefl_compress_normal(tdefl_compressor *d)
{
	const uint8_t *pSrc = d->m_pSrc;
	size_t src_buf_left = d->m_src_buf_left;
	tdefl_flush flush = d->m_flush;

	while(0!=src_buf_left||0!=flush&&0!=d->m_lookahead_size)
	{
		/* Update dictionary and hash chains. Keeps the lookahead size equal to TDEFL_MAX_MATCH_LEN. */
		if(d->m_lookahead_size+d->m_dict_size>=TDEFL_MIN_MATCH_LEN-1)
		{
			unsigned dst_pos = (d->m_lookahead_pos+d->m_lookahead_size)&TDEFL_LZ_DICT_SIZE_MASK;
			unsigned ins_pos = d->m_lookahead_pos+d->m_lookahead_size-2;
			unsigned hash = d->m_dict[ins_pos&TDEFL_LZ_DICT_SIZE_MASK];
			hash<<=TDEFL_LZ_HASH_SHIFT;
			hash^=d->m_dict[(ins_pos+1)&TDEFL_LZ_DICT_SIZE_MASK];
			unsigned num_bytes_to_process = (unsigned)std::min(src_buf_left,TDEFL_MAX_MATCH_LEN-d->m_lookahead_size);
			const uint8_t *pSrc_end = pSrc+num_bytes_to_process;
			src_buf_left-=num_bytes_to_process;
			d->m_lookahead_size += num_bytes_to_process;
			while(pSrc!=pSrc_end)
			{
				uint8_t c = *pSrc++;
				d->m_dict[dst_pos] = c;
				if(dst_pos<(TDEFL_MAX_MATCH_LEN-1))
				d->m_dict[TDEFL_LZ_DICT_SIZE+dst_pos] = c;
				hash = (hash<<TDEFL_LZ_HASH_SHIFT^c)&(TDEFL_LZ_HASH_SIZE-1);
				d->m_next[ins_pos&TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
				d->m_hash[hash] = uint16_t(ins_pos);
				dst_pos = (dst_pos+1)&TDEFL_LZ_DICT_SIZE_MASK;
				ins_pos++;
			}
		}
		else while(0!=src_buf_left&&d->m_lookahead_size<TDEFL_MAX_MATCH_LEN)
		{
			uint8_t c = *pSrc++;
			unsigned dst_pos = (d->m_lookahead_pos+d->m_lookahead_size)&TDEFL_LZ_DICT_SIZE_MASK;
			src_buf_left--;
			d->m_dict[dst_pos] = c;
			if(dst_pos<(TDEFL_MAX_MATCH_LEN-1))
			d->m_dict[TDEFL_LZ_DICT_SIZE+dst_pos] = c;
			if(++d->m_lookahead_size+d->m_dict_size>=TDEFL_MIN_MATCH_LEN)
			{
				unsigned ins_pos = d->m_lookahead_pos+(d->m_lookahead_size-1)-2;
				unsigned hash = d->m_dict[ins_pos&TDEFL_LZ_DICT_SIZE_MASK];
				hash<<=2*TDEFL_LZ_HASH_SHIFT;
				hash^=d->m_dict[(ins_pos+1)&TDEFL_LZ_DICT_SIZE_MASK]<<TDEFL_LZ_HASH_SHIFT;
				hash^=c;
				hash&=TDEFL_LZ_HASH_SIZE-1;
				d->m_next[ins_pos&TDEFL_LZ_DICT_SIZE_MASK] = d->m_hash[hash];
				d->m_hash[hash] = uint16_t(ins_pos);
			}
		}
		d->m_dict_size = std::min(TDEFL_LZ_DICT_SIZE-d->m_lookahead_size,d->m_dict_size);
		if(0==flush&&d->m_lookahead_size<TDEFL_MAX_MATCH_LEN)
		break;

		/* Simple lazy/greedy parsing state machine. */		
		unsigned len_to_move = 1;
		unsigned cur_match_dist = 0;
		unsigned cur_match_len = d->m_saved_match_len?d->m_saved_match_len:TDEFL_MIN_MATCH_LEN-1;
		unsigned cur_pos = d->m_lookahead_pos&TDEFL_LZ_DICT_SIZE_MASK;
		if(0==(d->m_flags&(TDEFL_RLE_MATCHES|TDEFL_FORCE_ALL_RAW_BLOCKS)))
		{
			tdefl_find_match(d,d->m_lookahead_pos,d->m_dict_size,d->m_lookahead_size,&cur_match_dist,&cur_match_len);
		}
		else if(0!=d->m_dict_size&&0==(d->m_flags&TDEFL_FORCE_ALL_RAW_BLOCKS))
		{
			uint8_t c = d->m_dict[(cur_pos-1)&TDEFL_LZ_DICT_SIZE_MASK];
			cur_match_len = 0;
			while(cur_match_len<d->m_lookahead_size)
			{
				if(c!=d->m_dict[cur_pos+cur_match_len])
				break;
				cur_match_len++;
			}
			if(cur_match_len<TDEFL_MIN_MATCH_LEN)
			cur_match_len = 0;
			else
			cur_match_dist = 1;
		}
		if(cur_match_len==TDEFL_MIN_MATCH_LEN&&cur_match_dist>=8U*1024U
		||cur_pos==cur_match_dist
		||0!=(d->m_flags&TDEFL_FILTER_MATCHES)&&cur_match_len<=5)
		{
			cur_match_dist = cur_match_len = 0;
		}
		if(d->m_saved_match_len)
		{
			if(cur_match_len>d->m_saved_match_len)
			{
				tdefl_record_literal(d,(uint8_t)d->m_saved_lit);
				if(cur_match_len>=128)
				{
					tdefl_record_match(d,cur_match_len,cur_match_dist);
					d->m_saved_match_len = 0;
					len_to_move = cur_match_len;
				}
				else
				{
					d->m_saved_lit = d->m_dict[cur_pos];
					d->m_saved_match_dist = cur_match_dist;
					d->m_saved_match_len = cur_match_len;
				}
			}
			else
			{
				tdefl_record_match(d,d->m_saved_match_len,d->m_saved_match_dist);
				len_to_move = d->m_saved_match_len-1;
				d->m_saved_match_len = 0;
			}
		}
		else if(!cur_match_dist)
		{
			tdefl_record_literal(d,d->m_dict[std::min(cur_pos,sizeof(d->m_dict)-1)]);
		}
		else if(d->m_greedy_parsing||0!=(d->m_flags&TDEFL_RLE_MATCHES)||cur_match_len>=128)
		{
			tdefl_record_match(d,cur_match_len,cur_match_dist);
			len_to_move = cur_match_len;
		}
		else
		{
			d->m_saved_lit = d->m_dict[std::min(cur_pos,sizeof(d->m_dict)-1)];
			d->m_saved_match_dist = cur_match_dist;
			d->m_saved_match_len = cur_match_len;
		}
		/* Move the lookahead forward by len_to_move bytes. */
		d->m_lookahead_pos+=len_to_move;
		assert(d->m_lookahead_size>=len_to_move);
		d->m_lookahead_size-=len_to_move;
		d->m_dict_size = std::min(d->m_dict_size+len_to_move,(unsigned)TDEFL_LZ_DICT_SIZE);
		/* Check if it's time to flush the current LZ codes to the internal output buffer. */
		if(d->m_pLZ_code_buf>&d->m_lz_code_buf[TDEFL_LZ_CODE_BUF_SIZE-8]
		||(d->m_total_lz_bytes>31*1024
		&&(unsigned(d->m_pLZ_code_buf-d->m_lz_code_buf)*115>>7>=d->m_total_lz_bytes
		||0!=(d->m_flags&TDEFL_FORCE_ALL_RAW_BLOCKS))))
		{
			int n;
			d->m_pSrc = pSrc;
			d->m_src_buf_left = src_buf_left;
			n = tdefl_flush_block(d,0);
			if(0!=n)
			return n<0?false:true;
		}
	}

	d->m_pSrc = pSrc;
	d->m_src_buf_left = src_buf_left; return true;
}

static tdefl_status tdefl_flush_output_buffer(tdefl_compressor *d)
{
	if(0!=d->m_pIn_buf_size)
	*d->m_pIn_buf_size = d->m_pSrc-(const uint8_t*)d->m_pIn_buf;	
	if(0!=d->m_pOut_buf_size)
	{
		size_t n = std::min(*d->m_pOut_buf_size-d->m_out_buf_ofs,d->m_output_flush_remaining);
		memcpy((uint8_t*)d->m_pOut_buf+d->m_out_buf_ofs,d->m_output_buf+d->m_output_flush_ofs,n);
		d->m_output_flush_ofs+=(unsigned)n;
		d->m_output_flush_remaining-=(unsigned)n;
		d->m_out_buf_ofs+=n;

		*d->m_pOut_buf_size = d->m_out_buf_ofs;
	}
	return 0!=d->m_finished&&0==d->m_output_flush_remaining?TDEFL_STATUS_DONE:TDEFL_STATUS_OKAY;
}
tdefl_status tdefl_compress(tdefl_compressor*d, const void*pIn_buf,size_t *pIn_buf_size,void *pOut_buf, size_t*pOut_buf_size, tdefl_flush flush)
{
	if(d==nullptr)
	{
		if(pIn_buf_size)
		*pIn_buf_size = 0;
		if(pOut_buf_size)
		*pOut_buf_size = 0;
		return TDEFL_STATUS_BAD_PARAM;
	}

	d->m_pIn_buf = pIn_buf;
	d->m_pIn_buf_size = pIn_buf_size;
	d->m_pOut_buf = pOut_buf;
	d->m_pOut_buf_size = pOut_buf_size;
	d->m_pSrc = (const uint8_t*)pIn_buf;
	d->m_src_buf_left = pIn_buf_size?*pIn_buf_size:0;
	d->m_out_buf_ofs = 0;
	d->m_flush = flush;

	if((d->m_pPut_buf_func!=nullptr)
	==(pOut_buf!=nullptr||pOut_buf_size!=nullptr)
	||(d->m_prev_return_status!=TDEFL_STATUS_OKAY)
	||(d->m_wants_to_finish&&flush!=TDEFL_FINISH)
	||(0!=pIn_buf_size&&0!=*pIn_buf_size&&pIn_buf==nullptr)
	||(0!=pOut_buf_size&&0!=*pOut_buf_size&&pOut_buf==nullptr))
	{
		if(pIn_buf_size)
		*pIn_buf_size = 0;
		if(pOut_buf_size)
		*pOut_buf_size = 0;
		return d->m_prev_return_status = TDEFL_STATUS_BAD_PARAM;
	}
	if(flush==TDEFL_FINISH)
	d->m_wants_to_finish = true;

	if(0==d->m_output_flush_remaining&&0==d->m_finished)
	{			
		//tdefl_compress_fast OMITTED
		if(!tdefl_compress_normal(d)) 
		return d->m_prev_return_status;

	//	if(0!=(d->m_flags&(TDEFL_WRITE_ZLIB_HEADER|TDEFL_COMPUTE_ADLER32))&&pIn_buf!=nullptr)
	//	d->m_adler32 = (uint32_t)mz_adler32(d->m_adler32,(const uint8_t*)pIn_buf,d->m_pSrc-(const uint8_t*)pIn_buf);

		if(0!=flush&&0==d->m_lookahead_size&&0==d->m_src_buf_left&&0==d->m_output_flush_remaining)
		{
			if(tdefl_flush_block(d,flush)<0)
			return d->m_prev_return_status;
			d->m_finished = (flush==TDEFL_FINISH);
			if(flush==TDEFL_FULL_FLUSH)
			{
				TINFL_ZERO_MEM(d->m_hash) 
				TINFL_ZERO_MEM(d->m_next)
				d->m_dict_size = 0;
			}
		}
	}

	return d->m_prev_return_status = tdefl_flush_output_buffer(d);
}



////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp
////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp
////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp ////daeZAEUncompressHandler.cpp



extern unsigned *get_tinfl_decompressor_state_etc(void *decomp, size_t sizeof_decomp, size_t etc)
{
	assert(sizeof_decomp+etc*sizeof(unsigned)>=sizeof(tinfl_decompressor));
	daeCTC<(daeZAEPlugin::_gz_implementation>=sizeof(tinfl_decompressor))>();
	return &((tinfl_decompressor*)decomp)->m_state;
}
extern daeError tinfl_decompress(void *decomp, const void *a, size_t *b, void *c, void *d, size_t *e)
{
	tinfl_status st = tinfl_decompress((tinfl_decompressor*)decomp,(uint8_t*)a,b,(uint8_t*)c,(uint8_t*)d,e);
	switch(st)
	{
	case TINFL_STATUS_DONE: return DAE_OK;
	case TINFL_STATUS_HAS_MORE_OUTPUT:
		//daeGZ: Assuming input is unbounded on both read and write operations. (Writing is always so.)
	case TINFL_STATUS_FAILED_CANNOT_MAKE_PROGRESS: //TINFL_STATUS_NEEDS_MORE_INPUT: 
		return DAE_NOT_NOW; default: return DAE_ERROR; 
	}
}

//A tdefl_compressor is new allocated if io==nullptr unless nullptr==pPut_buf_func;
//The tdefl_compressor io is SAFELY delete deallocated when nullptr==pPut_buf_func;
//Otherwise, if io!=nullptr, no allocation occurs, except io will be reinitialized.
extern void *new_tdefl_compressor(bool(*pPut_buf_func)(const void*,size_t,void*), void *pPut_buf_user, void *io)
{
	enum{ flags=0xFFF };

	tdefl_compressor *d = (tdefl_compressor*)io;
	if(d==nullptr) d = new tdefl_compressor();
	if(nullptr==pPut_buf_func)
	{
		delete d; return nullptr;
	}

    d->m_pPut_buf_func = pPut_buf_func;
    d->m_pPut_buf_user = pPut_buf_user;
    d->m_flags = (unsigned)(flags);
    d->m_max_probes[0] = 1 + ((flags & 0xFFF) + 2) / 3;
//    d->m_greedy_parsing = (flags & TDEFL_GREEDY_PARSING_FLAG) != 0;
    d->m_max_probes[1] = 1 + (((flags & 0xFFF) >> 2) + 2) / 3;
//    if (!(flags & TDEFL_NONDETERMINISTIC_PARSING_FLAG))
        TINFL_ZERO_MEM(d->m_hash);
    d->m_lookahead_pos = d->m_lookahead_size = d->m_dict_size = d->m_total_lz_bytes = d->m_lz_code_buf_dict_pos = d->m_bits_in = 0;
    d->m_output_flush_ofs = d->m_output_flush_remaining = d->m_finished = d->m_block_index = d->m_bit_buffer = d->m_wants_to_finish = 0;
    d->m_pLZ_code_buf = d->m_lz_code_buf + 1;
    d->m_pLZ_flags = d->m_lz_code_buf;
    d->m_num_flags_left = 8;
    d->m_pOutput_buf = d->m_output_buf;
    d->m_pOutput_buf_end = d->m_output_buf;
    d->m_prev_return_status = TDEFL_STATUS_OKAY;
    d->m_saved_match_dist = d->m_saved_match_len = d->m_saved_lit = 0;
    d->m_adler32 = 1;
    d->m_pIn_buf = nullptr;
    d->m_pOut_buf = nullptr;
    d->m_pIn_buf_size = nullptr;
    d->m_pOut_buf_size = nullptr;
    d->m_flush = TDEFL_NO_FLUSH;
    d->m_pSrc = nullptr;
    d->m_src_buf_left = 0;
    d->m_out_buf_ofs = 0;
    memset(&d->m_huff_count[0][0],0,sizeof(d->m_huff_count[0][0])*TDEFL_MAX_HUFF_SYMBOLS_0);
    memset(&d->m_huff_count[1][0],0,sizeof(d->m_huff_count[1][0])*TDEFL_MAX_HUFF_SYMBOLS_1);   
	//return TDEFL_STATUS_OKAY;
	return d;
}
extern daeError tdefl_compress(void *comp, const void *pIn_buf=nullptr, size_t pIn_buf_size=0)
{
	//Reminder: TDEFL_FINISH introduces an artifical error (TDEFL_STATUS_BAD_PARAM)
	//when called more than once. Kind of annoying if there are more than one place
	//that checks that the final burst of input was flushed.
	//tdefl_flush flush = pIn_buf==nullptr?TDEFL_FULL_FLUSH:TDEFL_NO_FLUSH;
	tdefl_flush flush = TDEFL_NO_FLUSH; if(pIn_buf==nullptr)
	{
		//Hmm?? There is "TDEFL_PUT_BITS(flush==TDEFL_FINISH,1)" code that suggests
		//that a terminator (1bit?) is inserted into flush-points into DEFLATE data.
		flush = TDEFL_FINISH;
		if(TDEFL_STATUS_DONE==((tdefl_compressor*)comp)->m_prev_return_status)
		return DAE_OK;
	}
	tdefl_status st = tdefl_compress((tdefl_compressor*)comp,pIn_buf,&pIn_buf_size,nullptr,nullptr,flush);
	switch(st)
	{	
	case TDEFL_STATUS_OKAY:
	case TDEFL_STATUS_DONE:
		
		//assert(0==pIn_buf_size);
		return DAE_OK;

	case TDEFL_STATUS_BAD_PARAM: assert(0);
	case TDEFL_STATUS_PUT_BUF_FAILED:
	default: //compiler
		
		return DAE_ERROR;
	}
}


//---.
}//<-'

#endif //__COLLADA_DOM__ZAE

/*C1071*/