/*
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * This file defines functions that implement ARMv7 instruction
 * behavior as they are described in the ARM Architecture Reference
 * Manual (ARM v7-A and ARM v7-R edition). All section and page
 * numbers refer to that manual unless otherwise noted.
 */

#ifndef __ARMV7_INSTRUCTION_HPP__
#define __ARMV7_INSTRUCTION_HPP__

#include "processor.hpp"
#include <boost/cstdint.hpp>

namespace arm {


    /**
     * Add with Carry (immediate) adds an immediate value and the carry flag
     * value to a register value, and writes the result to the destination 
     * register. It can optionally update the condition flags based on 
     * the result.
     * @brief (A8.6.1, p.326)
     */
    template< typename proc_type >
    void ADC_imm_A1( proc_type& proc, uint32_t instr );
    
    /**
     * Add with Carry (register) adds a register value, the carry flag value,
     * and an optionally-shifted register value, and writes the result to the 
     * destination register. It can optionally update the condition flags 
     * based on the result.
     * @brief (A8.6.2, p.328)
     */
    template< typename proc_type >
    void ADC_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Add with Carry (register-shifted register) adds a register value, 
     * the carry flag value, and a register-shifted register value. 
     * It writes the result to the destination register, and can optionally
     * update the condition flags based on the result.
     * @brief (A8.6.3, p.330)
     */
    template< typename proc_type >
    void ADC_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction adds an immediate value to a register value, 
     * and writes the result to the destination register. It can optionally 
     * update the condition flags based on the result.
     * @brief (A8.6.5, p.334)
     */
    template< typename proc_type >
    void ADD_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction adds a register value and an optionally-shifted 
     * register value, and writes the result to the destination register.
     * It can optionally update the condition flags based on the result.
     * @brief (A8.6.6, p.336)
     */
    template< typename proc_type >
    void ADD_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Add (register-shifted register) adds a register value and a 
     * register-shifted register value. It writes the result to the destination
     * register, and can optionally update the condition flags based 
     * on the result.
     * @brief (A8.6.7, p.338)
     */
    template< typename proc_type >
    void ADD_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction adds an immediate value to the SP value, and writes 
     * the result to the destination register.
     * @brief (A8.6.8, p.340)
     */
    template< typename proc_type >
    void ADD_SP_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction adds an optionally-shifted register value to the SP 
     * value, and writes the result to the destination register.
     * @brief (A8.6.9, p.342)
     */
    template< typename proc_type >
    void ADD_SP_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction adds an immediate value to the PC value to form a 
     * PC-relative address, and writes the result to the destination register.
     * @brief (A8.6.10, p.344)
     */
    template< typename proc_type >
    void ADR_A1( proc_type& proc, uint32_t instr );

    template< typename proc_type >
    void ADR_A2( proc_type& proc, uint32_t instr );

    /**
     * This instruction performs a bitwise AND of a register value and an 
     * immediate value, and writes the result to the destination register.
     * @brief (A8.6.11, p.346)
     */
    template< typename proc_type >
    void AND_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction performs a bitwise AND of a register value and an 
     * optionally-shifted register value, and writes the result to the 
     * destination register. It can optionally update the condition flags 
     * based on the result.
     * @brief (A8.6.12, p.348)
     */
    template< typename proc_type >
    void AND_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * This instruction performs a bitwise AND of a register value and a 
     * register-shifted register value. It writes the result to the destination
     * register, and can optionally update the condition flags based on 
     * the result.
     * @brief (A8.6.13, p.350)
     */
    template< typename proc_type >
    void AND_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * Arithmetic Shift Right (immediate) shifts a register value right by an
     * immediate number of bits, shifting in copies of its sign bit, and writes
     * the result to the destination register. It can optionally update the condition
     * flags based on the result.
     * @brief (A8.6.14, p.352)
     */
    template< typename proc_type >
    void ASR_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Arithmetic Shift Right (register) shifts a register value right by a 
     * variable number of bits, shifting in copies of its sign bit, and writes
     * the result to the destination register. The variable number of bits is
     * read from the bottom byte of a register. It can optionally update the
     * condition flags based on the result.
     * @brief (A8.6.15, p.354)
     */
    template< typename proc_type >
    void ASR_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Branch causes a branch to a target address.
     * @brief (A8.6.16, p.356)
     */
    template< typename proc_type >
    void B_A1( proc_type& proc, uint32_t instr );

    /**
     * Bit Field Clear clears any number of adjacent bits at any position in a
     * register, without affecting the other bits in the register.
     * @brief (A8.6.17, p.358)
     */
    template< typename proc_type >
    void BFC_A1( proc_type& proc, uint32_t instr );

    /**
     * Bit Field Insert copies any number of low order bits from a register 
     * into the same number of adjacent bits at any position in the 
     * destination register.
     * @brief (A8.6.18, p.360)
     */
    template< typename proc_type >
    void BFI_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Bit Clear (immediate) performs a bitwise AND of a register value
     * and the complement of an immediate value, and writes the result to the 
     * destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.19, p.362)
     */
    template< typename proc_type >
    void BIC_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Bit Clear (register) performs a bitwise AND of a register value 
     * and the complement of an optionally-shifted register value, and writes 
     * the result to the destination register. It can optionally update the
     * condition flags based on the result.
     * @brief (A8.6.20, p.364)
     */
    template< typename proc_type >
    void BIC_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Bit Clear (register-shifted register) performs a bitwise AND of
     * a register value and the complement of a register-shifted register 
     * value. It writes the result to the destination register, and can 
     * optionally update the condition flags based on the result.
     * @brief (A8.6.21, p.366)
     */
    template< typename proc_type >
    void BIC_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * Branch with Link calls a subroutine at a PC-relative address.
     * @brief (A8.6.23, p.370)
     */
    template< typename proc_type >
    void BL_A1( proc_type& proc, uint32_t instr );

    /**
     * Branch with Link and Exchange Instruction Sets (immediate) calls a 
     * subroutine at a PC-relative address, and changes instruction set from 
     * ARM to Thumb, or from Thumb to ARM.
     * @brief (A8.6.23, p.370)
     */
    template< typename proc_type >
    void BLX_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Branch with Link and Exchange (register) calls a subroutine at an 
     * address and instruction set specified by a register.
     * @brief (A8.6.24, p.372)
     */
    template< typename proc_type >
    void BLX_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Branch and Exchange causes a branch to an address and instruction set
     * specified by a register.
     * @brief (A8.6.25, p.374)
     */
    template< typename proc_type >
    void BX_A1( proc_type& proc, uint32_t instr );

    /**
     * Count Leading Zeros returns the number of binary zero bits before the 
     * first binary one bit in a value.
     * @brief (A8.6.31, p.384)
     */
    template< typename proc_type >
    void CLZ_A1( proc_type& proc, uint32_t instr );

    /**
     * Compare Negative (immediate) adds a register value and an immediate 
     * value. It updates the condition flags based on the result, and 
     * discards the result.
     * @brief (A8.6.32, p.386)
     */
    template< typename proc_type >
    void CMN_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Compare Negative (register) adds a register value and an optionally-
     * shifted register value. It updates the condition flags based on the 
     * result, and discards the result.
     * @brief (A8.6.33, p.388)
     */
    template< typename proc_type >
    void CMN_reg_A1( proc_type& proc, uint32_t instr );
    
    /**
     * Compare Negative (register-shifted register) adds a register value and
     * a register-shifted register value. It updates the condition flags based 
     * on the result, and discards the result.
     * @brief (A8.6.34, p.390)
     */
    template< typename proc_type >
    void CMN_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * Compare (immediate) subtracts an immediate value from a register value. 
     * It updates the condition flags based on the result, and discards 
     * the result.
     * @brief (A8.6.35, p.392)
     */
    template< typename proc_type >
    void CMP_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Compare (register) subtracts an optionally-shifted register value from a
     * register value. It updates the condition flags based on the result, and
     * discards the result.
     * @brief (A8.6.36, p.394)
     */
    template< typename proc_type >
    void CMP_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Compare (register-shifted register) subtracts a register-shifted 
     * register value from a register value. It updates the condition flags
     * based on the result, and discards the result.
     * @brief (A8.6.37, p.396)
     */
    template< typename proc_type >
    void CMP_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Exclusive OR (immediate) performs a bitwise Exclusive OR of a 
     * register value and an immediate value, and writes the result to the 
     * destination register. It can optionally update the condition flags 
     * based on the result.
     * @brief (A8.6.44, p.406)
     */
    template< typename proc_type >
    void EOR_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Exclusive OR (register) performs a bitwise Exclusive OR of a 
     * register value and an optionally-shifted register value, and writes the 
     * result to the destination register. It can optionally update the
     * condition flags based on the result.
     * @brief (A8.6.45, p.408)
     */
    template< typename proc_type >
    void EOR_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise Exclusive OR (register-shifted register) performs a bitwise 
     * Exclusive OR of a register value and a register-shifted register value. 
     * It writes the result to the destination register, and can optionally 
     * update the condition flags based on the result.
     * @brief (A8.6.46, p.410)
     */
    template< typename proc_type >
    void EOR_rsr_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Multiple (Increment After) loads multiple registers from
     * consecutive memory locations using an address from a base register.
     * The consecutive memory locations start at this address, and the address
     * just above the highest of those locations can optionally be written back
     * to the base register. The registers loaded can include the PC, causing a
     * branch to a loaded address.
     * @brief (A8.6.53, p.422)
     */
    template< typename proc_type >
    void LDM_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Multiple Decrement After (Load Multiple Full Ascending) loads
     * multiple registers from consecutive memory locations using an address
     * from a base register. The consecutive memory locations end at this
     * address, and the address just below the lowest of those locations can
     * optionally be written back to the base register. The registers loaded
     * can include the PC, causing a branch to a loaded address.
     * @brief (A8.6.54, p.424)
     */
    template< typename proc_type >
    void LDMDA_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Multiple Decrement Before (Load Multiple Empty Ascending) loads
     * multiple registers from consecutive memory locations using an address
     * from a base register. The consecutive memory locations end just below
     * this address, and the address of the lowest of those locations can
     * optionally be written back to the base register. The registers loaded
     * can include the PC, causing a branch to a loaded address.
     * @brief (A8.6.55, p.426)
     */
    template< typename proc_type >
    void LDMDB_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Multiple Increment Before loads multiple registers from consecutive
     * memory locations using an address from a base register. The consecutive
     * memory locations start just above this address, and the address of the
     * last of those locations can optionally be written back to the base
     * register. The registers loaded can include the PC, causing a branch to
     * a loaded address.
     * @brief (A8.6.56, p.428)
     */
    template< typename proc_type >
    void LDMIB_A1( proc_type& proc, uint32_t instr );

    /**
     * LDR (immediate, ARM)
     * Load Register (immediate) calculates an address from a base register
     * value and an immediate offset, loads a word from memory, and writes
     * it to a register. It can use offset, post-indexed, or pre-indexed
     * addressing.
     * @brief (A8.6.58, p.432)
     */
    template< typename proc_type >
    void LDR_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * LDR (literal)
     * Load Register (literal) calculates an address from the PC value and an
     * immediate offset, loads a word from memory, and writes it to a register.
     * @brief (A8.6.59, p.434)
     */
    template< typename proc_type >
    void LDR_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register (register) calculates an address from a base
     * register value and an offset register value, loads a word from
     * memory, and writes it to a register. The offset register value
     * can optionally be shifted.
     * @brief (A8.6.60, p.436)
     */
    template< typename proc_type >
    void LDR_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Byte (immediate) calculates an address from a base
     * register value and an immediate offset, loads a byte from memory,
     * zero-extends it to form a 32-bit word, and writes it to a register.
     * It can use offset, post-indexed, or pre-indexed addressing.
     * @brief (A8.6.62, p.440)
     */
    template< typename proc_type >
    void LDRB_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Byte (literal) calculates an address from the PC
     * value and an immediate offset, loads a byte from memory,
     * zero-extends it to form a 32-bit word, and writes it to a
     * register.
     * @brief (A8.6.63, p.442)
     */
    template< typename proc_type >
    void LDRB_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Byte (register) calculates an address from abase register
     * value and an offset register value, loads a byte from memory,
     * zero-extends it to form a 32-bit word, and writes it to a register. The
     * offset register value can optionally be shifted.
     * @brief (A8.6.64, p.444)
     */
    template< typename proc_type >
    void LDRB_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Byte Unprivileged loads a byte from memory, zero-extends
     * it to form a 32-bit word, and writes it to a register. For information
     * about memory accesses see Memory accesses on page A8-13. The memory
     * access is restricted as if the processor were running in User mode.
     * (This makes no difference if the processor is actually running in User
     * mode.) The ARM instruction uses a post-indexed addressing mode, that
     * uses a base register value as the address for the memory access, and
     * calculates a new address from a base register value and an offset and
     * writes it back to the base register. The offset can be an immediate
     * value or an optionally-shifted register value.
     * @brief (A8.6.65, p.446)
     */
    template< typename proc_type >
    void LDRBT_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Byte Unprivileged loads a byte from memory, zero-extends
     * it to form a 32-bit word, and writes it to a register. For information
     * about memory accesses see Memory accesses on page A8-13. The memory
     * access is restricted as if the processor were running in User mode.
     * (This makes no difference if the processor is actually running in User
     * mode.) The ARM instruction uses a post-indexed addressing mode, that
     * uses a base register value as the address for the memory access, and
     * calculates a new address from a base register value and an offset and
     * writes it back to the base register. The offset can be an immediate
     * value or an optionally-shifted register value.
     * @brief (A8.6.65, p.446)
     */
    template< typename proc_type >
    void LDRBT_A2( proc_type& proc, uint32_t instr );

    /**
     * Load Register Dual (immediate) calculates an address from a base
     * register value and an immediate offset, loads two words from memory,
     * and writes them to two registers. It can use offset, post-indexed, or
     * pre-indexed addressing.
     * @brief (A8.6.66, p.448)
     */
    template< typename proc_type >
    void LDRD_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Dual (literal) calculates an address from the PC value and
     * an immediate offset, loads two words from memory, and writes them to two
     * registers.
     * @brief (A8.6.67, p.450)
     */
    template< typename proc_type >
    void LDRD_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Dual (register) calculates an address from abase register
     * value and a register offset, loads two words from memory, and writes
     * them to two registers. It can use offset, post-indexed, or pre-indexed
     * addressing.
     * @brief (A8.6.68, p.452)
     */
    template< typename proc_type >
    void LDRD_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Halfword (immediate) calculates an address from a base
     * register value and an immediate offset, loads a halfword from memory,
     * zero-extends it to form a 32-bit word, and writes it to a register. It
     * can use offset, post-indexed, or pre-indexed addressing.
     * @brief (A8.6.74, p.464)
     */
    template< typename proc_type >
    void LDRH_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Halfword (literal) calculates an address from the PC value
     * and an immediate offset, loads a halfword from memory, zero-extends it
     * to form a 32-bit word, and writes it to a register.
     * @brief (A8.6.75, p.466)
     */
    template< typename proc_type >
    void LDRH_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Halfword (register) calculates an address from a base
     * register value and an offset register value, loads a halfword from
     * memory, zero-extends it to form a 32-bit word, and writes it to a
     * register. The offset register value can be shifted left by 0, 1, 2, or
     * 3 bits.
     * @brief (A8.6.76, p.468)
     */
    template< typename proc_type >
    void LDRH_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Halfword Unprivileged loads a halfword from memory,
     * zero-extends it to form a 32-bit word, and writes it to a register.
     * For information about memory accesses see Memory accesses on page A8-13.
     * The memory access is restricted as if the processor were running in User
     * mode. (This makes no difference if the processor is actually running in
     * User mode.) The ARM instruction uses a post-indexed addressing mode,
     * that uses a base register value as the address for the memory access,
     * and calculates a new address from a base register value and an offset
     * and writes it back to the base register. The offset can be an immediate
     * value or a register value.
     * @brief (A8.6.77, p.470)
     */
    template< typename proc_type >
    void LDRHT_A1( proc_type& proc, uint32_t instr );

    template< typename proc_type >
    void LDRHT_A2( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Byte (immediate) calculates an address from a base
     * register value and an immediate offset, loads a byte from memory,
     * sign-extends it to form a 32-bit word, and writes it to a register. It
     * can use offset, post-indexed, or pre-indexed addressing.
     * @brief (A8.6.78, p.472)
     */
    template< typename proc_type >
    void LDRSB_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Byte (literal) calculates an address from the PC
     * value and an immediate offset, loads a byte from memory, sign-extends
     * it to form a 32-bit word, and writes it to a register.
     * @brief (A8.6.79, p.474)
     */
    template< typename proc_type >
    void LDRSB_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Byte (register) calculates an address from a base
     * register value and an offset register value, loads a byte from memory,
     * sign-extends it to form a 32-bit word, and writes it to a register. The
     * offset register value can be shifted left by 0, 1, 2, or 3 bits.
     * @brief (A8.6.80, p.476)
     */
    template< typename proc_type >
    void LDRSB_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Byte Unprivileged loads a byte from memory,
     * sign-extends it to form a 32-bit word, and writes it to a register. For
     * information about memory accesses see Memory accesses on page A8-13.
     * The memory access is restricted as if the processor were running in User
     * mode. (This makes no difference if the processor is actually running in
     * User mode.) The ARM instruction uses a post-indexed addressing mode,
     * that uses a base register value as the address for the memory access,
     * and calculates a new address from a base register value and an offset
     * and writes it back to the base register. The offset can be an immediate
     * value or a register value.
     * @brief (A8.6.81, p.478)
     */
    template< typename proc_type >
    void LDRSBT_A1( proc_type& proc, uint32_t instr );

    template< typename proc_type >
    void LDRSBT_A2( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Halfword (immediate) calculates an address from a
     * base register value and an immediate offset, loads a halfword from
     * memory, sign-extends it to form a 32-bit word, and writes it to a
     * register. It can use offset, post-indexed, or pre-indexed addressing.
     * @brief (A8.6.82, p.480)
     */
    template< typename proc_type >
    void LDRSH_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Halfword (literal) calculates an address from the
     * PC value and an immediate offset, loads a halfword from memory,
     * sign-extends it to form a 32-bit word, and writes it to a register.
     * @brief (A8.6.83, p.482)
     */
    template< typename proc_type >
    void LDRSH_lit_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Halfword (register) calculates an address from a
     * base register value and an offset register value, loads a halfword from
     * memory, sign-extends it to form a 32-bit word, and writes it to a
     * register. The offset register value can be shifted left by 0, 1, 2, or 3
     * bits.
     * @brief (A8.6.84, p.484)
     */
    template< typename proc_type >
    void LDRSH_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Load Register Signed Halfword Unprivileged loads a halfword from memory,
     * sign-extends it to form a 32-bit word, and writes it to a register. For
     * information about memory accesses see Memory accesses on page A8-13.
     * The memory access is restricted as if the processor were running in
     * User mode. (This makes no difference if the processor is actually
     * running in User mode.) The ARM instruction uses a post-indexed
     * addressing mode, that uses a base register value as the address for
     * the memory access, and calculates a new address from a base register
     * value and an offset and writes it back to the base register. The offset
     * can be an immediate value or a register value.
     * @brief (A8.6.85, p.486)
     */
    template< typename proc_type >
    void LDRSHT_A1( proc_type& proc, uint32_t instr );

    template< typename proc_type >
    void LDRSHT_A2( proc_type& proc, uint32_t instr );

    /**
     * Load Register Unprivileged loads a word from memory, and writes it to a
     * register. For information about memory accesses see Memory accesses on
     * page A8-13. The memory access is restricted as if the processor were
     * running in User mode. (This makes no difference if the processor is
     * actually running in User mode.) The ARM instruction uses a post-indexed
     * addressing mode, that uses a base register value as the address for the
     * memory access, and calculates a new address from a base register value
     * and an offset and writes it back to the base register. The offset can
     * be an immediate value or an optionally-shifted register value.
     * @brief (A8.6.86, p.488)
     */
    template< typename proc_type >
    void LDRT_A1( proc_type& proc, uint32_t instr );

    template< typename proc_type >
    void LDRT_A2( proc_type& proc, uint32_t instr );

    /**
     * Logical Shift Left (immediate) shifts a register value left by an
     * immediate number of bits, shifting in zeros, and writes the result to
     * the destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.88, p.490)
     */
    template< typename proc_type >
    void LSL_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Logical Shift Left (register) shifts a register value left by a variable
     * number of bits, shifting in zeros, and writes the result to the
     * destination register. The variable number of bits is read from the
     * bottom byte of a register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.89, p.492)
     */
    template< typename proc_type >
    void LSL_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Logical Shift Right (immediate) shifts a register value right by an
     * immediate number of bits, shifting in zeros, and writes the result to
     * the destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.90, p.494)
     */
    template< typename proc_type >
    void LSR_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Logical Shift Right (register) shifts a register value right by a
     * variable number of bits, shifting in zeros, and writes the result to the
     * destination register. The variable number of bits is read from the
     * bottom byte of a register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.91, p.496)
     */
    template< typename proc_type >
    void LSR_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Multiply Accumulate multiplies two register values, and adds a third
     * register value. The least significant 32 bits of the result are written
     * to the destination register. These 32 bits do not depend on whether the
     * source register values are considered to be signed values or unsigned
     * values.
     * @brief (A8.6.94, p.502)
     */
    template< typename proc_type >
    void MLA_A1( proc_type& proc, uint32_t instr );

    /**
     * Multiply and Subtract multiplies two register values, and subtracts the
     * product from a third register value. The least significant 32 bits of
     * the result are written to the destination register. These 32 bits do
     * not depend on whether the source register values are considered to be
     * signed values or unsigned values.
     * @brief (A8.6.95, p.504)
     */
    template< typename proc_type >
    void MLS_A1( proc_type& proc, uint32_t instr );

    /**
     * Move (immediate) writes an immediate value to the destination register.
     * It can optionally update the condition flags based on the value.
     * #const
     * @brief (A8.6.96, p.506)
     */
    template< typename proc_type >
    void MOV_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Move (immediate) writes an immediate value to the destination register.
     * It can optionally update the condition flags based on the value.
     * #imm16
     * @brief (A8.6.96, p.506)
     */
    template< typename proc_type >
    void MOV_imm_A2( proc_type& proc, uint32_t instr );

    /**
     * Move (register) copies a value from a register to the destination
     * register. It can optionally update the condition flags based on the
     * value.
     * @brief (A8.6.97, p.508)
     */
    template< typename proc_type >
    void MOV_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Move Top writes an immediate value to the top halfword of the
     * destination register. It does not affect the contents of the bottom
     * halfword.
     * @brief (A8.6.99, p.512)
     */
    template< typename proc_type >
    void MOVT_A1( proc_type& proc, uint32_t instr );

    /**
     * Move to Register from Special Register moves the value from the APSR
     * into a general-purpose register.
     * @brief (A8.6.102, p.518)
     */
    template< typename proc_type >
    void MRS_A1( proc_type& proc, uint32_t instr );

    /**
     * Move immediate value to Special Register moves selected bits of an
     * immediate value to the corresponding bits in the APSR.
     * @brief (A8.6.103, p.520)
     */
    template< typename proc_type >
    void MSR_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Move to Special Register from ARM core register moves selected bits of
     * a general-purpose register to the APSR.
     * @brief (A8.6.104, p.522)
     */
    template< typename proc_type >
    void MSR_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Multiply multiplies two register values. The least significant 32 bits
     * of the result are written to the destination register. These 32 bits do
     * not depend on whether the source register values are considered to be
     * signed values or unsigned values.
     * @brief (A8.6.105, p.524)
     */
    template< typename proc_type >
    void MUL_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise NOT (immediate) writes the bitwise inverse of an immediate value
     * to the destination register. It can optionally update the condition
     * flags based on the value.
     * @brief (A8.6.106, p.526)
     */
    template< typename proc_type >
    void MVN_imm_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise NOT (register) writes the bitwise inverse of a register value to
     * the destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.107, p.528)
     */
    template< typename proc_type >
    void MVN_reg_A1( proc_type& proc, uint32_t instr );

    /**
     * Bitwise NOT (register-shifted register) writes the bitwise inverse of a
     * register-shifted register value to the destination register. It can
     * optionally update the condition flags based on the result.
     * @brief (A8.6.108, p.530)
     */
    template< typename proc_type >
    void MVN_rsr_A1( proc_type& proc, uint32_t instr );


    /**
     * NOP
     * No Operation does nothing. This instruction can be used for 
     * code alignment purposes.
     * @brief (A8.6.110, p.534)
     */
    template< typename proc_type >
    void NOP_A1( proc_type& proc, uint32_t instr );


    /**
     * Bitwise OR (immediate)
     * Performs a bitwise (inclusive) OR of a register value and an immediate
     * value, and writes the result to the destination register. It can
     * optionally update the condition flags based on the result.
     * @brief (A8.6.113, p.540)
     */
    template< typename proc_type >
    void ORR_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * Bitwise OR (register)
     * Performs a bitwise (inclusive) OR of a register value and an optionally
     * shifted register value, and writes the result to the destination
     * register. It can optionally update the condition flags based on the
     * result.
     * @brief (A8.6.114, p.542)
     */
    template< typename proc_type >
    void ORR_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * Bitwise OR (register-shifted register)
     * Performs a bitwise (inclusive) OR of a register value and a
     * register-shifted register value, and writes the result to the destination
     * register. It can optionally update the condition flags based on the
     * result.
     * @brief (A8.6.115, p.544)
     */
    template< typename proc_type >
    void ORR_reg_shift_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * PKH
     * Pack Halfword combines one halfword of its first operand with the other
     * halfword of its shifted second operand.
     * @brief (A8.6.116 p.546)
     */    
    template< typename proc_type >
    void PKH_A1( proc_type& proc, uint32_t instr );


    /**
    * PLD/PLDW (immediate)
    * Preload Data signals the memory system that data memory accesses from a 
    * specified address are likely in the near future. The memory system can
    * respond by taking actions that are expected to speed up the memory
    * accesses when they do occur, such as pre-loading the cache line containing
    * the specified address into the data cache.
    * NOTE :    Implemented as NOP by this library as preload operations are not
    *           covered by the ARMv7 documentation. Functionality is preserved.
    * (A8.6.117 p.548)
    */
    template< typename proc_type >
    void PLD_imm_A1( proc_type& proc, uint32_t instr );


    /**
    * PLD (literal)
    * Preload Data signals the memory system that data memory accesses from a 
    * specified address are likely in the near future. The memory system can
    * respond by taking actions that are expected to speed up the memory
    * accesses when they do occur, such as pre-loading the cache line containing
    * the specified address into the data cache.
    * NOTE :    Implemented as NOP by this library as preload operations are not
    *           covered by the ARMv7 documentation. Functionality is preserved.
    * (A8.6.118 p.550)
    */
    template< typename proc_type >
    void PLD_lit_A1( proc_type& proc, uint32_t instr );


    /**
    * PLD/PLDW (register)
    * Preload Data signals the memory system that data memory accesses from a 
    * specified address are likely in the near future. The memory system can
    * respond by taking actions that are expected to speed up the memory
    * accesses when they do occur, such as pre-loading the cache line containing
    * the specified address into the data cache.
    * NOTE :    Implemented as NOP by this library as preload operations are not
    *           covered by the ARMv7 documentation. Functionality is preserved.
    * (A8.6.119 p.552)
    */
    template< typename proc_type >
    void PLD_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * PLI (immediate, literal)
     * Preload Instruction signals the memory system that instruction memory
     * accesses from a specified address are likely in the near future. The
     * memory system can respond by taking actions that are expected to speed up
     * the memory accesses when they do occur, such as pre-loading the cache
     * line containing the specified address into the instruction cache.
    * (A8.6.120 p.554)
     */
    template< typename proc_type >
    void PLI_imm_lit_A1( proc_type& proc, uint32_t instr );


    /**
     * PLI (register)
     * Preload Instruction signals the memory system that instruction memory
     * accesses from a specified address are likely in the near future. The
     * memory system can respond by taking actions that are expected to speed up
     * the memory accesses when they do occur, such as pre-loading the cache
     * line containing the specified address into the instruction cache.
     * NOTE :   Implemented as NOP by this library as preload operations are not
     *          covered by the ARMv7 documentation. Functionality is preserved.
     * @brief (A8.6.121 p.556)
     */
    template< typename proc_type >
    void PLI_reg_A1( proc_type& proc, uint32_t instr );


     /**
      * POP (custom encoding 1)
      * Pop Multiple Registers loads multiple registers from the stack, loading
      * from consecutive memory locations starting at the address in SP, and
      * updates SP to point just above the loaded data.
      * @brief (A8.6.122 p.558)
      */
    template< typename proc_type >
    void POP_A1( proc_type& proc, uint32_t instr );


    /**
      * POP (custom encoding 2)
      * Pop Multiple Registers loads multiple registers from the stack, loading
      * from consecutive memory locations starting at the address in SP, and
      * updates SP to point just above the loaded data.
      * @brief (A8.6.122 p.558)
      */
    template< typename proc_type >
    void POP_A2( proc_type& proc, uint32_t instr );


    /**
      * PUSH (custom encoding 1)
      * Push Multiple Registers stores multiple registers to the stack, storing
      * to consecutive memory locations ending just below the address in SP,
      * and updates SP to point to the start of the stored data.
      * @brief (A8.6.123 p.560)
      */
    template< typename proc_type >
    void PUSH_A1( proc_type& proc, uint32_t instr );


    /**
      * PUSH (custom encoding 2)
      * Push Multiple Registers stores multiple registers to the stack, storing
      * to consecutive memory locations ending just below the address in SP,
      * and updates SP to point to the start of the stored data.
      * @brief (A8.6.123 p.560)
      */
    template< typename proc_type >
    void PUSH_A2( proc_type& proc, uint32_t instr );


    /**
     * QADD
     * Saturating Add adds two register values, saturates the result to the
     * 32-bit signed integer range –2^31 ≤ x ≤ 2^31 – 1, and writes the result to
     * the destination register. If saturation occurs, it sets the Q flag in
     * the APSR.
     * @brief (A8.6.124 p.562)
     */
    template< typename proc_type >
    void QADD_A1( proc_type& proc, uint32_t instr );


    /**
     * QADD16
     * Saturating Add 16 performs two 16-bit integer additions, saturates the
     * results to the 16-bit signed integer range –2^15 ≤ x ≤ 2^15 – 1, and
     * writes the results to the destination register.
     * @brief (A8.6.125 p.564)
     */
    template< typename proc_type >
    void QADD16_A1( proc_type& proc, uint32_t instr );


    /**
     * QADD8
     * Saturating Add 8 performs four 8-bit integer additions, saturates the
     * results to the 8-bit signed integer range –2^7 ≤ x ≤ 2^7 – 1, and
     * writes the results to the destination register.
     * @brief (A8.6.126 p.566)
     */
    template< typename proc_type >
    void QADD8_A1( proc_type& proc, uint32_t instr );


    /**
     * QASX
     * Saturating Add and Subtract with Exchange exchanges the two halfwords of
     * the second operand, performs one 16-bit integer addition and one 16-bit
     * subtraction, saturates the results to the 16-bit signed integer range
     * –2^15 ≤ x ≤ 2^15 – 1, and writes the results to the destination register.
     * @brief (A8.6.127 p.568)
     */
    template< typename proc_type >
    void QASX_A1( proc_type& proc, uint32_t instr );


    /**
     * QDADD
     * Saturating Double and Add adds a doubled register value to another
     * register value, and writes the result to the destination register.
     * Both the doubling and the addition have their results saturated to the
     * 32-bit signed integer range –2^31 ≤ x ≤ 2^31 – 1. If saturation occurs in
     * either operation, it sets the Q flag in the APSR.
     * @brief (A8.6.128 p.570)
     */
    template< typename proc_type >
    void QDADD_A1( proc_type& proc, uint32_t instr );


    /**
     * QDSUB
     * Saturating Double and Subtract subtracts a doubled register value from
     * another register value, and writes the result to the destination
     * register. Both the doubling and the subtraction have their results
     * saturated to the 32-bit signed integer range –2^31 ≤ x ≤ 2^31 – 1.
     * If saturation occurs in either operation, it sets the Q flag in the APSR.
     * @brief (A8.6.129 p.572)
     */
    template< typename proc_type >
    void QDSUB_A1( proc_type& proc, uint32_t instr );


    /**
     * QSAX
     * Saturating Subtract and Add with Exchange exchanges the two halfwords of
     * the second operand, performs one 16-bit integer subtraction and one 
     * 16-bit addition, saturates the results to the 16-bit signed integer range
     * –2^15 ≤ x ≤ 2^15 – 1, and writes the results to the destination register.
     * @brief (A8.6.130 p.574)
     */
    template< typename proc_type >
    void QSAX_A1( proc_type& proc, uint32_t instr );


    /**
     * QSUB
     * Saturating Subtract subtracts one register value from another register
     * value, saturates the result to the 32-bit signed integer range
     * –2^31 ≤ x ≤ 2^31 – 1, and writes the result to the destination register.
     * If saturation occurs, it sets the Q flag in the APSR.
     * @brief (A8.6.131 p.576)
     */
    template< typename proc_type >
    void QSUB_A1( proc_type& proc, uint32_t instr );


    /**
     * QSUB16
     * Saturating Subtract 16 performs two 16-bit integer subtractions,
     * saturates the results to the 16-bit signed integer range
     * –2^15 ≤ x ≤ 2^15 – 1, and writes the results to the destination register.
     * @brief (A8.6.132 p.578)
     */
    template< typename proc_type >
    void QSUB16_A1( proc_type& proc, uint32_t instr );


    /**
     * QSUB8
     * Saturating Subtract 8 performs four 8-bit integer subtractions, saturates
     * the results to the 8-bit signed integer range –2^7 ≤ x ≤ 2^7 – 1, and
     * writes the results to the destination register.
     * @brief (A8.6.133 p.580)
     */
    template< typename proc_type >
    void QSUB8_A1( proc_type& proc, uint32_t instr );


    /**
     * RBIT
     * Reverse Bits reverses the bit order in a 32-bit register.
     * @brief (A8.6.134 p.582)
     */
    template< typename proc_type >
    void RBIT_A1( proc_type& proc, uint32_t instr );


    /**
     * REV
     * Byte-Reverse Word reverses the byte order in a 32-bit register.
     * @brief (A8.6.135 p.584)
     */
    template< typename proc_type >
    void REV_A1( proc_type& proc, uint32_t instr );


    /**
     * REV16
     * Byte-Reverse Packed Halfword reverses the byte order in each 16-bit
     * halfword of a 32-bit register.
     * @brief (A8.6.136 p.586)
     */
    template< typename proc_type >
    void REV16_A1( proc_type& proc, uint32_t instr );


    /**
     * REVSH
     * Byte-Reverse Signed Halfword reverses the byte order in the lower 16-bit
     * halfword of a 32-bit register, and sign-extends the result to 32 bits.
     * @brief (A8.6.137 p.588)
     */
    template< typename proc_type >
    void REVSH_A1( proc_type& proc, uint32_t instr );


    /**
     * RFE
     * Return From Exception is a system instruction. For details see RFE on page
     * B6-16.
     * @brief (A8.6.138 p.590)
     */
    template< typename proc_type >
    void RFE_A1( proc_type& proc, uint32_t instr );


    /**
     * ROR (immediate)
     * Rotate Right (immediate) provides the value of the contents of a register
     * rotated by a constant value. The bits that are rotated off the right end
     * are inserted into the vacated bit positions on the left. It can optionally
     * update the condition flags based on the result.
     * @brief (A8.6.139 p.590)
     */
    template< typename proc_type >
    void ROR_IMM_A1( proc_type& proc, uint32_t instr );


    /**
     * ROR (register)
     * Rotate Right (register) provides the value of the contents of a register
     * rotated by a variable number of bits. The bits that are rotated off the
     * right end are inserted into the vacated bit positions on the left. The
     * variable number of bits is read from the bottom byte of a register. It
     * can optionally update the condition flags based on the result.
     * @brief (A8.6.140 p.592)
     */
    template< typename proc_type >
    void ROR_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * RRX
     * Rotate Right with Extend provides the value of the contentsof a register
     * shifted right by one place, with the carry flag shifted into bit [31].
     * RRX can optionally update the condition flags based on the result. In
     * that case, bit [0] is shifted into the carry flag.
     * @brief (A8.6.141 p.594)
     */
    template< typename proc_type >
    void RRX_A1( proc_type& proc, uint32_t instr );


    /**
     * RSB (immediate)
     * Reverse Subtract (immediate) subtracts a register value from an immediate
     * value, and writes the result to the destination register. It can
     * optionally update the condition flags based on the result.
     * @brief (A8.6.142 p.596)
     */
    template< typename proc_type >
    void RSB_IMM_A1( proc_type& proc, uint32_t instr );


    /**
     * RSB (register)
     * Reverse Subtract (register) subtracts a register value from an
     * optionally-shifted register value, and writes the result to the
     * destination register. It can optionally update the condition flags based
     * on the result.
     * @brief (A8.6.143 p.598)
     */
    template< typename proc_type >
    void RSB_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * RSB (register-shifted register)
     * Reverse Subtract (register-shifted register) subtracts a register value
     * from a register-shifted register value, and writes the result to the
     * destination register. It can optionally update the condition flags based
     * on the result.
     * @brief (A8.6.144 p.600)
     */
    template< typename proc_type >
    void RSB_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * RSC (immediate)
     * Reverse Subtract with Carry (immediate) subtracts a register value and the
     * value of NOT (Carry flag) from an immediate value, and writes the result
     * to the destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.145 p.602)
     */
    template< typename proc_type >
    void RSC_IMM_A1( proc_type& proc, uint32_t instr );


    /**
     * RSC (register)
     * Reverse Subtract with Carry (register) subtracts a register value and the
     * value of NOT (Carry flag) from an optionally-shifted register value, and
     * writes the result to the destination register. It can optionally update
     * the condition flags based on the result.
     * @brief (A8.6.146 p.604)
     */
    template< typename proc_type >
    void RSC_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * RSC (register-shifted register)
     * Reverse Subtract (register-shifted register) subtracts a register value
     * and the value of NOT (Carry flag) from a register-shifted register value,
     * and writes the result to the destination register. It can optionally
     * update the condition flags based on the result.
     * @brief (A8.6.147 p.606)
     */
    template< typename proc_type >
    void RSC_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr );


    /** 
     * SADD16
     * Signed Add 16 performs two 16-bit signed integer additions, and writes
     * the results to the destination register. It sets the APSR.GE bits
     * according to the results of the additions.
     * @brief (A8.6.148 p.608)
     */
    template< typename proc_type >
    void SADD16_A1( proc_type& proc, uint32_t instr );


    /** 
     * SADD8
     * Signed Add 8 performs four 8-bit signed integer additions, and writes
     * the results to the destination register. It sets the APSR.GE bits
     * according to the results of the additions.
     * @brief (A8.6.149 p.610)
     */
    template< typename proc_type >
    void SADD8_A1( proc_type& proc, uint32_t instr );


    /**
     * SASX
     * Signed Add and Subtract with Exchange exchanges the two halfwords of
     * the second operand, performs one 16-bit integer addition and one 16-bit
     * subtraction, and writes the results to the destination register. It sets
     * APSR.GE bits according to the results.
     * @brief (A8.6.150 p.612)
     */
    template< typename proc_type >
    void SASX_A1( proc_type& proc, uint32_t instr );


    /**
     * SBC (immediate)
     * Subtract with Carry (immediate) subtracts an immediate value and the
     * value of NOT (Carry flag) from a register value, and writes the result
     * to the destination register. It can optionally update the condition flags
     * based on the result.
     * @brief (A8.6.151 p.614)
     */
    template< typename proc_type >
    void SBC_IMM_A1( proc_type& proc, uint32_t instr );


    /**
     * SBC (register)
     * Subtract with Carry (register) subtracts an optionally-shifted register
     * value and the value of NOT (Carry flag) from a register value, and writes
     * the result to the destination register. It can optionally update the
     * condition flags based on the result.
     * @brief (A8.6.152 p.616)
     */
    template< typename proc_type >
    void SBC_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * SBC (register-shifted register)
     * Subtract with Carry (register-shifted register) subtracts a 
     * register-shifted register value and the value of NOT (Carry flag) from a
     * register value, and writes the result to the destination register. It can
     * optionally update the condition flags based on the result.
     * @brief (A8.6.153 p.618)
     */
    template< typename proc_type >
    void SBC_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr );


    /**
     * SBFX
     * Signed Bit Field Extract extracts any number of adjacent bits at any
     * position from a register, sign-extends them to 32 bits, and writes the
     * result to the destination register.
     * @brief (A8.6.154 p.620)
     */
    template< typename proc_type >
    void SBFX_A1( proc_type& proc, uint32_t instr );


    /**
     * SEL
     * Select Bytes selects each byte of its result from either its first
     * operand or its second operand, according to the values of the GE flags.
     * @brief (A8.6.156 p.624)
     */
    template< typename proc_type >
    void SEL_A1( proc_type& proc, uint32_t instr );

    
    /**
     * SETEND
     * Set Endianness writes a new value to ENDIANSTATE.
     * @brief (A8.6.157 p.626)
     */
    template< typename proc_type >
    void SETEND_A1( proc_type& proc, uint32_t instr );



    /**
     * SHADD16
     * Signed Halving Add 16 performs two signed 16-bit integer additions,
     * halves the results, and writes the results to the destination register.
     * @brief (A8.6.159, p.630)
     */
    template< typename proc_type >
    void SHADD16_A1( proc_type& proc, uint32_t instr );


    /**
     * SHADD8
     * Signed Halving Add 8 performs four signed 8-bit integer additions, 
     * halves the results, and writes the results to the destination register.
     * @brief (A8.6.160, p.632)
     */
    template< typename proc_type >
    void SHADD8_A1( proc_type& proc, uint32_t instr );

    
    /**
     * SHASX
     * Signed Halving Add and Subtract with Exchange exchanges the two
     * halfwords of the second operand, performs one signed 16-bit integer
     * addition and one signed 16-bit subtraction, halves the results, and
     * writes the results to the destination register.
     * @brief (A8.6.161, p.634)
     */
    template< typename proc_type >
    void SHASX_A1( proc_type& proc, uint32_t instr );
    
    
    /**
     * SHSAX
     * Signed Halving Subtract and Add with Exchange exchanges the two
     * halfwords of the second operand, performs one signed 16-bit integer
     * subtraction and one signed 16-bit addition, halves the results, and
     * writes the results to the destination register.
     * @brief (A8.6.162, p.636)
     */    
    template< typename proc_type >
    void SHSAX_A1( proc_type& proc, uint32_t instr );
    
    
    /**
     * SHSUB16
     * Signed Halving Subtract 16 performs two signed 16-bit integer
     * subtractions, halves the results, and writes the results to the
     * destination register.
     * @brief (A8.6.163, p.638)
     */
    template< typename proc_type >
    void SHSUB16_A1( proc_type& proc, uint32_t instr );
    
    
    /**
     * SHSUB8
     * Signed Halving Subtract 8 performs four signed 8-bit integer
     * subtractions, halves the results, and writes the results to the
     * destination register.
     * @brief (A8.6.164, p.640)
     */
    template< typename proc_type >
    void SHSUB8_A1( proc_type& proc, uint32_t instr );

    
    /**
     * SMLABB, SMLABT, SMLATB, SMLATT
     * Signed Multiply Accumulate Long multiplies two signed 32-bit values to
     * produce a 64-bit value, and accumulates this with a 64-bit value.
     * @brief (A8.6.166, p.642)
     */
    template< typename proc_type >
    void SMLAxy_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLAD
     * Signed Multiply Accumulate Dual performs two signed 16 x 16-bit 
     * multiplications. It adds the products to a 32-bit accumulate operand.
     * @brief (A8.6.167, 644)
     */
    template< typename proc_type >
    void SMLAD_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLAL
     * Signed Multiply Accumulate Long multiplies two signed 32-bit values to
     * produce a 64-bit value, and accumulates this with a 64-bit value.
     * @brief (A8.6.168, 646)
     */
    template< typename proc_type >
    void SMLAL_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLALBB, SMLALBT, SMLALTB, SMLALTT
     * Signed Multiply Accumulate Long (halfwords) multiplies two signed 16-bit
     * values to produce a 32-bit value, and accumulates this with a 64-bit
     * value. The multiply acts on two signed 16-bit quantities, taken from
     * either the bottom or the top half of their respective source registers.
     * The other halves of these source registers are ignored. The 32-bit
     * product is sign-extended and accumulated with a 64-bit accumulate value.
     * @brief (A8.6.169, 648)
     */
    template< typename proc_type >
    void SMLALxy_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLALD
     * Signed Multiply Accumulate Long Dual performs two signed 16 x 16-bit 
     * multiplications. It adds the products to a 64-bit accumulate operand.
     * Optionally, you can exchange the halfwords of the second operand before
     * performing the arithmetic. This produces top " bottom and bottom " top
     * multiplication.
     * @brief (A8.6.170, 650)
     */
    template< typename proc_type >
    void SMLALD_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLAWB, SMLAWT
     * Signed Multiply Accumulate (word by halfword) performs a signed 
     * multiply-accumulate operation. The multiply acts on a signed 32-bit
     * quantity and a signed 16-bit quantity. The signed 16-bit quantity is
     * taken from either the bottom or the top half of its source register. The
     * other half of the second source register is ignored. The top 32 bits of
     * the 48-bit product are added to a 32-bit accumulate value and the result
     * is written to the destination register. The bottom 16 bits of the 48-bit 
     * product are ignored..
     * @brief (A8.6.171, 652)
     */
    template< typename proc_type >
    void SMLAWx_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLSD
     * Signed Multiply Subtract Dual performs two signed 16 x 16-bit
     * multiplications. It adds the difference of the products to a 32-bit
     * accumulate operand. Optionally, you can exchange the halfwords of the
     * second operand before performing the arithmetic. This produces 
     * top x bottom and bottom x top multiplication.
     * @brief (A8.6.172, 654)
     */
    template< typename proc_type >
    void SMLSD_A1( proc_type& proc, uint32_t instr );


    /*
     * SMLSLD
     * Signed Multiply Subtract Long Dual performs two signed 16 x 16-bit
     * multiplications. It adds the difference of the products to a 64-bit
     * accumulate operand. Optionally, you can exchange the halfwords of the
     * second operand before performing the arithmetic. This produces
     * top x bottom and bottom x top multiplication.
     * @brief (A8.6.173, 656)
     */
    template< typename proc_type >
    void SMLSLD_A1( proc_type& proc, uint32_t instr );
   

    /**
     * SMMLA
     * Signed Most Significant Word Multiply Accumulate multiplies two signed
     * 32-bit values, extracts the most significant 32 bits of the result, and 
     * adds an accumulate value. Optionally, you can specify that the result is
     * rounded instead of being truncated. In this case, the constant
     * 0x80000000 is added to the product before the high word is extracted.
     * @brief (A8.6.174, 658)
     */ 
    template< typename proc_type >
    void SMMLA_A1( proc_type& proc, uint32_t instr );


    /**
     * SMMLS
     * Signed Most Significant Word Multiply Subtract multiplies two signed
     * 32-bit values, subtracts the result from a 32-bit accumulate value that
     * is shifted left by 32 bits, and extracts the most significant 32 bits of
     * the result of that subtraction. Optionally, you can specify that the
     * result is rounded instead of being truncated. In this case, the constant
     * 0x80000000 is added to the result of the subtraction before the high
     * word is extracted.
     * @brief (A8.6.175, 660)
     */ 
    template< typename proc_type >
    void SMMLS_A1( proc_type& proc, uint32_t instr );


    /**
     * SMMUL
     * Signed Most Significant Word Multiply multiplies two signed 32-bit
     * values, extracts the most significant 32 bits of the result, and writes
     * those bits to the destination register. Optionally, you can specify that
     * the result is rounded instead of being truncated. In this case, the
     * constant 0x80000000 is added to the product before the high word is
     * extracted.
     * @brief (A8.6.176, 662)
     */
    template< typename proc_type >
    void SMMUL_A1( proc_type& proc, uint32_t instr );


    /**
     * SMUAD
     * Signed Dual Multiply Add performs two signed 16 x 16-bit 
     * multiplications. It adds the products together, and writes the result to
     * the destination register. Optionally, you can exchange the halfwords of
     * the second operand before performing the arithmetic. This produces 
     * top x bottom and bottom x top multiplication.
     * @brief (A8.6.177, 664)
     */
    template< typename proc_type >
    void SMUAD_A1( proc_type& proc, uint32_t instr );


    /**
     * SMULBB, SMULBT, SMULTB, SMULTT
     * Signed Multiply (halfwords) multiplies two signed 16-bit quantities,
     * taken from either the bottom or the top half of their respective source
     * registers. The other halves of these source registers are ignored. The
     * 32-bit product is written to the destination register. No overflow is 
     * possible during this instruction.
     * @brief (A8.6.178, 666)
     */
    template< typename proc_type >
    void SMULxy_A1( proc_type& proc, uint32_t instr );


    /**
     * SMULL
     * Signed Multiply Long multiplies two 32-bit signed values to produce a
     * 64-bit result.
     * @brief (A8.6.179, 668)
     */
    template< typename proc_type >
    void SMULL_A1( proc_type& proc, uint32_t instr );


    /**
     * SMULWB, SMULWT
     * Signed Multiply (word by halfword) multiplies a signed 32-bit quantity
     * and a signed 16-bit quantity. The signed 16-bit quantity is taken from 
     * either the bottom or the top half of its source register. The other half
     * of the second source register is ignored. The top 32 bits of the 48-bit
     * product are written to the destination register. The bottom 16 bits of
     * the 48-bit product are ignored. No overflow is possible during this
     * instruction.
     * @brief (A8.6.180, 670)
     */
    template< typename proc_type >
    void SMULWx_A1( proc_type& proc, uint32_t instr );


    /**
     * SMUAD
     * Signed Dual Multiply Subtract performs two signed 16 x 16-bit
     * multiplications. It subtracts one of the products from the other, and
     * writes the result to the destination register. Optionally, you can
     * exchange the halfwords of the second operand before performing the
     * arithmetic. This produces top " bottom and bottom " top multiplication.
     * @brief (A8.6.181, 672)
     */
    template< typename proc_type >
    void SMUSD_A1( proc_type& proc, uint32_t instr );


    /**
     * SSAT
     * Signed Saturate saturates an optionally-shifted signed value to a
     * selectable signed range.
     * @brief (A8.6.183, 674)
     */
    template< typename proc_type >
    void SSAT_A1( proc_type& proc, uint32_t instr );


    /**
     * SSAT16
     * Signed Saturate 16 saturates two signed 16-bit values to a selected
     * signed range.
     * @brief (A8.6.184, 676)
     */
    template< typename proc_type >
    void SSAT16_A1( proc_type& proc, uint32_t instr );


    /**
     * SSAX
     * Signed Subtract and Add with Exchange exchanges the two halfwords of the
     * second operand, performs one 16-bit integer subtraction and one 16-bit
     * addition, and writes the results to the destination register. It sets
     * the APSR.GE bits according to the results.
     * @brief (A8.6.185, 678)
     */
    template< typename proc_type >
    void SSAX_A1( proc_type& proc, uint32_t instr );


    /**
     * SSUB16
     * Signed Subtract 16 performs two 16-bit signed integer subtractions, and
     * writes the results to the destination register. It sets the APSR.GE bits
     * according to the results of the subtractions.
     * @brief (A8.6.186, 680)
     */
    template< typename proc_type >
    void SSUB16_A1( proc_type& proc, uint32_t instr );


    /**
     * SSUB8
     * Signed Subtract 8 performs four 8-bit signed integer subtractions, and
     * writes the results to the destination register. It sets the APSR.GE bits
     * according to the results of the subtractions.
     * @brief (A8.6.187, 682)
     */
    template< typename proc_type >
    void SSUB8_A1( proc_type& proc, uint32_t instr );


    /**
     * STM / STMIA / STMEA
     * Store Multiple Increment After (Store Multiple Empty Ascending) stores
     * multiple registers to consecutive memory locations using an address from
     * a base register. The consecutive memory locations start at this address,
     * and the address just above the last of those locations can optionally be
     * written back to the base register.
     * @brief (A8.6.189, 686)
     */
    template< typename proc_type >
    void STM_STMIA_STMEA_A1( proc_type& proc, uint32_t instr );


    /**
     * STMDA / STMED
     * Store Multiple Decrement After (Store Multiple Empty Descending) stores
     * multiple registers to consecutive memory locations using an address from
     * a base register. The consecutive memory locations end at this address,
     * and the address just below the lowest of those locations can optionally
     * be written back to the base register.
     * @brief (A8.6.190, 688)
     */
    template< typename proc_type >
    void STMDA_STMED_A1( proc_type& proc, uint32_t instr );


    /**
     * STMDB / STMFD
     * Store Multiple Decrement Before (Store Multiple Full Descending) stores
     * multiple registers to consecutive memory locations using an address from
     * a base register. The consecutive memory locations end just below this
     * address, and the address of the first of those locations can optionally
     * be written back to the base register.
     * @brief (A8.6.191, 690)
     */
    template< typename proc_type >
    void STMDB_STMFD_A1( proc_type& proc, uint32_t instr );


    /**
     * STMIB / STMFA
     * Store Multiple Increment Before (Store Multiple Full Ascending) stores
     * multiple registers to consecutive memory locations using an address from
     * a base register. The consecutive memory locations start just above this
     * address, and the address of the last of those locations can optionally
     * be written back to the base register.
     * @brief (A8.6.192, 692)
     */
    template< typename proc_type >
    void STMIB_STMFA_A1( proc_type& proc, uint32_t instr );


    /**
     * STR (immediate, ARM)
     * Store Register (immediate) calculates an address from a base register
     * value and an immediate offset, and stores a word from a register to
     * memory. It can use offset, post-indexed, or pre-indexed addressing. For
     * information about memory accesses see Memory accesses on page A8-13.
     * @brief (A8.6.194, 696)
     */
    template< typename proc_type >
    void STR_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * STR (register)
     * Store Register (register) calculates an address from a base register
     * value and an offset register value, stores a word from a register to
     * memory. The offset register value can optionally be shifted. For
     * information about memory accesses see Memory accesses on page A8-13.
     * @brief (A8.6.195, 698)
     */
    template< typename proc_type >
    void STR_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * STRB (immediate, ARM)
     * Store Register Byte (immediate) calculates an address from a base
     * register value and an immediate offset, and stores a byte from a
     * register to memory. It can use offset, post-indexed, or pre-indexed
     * addressing. For information about memory accesses see Memory accesses
     * on page A8-13.
     * @brief (A8.6.197, 702)
     */
    template< typename proc_type >
    void STRB_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * STRB (register)
     * Store Register Byte (register) calculates an address from a
     * base register value and an offset register value, and stores a
     * byte from a register to memory. The offset register value can
     * optionally be shifted. For information about memory accesses
     * see Memory accesses on page A8-13.
     * @brief (A8.6.198, 704)
     */
    template< typename proc_type >
    void STRB_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * STRBT
     * Store Register Byte Unprivileged and stores a byte from a
     * register to memory. For information about memory accesses see
     * Memory accesses on page A8-13.
     * @brief (A8.6.199, 706)
     */
    template< typename proc_type >
    void STRBT_A1( proc_type& proc, uint32_t instr );


    /**
     * STRBT
     * Store Register Byte Unprivileged and stores a byte from a
     * register to memory. For information about memory accesses see
     * Memory accesses on page A8-13.
     * @brief (A8.6.199, 706)
     */
    template< typename proc_type >
    void STRBT_A2( proc_type& proc, uint32_t instr );


    /**
     * STRD (immediate)
     * Store Register Dual (immediate) calculates an address from a
     * base register value and an immediate offset, and stores two
     * words from two registers to memory. It can use offset, post-
     * indexed, or pre-indexed addressing. For information about
     * memory accesses see Memory accesses on page A8-13.
     * @brief (A8.6.200, 708)
     */
    template< typename proc_type >
    void STRD_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * STRD (register)
     * Store Register Dual (register) calculates an address from a
     * base register value and a register offset, and stores two
     * words from two registers to memory. It can use offset, post-
     * indexed, or pre-indexed addressing. For information about
     * memory accesses see Memory accesses on page A8-13.3.
     * @brief (A8.6.201, 710)
     */
    template< typename proc_type >
    void STRD_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * STRH (immediate, ARM)
     * Store Register Halfword (immediate) calculates an address from
     * a base register value and an immediate offset, and stores a
     * halfword from a register to memory. It can use offset, post-
     * indexed, or pre-indexed addressing. For information about
     * memory accesses see Memory accesses on page A8-13.
     * @brief (A8.6.207, 722)
     */
    template< typename proc_type >
    void STRH_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * STRH (register)
     * Store Register Halfword (register) calculates an address from
     * a base register value and an offset register value, and stores
     * a halfword from a register to memory. The offset register
     * value can be shifted left by 0, 1, 2, or 3 bits. For
     * information about memory accesses see Memory accesses on page
     * A8-13.
     * @brief (A8.6.208, 724)
     */
    template< typename proc_type >
    void STRH_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * STRHT
     * Store Register Halfword Unprivileged and stores a halfword
     * from a register to memory. For information about memory
     * accesses see Memory accesses on page A8-13.
     * @brief (A8.6.209, 726)
     */
    template< typename proc_type >
    void STRHT_A1( proc_type& proc, uint32_t instr );


    /**
     * STRHT
     * Store Register Halfword Unprivileged and stores a halfword
     * from a register to memory. For information about memory
     * accesses see Memory accesses on page A8-13.
     * @brief (A8.6.209, 726)
     */
    template< typename proc_type >
    void STRHT_A2( proc_type& proc, uint32_t instr );


    /**
     * STRT
     * Store Register Unprivileged stores a word from a register to
     * memory. For information about memory accesses see Memory
     * accesses on page A8-13.
     * @brief (A8.6.210, 728)
     */
    template< typename proc_type >
    void STRT_A1( proc_type& proc, uint32_t instr );


    /**
     * STRT
     * Store Register Unprivileged stores a word from a register to
     * memory. For information about memory accesses see Memory
     * accesses on page A8-13.
     * @brief (A8.6.210, 728)
     */
    template< typename proc_type >
    void STRT_A2( proc_type& proc, uint32_t instr );


    /**
     * SUB (immediate, ARM)
     * This instruction subtracts an immediate value from a register
     * value, and writes the result to the destination register. It
     * can optionally update the condition flags based on the result.
     * @brief (A8.6.212, p.732)
     */
    template< typename proc_type >
    void SUB_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * SUB (register)
     * This instruction subtracts an optionally-shifted register value
     * from a register value, and writes the result to the destination
     * register. It can optionally update the condition flags based on
     * the result.
     * @brief (A8.6.213, p.734)
     */
    template< typename proc_type >
    void SUB_reg_A1( proc_type& proc, uint32_t instr );
    

    /**
     * SUB (register-shifted register)
     * This instruction subtracts a register-shifted register value
     * from a register value, and writes the result to the destination
     * register. It can optionally update the condition flags based on
     * the result.
     * @brief (A8.6.214, p.736)
     */
    template< typename proc_type >
    void SUB_sh_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTAB
     * Signed Extend and Add Byte extracts an 8-bit value from a
     * register, sign-extends it to 32 bits, adds the result to the
     * value in another register, and writes the final result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 8-bit value.
     * @brief (A8.6.220, p.746)
     */
    template< typename proc_type >
    void SXTAB_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTAB16
     * Signed Extend and Add Byte 16 extracts two 8-bit values from a
     * register, sign-extends them to 16 bits each, adds the results
     * to two 16-bit values from another register, and writes the
     * final results to the destination register. You can specify a
     * rotation by 0, 8, 16, or 24 bits before extracting the 8-bit
     * values.
     * @brief (A8.6.221, p.748)
     */
    template< typename proc_type >
    void SXTAB16_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTAH
     * Signed Extend and Add Halfword extracts a 16-bit value from a
     * register, sign-extends it to 32 bits, adds the result to a
     * value from another register, and writes the final result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 16-bit value.
     * @brief (A8.6.222, p.750)
     */
    template< typename proc_type >
    void SXTAH_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTB
     * Signed Extend Byte extracts an 8-bit value from a register,
     * sign-extends it to 32 bits, and writes the result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 8-bit value.
     * @brief (A8.6.223, p.752)
     */
    template< typename proc_type >
    void SXTB_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTB16
     * Signed Extend Byte 16 extracts two 8-bit values from a
     * register, sign-extends them to 16 bits each, and writes the
     * results to the destination register. You can specify a rotation
     * by 0, 8, 16, or 24 bits before extracting the 8-bit values.
     * @brief (A8.6.224, p.754)
     */
    template< typename proc_type >
    void SXTB16_A1( proc_type& proc, uint32_t instr );


    /**
     * SXTH
     * Signed Extend Halfword extracts a 16-bit value from a register,
     * sign-extends it to 32 bits, and writes the result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 16-bit value.
     * @brief (A8.6.225, p.756)
     */
    template< typename proc_type >
    void SXTH_A1( proc_type& proc, uint32_t instr );


    /**
     * TEQ (immediate)
     * Test Equivalence (immediate) performs a bitwise exclusive OR
     * operation on a register value and an immediate value. It
     * updates the condition flags based on the result, and discards
     * the result.
     * @brief (A8.6.227, p.760)
     */
    template< typename proc_type >
    void TEQ_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * TEQ (register)
     * Test Equivalence (register) performs a bitwise exclusive OR
     * operation on a register value and an optionally-shifted
     * register value. It updates the condition flags based on the
     * result, and discards the result.
     * @brief (A8.6.228, p.762)
     */
    template< typename proc_type >
    void TEQ_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * TEQ (register-shifted register)
     * Test Equivalence (register-shifted register) performs a bitwise
     * exclusive OR operation on a register value and a
     * register-shifted register value. It updates the condition flags
     * based on the result, and discards the result.
     * @brief (A8.6.229, p.764)
     */
    template< typename proc_type >
    void TEQ_sh_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * TST (immediate)
     * Test (immediate) performs a bitwise AND operation on a register
     * value and an immediate value. It updates the condition flags
     * based on the result, and discards the result.
     * @brief (A8.6.230, p.766)
     */
    template< typename proc_type >
    void TST_imm_A1( proc_type& proc, uint32_t instr );


    /**
     * TST (register)
     * Test (register) performs a bitwise AND operation on a register
     * value and an optionally-shifted register value.  It updates the
     * condition flags based on the result, and discards the result.
     * @brief (A8.6.231, p.768)
     */
    template< typename proc_type >
    void TST_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * TST (register-shifted register)
     * Test (register-shifted register) performs a bitwise AND
     * operation on a register value and a register-shifted register
     * value. It updates the condition flags based on the result, and
     * discards the result.
     * @brief (A8.6.232, p.770)
     */
    template< typename proc_type >
    void TST_sh_reg_A1( proc_type& proc, uint32_t instr );


    /**
     * UADD16
     * Unsigned Add 16 performs two 16-bit unsigned integer additions,
     * and writes the results to the destination register. It sets the
     * APSR.GE bits according to the results of the additions.
     * @brief (A8.6.233, p.772)
     */
    template< typename proc_type >
    void UADD16_A1( proc_type& proc, uint32_t instr );


    /**
     * UADD8
     * Unsigned Add 8 performs four unsigned 8-bit integer additions,
     * and writes the results to the destination register. It sets the
     * APSR.GE bits according to the results of the additions.
     * @brief (A8.6.234, p.774)
     */
    template< typename proc_type >
    void UADD8_A1( proc_type& proc, uint32_t instr );


    /**
     * UASX
     * Unsigned Add and Subtract with Exchange exchanges the two
     * halfwords of the second operand, performs one unsigned 16-bit
     * integer addition and one unsigned 16-bit subtraction, and
     * writes the results to the destination register. It sets the
     * APSR.GE bits according to the results.
     * @brief (A8.6.235, p.776)
     */
    template< typename proc_type >
    void UASX_A1( proc_type& proc, uint32_t instr );


    /**
     * UBFX
     * Unsigned Bit Field Extract extracts any number of adjacent bits
     * at any position from a register, zero-extends them to 32 bits,
     * and writes the result to the destination register.
     * @brief (A8.6.236, p.778)
     */
    template< typename proc_type >
    void UBFX_A1( proc_type& proc, uint32_t instr );


    /**
     * UHADD16
     * Unsigned Halving Add 16 performs two unsigned 16-bit integer
     * additions, halves the results, and writes the results to the
     * destination register.
     * @brief (A8.6.238, p.782)
     */
    template< typename proc_type >
    void UHADD16_A1( proc_type& proc, uint32_t instr );


    /**
     * UHADD8
     * Unsigned Halving Add 8 performs four unsigned 8-bit integer
     * additions, halves the results, and writes the results to the
     * destination register.
     * @brief (A8.6.239, p.784)
     */
    template< typename proc_type >
    void UHADD8_A1( proc_type& proc, uint32_t instr );


    /**
     * UHASX
     * Unsigned Halving Add and Subtract with Exchange exchanges the
     * two halfwords of the second operand, performs one unsigned
     * 16-bit integer addition and one unsigned 16-bit subtraction,
     * halves the results, and writes the results to the destination
     * register.
     * @brief (A8.6.240, p.786)
     */
    template< typename proc_type >
    void UHASX_A1( proc_type& proc, uint32_t instr );


    /**
     * UHSAX
     * Unsigned Halving Subtract and Add with Exchange exchanges the
     * two halfwords of the second operand, performs one unsigned
     * 16-bit integer subtraction and one unsigned 16-bit addition,
     * halves the results, and writes the results to the destination
     * register.
     * @brief (A8.6.241, p.788)
     */
    template< typename proc_type >
    void UHSAX_A1( proc_type& proc, uint32_t instr );


    /**
     * UHSUB16
     * Unsigned Halving Subtract 16 performs two unsigned 16-bit
     * integer subtractions, halves the results, and writes the
     * results to the destination register.
     * @brief (A8.6.242, p.790)
     */
    template< typename proc_type >
    void UHSUB16_A1( proc_type& proc, uint32_t instr );


    /**
     * UHSUB8
     * Unsigned Halving Subtract 8 performs four unsigned 8-bit
     * integer subtractions, halves the results, and writes the
     * results to the destination register.
     * @brief (A8.6.243, p.792)
     */
    template< typename proc_type >
    void UHSUB8_A1( proc_type& proc, uint32_t instr );


    /**
     * UMAAL
     * Unsigned Multiply Accumulate Accumulate Long multiplies two
     * unsigned 32-bit values to produce a 64-bit value, adds two
     * unsigned 32-bit values, and writes the 64-bit result to two
     * registers.
     * @brief (A8.6.244, p.794)
     */
    template< typename proc_type >
    void UMAAL_A1( proc_type& proc, uint32_t instr );


    /**
     * UMLAL
     * Unsigned Multiply Accumulate Long multiplies two unsigned32-bit
     * values to produce a 64-bit value, and accumulates this with a
     * 64-bit value.
     * @brief (A8.6.245, p.796)
     */
    template< typename proc_type >
    void UMLAL_A1( proc_type& proc, uint32_t instr );


    /**
     * UMULL
     * Unsigned Multiply Long multiplies two 32-bit unsigned values to
     * produce a 64-bit result.
     * @brief (A8.6.246, p.798)
     */
    template< typename proc_type >
    void UMULL_A1( proc_type& proc, uint32_t instr );


    /**
     * UQADD16
     * Unsigned Saturating Add 16 performs two unsigned 16-bit integer
     * additions, saturates the results to the 16-bit unsigned integer
     * range 0 <= x <= 2^16 - 1, and writes the results to the
     * destination register.
     * @brief (A8.6.247, p.800)
     */
    template< typename proc_type >
    void UQADD16_A1( proc_type& proc, uint32_t instr );


    /**
     * UQADD8
     * Unsigned Saturating Add 8 performs four unsigned 8-bit integer
     * additions, saturates the results to the 8-bit unsigned integer
     * range 0 <= x <= 2^8 - 1, and writes the results to the
     * destination register.
     * @brief (A8.6.248, p.802)
     */
    template< typename proc_type >
    void UQADD8_A1( proc_type& proc, uint32_t instr );


    /**
     * UQASX
     * Unsigned Saturating Add and Subtract with Exchange exchanges
     * the two halfwords of the second operand, performs one unsigned
     * 16-bit integer addition and one unsigned 16-bit subtraction,
     * saturates the results to the 16-bit unsigned integer range 0 <=
     * x <= 2^16 - 1, and writes the results to the destination
     * register.
     * @brief (A8.6.249, p.804)
     */
    template< typename proc_type >
    void UQASX_A1( proc_type& proc, uint32_t instr );


    /**
     * UQSAX
     * Unsigned Saturating Subtract and Add with Exchange exchanges
     * the two halfwords of the second operand, performs one unsigned
     * 16-bit integer subtraction and one unsigned 16-bit addition,
     * saturates the results to the 16-bit unsigned integer range 0 <=
     * x <= 2^16 - 1, and writes the results to the destination
     * register.
     * @brief (A8.6.250, p.806)
     */
    template< typename proc_type >
    void UQSAX_A1( proc_type& proc, uint32_t instr );


    /**
     * UQSUB16
     * Unsigned Saturating Subtract 16 performs two unsigned 16-bit
     * integer subtractions, saturates the results to the 16-bit
     * unsigned integer range 0 <= x <= 2^16 - 1, and writes the
     * results to the destination register.
     * @brief (A8.6.251, p.808)
     */
    template< typename proc_type >
    void UQSUB16_A1( proc_type& proc, uint32_t instr );


    /**
     * UQSUB8
     * Unsigned Saturating Subtract 8 performs four unsigned 8-bit
     * integer subtractions, saturates the results to the 8-bit
     * unsigned integer range 0 <= x <= 28 - 1, and writes the results
     * to the destination register.
     * @brief (A8.6.252, p.810)
     */
    template< typename proc_type >
    void UQSUB8_A1( proc_type& proc, uint32_t instr );


    /**
     * USAD8
     * Unsigned Sum of Absolute Differences performs four unsigned
     * 8-bit subtractions, and adds the absolute values of the
     * differences together.
     * @brief (A8.6.253, p.812)
     */
    template< typename proc_type >
    void USAD8_A1( proc_type& proc, uint32_t instr );


    /**
     * USADA8
     * Unsigned Sum of Absolute Differences and Accumulate performs
     * four unsigned 8-bit subtractions, and adds the absolute values
     * of the differences to a 32-bit accumulate operand.
     * @brief (A8.6.254, p.814)
     */
    template< typename proc_type >
    void USADA8_A1( proc_type& proc, uint32_t instr );


    /**
     * USAT
     * Unsigned Saturate saturates an optionally-shifted signed value
     * to a selected unsigned range.
     * @brief (A8.6.255, p.816)
     */
    template< typename proc_type >
    void USAT_A1( proc_type& proc, uint32_t instr );


    /**
     * USAT16
     * Unsigned Saturate 16 saturates two signed 16-bit values to a
     * selected unsigned range.
     * @brief (A8.6.256, p.818)
     */
    template< typename proc_type >
    void USAT16_A1( proc_type& proc, uint32_t instr );


    /**
     * USAX
     * Unsigned Subtract and Add with Exchange exchanges the two
     * halfwords of the second operand, performs one unsigned 16-bit
     * integer subtraction and one unsigned 16-bit addition, and
     * writes the results to the destination register. It sets the
     * APSR.GE bits according to the results.
     * @brief (A8.6.257, p.820)
     */
    template< typename proc_type >
    void USAX_A1( proc_type& proc, uint32_t instr );


    /**
     * USUB16
     * Unsigned Subtract 16 performs two 16-bit unsigned integer
     * subtractions, and writes the results to the destination
     * register. It sets the APSR.GE bits according to the results of
     * the subtractions.
     * @brief (A8.6.258, p.822)
     */
    template< typename proc_type >
    void USUB16_A1( proc_type& proc, uint32_t instr );


    /**
     * USUB8
     * Unsigned Subtract 8 performs four 8-bit unsigned integer
     * subtractions, and writes the results to the destination
     * register. It sets the APSR.GE bits according to the results of
     * the subtractions.
     * @brief (A8.6.259, p.824)
     */
    template< typename proc_type >
    void USUB8_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTAB
     * Unsigned Extend and Add Byte extracts an 8-bit value from a
     * register, zero-extends it to 32 bits, adds the result to the
     * value in another register, and writes the final result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 8-bit value.
     * @brief (A8.6.260, p.826)
     */
    template< typename proc_type >
    void UXTAB_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTAB16
     * Unsigned Extend and Add Byte 16 extracts two 8-bit values from
     * a register, zero-extends them to 16 bits each, adds the results
     * to two 16-bit values from another register, and writes the
     * final results to the destination register. You can specify a
     * rotation by 0, 8, 16, or 24 bits before extracting the 8-bit
     * values.
     * @brief (A8.6.261, p.828)
     */
    template< typename proc_type >
    void UXTAB16_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTAH
     * Unsigned Extend and Add Halfword extracts a 16-bit value from a
     * register, zero-extends it to 32 bits, adds the result to a
     * value from another register, and writes the final result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 16-bit value.
     * @brief (A8.6.262, p.830)
     */
    template< typename proc_type >
    void UXTAH_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTB
     * Unsigned Extend Byte extracts an 8-bit value from a register,
     * zero-extends it to 32 bits, and writes the result to the
     * destination register. You can specify a rotation by 0, 8, 16,
     * or 24 bits before extracting the 8-bit value.
     * @brief (A8.6.263, p.832)
     */
    template< typename proc_type >
    void UXTB_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTB16
     * Unsigned Extend Byte 16 extracts two 8-bit values from a
     * register, zero-extends them to 16 bits each, and writes the
     * results to the destination register. You can specify a rotation
     * by 0, 8, 16, or 24 bits before extracting the 8-bit values.
     * @brief (A8.6.264, p.834)
     */
    template< typename proc_type >
    void UXTB16_A1( proc_type& proc, uint32_t instr );


    /**
     * UXTH
     * Unsigned Extend Halfword extracts a 16-bit value from a
     * register, zero-extends it to 32 bits, and writes the result to
     * the destination register. You can specify a rotation by 0, 8,
     * 16, or 24 bits before extracting the 16-bit value.
     * @brief (A8.6.265, p.836)
     */
    template< typename proc_type >
    void UXTH_A1( proc_type& proc, uint32_t instr );


} // namespace arm

#endif // __ARMV7_INSTRUCTION_HPP__
