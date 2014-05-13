#include "A256Def.h"

#pragma pack(push, 1)

// basic scalarity selector 1

/*
1) selected data is converted (or not) and repeated
2) "packing" means, selected fragment is broadcasted as is, with no conversion
3) "saturation" means, conversion to operating type where values exceeding bounds are set to those bounds
4) "convert" means, source or destination is interleaved with zero values (depends on sizes)
000 ##### - select _ub[] with saturation ($00.ub0 .. $FF.ub31)
001 ##### - select _sb[] with saturation ($00.sb0 .. $FF.sb31)
0100 #### - select _uw[] with packing ($00.uw0 .. $FF.uw15, $00.sw0 .. $FF.sw15)
0101 #### - not used
0110 #### - select _uw[] with saturation ($00.uws0 .. $FF.uws15)
0111 #### - select _sw[] with saturation ($00.sws0 .. $FF.sws15)
10000 ### - select _ud[] with packing ($00.ud0 .. $FF.ud7, $00.sd0 .. $FF.sd7, $00.fs0 .. $FF.fs7)
10001 ### - select _fs[] with saturation ($00.fss0 .. $FF.fss7)
10010 ### - select _ud[] with saturation ($00.uds0 .. $FF.uds7)
10011 ### - select _sd[] with saturation ($00.sds0 .. $FF.sds7)
101 @@ ### - select _fs[] with rounding and saturation ($00.fsr0, $00.fst0, $00.fsf0, $00.fsc0 .. $FF.fsc7)
110000 ## - select _uq[] with packing ($00.uq0 .. $FF.uq3, $00.sq0 .. $FF.sq3, $00.fd0 .. $FF.fd3)
110001 ## - select _fd[] with saturation ($00.fds0 .. $FF.fds3)
110010 ## - select _uq[] with saturation ($00.uqs0 .. $FF.uqs3)
110011 ## - select _sq[] with saturation ($00.sqs0 .. $FF.sqs3)
1101 @@ ## - select _fd[] with rounding and saturation ($00.fdr0, $00.fdt0, $00.fdf0, $00.fdc0 .. $FF.fdc3)
1110000 # - select _dq[] with packing ($00.dq0 .. $FF.dq1)

11100010 - zero-extend bytes to words ($00.zxbw)
11100011 - sign-extend bytes to words ($00.sxbw)
11100100 - zero-extend bytes to dwords ($00.zxbd)
11100101 - sign-extend bytes to dwords ($00.sxbd)
11100110 - zero-extend bytes to qwords ($00.zxbq)
11100111 - sign-extend bytes to qwords ($00.sxbq)
11101000 - zero-extend words to dwords ($00.zxwd)
11101001 - sign-extend words to dwords ($00.sxwd)
11101010 - zero-extend words to qwords ($00.zxwq)
11101011 - sign-extend words to qwords ($00.sxwq)
11101100 - zero-extend dwords to qwords ($00.zxdq)
11101101 - sign-extend dwords to qwords ($00.sxdq)
11101110 - zero-extend qwords to double qwords ($00.zxqdq)
11101111 - sign-extend qwords to double qwords ($00.sxqdq)

11110000 - convert from single with saturation ($00.getfs)
11110001 - convert from double with saturation ($00.getfd)
11110010 - convert from unsigned byte with saturation ($00.getub)
11110011 - convert from signed byte with saturation ($00.getsb)
11110100 - convert from unsigned word with saturation ($00.getuw)
11110101 - convert from signed word with saturation ($00.getsw)
11110110 - convert from unsigned dword with saturation ($00.getud)
11110111 - convert from signed dword with saturation ($00.getsd)
11111000 - convert from unsigned qword with saturation ($00.getuq)
11111001 - convert from signed qword with saturation ($00.getsq)

11111010 - load immediate (0 plus reg number) (0 .. 255)
11111011 - load immediate (-256 plus reg number) (-1 .. -256)
11111100 - load immediate (256 plus reg number) (256 .. 511)
11111101 - load immediate (-512 plus reg number) (-257 .. -512)

11111110 - full data (inverted - logical NOT) ($00.not)
11111111 - full data (unchanged) ($00 .. $FF)
*/

// basic sign manipulator 1 (for 4 op)

/*
aabbccrr: 2 bit for each argument (a, b, c and result)
00: do nothing
01: abs(): set sign to positive, may not work correctly for minimal int (abs, $FF.abs)
10: neg(): invert sign, may not work correctly for minimal int (neg, $FF.neg)
11: neg(abs()): set sign to negative (negabs, $FF.negabs)
*/

// 256-bit register struct
union A256Reg
{
	u256 _qq;
	u128 _dq[2];
	f32 _fs[8];
	f64 _fd[4];
	u8 _ub[32];
	u16 _uw[16];
	u32 _ud[8];
	u64 _uq[4];
	s8 _sb[32];
	s16 _sw[16];
	s32 _sd[8];
	s64 _sq[4];

	A256Reg operator ~()
	{
		A256Reg res;
		for (u32 i = 0; i < 4; i++)
		{
			res._uq[i] = ~_uq[i];
		}
		return res;
	}

	A256Reg operator &(A256Reg& right)
	{
		A256Reg res;
		for (u32 i = 0; i < 4; i++)
		{
			res._uq[i] = _uq[i] & right._uq[i];
		}
		return res;
	}

	A256Reg operator |(A256Reg& right)
	{
		A256Reg res;
		for (u32 i = 0; i < 4; i++)
		{
			res._uq[i] = _uq[i] | right._uq[i];
		}
		return res;
	}

	A256Reg operator ^(A256Reg& right)
	{
		A256Reg res;
		for (u32 i = 0; i < 4; i++)
		{
			res._uq[i] = _uq[i] ^ right._uq[i];
		}
		return res;
	}

	template<typename T>
	T& get(u32 index)
	{
		if (index >= 32 / sizeof(T))
		{
			// TODO
		}
		return *(((T*)this) + index);
	}

	template<typename T>
	void fill(const T& data)
	{
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			get<T>(i) = data;
		}
	}

	template<typename T>
	static A256Reg set(const T& data)
	{
		A256Reg res;
		res.fill<T>(data);
		return res;
	}

	template<typename Tout, typename Tin>
	static void saturate(Tout& out, const Tin& in)
	{
		const Tout out_max = std::numeric_limits<Tout>::max();
		const Tout out_min = std::numeric_limits<Tout>::min();
		const bool out_signed = std::numeric_limits<Tout>::is_signed;
		const bool out_float = !std::numeric_limits<Tout>::is_exact;
		const Tin in_max = std::numeric_limits<Tin>::max();
		const Tin in_min = std::numeric_limits<Tin>::min();
		const bool in_signed = std::numeric_limits<Tin>::is_signed;
		const bool in_float = !std::numeric_limits<Tin>::is_exact;

		if (sizeof(Tout) == sizeof(Tin) && out_signed == in_signed && out_float == in_float)
		{
			out = (Tout)in;
		}
		else if (out_float || in_float)
		{
			// float input/output
			if (in_float && !out_float)
			{
				out = (in < (Tin)out_min) ? out_min : ((Tin)out_max < in) ? out_max : (Tout)in;
			}
			else
			{
				out = (Tout)in;
			}
		}
		else if (sizeof(Tout) == 8 || sizeof(Tin) == 8)
		{
			if (sizeof(Tout) == 8 && sizeof(Tin) == 8)
			{
				if (in_signed)
				{
					out = (in < 0) ? 0 : (Tout)in;
				}
				else // out_signed
				{
					out = ((s64)in < 0) ? out_max : (Tout)in;
				}
			}
			else if (sizeof(Tout) == 8)
			{
				if (out_signed == in_signed)
				{
					out = (Tout)in;
				}
				else if (in_signed)
				{
					out = (in < 0) ? 0 : (Tout)in;
				}
				else // out_signed
				{
					out = (Tout)in;
				}
			}
			else // sizeof(Tin) == 8
			{
				if (in_signed)
				{
					out = ((s64)in < (s64)out_min) ? out_min : ((s64)out_max < (s64)in) ? out_max : (Tout)in;
				}
				else
				{
					out = ((u64)out_max < (u64)in) ? out_max : (Tout)in;
				}
			}
		}
		else
		{
			out = ((s64)in < (s64)out_min) ? out_min : ((s64)out_max < (s64)in) ? out_max : (Tout)in;
		}
		static_assert(sizeof(Tin) <= 8 && sizeof(Tout) <= 8, __FUNCTION__"(): invalid type");
	}

	template<typename Tin, typename Tout>
	void convert(A256Reg* data)
	{
		if (sizeof(Tout) > sizeof(Tin))
		{
			for (u32 i = 0; i < 32 / sizeof(Tout); i++)
			{
				saturate<Tout, Tin>(*(((Tout*)this) + i), *(Tin*)(((Tout*)data) + i));
			}
		}
		else
		{
			for (u32 i = 0; i < 32 / sizeof(Tin); i++)
			{
				*(Tout*)(((Tin*)this) + i) = 0;
				saturate<Tout, Tin>(*(Tout*)(((Tin*)this) + i), *(((Tin*)this) + i));
			}
		}
	}

	template<typename T>
	A256Reg bsc1(u8 code, u8 regnum) // basic scalarity selector
	{
		A256Reg res = *this;
		T sel;
		switch (code >> 5)
		{
		case 0: // select unsigned byte with saturation
		{
			saturate(sel, _ub[code % 32]);
			res.fill(sel);
			break;
		}
		case 1: // select signed byte with saturation
		{
			saturate(sel, _sb[code % 32]);
			res.fill(sel);
			break;
		}
		case 2: // select (...) word with packing
		{
			if (code & 0x10)
			{
				throw fmt::format("Unsupported bsc1 encoding");
			}
			res.fill(_uw[code % 16]); // packing
			break;
		}
		case 3: // select (...) word with saturation
		{
			if (code & 0x10) // signed word
			{
				saturate(sel, _sw[code % 16]);
				res.fill(sel);
			}
			else // unsigned word
			{
				saturate(sel, _uw[code % 16]);
				res.fill(sel);
			}
			break;
		}
		case 4: // select (...) dword (...)
		{
			if (code & 0x10) // saturation
			{
				if (code & 8) // signed dword
				{
					saturate(sel, _sd[code % 8]);
					res.fill(sel);
				}
				else // unsigned dword
				{
					saturate(sel, _ud[code % 8]);
					res.fill(sel);
				}
			}
			else if (code & 8) // single
			{
				saturate(sel, _fs[code % 8]);
				res.fill(sel);
			}
			else // packing
			{
				res.fill(_ud[code % 8]);
			}
			break;
		}
		case 5: // select single precision float
		{
			if (code & 0x10)
			{
				if (code & 8)
				{
					saturate(sel, trunc(_fs[code % 8]));
				}
				else
				{
					saturate(sel, ceil(_fs[code % 8]));
				}
			}
			else
			{
				if (code & 8)
				{
					saturate(sel, floor(_fs[code % 8]));
				}
				else
				{
					saturate(sel, round(_fs[code % 8]));
				}
			}
			res.fill(sel);
			break;
		}
		case 6: // select (...) qword (...) or double precision float
		{
			if (code & 0x10) // double
			{
				if (code & 8)
				{
					if (code & 4)
					{
						saturate(sel, trunc(_fd[code % 4]));
					}
					else
					{
						saturate(sel, ceil(_fd[code % 4]));
					}
				}
				else
				{
					if (code & 4)
					{
						saturate(sel, floor(_fd[code % 4]));
					}
					else
					{
						saturate(sel, round(_fd[code % 4]));
					}
				}
				res.fill(sel);
			}
			else
			{
				if (code & 0x8) // saturation
				{
					if (code & 4) // signed
					{
						saturate(sel, _sq[code % 4]);
						res.fill(sel);
					}
					else // unsigned
					{
						saturate(sel, _uq[code % 4]);
						res.fill(sel);
					}
				}
				else if (code & 4) // double
				{
					saturate(sel, _fd[code % 4]);
					res.fill(sel);
				}
				else // packing
				{
					res.fill(_uq[code % 4]);
				}
			}
			break;
		}
		case 7: switch (code % 32) // select dqword with packing or the whole vector (...)
		{
		case 0x00:
		case 0x01:
		{
			res.fill(_dq[code % 2]); // double qword
			break;
		}
		case 0x02:
		{
			res.convert<u8, u16>(this);
			break;
		}
		case 0x03:
		{
			res.convert<s8, s16>(this);
			break;
		}
		case 0x04:
		{
			res.convert<u8, u32>(this);
			break;
		}
		case 0x05:
		{
			res.convert<s8, s32>(this);
			break;
		}
		case 0x06:
		{
			res.convert<u8, u64>(this);
			break;
		}
		case 0x07:
		{
			res.convert<s8, s64>(this);
			break;
		}
		case 0x08:
		{
			res.convert<u16, u32>(this);
			break;
		}
		case 0x09:
		{
			res.convert<s16, s32>(this);
			break;
		}
		case 0x0a:
		{
			res.convert<u16, u64>(this);
			break;
		}
		case 0x0b:
		{
			res.convert<s16, s64>(this);
			break;
		}
		case 0x0c:
		{
			res.convert<u32, u64>(this);
			break;
		}
		case 0x0d:
		{
			res.convert<s32, s64>(this);
			break;
		}
		case 0x0e:
		{
			res._uq[0] = _uq[0];
			res._uq[1] = 0;
			res._uq[2] = _uq[2];
			res._uq[3] = 0;
			break;
		}
		case 0x0f:
		{
			res._sq[0] = _sq[0];
			res._sq[1] = _sq[0] >> 63;
			res._sq[2] = _sq[2];
			res._sq[3] = _sq[2] >> 63;
			break;
		}
		case 0x10:
		{
			res.convert<f32, T>(this);
			break;
		}
		case 0x11:
		{
			res.convert<f64, T>(this);
			break;
		}
		case 0x12:
		{
			res.convert<u8, T>(this);
			break;
		}
		case 0x13:
		{
			res.convert<s8, T>(this);
			break;
		}
		case 0x14:
		{
			res.convert<u16, T>(this);
			break;
		}
		case 0x15:
		{
			res.convert<s16, T>(this);
			break;
		}
		case 0x16:
		{
			res.convert<u32, T>(this);
			break;
		}
		case 0x17:
		{
			res.convert<s32, T>(this);
			break;
		}
		case 0x18:
		{
			res.convert<u64, T>(this);
			break;
		}
		case 0x19:
		{
			res.convert<s64, T>(this);
			break;
		}
		case 0x1a:
		{
			sel = (T)regnum;
			res.fill(sel);
			break;
		}
		case 0x1b:
		{
			sel = (T)(~0ull & regnum);
			res.fill(sel);
			break;
		}
		case 0x1c:
		{
			sel = (T)(256ull + regnum);
			res.fill(sel);
			break;
		}
		case 0x1d:
		{
			sel = (T)(~256ull & regnum);
			res.fill(sel);
			break;
		}
		case 0x1e:
		{
			res = ~*this;
			break;
		}
		case 0x1f:
		{
			// do nothing
			break;
		}
		}
		}
		return res;
	}

	std::string bsc1_fmt(u8 code)
	{
		// TODO
		return (code == 0xff) ? "" : fmt::format(" (bsc 0x%x)", code);
	}

	template<typename T>
	A256Reg bsign1(u8 code)
	{
		A256Reg res = *this;
		if (code & 1) // abs
		{
			for (u32 i = 0; i < 32 / sizeof(T); i++)
			{
				T& data = get<T>(i);
				data = abs(data);
			}
		}
		if (code & 2) // minus
		{
			for (u32 i = 0; i < 32 / sizeof(T); i++)
			{
				T& data = get<T>(i);
				data = (T)0 - data;
			}
		}
		return res;
	}
};

// instruction (size 64 bit)
struct A256Cmd
{
	u16 cmd;
	union
	{
		u8 raw[6];
		u16 raww[3];

		struct A256Op1Imm32 // 1 reg + immediate
		{
			u8 r; // result register
			u8 r_mask; // special info (for example, store mask immediate)
			u32 imm; // data
		} op1i;

		struct A256Op2Imm32 // 2 reg + immediate
		{
			u8 r;
			u8 a;
			u32 imm;
		} op2i;

		struct A256Op3 // 3 regs
		{
			u8 r;
			u8 r_mask;
			u8 a; // argument 1 (for example, res = a + b)
			u8 a_mask; // special info (for example, scalarity selector)
			u8 b; // argument 2
			u8 b_mask; // special info
		} op3;

		struct A256Op4 // 4 regs
		{
			u8 r;
			u8 r_mask;
			u8 a;
			u8 b;
			u8 c;
			u8 arg_mask; // special info (for example, sign manipulator)
		} op4;

		struct A256Op6 // 6 regs
		{
			u8 r;
			u8 arg[5];
		} op6;
	};
};

#pragma pack(pop)