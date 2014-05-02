#pragma once

#include "A256Reg.h"

#define RSAVE1(dst, src, mask) for (u32 i = 0; i < 8; i++) if ((mask) & (1 << i)) (dst)._ud[i] = (src)._ud[i];

struct A256Machine
{
	/*
	Register $00 is special.
	1) $00.uq0 is pointer to next instruction, mnemonic: $NP
	2) $00.uq1 reserved
	3) $00.uq2 is stack pointer, mnemonic: $SP
	4) $00.uq3 is callstack pointer, mnemonic: $CS

	Registers $01 .. $9F are volatile, first ones are function arguments and results.
	$01 - first argument and function result
	$02 - second argument
	$03 - third argument, etc.
	For example, 64 bit arguments:
	$01.uq0 - first argument and function result
	$02.uq0 - second argument
	$03.uq0 - thirt argument, etc.

	Registers $A0 .. $FF should be preserved by subroutine.
	*/

	A256Reg reg[256]; // registers $00 .. $FF
	A256Cmd op; // current operation (copied from memory)

	A256Machine()
	{
		memset(&reg, 0, sizeof(reg));
	}

	void stop() // 0x0000: interrupt (stop r.bsc, imm32)
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

	void set() // 0x0001: set register to immediate (set r.mask, imm32)
	{
		A256Reg res = A256Reg::set(op.op1i.imm);
		RSAVE1(reg[op.op1i.r], res, op.op1i.r_mask);
	}

	void mmovb() // 0x0002: mov selected bytes using immediate mask (mmovb r, a, imm32)
	{
		for (u32 i = 0; i < 32; i++)
		{
			if (op.op2i.imm & (1 << i))
			{
				reg[op.op2i.r]._ub[i] = reg[op.op2i.a]._ub[i];
			}
		}
	}

	void mswapb() // 0x0003: swap selected bytes using immediate (mswapb r, a, imm32)
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

	void ld() // 0x0004: load from memory (ld r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		u64 addr = arg1._uq[0] + arg2._uq[0];
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(reg[op.op3.r], *data, op.op3.r_mask);
	}

	void st() // 0x0005: store to memory (st r.mask, a.bsc, b.bsc)
	{
		A256Reg arg1 = reg[op.op3.a].bsc1<u64>(op.op3.a_mask, op.op3.a);
		A256Reg arg2 = reg[op.op3.b].bsc1<u64>(op.op3.b_mask, op.op3.b);
		u64 addr = arg1._uq[0] + arg2._uq[0];
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(*data, reg[op.op3.r], op.op3.r_mask);
	}

	void ldr() // 0x0006: load relative from memory (ldr r.mask, imm32)
	{
		u64 addr = reg[0]._uq[0] + (s32)op.op1i.imm;
		A256Reg* data = (A256Reg*)addr;
		RSAVE1(reg[op.op3.r], *data, op.op1i.r_mask);
	}

	void str() // 0x0007: store relative to memory (str r.mask, imm32)
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

	void cmovb() // 0x0008: conditional move byte if not zero
	{
		cmov_<s8>();
	}

	void cmovw() // 0x0009: conditional move word if not zero
	{
		cmov_<s16>();
	}

	void cmovd() // 0x000a: conditional move dword if not zero
	{
		cmov_<s32>();
	}

	void cmovq() // 0x000b: conditional move qword if not zero
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

	void cmovzb() // 0x000c: conditional move byte if zero
	{
		cmovz_<s8>();
	}

	void cmovzw() // 0x000d: conditional move word if zero
	{
		cmovz_<s16>();
	}

	void cmovzd() // 0x000e: conditional move dword if zero
	{
		cmovz_<s32>();
	}

	void cmovzq() // 0x000f: conditional move qword if zero 
	{
		cmovz_<s64>();
	}

	template<typename T>
	void add_() // basic add (add* r.mask, a.bsc, b.bsc)
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

	void addfs() // 0x0010: add float
	{
		add_<f32>();
	}

	void addfd() // 0x0011: add double
	{
		add_<f64>();
	}

	void addb() // 0x0014: add byte
	{
		add_<s8>();
	}

	void addw() // 0x0015: add word
	{
		add_<s16>();
	}

	void addd() // 0x0016: add dword
	{
		add_<s32>();
	}

	void addq() // 0x0017: add qword
	{
		add_<s64>();
	}

	template<typename T, typename Timm>
	void add_i(const Timm& imm) // basic add immediate (addfsi r.mask, imm32)
	{
		A256Reg result = reg[op.op1i.r];
		A256Reg arg = A256Reg::set<Timm>(imm);
		for (u32 i = 0; i < 32 / sizeof(T); i++)
		{
			result.get<T>(i) += (arg.get<T>(i));
		}
		RSAVE1(reg[op.op3.r], result, op.op3.r_mask);
	}

	void addfsi() // 0x0018: add single float immediate 
	{
		add_i<f32, f32>((f32&)op.op1i.imm);
	}

	void addfdi() // 0x0019: add double float from single immediate
	{
		add_i<f64, f32>((f32&)op.op1i.imm);
	}

	void addbi() // 0x001a: add packed 4-byte immediate (addbi r.mask, imm8, imm8, imm8, imm8)
	{
		add_i<s8, u32>(op.op1i.imm);
	}

	void addwi() // 0x001b: add packed 2-word immediate (addwi r.mask, imm16, imm16)
	{
		add_i<s16, u32>(op.op1i.imm);
	}

	void adddi() // 0x001c: add dword immediate
	{
		add_i<s32, u32>(op.op1i.imm);
	}

	void addqip() // 0x001d: add lowpart quadword positive immediate (addqip r.mask, imm32p)
	{
		add_i<s64, u64>(op.op1i.imm);
	}

	void addqin() // 0x001e: add lowpart quadword negative immediate (addqin r.mask, imm32n)
	{
		add_i<s64, u64>(~0ull & op.op1i.imm);
	}

	template<typename T>
	void sub_() // basic sub (sub* r.mask, a.bsc, b.bsc)
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

	void subfs() // 0x0020: sub float
	{
		sub_<f32>();
	}

	void subfd() // 0x0021: sub double
	{
		sub_<f64>();
	}

	void subb() // 0x0024: sub byte
	{
		sub_<s8>();
	}

	void subw() // 0x0025: sub word
	{
		sub_<s16>();
	}

	void subd() // 0x0026: sub dword
	{
		sub_<s32>();
	}

	void subq() // 0x0027: sub qword
	{
		sub_<s64>();
	}

	template<typename T>
	void jnz_() // jump relative if not zero (jnz* a.bsc, imm32)
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

	void jnzfs() // 0x0028: jump relative if not zero single floats
	{
		jnz_<f32>();
	}

	void jnzfd() // 0x0029: jump relative if not zero double floats
	{
		jnz_<f64>();
	}

	void jnzb() // 0x002c: jump relative if not zero bytes
	{
		jnz_<s8>();
	}

	void jnzw() // 0x002d: jump relative if not zero words
	{
		jnz_<s16>();
	}

	void jnzd() // 0x002e: jump relative if not zero dwords
	{
		jnz_<s32>();
	}

	void jnzq() // 0x002f: jump relative if not zero qwords
	{
		jnz_<s64>();
	}

	template<typename T>
	void mul_() // basic multiply (sub* r.mask, a.bsc, b.bsc)
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

	void mulfs() // 0x0030: mul float
	{
		mul_<f32>();
	}

	void mulfd() // 0x0031: mul double (mulfd r.mask, a.bsc, b.bsc)
	{
		mul_<f64>();
	}

	void mulb() // 0x0034: mul byte (mulb r.mask, a.bsc, b.bsc)
	{
		mul_<s8>();
	}

	void mulw() // 0x0035: mul word (mulw r.mask, a.bsc, b.bsc)
	{
		mul_<s16>();
	}

	void muld() // 0x0036: mul dword (muld r.mask, a.bsc, b.bsc)
	{
		mul_<s32>();
	}

	void mulq() // 0x0037: mul qword (mulq r.mask, a.bsc, b.bsc)
	{
		mul_<s64>();
	}

	template<typename T>
	void jz_() // jump relative if zero (jz* a.bsc, imm32)
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

	void jzfs() // 0x0038: jump relative if zero single floats
	{
		jz_<f32>();
	}

	void jzfd() // 0x0039: jump relative if zero double floats
	{
		jz_<f64>();
	}

	void jzb() // 0x003c: jump relative if zero bytes
	{
		jz_<s8>();
	}

	void jzw() // 0x003d: jump relative if zero words
	{
		jz_<s16>();
	}

	void jzd() // 0x003e: jump relative if zero dwords
	{
		jz_<s32>();
	}

	void jzq() // 0x003f: jump relative if zero qwords
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

	void fmafs() // 0x0040: multiply and add single float
	{
		fma_<f32>();
	}

	void fmafd() // 0x0041: multiply and add double float
	{
		fma_<f64>();
	}

	void fmab() // 0x0044: multiply and add byte
	{
		fma_<s8>();
	}

	void fmaw() // 0x0045: multiply and add word
	{
		fma_<s16>();
	}

	void fmad() // 0x0046: multiply and add dword
	{
		fma_<s32>();
	}

	void fmaq() // 0x0047: multiply and add qword
	{
		fma_<s64>();
	}

	void call() // 0x0048: jump relative using call stack (call r.mask, imm32)
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

	void ret() // 0x004f: return using call stack (ret r.mask, imm32)
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
					throw fmt::format(__FUNCTION__"(): multiple stack pointer updates.");
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

	void andfs() // 0x0050: bitwise and single float
	{
		and_<f32>();
	}

	void andfd() // 0x0051: bitwise and double float
	{
		and_<f64>();
	}

	void andb() // 0x0054: bitwise and byte
	{
		and_<s8>();
	}

	void andw() // 0x0055: bitwise and word
	{
		and_<s16>();
	}

	void andd() // 0x0056: bitwise and dword
	{
		and_<s32>();
	}

	void andq() // 0x0057: bitwise and qword
	{
		and_<s64>();
	}

	template<typename T>
	void push_() // push value onto stack (push* v.bsc, a.bsc, b.bsc)
	{

	}

	void pushd() // 0x005a: push dword
	{
		push_<u32>();
	}

	void pushq() // 0x005b: push qword
	{
		push_<u64>();
	}

	void pushdq() // 0x005c: push double qword
	{
		push_<u128>();
	}

	void push() // 0x005d: push the whole register
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

	void orfs() // 0x0060: bitwise or single float
	{
		or_<f32>();
	}

	void orfd() // 0x0061: bitwise or double float
	{
		or_<f64>();
	}

	void orb() // 0x0064: bitwise or byte
	{
		or_<s8>();
	}

	void orw() // 0x0065: bitwise or word
	{
		or_<s16>();
	}

	void ord() // 0x0066: bitwise or dword
	{
		or_<s32>();
	}

	void orq() // 0x0067: bitwise or qword
	{
		or_<s64>();
	}

	template<typename T>
	void pop_() // pop value from stack (pop* r.mask, a.bsc, b.bsc)
	{

	}

	void popd() // 0x006a: pop dword
	{
		pop_<u32>();
	}

	void popq() // 0x006b: pop qword
	{
		pop_<u64>();
	}

	void popdq() // 0x006c: pop double qword
	{
		pop_<u128>();
	}

	void pop() // 0x006d: pop the whole register
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

	void xorfs() // 0x0070: bitwise xor single float
	{
		xor_<f32>();
	}

	void xorfd() // 0x0071: bitwise xor double float
	{
		xor_<f64>();
	}

	void xorb() // 0x0074: bitwise xor byte
	{
		xor_<s8>();
	}

	void xorw() // 0x0075: bitwise xor word
	{
		xor_<s16>();
	}

	void xord() // 0x0076: bitwise xor dword
	{
		xor_<s32>();
	}

	void xorq() // 0x0077: bitwise xor qword
	{
		xor_<s64>();
	}

	void shufb() // 0x0080: basic shuffle bytes (shufb r.mask, a.bsc, b.bsc)
	{
		// TODO
	}

	void shufbx() // 0x0081: advanced shuffle bytes (shufbx r, arg0, arg1, arg2, arg3, arg4)
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

	template<typename Tfrom, typename Tto>
	void unpk_()
	{

	}

	// 0x0090: unpk*

	template<typename Tfrom, typename Tto>
	void pack_()
	{

	}

	// 0x00a0: pack*

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

	void divfs() // 0x00b0: divide single float
	{
		div_<f32>();
	}

	void divfd() // 0x00b1: divide double float
	{
		div_<f64>();
	}

	void divsb() // 0x00b4: divide signed byte
	{
		div_<s8>();
	}

	void divsw() // 0x00b5: divide signed word
	{
		div_<s16>();
	}

	void divsd() // 0x00b6: divide signed dword
	{
		div_<s32>();
	}

	void divsq() // 0x00b7: divide signed qword
	{
		div_<s64>();
	}

	void divub() // 0x00bc: divide unsigned byte
	{
		div_<u8>();
	}

	void divuw() // 0x00bd: divide unsigned word
	{
		div_<u16>();
	}

	void divud() // 0x00be: divide unsigned dword
	{
		div_<u32>();
	}

	void divuq() // 0x00bf: divide unsigned qword
	{
		div_<u64>();
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
			REG(0x000e, ldr, itOp1_m1_imm32);
			REG(0x000f, str, itOp1_m1_imm32);
			REG(0x0006, cmovb, itOp3_m1_bsc2);
			REG(0x0007, cmovw, itOp3_m1_bsc2);
			REG(0x0008, cmovd, itOp3_m1_bsc2);
			REG(0x0009, cmovq, itOp3_m1_bsc2);
			REG(0x000a, cmovzb, itOp3_m1_bsc2);
			REG(0x000b, cmovzw, itOp3_m1_bsc2);
			REG(0x000c, cmovzd, itOp3_m1_bsc2);
			REG(0x000d, cmovzq, itOp3_m1_bsc2);
			
			REG(0x0010, addfs, itOp3_m1_bsc2);
			REG(0x0011, addfd, itOp3_m1_bsc2);

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

			REG(0x0020, subfs, itOp3_m1_bsc2);
			REG(0x0021, subfd, itOp3_m1_bsc2);

			REG(0x0024, subb, itOp3_m1_bsc2);
			REG(0x0025, subw, itOp3_m1_bsc2);
			REG(0x0026, subd, itOp3_m1_bsc2);
			REG(0x0027, subq, itOp3_m1_bsc2);

			REG(0x0028, jnzfs, itOp1_bsc1_imm32);
			REG(0x0029, jnzfd, itOp1_bsc1_imm32);

			REG(0x002c, jnzb, itOp1_bsc1_imm32);
			REG(0x002d, jnzw, itOp1_bsc1_imm32);
			REG(0x002e, jnzd, itOp1_bsc1_imm32);
			REG(0x002f, jnzq, itOp1_bsc1_imm32);

			REG(0x0030, mulfs, itOp3_m1_bsc2);
			REG(0x0031, mulfd, itOp3_m1_bsc2);

			REG(0x0034, mulb, itOp3_m1_bsc2);
			REG(0x0035, mulw, itOp3_m1_bsc2);
			REG(0x0036, muld, itOp3_m1_bsc2);
			REG(0x0037, mulq, itOp3_m1_bsc2);

			REG(0x0038, jzfs, itOp1_bsc1_imm32);
			REG(0x0039, jzfd, itOp1_bsc1_imm32);

			REG(0x003c, jzb, itOp1_bsc1_imm32);
			REG(0x003d, jzw, itOp1_bsc1_imm32);
			REG(0x003e, jzd, itOp1_bsc1_imm32);
			REG(0x003f, jzq, itOp1_bsc1_imm32);

			REG(0x0040, fmafs, itOp4_sign4);
			REG(0x0041, fmafd, itOp4_sign4);

			REG(0x0044, fmab, itOp4_sign4);
			REG(0x0045, fmaw, itOp4_sign4);
			REG(0x0046, fmad, itOp4_sign4);
			REG(0x0047, fmaq, itOp4_sign4);

			REG(0x0048, call, itOp1_m1_imm32);
			REG(0x004f, ret, itOp1_m1_imm32);

			REG(0x0050, andfs, itOp3_m1_bsc2);
			REG(0x0051, andfd, itOp3_m1_bsc2);

			REG(0x0054, andb, itOp3_m1_bsc2);
			REG(0x0055, andw, itOp3_m1_bsc2);
			REG(0x0056, andd, itOp3_m1_bsc2);
			REG(0x0057, andq, itOp3_m1_bsc2);

			REG(0x005a, pushd, itOp3_bsc3);
			REG(0x005b, pushq, itOp3_bsc3);
			REG(0x005c, pushdq, itOp3_bsc3);
			REG(0x005d, push, itOp3_bsc3);

			REG(0x0060, orfs, itOp3_m1_bsc2);
			REG(0x0061, orfd, itOp3_m1_bsc2);

			REG(0x0064, orb, itOp3_m1_bsc2);
			REG(0x0065, orw, itOp3_m1_bsc2);
			REG(0x0066, ord, itOp3_m1_bsc2);
			REG(0x0067, orq, itOp3_m1_bsc2);

			REG(0x006a, popd, itOp3_m1_bsc2);
			REG(0x006b, popq, itOp3_m1_bsc2);
			REG(0x006c, popdq, itOp3_m1_bsc2);
			REG(0x006d, pop, itOp3_m1_bsc2);

			REG(0x0070, xorfs, itOp3_m1_bsc2);
			REG(0x0071, xorfd, itOp3_m1_bsc2);

			REG(0x0074, xorb, itOp3_m1_bsc2);
			REG(0x0075, xorw, itOp3_m1_bsc2);
			REG(0x0076, xord, itOp3_m1_bsc2);
			REG(0x0077, xorq, itOp3_m1_bsc2);

			REG(0x0080, shufb, itOp3_m1_bsc2);
			REG(0x0081, shufbx, itOp6);

			REG(0x00b0, divfs, itOp3_m1_bsc2);
			REG(0x00b1, divfd, itOp3_m1_bsc2);

			REG(0x00b4, divsb, itOp3_m1_bsc2);
			REG(0x00b5, divsw, itOp3_m1_bsc2);
			REG(0x00b6, divsd, itOp3_m1_bsc2);
			REG(0x00b7, divsq, itOp3_m1_bsc2);

			REG(0x00bc, divub, itOp3_m1_bsc2);
			REG(0x00bd, divuw, itOp3_m1_bsc2);
			REG(0x00be, divud, itOp3_m1_bsc2);
			REG(0x00bf, divuq, itOp3_m1_bsc2);
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
				if (pos + 2 < len && !strncmp(&text[pos], "$SP", 3))
				{
					pos += 3;
					return 0x3000;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$CS", 3))
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
				if (pos + 2 < len && !strncmp(&text[pos], "$SP", 3))
				{
					pos += 3;
					return 0xc200;
				}
				if (pos + 2 < len && !strncmp(&text[pos], "$CS", 3))
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
				cmd.op1i.r_mask = 0xc0; // $CS
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
				cmd.op1i.r_mask = 0xc0; // $CS
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