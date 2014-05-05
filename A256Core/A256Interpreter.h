#pragma once

#include "A256Reg.h"

#define RSAVE1(dst, src, mask) for (u32 i = 0; i < 8; i++) if ((mask) & (1 << i)) (dst)._ud[i] = (src)._ud[i];

struct A256Machine
{
	/*
	Register $00 is special.
	1) $00.uq0 is pointer to next instruction ($NP).
	2) $00.uq1 is callstack pointer ($CS).
	3) $00.uq2 is stack base pointer ($BP), should be preserved by subroutine.
	4) $00.uq3 is stack pointer ($SP), should be preserved by subroutine.

	Registers $01 .. $9F are volatile, first ones are function arguments and results.
	$01 - first 256-bit argument and function result,
	$02 - second argument,
	$03 - third argument, etc.
	Can contain small (up to 32 byte) struct with smaller values packed.

	Passing 64-bit arguments (for example, pointers):
	$01.uq0 - first argument and function result,
	$02.uq0 - second argument,
	$03.uq0 - thirt argument, etc.

	Up to 159 256-bit parameters can be passed via registers.
	Passing parameters via stack is undefined.

	Registers $A0 .. $FF should be preserved by subroutine.
	*/

	A256Reg reg[256]; // registers $00 .. $FF
	A256Cmd op; // current operation (copied from memory)

	A256Machine()
	{
		memset(&reg, 0, sizeof(reg));
	}

	void stop() // interrupt (stop r.bsc, imm32)
	{
		switch (s32 code = op.op1i.imm)
		{
		case 0x00: // exit
		{
			(u64&)op = 0;
			break;
		}
		case 0x01: // print f32
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<f32>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:fs %f %f %f %f %f %f %f %f\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._fs[0], arg1._fs[1], arg1._fs[2], arg1._fs[3],
				arg1._fs[4], arg1._fs[5], arg1._fs[6], arg1._fs[7]);
			break;
		}
		case 0x02: // print f64
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<f64>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:fd %f %f %f %f\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._fd[0], arg1._fd[1], arg1._fd[2], arg1._fd[3]);
			break;
		}
		case 0x03: // print u8
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<u8>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:ub %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x "
				"%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._ub[0], arg1._ub[1], arg1._ub[2], arg1._ub[3],
				arg1._ub[4], arg1._ub[5], arg1._ub[6], arg1._ub[7],
				arg1._ub[8], arg1._ub[9], arg1._ub[10], arg1._ub[11],
				arg1._ub[12], arg1._ub[13], arg1._ub[14], arg1._ub[15],
				arg1._ub[16], arg1._ub[17], arg1._ub[18], arg1._ub[19],
				arg1._ub[20], arg1._ub[21], arg1._ub[22], arg1._ub[23],
				arg1._ub[24], arg1._ub[25], arg1._ub[26], arg1._ub[27],
				arg1._ub[28], arg1._ub[29], arg1._ub[30], arg1._ub[31]);
			break;
		}
		case 0x04: // print s8
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<s8>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:sb %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
				"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._sb[0], arg1._sb[1], arg1._sb[2], arg1._sb[3],
				arg1._sb[4], arg1._sb[5], arg1._sb[6], arg1._sb[7],
				arg1._sb[8], arg1._sb[9], arg1._sb[10], arg1._sb[11],
				arg1._sb[12], arg1._sb[13], arg1._sb[14], arg1._sb[15],
				arg1._sb[16], arg1._sb[17], arg1._sb[18], arg1._sb[19],
				arg1._sb[20], arg1._sb[21], arg1._sb[22], arg1._sb[23],
				arg1._sb[24], arg1._sb[25], arg1._sb[26], arg1._sb[27],
				arg1._sb[28], arg1._sb[29], arg1._sb[30], arg1._sb[31]);
			break;
		}
		case 0x05: // print u16
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<u16>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:uw %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._uw[0], arg1._uw[1], arg1._uw[2], arg1._uw[3],
				arg1._uw[4], arg1._uw[5], arg1._uw[6], arg1._uw[7],
				arg1._uw[8], arg1._uw[9], arg1._uw[10], arg1._uw[11],
				arg1._uw[12], arg1._uw[13], arg1._uw[14], arg1._uw[15]);
			break;
		}
		case 0x06: // print s16
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<s16>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:sw %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._sw[0], arg1._sw[1], arg1._sw[2], arg1._sw[3],
				arg1._sw[4], arg1._sw[5], arg1._sw[6], arg1._sw[7],
				arg1._sw[8], arg1._sw[9], arg1._sw[10], arg1._sw[11],
				arg1._sw[12], arg1._sw[13], arg1._sw[14], arg1._sw[15]);
			break;
		}
		case 0x07: // print u32
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<u32>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:ud %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._ud[0], arg1._ud[1], arg1._ud[2], arg1._ud[3],
				arg1._ud[4], arg1._ud[5], arg1._ud[6], arg1._ud[7]);
			break;
		}
		case 0x08: // print s32
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<s32>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:sd %d %d %d %d %d %d %d %d\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._sd[0], arg1._sd[1], arg1._sd[2], arg1._sd[3],
				arg1._sd[4], arg1._sd[5], arg1._sd[6], arg1._sd[7]);
			break;
		}
		case 0x09: // print u64
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<u64>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:uq %.16llx %.16llx %.16llx %.16llx\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._uq[0], arg1._uq[1], arg1._uq[2], arg1._uq[3]);
			break;
		}
		case 0x0a: // print s64
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<s64>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s]:sq %lld %lld %lld %lld\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._sq[0], arg1._sq[1], arg1._sq[2], arg1._sq[3]);
			break;
		}
		case 0x0b: // print full data
		{
			A256Reg arg1 = reg[op.op1i.r].bsc1<u64>(op.op1i.r_mask, op.op1i.r);
			printf("[$%.2X%s] %.16llx%.16llx%.16llx%.16llx\n",
				op.op1i.r, arg1.bsc1_fmt(op.op1i.r_mask).c_str(),
				arg1._uq[3], arg1._uq[2], arg1._uq[1], arg1._uq[0]);
			break;
		}
		default:
		{
			throw fmt::format(__FUNCTION__"(): invalid code 0x%x.", code);
		}
		}
	}

	void set() // set register to immediate (set r.mask, imm32)
	{
		A256Reg res = A256Reg::set(op.op1i.imm);
		RSAVE1(reg[op.op1i.r], res, op.op1i.r_mask);
	}

	void mmovb() // mov selected bytes using immediate mask (mmovb r, a, imm32)
	{
		for (u32 i = 0; i < 32; i++)
		{
			if (op.op2i.imm & (1 << i))
			{
				reg[op.op2i.r]._ub[i] = reg[op.op2i.a]._ub[i];
			}
		}
	}

	void mswapb() // swap selected bytes using immediate (mswapb r, a, imm32)
	{
		for (u32 i = 0; i < 32; i++)
		{
			if (op.op2i.imm & (1 << i))
			{
				const u8 temp = reg[op.op2i.r]._ub[i];
				reg[op.op2i.r]._ub[i] = reg[op.op2i.a]._ub[i];
				reg[op.op2i.a]._ub[i] = temp;
			}
		}
	}

	void ld() // load from memory (ld r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		u64 addr = arg1._uq[0] + arg2._uq[0];
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(reg[op.op3.r], *data, op.op3.r_mask);
	}

	void st() // store to memory (st r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		u64 addr = arg1._uq[0] + arg2._uq[0];
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(*data, reg[op.op3.r], op.op3.r_mask);
	}

	void ldr() // load relative from memory (ldr r.mask, imm32)
	{
		u64 addr = reg[0]._uq[0] + (s32)op.op1i.imm;
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(reg[op.op3.r], *data, op.op1i.r_mask);
	}

	void str() // store relative to memory (str r.mask, imm32)
	{
		u64 addr = reg[0]._uq[0] + (s32)op.op1i.imm;
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(*data, reg[op.op3.r], op.op1i.r_mask);
	}

	template<typename T>
	void cmov_() // conditional move if not zero (cmov* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result = reg[op.op3.r];
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			if (arg2.get<T>(i))
			{
				result.get<T>(i) = arg1.get<T>(i);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void cmovb()
	{
		cmov_<s8>();
	}

	void cmovw()
	{
		cmov_<s16>();
	}

	void cmovd()
	{
		cmov_<s32>();
	}

	void cmovq()
	{
		cmov_<s64>();
	}

	template<typename T>
	void cmovz_() // conditional move if zero (cmovz* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result = reg[op.op3.r];
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			if (!arg2.get<T>(i))
			{
				result.get<T>(i) = arg1.get<T>(i);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void cmovzb()
	{
		cmovz_<s8>();
	}

	void cmovzw()
	{
		cmovz_<s16>();
	}

	void cmovzd()
	{
		cmovz_<s32>();
	}

	void cmovzq()
	{
		cmovz_<s64>();
	}

	template<typename T>
	void add_() // addition (add* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) = (arg1.get<T>(i)) + (arg2.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void addfs()
	{
		add_<f32>();
	}

	void addfd()
	{
		add_<f64>();
	}

	void addb()
	{
		add_<s8>();
	}

	void addw()
	{
		add_<s16>();
	}

	void addd()
	{
		add_<s32>();
	}

	void addq()
	{
		add_<s64>();
	}

	template<typename T, typename Timm>
	void add_i(const Timm& imm) // add immediate (addfsi r.mask, imm32)
	{
		A256Reg result = reg[op.op1i.r];
		A256Reg arg = A256Reg::set<Timm>(imm);
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) += (arg.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void addfsi() // add single float immediate 
	{
		add_i<f32, f32>((f32&)op.op1i.imm);
	}

	void addfdi() // add double float from single immediate
	{
		add_i<f64, f32>((f32&)op.op1i.imm);
	}

	void addbi() // add packed 4-byte immediate (addbi r.mask, imm8, imm8, imm8, imm8)
	{
		add_i<s8, u32>(op.op1i.imm);
	}

	void addwi() // add packed 2-word immediate (addwi r.mask, imm16, imm16)
	{
		add_i<s16, u32>(op.op1i.imm);
	}

	void adddi() // add dword immediate
	{
		add_i<s32, u32>(op.op1i.imm);
	}

	void addqip() // add lowpart quadword positive immediate (addqip r.mask, imm32p)
	{
		add_i<s64, u64>(op.op1i.imm);
	}

	void addqin() // add lowpart quadword negative immediate (addqin r.mask, imm32n)
	{
		add_i<s64, u64>(~0ull & op.op1i.imm);
	}

	template<typename T>
	void sub_() // subtract (sub* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) = (arg1.get<T>(i)) - (arg2.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void subfs()
	{
		sub_<f32>();
	}

	void subfd()
	{
		sub_<f64>();
	}

	void subb()
	{
		sub_<s8>();
	}

	void subw()
	{
		sub_<s16>();
	}

	void subd()
	{
		sub_<s32>();
	}

	void subq()
	{
		sub_<s64>();
	}

	template<typename T>
	void jnz_() // jump relative if not zero elements (jnz* a.bsc, imm32)
	{
		A256Reg arg1 = reg[op.op1i.r].bsc1<T>(op.op1i.r_mask, op.op1i.r);
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			if (arg1.get<T>(i) != 0)
			{
				reg[0]._uq[0] += (s32)op.op1i.imm;
				break;
			}
		}
	}

	void jnzfs()
	{
		jnz_<f32>();
	}

	void jnzfd()
	{
		jnz_<f64>();
	}

	void jnzb()
	{
		jnz_<s8>();
	}

	void jnzw()
	{
		jnz_<s16>();
	}

	void jnzd()
	{
		jnz_<s32>();
	}

	void jnzq()
	{
		jnz_<s64>();
	}

	template<typename T>
	void mul_() // multiply (sub* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) = (arg1.get<T>(i)) * (arg2.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void mulfs()
	{
		mul_<f32>();
	}

	void mulfd()
	{
		mul_<f64>();
	}

	void mulb()
	{
		mul_<s8>();
	}

	void mulw()
	{
		mul_<s16>();
	}

	void muld()
	{
		mul_<s32>();
	}

	void mulq()
	{
		mul_<s64>();
	}

	template<typename T>
	void jz_() // jump relative if zero elements (jz* a.bsc, imm32)
	{
		A256Reg arg1 = reg[op.op1i.r].bsc1<T>(op.op1i.r_mask, op.op1i.r);
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			if (arg1.get<T>(i) != 0)
			{
				return;
			}
		}
		reg[0]._uq[0] += (s32)op.op1i.imm;
	}

	void jzfs()
	{
		jz_<f32>();
	}

	void jzfd()
	{
		jz_<f64>();
	}

	void jzb()
	{
		jz_<s8>();
	}

	void jzw()
	{
		jz_<s16>();
	}

	void jzd()
	{
		jz_<s32>();
	}

	void jzq()
	{
		jz_<s64>();
	}

	template<typename T>
	void fma_() // multiply and add (fma* r.mask, ss, a.ss, b.ss, c.ss)
	{
		A256Reg arg1 = reg[op.op4.a].bsign1<T>(op.op4.arg_mask >> 2);
		A256Reg arg2 = reg[op.op4.b].bsign1<T>(op.op4.arg_mask >> 4);
		A256Reg arg3 = reg[op.op4.c].bsign1<T>(op.op4.arg_mask >> 6);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) = (arg1.get<T>(i)) * (arg2.get<T>(i)) + (arg3.get<T>(i));
		}
		RSAVE1(reg[op.op4.r], result.bsign1<T>(op.op4.arg_mask), op.op4.r_mask);
	}

	void fmafs()
	{
		fma_<f32>();
	}

	void fmafd()
	{
		fma_<f64>();
	}

	void fmab()
	{
		fma_<s8>();
	}

	void fmaw()
	{
		fma_<s16>();
	}

	void fmad()
	{
		fma_<s32>();
	}

	void fmaq()
	{
		fma_<s64>();
	}

	void call() // jump relative using call stack (r) (call r.mask, imm32)
	{
		for (u32 i = 0; i < 4; i++)
		{
			switch ((op.op1i.r_mask >> (i * 2)) & 3)
			{
			case 0: break;
			case 3:
			{
				reg[op.op1i.r]._uq[i] -= sizeof(u64);
				*(u64*)(reg[op.op1i.r]._uq[i]) = reg[0]._uq[0];
				break;
			}
			default: throw fmt::format(__FUNCTION__"(): partial stack pointer update.");
			}
		}
		reg[0]._uq[0] += (s32)op.op1i.imm;
	}

	void ret() // return using call stack (r) (ret r.mask, imm32)
	{
		if (op.op1i.imm != 0)
		{
			throw fmt::format(__FUNCTION__"(): invalid immediate 0x%x.", op.op1i.imm);
		}
		for (u32 i = 0; i < 4; i++)
		{
			switch ((op.op1i.r_mask >> (i * 2)) & 3)
			{
			case 0: break;
			case 3:
			{
				if (op.op1i.r_mask != (3 << (i * 2)))
				{
					throw fmt::format(__FUNCTION__"(): multiple stack pointer update.");
				}
				reg[0]._uq[0] = *(u64*)(reg[op.op1i.r]._uq[i]);
				reg[op.op1i.r]._uq[i] += sizeof(u64);
				break;
			}
			default: throw fmt::format(__FUNCTION__"(): partial stack pointer update.");
			}
		}
	}

	template<typename T>
	void and_() // bitwise and (and* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result = arg1 & arg2;
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void andfs()
	{
		and_<f32>();
	}

	void andfd()
	{
		and_<f64>();
	}

	void andb()
	{
		and_<s8>();
	}

	void andw()
	{
		and_<s16>();
	}

	void andd()
	{
		and_<s32>();
	}

	void andq()
	{
		and_<s64>();
	}

	template<typename T>
	void push_() // push value (a) onto stack (r) with alignment using bitwise AND with (b) (push* r.mask, a.bsc, b.bsc)
	{
		A256Reg value = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg align = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		for (u32 i = 0; i < 4; i++)
		{
			switch ((op.op3.r_mask >> (i * 2)) & 3)
			{
			case 0: break;
			case 3:
			{
				u64& stack = reg[op.op3.r]._uq[i];
				stack -= sizeof(T);
				stack &= align._uq[0];
				*(T*)(stack) = *(T*)&value;
				break;
			}
			default: throw fmt::format(__FUNCTION__"(): partial stack pointer update.");
			}
		}
	}

	void pushd()
	{
		push_<u32>();
	}

	void pushq()
	{
		push_<u64>();
	}

	void pushdq()
	{
		push_<u128>();
	}

	void pushqq()
	{
		push_<u256>();
	}
	
	template<typename T>
	void or_() // bitwise or (or* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result = arg1 | arg2;
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void orfs()
	{
		or_<f32>();
	}

	void orfd()
	{
		or_<f64>();
	}

	void orb()
	{
		or_<s8>();
	}

	void orw()
	{
		or_<s16>();
	}

	void ord()
	{
		or_<s32>();
	}

	void orq()
	{
		or_<s64>();
	}

	template<typename T>
	void pop_() // pop value to (a) from stack (r) (pop* r.mask, a.mask, b.bsc)
	{
		A256Reg arg2 = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b); // ???
		for (u32 i = 0; i < 4; i++)
		{
			switch ((op.op3.r_mask >> (i * 2)) & 3)
			{
			case 0: break;
			case 3:
			{
				if (op.op3.r_mask != (3 << (i * 2)))
				{
					throw fmt::format(__FUNCTION__"(): multiple stack pointer update.");
				}
				u64& stack = reg[op.op3.r]._uq[i];
				A256Reg res = A256Reg::set(*(T*)(stack));
				stack += sizeof(T);
				RSAVE1(reg[op.op3.a], res, op.op3.a_mask);
				break;
			}
			default: throw fmt::format(__FUNCTION__"(): partial stack pointer update.");
			}
		}
	}

	void popd()
	{
		pop_<u32>();
	}

	void popq()
	{
		pop_<u64>();
	}

	void popdq()
	{
		pop_<u128>();
	}

	void popqq()
	{
		pop_<u256>();
	}

	template<typename T>
	void xor_() // bitwise xor (xor* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result = arg1 ^ arg2;
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void xorfs()
	{
		xor_<f32>();
	}

	void xorfd()
	{
		xor_<f64>();
	}

	void xorb()
	{
		xor_<s8>();
	}

	void xorw()
	{
		xor_<s16>();
	}

	void xord()
	{
		xor_<s32>();
	}

	void xorq()
	{
		xor_<s64>();
	}

	void shufb() // basic shuffle bytes (shufb r.mask, a.bsc, b.bsc)
	{
		// TODO
	}

	void shufbx() // advanced shuffle bytes (shufbx r, arg0, arg1, arg2, arg3, arg4)
	{
		A256Reg mask = reg[op.op6.arg[0]];
		A256Reg data[4];
		for (u32 i = 0; i < 4; i++)
		{
			data[i] = reg[op.op6.arg[4 - i]];
		}
		for (u32 i = 0; i < 32; i++)
		{
			if (mask._ub[i] & 0x80)
			{
				reg[op.op6.r]._ub[i] = 0;
			}
			else
			{
				reg[op.op6.r]._ub[i] = data[mask._ub[i] >> 5]._ub[mask._ub[i] % 32];
			}
		}
	}

	template<typename T, typename Tab>
	void unpk_() // interleave elements in a._dq[0] with b._dq[0] to (r) (unpk* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<Tab>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<Tab>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 16 / sizeof(T); i++)
		{
			result.get<T>(i * 2) = arg1.get<T>(i);
			result.get<T>(i * 2 + 1) = arg2.get<T>(i);
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void unpkfs()
	{
		unpk_<u32, f32>();
	}

	void unpkfd()
	{
		unpk_<u64, f64>();
	}

	void unpkdq()
	{
		unpk_<u128, u64>();
	}

	void unpkb()
	{
		unpk_<u8, s8>();
	}

	void unpkw()
	{
		unpk_<u16, s16>();
	}

	void unpkd()
	{
		unpk_<u32, s32>();
	}

	void unpkq()
	{
		unpk_<u64, s64>();
	}

	template<typename T, typename Tab, u32 high>
	void pack_() // pack low or high parts of elements in (a) to r._dq[0], and (b) to r._dq[1] (pack* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<Tab>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<Tab>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 16 / sizeof(T); i++)
		{
			result.get<T>(i) = arg1.get<T>(i * 2 + high);
			result.get<T>(i + 16 / sizeof(T)) = arg2.get<T>(i * 2 + high);
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void packlfs()
	{
		pack_<u32, f32, 0>();
	}

	void packlfd()
	{
		pack_<u64, f64, 0>();
	}

	void packldq()
	{
		pack_<u128, u64, 0>();
	}

	void packlb()
	{
		pack_<u8, s8, 0>();
	}

	void packlw()
	{
		pack_<u16, s16, 0>();
	}

	void packld()
	{
		pack_<u32, s32, 0>();
	}

	void packlq()
	{
		pack_<u64, s64, 0>();
	}

	void packhfs()
	{
		pack_<u32, f32, 1>();
	}

	void packhfd()
	{
		pack_<u64, f64, 1>();
	}

	void packhdq()
	{
		pack_<u128, u64, 1>();
	}

	void packhb()
	{
		pack_<u8, s8, 1>();
	}

	void packhw()
	{
		pack_<u16, s16, 1>();
	}

	void packhd()
	{
		pack_<u32, s32, 1>();
	}

	void packhq()
	{
		pack_<u64, s64, 1>();
	}

	template<typename T>
	void div_() // divide (div* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<T>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) = (arg1.get<T>(i)) / (arg2.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void divfs()
	{
		div_<f32>();
	}

	void divfd()
	{
		div_<f64>();
	}

	void divsb()
	{
		div_<s8>();
	}

	void divsw()
	{
		div_<s16>();
	}

	void divsd()
	{
		div_<s32>();
	}

	void divsq()
	{
		div_<s64>();
	}

	void divub()
	{
		div_<u8>();
	}

	void divuw()
	{
		div_<u16>();
	}

	void divud()
	{
		div_<u32>();
	}

	void divuq()
	{
		div_<u64>();
	}

	template<typename Ta, typename T>
	void rl_() // rotate left (rl* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg = reg[op.op3.a].bsc1<Ta>(op.op3.a_mask, op.op3.a);
		A256Reg rot = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			const u8 r = (u8)rot.get<T>(i) & (8 * sizeof(T) - 1);
			const T v = arg.get<T>(i);
			result.get<T>(i) = r ? (v << r) | (v << (8 * sizeof(T) - r)) : v;
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void rlfs()
	{
		rl_<f32, u32>();
	}

	void rlfd()
	{
		rl_<f64, u64>();
	}

	void rldq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg rot = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 j = 0; j < 4; j += 2)
		{
			const u8 qrot = (rot._uq[j] >> 6) & 1;
			const u8 brot = rot._uq[j] & 63;
			for (u32 i = 0; i < 2; i++)
			{
				result._uq[i + j] = (arg._uq[i ^ qrot + j] << brot) | (brot ? (arg._uq[i ^ qrot ^ 1 + j] >> (64 - brot)) : 0);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void rlqq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		const u8 rot = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b)._ub[0];
		const u8 qrot = rot >> 6;
		const u8 brot = rot & 63;
		A256Reg result;
		for (u32 i = 0; i < 4; i++)
		{
			result._uq[i] = (arg._uq[(i - qrot) & 3] << brot) | (brot ? (arg._uq[(i - qrot - 1) & 3] >> (64 - brot)) : 0);
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void rlb()
	{
		rl_<s8, u8>();
	}

	void rlw()
	{
		rl_<s16, u16>();
	}

	void rld()
	{
		rl_<s32, u32>();
	}

	void rlq()
	{
		rl_<s64, u64>();
	}

	template<typename Ta, typename T>
	void sll_() // shift left (shl* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg = reg[op.op3.a].bsc1<Ta>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			const T s = shift.get<T>(i);
			const T v = arg.get<T>(i);
			result.get<T>(i) = (s >= 8 * sizeof(T)) ? 0 : v << s;
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sllfs()
	{
		sll_<f32, u32>();
	}

	void sllfd()
	{
		sll_<f64, u64>();
	}

	void slldq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 j = 0; j < 4; j += 2)
		{
			const u64 qshift = shift._uq[j] >> 6;
			const u64 bshift = shift._uq[j] & 63;
			for (u32 i = 0; i < 2; i++)
			{
				if (qshift > i)
				{
					result._uq[i + j] = 0;
				}
				else if (qshift == i)
				{
					result._uq[i + j] = (arg._uq[j] << bshift);
				}
				else
				{
					result._uq[i + j] = (arg._uq[i + j] << bshift) | (bshift ? (arg._uq[i + 1 + j] >> (64 - bshift)) : 0);
				}
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sllqq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		const u64 shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b)._uq[0];
		const u64 qshift = shift >> 6;
		const u64 bshift = shift & 63;
		A256Reg result;
		for (u32 i = 0; i < 4; i++)
		{
			if (qshift > i)
			{
				result._uq[i] = 0;
			}
			else if (qshift == i)
			{
				result._uq[i] = (arg._uq[0] << bshift);
			}
			else
			{
				result._uq[i] = (arg._uq[(i - qshift) & 3] << bshift) | (bshift ? (arg._uq[(i - qshift - 1) & 3] >> (64 - bshift)) : 0);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sllb()
	{
		sll_<s8, u8>();
	}

	void sllw()
	{
		sll_<s16, u16>();
	}

	void slld()
	{
		sll_<s32, u32>();
	}

	void sllq()
	{
		sll_<s64, u64>();
	}

	template<typename Ta, typename T, typename Ts>
	void sar_() // shift right arithmetical (replicating sign bit) (sar* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg = reg[op.op3.a].bsc1<Ta>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			const T s = shift.get<T>(i);
			const Ts v = arg.get<Ts>(i);
			result.get<Ts>(i) = (s >= 8 * sizeof(T)) ? v >> (8 * sizeof(T) - 1) : v >> s;
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sarfs()
	{
		sar_<f32, u32, s32>();
	}

	void sarfd()
	{
		sar_<f64, u64, s64>();
	}

	void sardq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 j = 0; j < 4; j += 2)
		{
			const u64 qshift = shift._uq[j] >> 6;
			const u64 bshift = shift._uq[j] & 63;
			for (u32 i = 0; i < 2; i++)
			{
				if (qshift + i > 1)
				{
					result._uq[i + j] = (arg._sq[1 + j] >> 63);
				}
				else if (qshift + i == 1)
				{
					result._uq[i + j] = (arg._sq[1 + j] >> bshift);
				}
				else
				{
					result._uq[i + j] = (arg._uq[i + j] >> bshift) | (bshift ? (arg._uq[i + 1 + j] << (64 - bshift)) : 0);
				}
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sarqq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		const u64 shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b)._uq[0];
		const u64 qshift = shift >> 6;
		const u64 bshift = shift & 63;
		A256Reg result;
		for (u32 i = 0; i < 4; i++)
		{
			if (qshift + i > 3)
			{
				result._uq[i] = arg._sq[3] >> 63;
			}
			else if (qshift + i == 3)
			{
				result._uq[i] = (arg._sq[3] >> bshift);
			}
			else
			{
				result._uq[i] = (arg._uq[(i + qshift) & 3] >> bshift) | (bshift ? (arg._uq[(i + qshift + 1) & 3] << (64 - bshift)) : 0);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void sarb()
	{
		sar_<s8, u8, s8>();
	}

	void sarw()
	{
		sar_<s16, u16, s16>();
	}

	void sard()
	{
		sar_<s32, u32, s32>();
	}

	void sarq()
	{
		sar_<s64, u64, s64>();
	}

	template<typename Ta, typename T>
	void slr_() // shift right logical (shr* r.mask, a.bsc, b.bsc)
	{
		A256Reg arg = reg[op.op3.a].bsc1<Ta>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<T>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			const T s = shift.get<T>(i);
			const T v = arg.get<T>(i);
			result.get<T>(i) = (s >= 8 * sizeof(T)) ? 0 : v >> s;
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void slrfs()
	{
		slr_<f32, u32>();
	}

	void slrfd()
	{
		slr_<f64, u64>();
	}

	void slrdq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		A256Reg result;
		for (u32 j = 0; j < 4; j += 2)
		{
			const u64 qshift = shift._uq[j] >> 6;
			const u64 bshift = shift._uq[j] & 63;
			for (u32 i = 0; i < 2; i++)
			{
				if (qshift + i > 1)
				{
					result._uq[i + j] = 0;
				}
				else if (qshift + i == 1)
				{
					result._uq[i + j] = (arg._uq[1 + j] >> bshift);
				}
				else
				{
					result._uq[i + j] = (arg._uq[i + j] >> bshift) | (bshift ? (arg._uq[i + 1 + j] << (64 - bshift)) : 0);
				}
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void slrqq()
	{
		A256Reg arg = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		const u64 shift = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b)._uq[0];
		const u64 qshift = shift >> 6;
		const u64 bshift = shift & 63;
		A256Reg result;
		for (u32 i = 0; i < 4; i++)
		{
			if (qshift + i > 3)
			{
				result._uq[i] = 0;
			}
			else if (qshift + i == 3)
			{
				result._uq[i] = (arg._uq[3] >> bshift);
			}
			else
			{
				result._uq[i] = (arg._uq[(i + qshift) & 3] >> bshift) | (bshift ? (arg._uq[(i + qshift + 1) & 3] << (64 - bshift)) : 0);
			}
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void slrb()
	{
		slr_<s8, u8>();
	}

	void slrw()
	{
		slr_<s16, u16>();
	}

	void slrd()
	{
		slr_<s32, u32>();
	}

	void slrq()
	{
		slr_<s64, u64>();
	}

	enum A256InstrType
	{
		itUnknown,
		itEmpty,
		itOp1_m1_imm32,
		itOp1_m1_imm32p,
		itOp1_m1_imm32n,
		itOp1_m1_imm8x4,
		itOp1_m1_imm16x2,
		itOp1_bsc1_imm32,
		itOp2_imm32,
		itOp3_m1_bsc2,
		itOp3_m2_bsc1,
		itOp3_bsc3,
		itOp4_sign4,
		itOp6,
	};

	const struct A256InstrTable
	{
		void (A256Machine::*func[0x10000])();
		char* name[0x10000];
		A256InstrType type[0x10000];
		u32 max_num;

		A256InstrTable()
		{
			for (u32 i = 0; i < 0x10000; i++)
			{
				func[i] = nullptr;
				name[i] = nullptr;
			}

			max_num = 0;

#define REG(code, f, t) func[code] = &A256Machine::##f; name[code] = #f; type[code] = t; max_num = std::max<u32>(code, max_num)

			REG(0x0000, stop, itOp1_bsc1_imm32);
			REG(0x0001, set, itOp1_m1_imm32);
			REG(0x0002, mmovb, itOp2_imm32);
			REG(0x0003, mswapb, itOp2_imm32);

			REG(0x0004, ld, itOp3_m1_bsc2);
			REG(0x0005, st, itOp3_m1_bsc2);
			REG(0x0006, ldr, itOp1_m1_imm32);
			REG(0x0007, str, itOp1_m1_imm32);

			REG(0x0008, cmovb, itOp3_m1_bsc2);
			REG(0x0009, cmovw, itOp3_m1_bsc2);
			REG(0x000a, cmovd, itOp3_m1_bsc2);
			REG(0x000b, cmovq, itOp3_m1_bsc2);

			REG(0x000c, cmovzb, itOp3_m1_bsc2);
			REG(0x000d, cmovzw, itOp3_m1_bsc2);
			REG(0x000e, cmovzd, itOp3_m1_bsc2);
			REG(0x000f, cmovzq, itOp3_m1_bsc2);
			
			REG(0x0010, addfs, itOp3_m1_bsc2);
			REG(0x0011, addfd, itOp3_m1_bsc2);
			// 0x0012
			// 0x0013

			REG(0x0014, addb, itOp3_m1_bsc2);
			REG(0x0015, addw, itOp3_m1_bsc2);
			REG(0x0016, addd, itOp3_m1_bsc2);
			REG(0x0017, addq, itOp3_m1_bsc2);

			REG(0x0018, addfsi, itOp1_m1_imm32);
			REG(0x0019, addfdi, itOp1_m1_imm32);
			REG(0x001a, addbi, itOp1_m1_imm8x4);
			REG(0x001b, addwi, itOp1_m1_imm16x2);
			REG(0x001c, adddi, itOp1_m1_imm32);
			REG(0x001d, addqip, itOp1_m1_imm32p);
			REG(0x001e, addqin, itOp1_m1_imm32n);
			// 0x001f

			REG(0x0020, subfs, itOp3_m1_bsc2);
			REG(0x0021, subfd, itOp3_m1_bsc2);
			// 0x0022
			// 0x0023

			REG(0x0024, subb, itOp3_m1_bsc2);
			REG(0x0025, subw, itOp3_m1_bsc2);
			REG(0x0026, subd, itOp3_m1_bsc2);
			REG(0x0027, subq, itOp3_m1_bsc2);

			REG(0x0028, jnzfs, itOp1_bsc1_imm32);
			REG(0x0029, jnzfd, itOp1_bsc1_imm32);
			// 0x002a
			// 0x002b

			REG(0x002c, jnzb, itOp1_bsc1_imm32);
			REG(0x002d, jnzw, itOp1_bsc1_imm32);
			REG(0x002e, jnzd, itOp1_bsc1_imm32);
			REG(0x002f, jnzq, itOp1_bsc1_imm32);

			REG(0x0030, mulfs, itOp3_m1_bsc2);
			REG(0x0031, mulfd, itOp3_m1_bsc2);
			// 0x0032
			// 0x0033

			REG(0x0034, mulb, itOp3_m1_bsc2);
			REG(0x0035, mulw, itOp3_m1_bsc2);
			REG(0x0036, muld, itOp3_m1_bsc2);
			REG(0x0037, mulq, itOp3_m1_bsc2);

			REG(0x0038, jzfs, itOp1_bsc1_imm32);
			REG(0x0039, jzfd, itOp1_bsc1_imm32);
			// 0x003a
			// 0x003b

			REG(0x003c, jzb, itOp1_bsc1_imm32);
			REG(0x003d, jzw, itOp1_bsc1_imm32);
			REG(0x003e, jzd, itOp1_bsc1_imm32);
			REG(0x003f, jzq, itOp1_bsc1_imm32);

			REG(0x0040, fmafs, itOp4_sign4);
			REG(0x0041, fmafd, itOp4_sign4);
			// 0x0042
			// 0x0043

			REG(0x0044, fmab, itOp4_sign4);
			REG(0x0045, fmaw, itOp4_sign4);
			REG(0x0046, fmad, itOp4_sign4);
			REG(0x0047, fmaq, itOp4_sign4);

			REG(0x0048, call, itOp1_m1_imm32);
			// 0x0049
			// 0x004a
			// 0x004b

			REG(0x004c, ret, itOp1_m1_imm32);
			// 0x004d
			// 0x004e
			// 0x004f

			REG(0x0050, andfs, itOp3_m1_bsc2);
			REG(0x0051, andfd, itOp3_m1_bsc2);
			// 0x0052
			// 0x0053

			REG(0x0054, andb, itOp3_m1_bsc2);
			REG(0x0055, andw, itOp3_m1_bsc2);
			REG(0x0056, andd, itOp3_m1_bsc2);
			REG(0x0057, andq, itOp3_m1_bsc2);

			// 0x0058
			// 0x0059
			REG(0x005a, pushd, itOp3_m1_bsc2);
			REG(0x005b, pushq, itOp3_m1_bsc2);
			REG(0x005c, pushdq, itOp3_m1_bsc2);
			REG(0x005d, pushqq, itOp3_m1_bsc2);
			// 0x005e
			// 0x005f

			REG(0x0060, orfs, itOp3_m1_bsc2);
			REG(0x0061, orfd, itOp3_m1_bsc2);
			// 0x0062
			// 0x0063

			REG(0x0064, orb, itOp3_m1_bsc2);
			REG(0x0065, orw, itOp3_m1_bsc2);
			REG(0x0066, ord, itOp3_m1_bsc2);
			REG(0x0067, orq, itOp3_m1_bsc2);

			// 0x0068
			// 0x0069
			REG(0x006a, popd, itOp3_m2_bsc1);
			REG(0x006b, popq, itOp3_m2_bsc1);
			REG(0x006c, popdq, itOp3_m2_bsc1);
			REG(0x006d, popqq, itOp3_m2_bsc1);
			// 0x006e
			// 0x006f

			REG(0x0070, xorfs, itOp3_m1_bsc2);
			REG(0x0071, xorfd, itOp3_m1_bsc2);
			// 0x0072
			// 0x0073

			REG(0x0074, xorb, itOp3_m1_bsc2);
			REG(0x0075, xorw, itOp3_m1_bsc2);
			REG(0x0076, xord, itOp3_m1_bsc2);
			REG(0x0077, xorq, itOp3_m1_bsc2);

			// 0x0078
			// 0x0079
			// 0x007a
			// 0x007b
			// 0x007c
			// 0x007d
			// 0x007e
			// 0x007f

			REG(0x0080, shufb, itOp3_m1_bsc2);
			REG(0x0081, shufbx, itOp6);
			// 0x0082
			// 0x0083
			// 0x0084
			// 0x0085
			// 0x0086
			// 0x0087
			// 0x0088
			// 0x0089
			// 0x008a
			// 0x008b
			// 0x008c
			// 0x008d
			// 0x008e
			// 0x008f
			 
			REG(0x0090, unpkfs, itOp3_m1_bsc2);
			REG(0x0091, unpkfd, itOp3_m1_bsc2);
			REG(0x0092, unpkdq, itOp3_m1_bsc2);
			// 0x0093
			REG(0x0094, unpkb, itOp3_m1_bsc2);
			REG(0x0095, unpkw, itOp3_m1_bsc2);
			REG(0x0096, unpkd, itOp3_m1_bsc2);
			REG(0x0097, unpkq, itOp3_m1_bsc2);

			// 0x0098
			// 0x0099
			// 0x009a
			// 0x009b
			// 0x009c
			// 0x009d
			// 0x009e
			// 0x009f

			REG(0x00a0, packlfs, itOp3_m1_bsc2);
			REG(0x00a1, packlfd, itOp3_m1_bsc2);
			REG(0x00a2, packldq, itOp3_m1_bsc2);
			// 0x00a3
			REG(0x00a4, packlb, itOp3_m1_bsc2);
			REG(0x00a5, packlw, itOp3_m1_bsc2);
			REG(0x00a6, packld, itOp3_m1_bsc2);
			REG(0x00a7, packlq, itOp3_m1_bsc2);

			REG(0x00a8, packhfs, itOp3_m1_bsc2);
			REG(0x00a9, packhfd, itOp3_m1_bsc2);
			REG(0x00aa, packhdq, itOp3_m1_bsc2);
			// 0x00ab
			REG(0x00ac, packhb, itOp3_m1_bsc2);
			REG(0x00ad, packhw, itOp3_m1_bsc2);
			REG(0x00ae, packhd, itOp3_m1_bsc2);
			REG(0x00af, packhq, itOp3_m1_bsc2);

			REG(0x00b0, divfs, itOp3_m1_bsc2);
			REG(0x00b1, divfd, itOp3_m1_bsc2);
			// 0x00b2
			// 0x00b3

			REG(0x00b4, divsb, itOp3_m1_bsc2);
			REG(0x00b5, divsw, itOp3_m1_bsc2);
			REG(0x00b6, divsd, itOp3_m1_bsc2);
			REG(0x00b7, divsq, itOp3_m1_bsc2);

			// 0x00b8
			// 0x00b9
			// 0x00ba
			// 0x00bb

			REG(0x00bc, divub, itOp3_m1_bsc2);
			REG(0x00bd, divuw, itOp3_m1_bsc2);
			REG(0x00be, divud, itOp3_m1_bsc2);
			REG(0x00bf, divuq, itOp3_m1_bsc2);

			REG(0x00c0, rlfs, itOp3_m1_bsc2);
			REG(0x00c1, rlfd, itOp3_m1_bsc2);
			REG(0x00c2, rldq, itOp3_m1_bsc2);
			REG(0x00c3, rlqq, itOp3_m1_bsc2);

			REG(0x00c4, rlb, itOp3_m1_bsc2);
			REG(0x00c5, rlw, itOp3_m1_bsc2);
			REG(0x00c6, rld, itOp3_m1_bsc2);
			REG(0x00c7, rlq, itOp3_m1_bsc2);

			// 0x00c8
			// 0x00c9
			// 0x00ca
			// 0x00cb
			// 0x00cc
			// 0x00cd
			// 0x00ce
			// 0x00cf

			REG(0x00d0, sllfs, itOp3_m1_bsc2);
			REG(0x00d1, sllfd, itOp3_m1_bsc2);
			REG(0x00d2, slldq, itOp3_m1_bsc2);
			REG(0x00d3, sllqq, itOp3_m1_bsc2);

			REG(0x00d4, sllb, itOp3_m1_bsc2);
			REG(0x00d5, sllw, itOp3_m1_bsc2);
			REG(0x00d6, slld, itOp3_m1_bsc2);
			REG(0x00d7, sllq, itOp3_m1_bsc2);

			// 0x00d8
			// 0x00d9
			// 0x00da
			// 0x00db
			// 0x00dc
			// 0x00dd
			// 0x00de
			// 0x00df

			REG(0x00e0, sarfs, itOp3_m1_bsc2);
			REG(0x00e1, sarfd, itOp3_m1_bsc2);
			REG(0x00e2, sardq, itOp3_m1_bsc2);
			REG(0x00e3, sarqq, itOp3_m1_bsc2);

			REG(0x00e4, sarb, itOp3_m1_bsc2);
			REG(0x00e5, sarw, itOp3_m1_bsc2);
			REG(0x00e6, sard, itOp3_m1_bsc2);
			REG(0x00e7, sarq, itOp3_m1_bsc2);

			// 0x00e8
			// 0x00e9
			// 0x00ea
			// 0x00eb
			// 0x00ec
			// 0x00ed
			// 0x00ee
			// 0x00ef

			REG(0x00f0, slrfs, itOp3_m1_bsc2);
			REG(0x00f1, slrfd, itOp3_m1_bsc2);
			REG(0x00f2, slrdq, itOp3_m1_bsc2);
			REG(0x00f3, slrqq, itOp3_m1_bsc2);

			REG(0x00f4, slrb, itOp3_m1_bsc2);
			REG(0x00f5, slrw, itOp3_m1_bsc2);
			REG(0x00f6, slrd, itOp3_m1_bsc2);
			REG(0x00f7, slrq, itOp3_m1_bsc2);

			// 0x00f8
			// 0x00f9
			// 0x00fa
			// 0x00fb
			// 0x00fc
			// 0x00fd
			// 0x00fe
			// 0x00ff

#undef REG
		}

		const u16 find(void (A256Machine::*f)()) const
		{
			for (u32 i = 0; i <= max_num; i++)
			{
				if (func[i] == f)
				{
					return (u16)i;
				}
			}
			throw fmt::format(__FUNCTION__"(): unregistered instruction.");
		}

	} instr;

	std::vector<A256Cmd> compile(const std::string& text)
	{
		struct A256Label
		{
			size_t lpos;
			size_t text_pos;
			std::string name;
		};

		struct A256Const
		{
			u32 value;
			size_t text_pos;
			std::string name;
		};

		struct A256Reloc
		{
			size_t rpos;
			size_t text_pos;
			std::string target;
		};

		struct A256Compiler
		{
			const std::string& text;
			const size_t len;
			size_t pos;
			std::vector<A256Cmd> output;
			std::vector<A256Label> labels;
			std::vector<A256Const> consts;
			std::vector<A256Reloc> relocs;

			A256Compiler(const std::string& text)
				: text(text)
				, len(text.length())
				, pos(0)
			{
			}

			void read_space()
			{
				while (pos < len)
				{
					if (text[pos] != ' ') break;
					pos++;
				}
			}

			void read_comma()
			{
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file (',' expected).\n");
					throw pos;
				}
				if (text[pos] != ',')
				{
					printf(__FUNCTION__"(): '%c' found (',' expected).\n", text[pos]);
					throw pos;
				}
				pos++;
				read_space();
			}

			u8 read_hex()
			{
				u8 res = 0;
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file.\n");
					throw pos;
				}
				switch (text[pos])
				{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				{
					res = text[pos] - '0';
					break;
				}
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				{
					res = text[pos] - 'A' + 10;
					break;
				}
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				{
					res = text[pos] - 'a' + 10;
					break;
				}
				default:
				{
					printf(__FUNCTION__"(): '%c' found.\n", text[pos]);
					throw pos;
				}
				}
				pos++;
				return res;
			}

			u64 read_num()
			{
				int count = 0;
				u64 res = 0;
				bool hex = false;
				while (pos < len)
				{
					if (hex)
					{
						if (count > 16)
						{
							printf(__FUNCTION__"(): number too big.\n");
							throw pos;
						}
						if ((text[pos] >= '0' && text[pos] <= '9') ||
							(text[pos] >= 'a' && text[pos] <= 'f') ||
							(text[pos] >= 'A' && text[pos] <= 'F'))
						{
							res = (res << 4) | read_hex();
						}
						else
						{
							break;
						}
					}
					else
					{
						if (count > 18)
						{
							printf(__FUNCTION__"(): number too big.\n");
							throw pos;
						}
						if (text[pos] == 'x' && res == 0 && count == 1)
						{
							hex = true;
							pos++;
							count = 0;
							continue;
						}
						else if (text[pos] >= '0' && text[pos] <= '9')
						{
							res = res * 10 + (text[pos] - '0');
							pos++;
						}
						else
						{
							break;
						}
					}
					count++;
				}
				if (count)
				{
					return res;
				}
				else
				{
					printf(__FUNCTION__"(): '%c' found.\n", text[pos]);
					throw pos;
				}
			}

			u8 read_r()
			{
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file ('$' expected).\n");
					throw pos;
				}
				if (text[pos] != '$')
				{
					printf(__FUNCTION__"(): '%c' found ('$' expected).\n", text[pos]);
					throw pos;
				}
				pos++;
				u8 res = read_hex() << 4;
				return res | read_hex();
			}

			u16 read_rmask1()
			{
				if (pos + 2 < len && !strncmp(&text[pos], "$NP", 3))
				{
					pos += 3;
					return 0x0300;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$CS", 3))
				{
					pos += 3;
					return 0x0c00;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$BP", 3))
				{
					pos += 3;
					return 0x3000;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$SP", 3))
				{
					pos += 3;
					return 0xc000;
				}
				if (pos + 2 < len && text[pos] == '$')
				{
					u16 r = read_r();
					if (pos < len && text[pos] == '.')
					{
						pos++;
						if (pos + 1 < len && ((text[pos] == 'u' || text[pos] == 's') && text[pos + 1] == 'd')
							|| (text[pos] == 'f' && text[pos + 1] == 's'))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 7)
							{
								printf(__FUNCTION__"(): selector too big (0..7 expected).\n");
								throw pos;
							}
							return r | ((u16)0x0100 << num);
						}
						else if (pos + 1 < len && ((text[pos] == 'u' || text[pos] == 's') && text[pos + 1] == 'q')
							|| (text[pos] == 'f' && text[pos + 1] == 'd'))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 3)
							{
								printf(__FUNCTION__"(): selector too big (0..3 expected).\n");
								throw pos;
							}
							return r | ((u16)0x0300 << (num * 2));
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "dq", 2))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 1)
							{
								printf(__FUNCTION__"(): selector too big (0..1 expected).\n");
								throw pos;
							}
							return r | ((u16)0x0700 << (num * 4));
						}
						else
						{
							u64 num = read_num();
							if (num > 255)
							{
								printf(__FUNCTION__"(): mask too big (0..255 expected).\n");
								throw pos;
							}
							return r | ((u16)num << 8);
						}
					}
					else
					{
						r |= 0xff00;
						return r;
					}
				}
				else
				{
					printf(__FUNCTION__"(): '%c' found ('$' expected).\n", text[pos]);
					throw pos;
				}
			}

			u16 read_rbsc1()
			{
				if (pos + 2 < len && !strncmp(&text[pos], "$NP", 3))
				{
					pos += 3;
					return 0xc000;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$CS", 3))
				{
					pos += 3;
					return 0xc100;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$BP", 3))
				{
					pos += 3;
					return 0xc200;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$SP", 3))
				{
					pos += 3;
					return 0xc300;
				}
				if (pos + 2 < len && text[pos] == '$')
				{
					u16 r = read_r();
					if (pos < len && text[pos] == '.')
					{
						pos++;
						if (pos + 1 < len && !strncmp(&text[pos], "ub", 2))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 31)
							{
								printf(__FUNCTION__"(): selector too big (0..31 expected).\n");
								throw pos;
							}
							return r | 0x0000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "sb", 2))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 31)
							{
								printf(__FUNCTION__"(): selector too big (0..31 expected).\n");
								throw pos;
							}
							return r | 0x2000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "uw", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x2000;
							}
							u64 num = read_num();
							if (num > 15)
							{
								printf(__FUNCTION__"(): selector too big (0..15 expected).\n");
								throw pos;
							}
							return r | 0x4000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "sw", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x2000;
							}
							u64 num = read_num();
							if (num > 15)
							{
								printf(__FUNCTION__"(): selector too big (0..15 expected).\n");
								throw pos;
							}
							return r | 0x5000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "ud", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x1000;
							}
							u64 num = read_num();
							if (num > 7)
							{
								printf(__FUNCTION__"(): selector too big (0..7 expected).\n");
								throw pos;
							}
							return r | 0x8000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "sd", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x1000;
							}
							u64 num = read_num();
							if (num > 7)
							{
								printf(__FUNCTION__"(): selector too big (0..7 expected).\n");
								throw pos;
							}
							return r | 0x8800 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "fs", 2))
						{
							pos += 2;
							if (pos >= len)
							{
								printf(__FUNCTION__"(): end of file (rounding mode expected).\n");
								throw pos;
							}
							switch (text[pos])
							{
							case 'r': r |= 0x0000; break;
							case 't': r |= 0x0800; break;
							case 'f': r |= 0x1000; break;
							case 'c': r |= 0x1800; break;
							default:
							{
								printf(__FUNCTION__"(): '%c' found (rounding mode expected).\n", text[pos]);
								throw pos;
							}
							}
							pos++;
							u64 num = read_num();
							if (num > 7)
							{
								printf(__FUNCTION__"(): selector too big (0..7 expected).\n");
								throw pos;
							}
							return r | 0xa000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "uq", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x0800;
							}
							u64 num = read_num();
							if (num > 3)
							{
								printf(__FUNCTION__"(): selector too big (0..3 expected).\n");
								throw pos;
							}
							return r | 0xc000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "sq", 2))
						{
							pos += 2;
							if (pos < len && text[pos] == 's')
							{
								pos++;
								r |= 0x0800;
							}
							u64 num = read_num();
							if (num > 3)
							{
								printf(__FUNCTION__"(): selector too big (0..3 expected).\n");
								throw pos;
							}
							return r | 0xc400 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "fd", 2))
						{
							pos += 2;
							if (pos >= len)
							{
								printf(__FUNCTION__"(): end of file (rounding mode expected).\n");
								throw pos;
							}
							switch (text[pos])
							{
							case 'r': r |= 0x0000; break;
							case 't': r |= 0x0400; break;
							case 'f': r |= 0x0800; break;
							case 'c': r |= 0x0c00; break;
							default:
							{
								printf(__FUNCTION__"(): '%c' found (rounding mode expected).\n", text[pos]);
								throw pos;
							}
							}
							pos++;
							u64 num = read_num();
							if (num > 3)
							{
								printf(__FUNCTION__"(): selector too big (0..3 expected).\n");
								throw pos;
							}
							return r | 0xd000 | ((u16)num << 8);
						}
						else if (pos + 1 < len && !strncmp(&text[pos], "dq", 2))
						{
							pos += 2;
							u64 num = read_num();
							if (num > 1)
							{
								printf(__FUNCTION__"(): selector too big (0..1 expected).\n");
								throw pos;
							}
							return r | 0xe000 | ((u16)num << 8);
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxbw", 4))
						{
							pos += 4;
							return r | 0xe200;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxbw", 4))
						{
							pos += 4;
							return r | 0xe300;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxbd", 4))
						{
							pos += 4;
							return r | 0xe400;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxbd", 4))
						{
							pos += 4;
							return r | 0xe500;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxbq", 4))
						{
							pos += 4;
							return r | 0xe600;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxbq", 4))
						{
							pos += 4;
							return r | 0xe700;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxwd", 4))
						{
							pos += 4;
							return r | 0xe800;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxwd", 4))
						{
							pos += 4;
							return r | 0xe900;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxwq", 4))
						{
							pos += 4;
							return r | 0xea00;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxwq", 4))
						{
							pos += 4;
							return r | 0xeb00;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "zxdq", 4))
						{
							pos += 4;
							return r | 0xec00;
						}
						else if (pos + 3 < len && !strncmp(&text[pos], "sxdq", 4))
						{
							pos += 4;
							return r | 0xed00;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "zxqdq", 5))
						{
							pos += 5;
							return r | 0xee00;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "sxqdq", 5))
						{
							pos += 5;
							return r | 0xef00;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getfs", 5))
						{
							pos += 5;
							return r | 0xf000;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getfd", 5))
						{
							pos += 5;
							return r | 0xf100;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getub", 5))
						{
							pos += 5;
							return r | 0xf200;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getsb", 5))
						{
							pos += 5;
							return r | 0xf300;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getuw", 5))
						{
							pos += 5;
							return r | 0xf400;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getsw", 5))
						{
							pos += 5;
							return r | 0xf500;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getud", 5))
						{
							pos += 5;
							return r | 0xf600;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getsd", 5))
						{
							pos += 5;
							return r | 0xf700;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getuq", 5))
						{
							pos += 5;
							return r | 0xf800;
						}
						else if (pos + 4 < len && !strncmp(&text[pos], "getsq", 5))
						{
							pos += 5;
							return r | 0xf900;
						}
						else if (pos + 2 < len && !strncmp(&text[pos], "not", 3))
						{
							pos += 3;
							return r | 0xfe00;
						}
						else
						{
							printf(__FUNCTION__"(): unknown bsc1.\n");
							throw pos;
						}
					}
					else
					{
						r |= 0xff00;
						return r;
					}
				}
				else
				{
					if (pos >= len)
					{
						printf(__FUNCTION__"(): end of file (bsc1 expected).\n");
						throw pos;
					}
					if (text[pos] == '-')
					{
						pos++;
						u64 num = read_num();
						if (num == 0 || num > 512)
						{
							printf(__FUNCTION__"(): invalid negative immediate (-1..-512 expected).\n");
							throw pos;
						}
						if (num < 257)
						{
							return 0xfb00 | (u8)(0 - num);
						}
						else
						{
							return 0xfd00 | (u8)(0 - num);
						}
					}
					else
					{
						u64 num = read_num();
						if (num > 511)
						{
							printf(__FUNCTION__"(): immediate too big (0..511 expected).\n");
							throw pos;
						}
						if (num < 256)
						{
							return 0xfa00 | (u8)num;
						}
						else
						{
							return 0xfc00 | (u8)num;
						}
					}
				}
			}

			u8 read_sign1()
			{
				u8 res = 0;
				if (pos < len && text[pos] == '-')
				{
					res |= 1;
					pos++;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "abs", 3))
				{
					res |= 2;
					pos += 3;
				}
				return res;
			}

			u32 read_imm32(bool allow_reloc = true)
			{
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file.\n");
					throw pos;
				}
				if (text[pos] == '@' || text[pos] == '#')
				{
					if (!allow_reloc)
					{
						printf(__FUNCTION__"(): '%c' found (consts and labels not allowed).\n", text[pos]);
						throw pos;
					}
					size_t start = pos;
					pos++;
					while (pos < len)
					{
						if (text[pos] == '\n' || text[pos] == '\r' || text[pos] == ',' || text[pos] == ';' || text[pos] == ' ') break;
						pos++;
					}
					A256Reloc r1;
					r1.rpos = output.size();
					r1.target = std::string(&text[start], pos - start);
					r1.text_pos = start;
					relocs.push_back(r1);
					return 0;
				}
				else if (text[pos] == '-')
				{
					pos++;
					u64 num = read_num();
					if (num == 0 || num > 0x100000000ull)
					{
						printf(__FUNCTION__"(): invalid negative immediate (-1..-0x100000000 expected).\n");
						throw pos;
					}
					return 0 - (u32)num;
				}
				else
				{
					u64 num = read_num();
					if (num > 0xffffffff)
					{
						printf(__FUNCTION__"(): immediate too big (0..0xffffffff expected).\n");
						throw pos;
					}
					return (u32)num;
				}
			}

			u8 read_imm8()
			{
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file.\n");
					throw pos;
				}
				if (text[pos] == '-')
				{
					pos++;
					u64 num = read_num();
					if (num == 0 || num > 0x100)
					{
						printf(__FUNCTION__"(): invalid negative immediate (-1..-256 expected).\n");
						throw pos;
					}
					return 0 - (u8)num;
				}
				else
				{
					u64 num = read_num();
					if (num > 0xff)
					{
						printf(__FUNCTION__"(): immediate too big (0..255 expected).\n");
						throw pos;
					}
					return (u8)num;
				}
			}

			u16 read_imm16()
			{
				if (pos >= len)
				{
					printf(__FUNCTION__"(): end of file.\n");
					throw pos;
				}
				if (text[pos] == '-')
				{
					pos++;
					u64 num = read_num();
					if (num == 0 || num > 0x10000)
					{
						printf(__FUNCTION__"(): invalid negative immediate (-1..-0x10000 expected).\n");
						throw pos;
					}
					return 0 - (u16)num;
				}
				else
				{
					u64 num = read_num();
					if (num > 0xffff)
					{
						printf(__FUNCTION__"(): immediate too big (0..0xffff expected).\n");
						throw pos;
					}
					return (u16)num;
				}
			}
		} compiler(text);

		size_t& pos = compiler.pos;
		const size_t& len = compiler.len;
		std::vector<A256Cmd>& output = compiler.output;
		std::vector<A256Label>& labels = compiler.labels;
		std::vector<A256Reloc>& relocs = compiler.relocs;
		std::vector<A256Const>& consts = compiler.consts;

		while (pos < len)
		{
			// filter comments or end-of-line
			switch (text[pos])
			{
			case ';':
			{
				while (text[pos] != '\n' && text[pos] != '\r')
				{
					pos++;
					if (pos >= len) break;
				}
			}
			case '\n':
			case '\r':
			case ' ':
			{
				pos++;
				continue;
			}
			case '@':
			{
				size_t start = pos;
				pos++;
				while (text[pos] != ':')
				{
					pos++;
					if (pos >= len)
					{
						printf(__FUNCTION__"(): end of file (':' expected).\n");
						throw pos;
					}
					if (text[pos] == '\n' || text[pos] == '\r' || text[pos] == ' ')
					{
						printf(__FUNCTION__"(): end of identifier (':' expected).\n");
						throw start;
					}
				}
				A256Label l1;
				l1.lpos = output.size();
				l1.name = std::string(&text[start], pos - start);
				l1.text_pos = start;
				// TODO: check existing labels
				labels.push_back(l1);
				pos++;
				continue;
			}
			case '#':
			{
				size_t start = pos;
				pos++;
				while (text[pos] != ' ')
				{
					pos++;
					if (pos >= len)
					{
						printf(__FUNCTION__"(): end of file (' ' and number expected).\n");
						throw start;
					}
					if (text[pos] == '\n' || text[pos] == '\r')
					{
						printf(__FUNCTION__"(): end of line (' ' and number expected).\n");
						throw start;
					}	
				}
				A256Const c1;
				c1.name = std::string(&text[start], pos - start);
				compiler.read_space();
				c1.value = compiler.read_imm32(false);
				c1.text_pos = start;
				// TODO: check existing consts
				consts.push_back(c1);
				continue;
			}
			case 'j':
			{
				if ((pos + 1 == len) || text[pos + 1] != ' ') break;
				pos++;
				// generate jump to label
				compiler.read_space();
				A256Cmd jcmd;
				jcmd.cmd = instr.find(&A256Machine::jnzq);
				jcmd.op1i.r = 0; // $00
				jcmd.op1i.r_mask = 0xff;
				jcmd.op1i.imm = compiler.read_imm32();
				output.push_back(jcmd);
				continue;
			}
			case 'c':
			{
				if ((pos + 1 == len) || text[pos + 1] != ' ') break;
				pos++;
				compiler.read_space();
				// generate call to label
				A256Cmd cmd;
				cmd.cmd = instr.find(&A256Machine::call);
				cmd.op1i.r = 0; // $00
				cmd.op1i.r_mask = 0x0c; // $CS
				cmd.op1i.imm = compiler.read_imm32();
				output.push_back(cmd);
				continue;
			}
			case 'r':
			{
				if ((pos + 1 == len) || text[pos + 1] != ' ') break;
				pos++;
				compiler.read_space();
				// generate return
				A256Cmd cmd;
				cmd.cmd = instr.find(&A256Machine::ret);
				cmd.op1i.r = 0; // $00
				cmd.op1i.r_mask = 0x0c; // $CS
				cmd.op1i.imm = compiler.read_imm32();
				output.push_back(cmd);
				continue;
			}
			case 's':
			{
				if ((pos + 1 == len) || text[pos + 1] != ' ') break;
				pos++;
				compiler.read_space();
				// generate stop for register $00
				A256Cmd cmd;
				cmd.cmd = instr.find(&A256Machine::stop);
				cmd.op1i.r = 0; // $00
				cmd.op1i.r_mask = 0xff;
				cmd.op1i.imm = compiler.read_imm32();
				output.push_back(cmd);
				continue;
			}
			default: break;
			}

			// find opcode
			u32 opcode;
			for (opcode = 0; opcode <= instr.max_num; opcode++)
			{
				if (!instr.name[opcode]) continue;
				const size_t op_len = strlen(instr.name[opcode]);
				if (!strncmp(&text[pos], instr.name[opcode], op_len))
				{
					if (pos + op_len < len && text[pos + op_len] >= 'a' && text[pos + op_len] <= 'z') continue;
					pos += op_len;
					break;
				}
			}
			if (opcode > instr.max_num)
			{
				printf(__FUNCTION__"(): unknown instruction found.\n");
				throw pos;
			}

			compiler.read_space();

			// decode
			A256Cmd cmd = { (u16)opcode, 0, 0, 0, 0, 0, 0 };

			switch (instr.type[opcode & 0xffff])
			{
			case itEmpty:
			{
				break;
			}
			case itOp1_m1_imm32:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.op1i.imm = compiler.read_imm32();
				break;
			}
			case itOp1_m1_imm32p:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				if (pos < len && text[pos] == '-')
				{
					printf(__FUNCTION__"(): '-' found (positive number expected).\n");
					throw pos;
				}
				cmd.op1i.imm = compiler.read_imm32();
				break;
			}
			case itOp1_m1_imm32n:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				if (pos < len && text[pos] != '-')
				{
					printf(__FUNCTION__"(): '-' not found (negative number expected).\n");
					throw pos;
				}
				cmd.op1i.imm = compiler.read_imm32();
				break;
			}
			case itOp1_m1_imm8x4:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.raw[5] = compiler.read_imm8();
				compiler.read_comma();
				cmd.raw[4] = compiler.read_imm8();
				compiler.read_comma();
				cmd.raw[3] = compiler.read_imm8();
				compiler.read_comma();
				cmd.raw[2] = compiler.read_imm8();
				break;
			}
			case itOp1_m1_imm16x2:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.raww[2] = compiler.read_imm16();
				compiler.read_comma();
				cmd.raww[1] = compiler.read_imm16();
				break;
			}
			case itOp1_bsc1_imm32:
			{
				cmd.raww[0] = compiler.read_rbsc1();
				compiler.read_comma();
				cmd.op1i.imm = compiler.read_imm32();
				break;
			}
			case itOp2_imm32:
			{
				cmd.op2i.r = compiler.read_r();
				compiler.read_comma();
				cmd.op2i.a = compiler.read_r();
				compiler.read_comma();
				cmd.op1i.imm = compiler.read_imm32();
				break;
			}
			case itOp3_m1_bsc2:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.raww[1] = compiler.read_rbsc1();
				compiler.read_comma();
				cmd.raww[2] = compiler.read_rbsc1();
				break;
			}
			case itOp3_m2_bsc1:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.raww[1] = compiler.read_rmask1();
				compiler.read_comma();
				cmd.raww[2] = compiler.read_rbsc1();
				break;
			}
			case itOp3_bsc3:
			{
				cmd.raww[0] = compiler.read_rbsc1();
				compiler.read_comma();
				cmd.raww[1] = compiler.read_rbsc1();
				compiler.read_comma();
				cmd.raww[2] = compiler.read_rbsc1();
				break;
			}
			case itOp4_sign4:
			{
				cmd.raww[0] = compiler.read_rmask1();
				compiler.read_comma();
				u8 sign;
				if (sign = compiler.read_sign1())
				{
					compiler.read_comma();
				}
				cmd.op4.a = compiler.read_r();
				if (pos < len && text[pos] == '.')
				{
					pos++;
					sign |= compiler.read_sign1() << 2;
				}
				compiler.read_comma();
				cmd.op4.b = compiler.read_r();
				if (pos < len && text[pos] == '.')
				{
					pos++;
					sign |= compiler.read_sign1() << 4;
				}
				compiler.read_comma();
				cmd.op4.c = compiler.read_r();
				if (pos < len && text[pos] == '.')
				{
					pos++;
					sign |= compiler.read_sign1() << 6;
				}
				cmd.op4.arg_mask = sign;
				break;
			}
			case itOp6:
			{
				cmd.op6.r = compiler.read_r();
				compiler.read_comma();
				cmd.op6.arg[0] = compiler.read_r();
				compiler.read_comma();
				cmd.op6.arg[1] = compiler.read_r();
				compiler.read_comma();
				cmd.op6.arg[2] = compiler.read_r();
				compiler.read_comma();
				cmd.op6.arg[3] = compiler.read_r();
				compiler.read_comma();
				cmd.op6.arg[4] = compiler.read_r();
				break;
			}
			default:
			{
				printf("Unknown type of '%s'.\n", instr.name[opcode]);
				throw pos;
			}
			}

			output.push_back(cmd);
		}
		output.push_back(A256Cmd({ instr.find(&A256Machine::stop), 0, 0, 0xef, 0xbe, 0xad, 0xde }));

		// relocations:
		for (auto& r : relocs)
		{
			if (r.target[0] == '@')
			{
				A256Label* found = nullptr;
				for (auto& l : labels)
				{
					if (l.name == r.target)
					{
						found = &l;
						break;
					}
				}
				if (found)
				{
					output[r.rpos].op1i.imm = (u32)((found->lpos - r.rpos - 1) * sizeof(A256Cmd));
				}
				else
				{
					printf(__FUNCTION__"(): label '%s' not found.\n", r.target.c_str());
					throw r.text_pos;
				}
			}
			else if (r.target[0] == '#')
			{
				A256Const* found = nullptr;
				for (auto& c : consts)
				{
					if (c.name == r.target)
					{
						found = &c;
						break;
					}
				}
				if (found)
				{
					output[r.rpos].op1i.imm = found->value;
				}
				else
				{
					printf(__FUNCTION__"(): const '%s' not found.\n", r.target.c_str());
					throw r.text_pos;
				}
			}
			else
			{
				printf("Unknown A256Reloc::target: '%s'\n", r.target.c_str());
				throw r.text_pos;
			}
		}
		return output;
	}

	bool execute()
	{
		op = *(A256Cmd*)reg[0]._uq[0];
		reg[0]._uq[0] += sizeof(A256Cmd);
		const u32 cmd = op.cmd;
		(this->*instr.func[cmd])();
		return (u64&)op != 0;
	}
};

#undef op