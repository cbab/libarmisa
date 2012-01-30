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
 * This file defines structures that correspond to the processor's
 * internals.
 */


#ifndef __ARMV7_PROCESSOR_HPP__
#define __ARMV7_PROCESSOR_HPP__

#include <boost/cstdint.hpp>

namespace arm {

    /**
     * Current program status register.
     * (B1.3.3, p.1166)
     */    
    template< typename field_type >
    struct cpsr_adaptor
    {
        field_type N;    /// bit [31] Negative condition code flag
        field_type Z;    /// bit [30] Zero condition code flag
        field_type C;    /// bit [29] Carry condition code flag
        field_type V;    /// bit [28] Overflow condition code flag.
        field_type Q;    /// bit [27] Cumulative saturation flag
        field_type IT_L; /// bits [26:25] Thumb if-Then execution state bits
        field_type J;    /// bit [24] Jazelle bit
        field_type reserved;
        field_type GE;   /// bits [19:16] SIMD greater than or Equal flags
        field_type IT_H; /// bits [15:10] Thumb if-Then execution state bits
        field_type E;    /// bit [9] Endianness execution state bit
        field_type A;    /// bit [8] Asynchronous abort disable bit
        field_type I;    /// bit [7] Interrupt disable bit
        field_type F;    /// bit [6] Fast interrupt disable bit
        field_type T;    /// bit [5] Thumb execution state bit
        field_type M;    /// bits [4:0] Mode field
    };

    /**
     * Virtual core structure that contains the registers manipulated
     * by the ARMv7 instruction set.
     */
    template< typename cpsr_type,
              typename reg_type,
              typename bank_type,
              typename mem_type >
    struct armv7_core
    {
        // In ARMv7-A and ARMv7-R, the APSR is the same register
        // as the CPSR, but the APSR must be used only to access
        // the N, Z, C, V, Q, and GE[3:0] bits.

        cpsr_type CPSR; /// Current program status register
        reg_type  PC;   /// Program counter
        bank_type R;    /// Register bank
        mem_type  iMem; /// Instruction memory
        mem_type  dMem; /// Data memory
    };

} // namespace arm

#endif // __ARMV7_PROCESSOR_HPP__
