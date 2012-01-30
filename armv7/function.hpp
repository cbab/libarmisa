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
 * The ARM Architecture Reference Manual (ARM v7-A and ARM v7-R
 * edition) defines behavior using pseudocode that calls utility
 * pseudofunctions. The functions defined here implements these
 * pseudofunctions. All section and page numbers refer to that manual
 * unless otherwise noted.
 */

#ifndef __ARMV7_FUNCTION_HPP__
#define __ARMV7_FUNCTION_HPP__

#include "types.hpp"
#include <boost/cstdint.hpp>

#define SET_BIT(val, bitIndex) val |= (1 << (bitIndex))
#define CLEAR_BIT(val, bitIndex) val &= ~(1 << (bitIndex))
#define TOGGLE_BIT(val, bitIndex) val ^= (1 << (bitIndex))
#define BIT_IS_SET(val, bitIndex) (val & (1 << (bitIndex)))

namespace arm {


    /*
     * Libarmisa-specific helper functions.
     */


    /**
     * Extracts bits b1:b0 of a bit string.
     * @param s  original bit string
     * @param b1 leftmost bit
     * @param b0 rightmost bit
     * @return   a new bit string with bits b1:b0 at the end
     */
    uint32_t Bits( uint32_t s, uint32_t b1, uint32_t b0 );

    /**
     * Extracts bits b1:b0 of a 64-bit bit string.
     * @param s  original bit string
     * @param b1 leftmost bit
     * @param b0 rightmost bit
     * @return   a new bit string with bits b1:b0 at the end
     */
    uint64_t Bits64( uint64_t s, uint64_t b1, uint64_t b0 );



    /*
     * Implementation of pseudo-functions defined by the ARM
     * Architecture Reference Manual.
     */


    /**
     * Expands an immediate 12-bit constant.
     * (A5.2.4, p.216)
     */
    template< typename proc_type >
    uint32_t ARMExpandImm( proc_type& proc, uint32_t imm12 );


    /**
     * Expands an immediate 12-bit.
     * (A5.2.4, p.216)
     */
    UValueCarry ARMExpandImm_C( uint32_t imm12, bool carry_in );


    /**
     * Decodes shift type and amount for an immediate shift.
     * (A8.4.3, p.323)
     */
    ShiftUValue DecodeImmShift( uint32_t type, uint32_t imm5 );


    /**
     * Decode shift type for a register-controlled shift.
     * (A8.4.3, p.324)
     */

    SRType DecodeRegShift( uint32_t type );


    /**
     * Returns true if security extension is implemented
     */
    bool HaveSecurityExt();


    /**
     * Returns true if MP extension is implemented
     */
    bool HaveMPExt();


    /**
     * Returns the instruction's condition specifier
     * (A8.3.1, p.321)
     */
    int CurrentCond( uint32_t instr );


    /**
     * (A8.3.1, p.321)
     */
    template< typename proc_type >
    bool ConditionPassed( proc_type& proc, uint32_t instr );


    /**
     * (A2.2.1, p.40)
     */
    template< typename value_type, typename flag_type >
    value_type AddWithCarry( value_type x, value_type y, flag_type carry_in,
                             flag_type& carry_out, flag_type& overflow );


    /**
     * (A2.5.1, p.48)
     */
    template< typename proc_type >
    InstrSet CurrentInstrSet( proc_type& proc );

    /**
     * (A2.5.1, p.48)
     */
    template< typename proc_type >
    void SelectInstrSet( proc_type& proc, InstrSet iset );

    /**
     * Counts the number of "1" bits in a bitstring
     * (I.5.3, p.2088)
     * Never directly cast a parameter to unsigned int, use reinterpret_cast
     * to preserve the binary representation of signed numbers.
     */
    int BitCount( uint64_t bitstring );


    /**
     * Checks if a bitstring is all zero
     * (I.5.3, p.2088)
     */
    bool IsZeroBit( uint64_t bitstring );


    /**
     * (I.5.3, p.2089)
     */
    bool IsZero( uint64_t x );


    /**
     * Left logic shift, with carry output
     */
    UValueCarry LSL_C( uint32_t value, int amount );


    /**
     * Right logic shift, with carry output
     */
    UValueCarry LSR_C( uint32_t value, int amount );


    /**
     * Right arithmetic shift, with carry output
     */
    UValueCarry ASR_C( uint32_t value, int amount );


    /**
     * Rotate right of a bitstring, with carry output
     */
    UValueCarry ROR_C( uint32_t value, int amount );


    /**
     * Rotate right with extend of a bitstring, with carry output
     */
    UValueCarry RRX_C( uint32_t value, bool carry_in );


    /**
     * Shift a value, following SRType method, with carry output
     * (I.5.3, p.2088)
     */
    UValueCarry Shift_C( uint32_t value, SRType type,
                         int amount, bool carry_in );


    /**
     * Shift a value, following the SRType method
     * (I.5.3, p.2088)
     */
    uint32_t Shift( uint32_t value, SRType type, int amount, bool carry_in );


    /**
     * Left arithmetic shift
     * (A2.2.1, p.37)
     */
    uint32_t LSL( uint32_t value, int amount );


    /**
     * Right logic shift
     * (A2.2.1, p.38)
     */
    uint32_t LSR( uint32_t value, int amount );


    /**
     * Right arithmetic shift
     * (A2.2.1, p.38)
     */
    uint32_t ASR( uint32_t value, int amount );


    /**
     * Rotate right of a bitstring
     * (A2.2.1, p.39)
     */
    uint32_t ROR( uint32_t value, int amount );


    /**
     * Rotate right with extend of a bitstring
     * (A2.2.1, p.39)
     */
    uint32_t RRX( uint32_t value, bool carry_in );


    /**
     * Write to program counter
     * (A2.3.1, p.45)
     */
    template< typename proc_type >
    void ALUWritePC( proc_type& proc, uint32_t address );

    /**
     * Write value to PC, with interworking (without it before ARMv5T) 
     * (A2.3.1, p.45)
     */
    template< typename proc_type >
    void LoadWritePC( proc_type& proc, uint32_t address);

    /**
     * Write value to PC, with interworking
     * (A2.3.1, p.45)
     */
    template< typename proc_type >
    void BXWritePC( proc_type& proc, uint32_t address );

    /**
     * Write value to PC, without interworking 
     * (A2.3.1, p.44)
     */
    template< typename proc_type >
    void BranchWritePC( proc_type& proc, uint32_t address );

    /**
     * Continue execution at specified address 
     * (B1.3.3, p.1164)
     */
    template< typename proc_type >
    void BranchTo( proc_type& proc, uint32_t address );


    /**
     * Align value to "align". This implementation might not be exact
     * since the description is very vague.
     * (I.5.4, p.2092)
     */
    uint32_t Align( uint32_t value, uint32_t align );


    /**
     * Signed saturation of width "N", with saturation indicator
     * (A2.2.1, p.41)
     * This implementation assumes N < 64
     */
    ValueSat SignedSatQ( int64_t i, unsigned int N );


    /**
     * Unsigned saturation of width "N", with saturation indicator
     * (A2.2.1, p.41)
     * This implementation assumes N < 64
     */
    UValueSat UnsignedSatQ( int64_t i, unsigned int N );


    /**
     * Signed saturation of width "N"
     * (A2.2.1, p.41)
     * This implementation assumes N < 64
     * @return The saturated value as a 64-bit signed integer.
     */
    int64_t SignedSat( int64_t i, unsigned int N );


    /**
     * Unsigned saturation of width "N"
     * (A2.2.1, p.41)
     * This implementation assumes N < 64
     */
    uint64_t UnsignedSat( int64_t i, unsigned int N );


    /**
     * (I.5.3, p.2090)
     */ 
    uint8_t  UInt( int8_t   x );
    uint8_t  UInt( uint8_t  x );
    uint16_t UInt( int16_t  x );
    uint16_t UInt( uint16_t x );
    uint32_t UInt( int32_t  x );
    uint32_t UInt( uint32_t x );
    uint64_t UInt( int64_t  x );
    uint64_t UInt( uint64_t x );


    /**
     * (I.5.3, p.2090)
     */
    int8_t  SInt( int8_t   x );
    int8_t  SInt( uint8_t  x );
    int16_t SInt( int16_t  x );
    int16_t SInt( uint16_t x );
    int32_t SInt( int32_t  x );
    int32_t SInt( uint32_t x );
    int64_t SInt( int64_t  x );
    int64_t SInt( uint64_t x );


    /**
     * If x is an integer or real, Abs(x) is the absolute value of x.
     * (I5.4, p.2091)
     */
    template< typename value_type >
    value_type Abs( value_type value );


    /**
     * (A2.3.1, p.44)
     */
    template< typename proc_type >
    int PCStoreValue( proc_type& proc );


    /**
     * Position of rightmost 1 in a bitstring
     * (I.5.3, p.2089)
     */
    int LowestSetBit( unsigned int x );

    
    /**
     * NB: If this function returns false, the instruction must be
     * interrupted
     * (A9.1.2, p.1129)
     */
    template< typename proc_type >
    bool NullCheckIfThumbEE( proc_type& proc, int n );


    /**
     * This function returns the major version number of the
     * architecture.
     * (I.7.1, p.2098)
     */
    int ArchVersion();
    

    /**
     * (I.7.28, p.2102)
     */
    MemArch MemorySystemArchitecture();
    

    /**
     * If x is a bitstring and i is an integer, then ZeroExtend(x,i)
     * is x extended to a length of i bits, by adding sufficient zero
     * bits to its left.
     * (I.5.3, p.2089)
     */
    uint8_t  ZeroExtend( uint8_t  bitstring );
    uint8_t  ZeroExtend( int8_t   bitstring );
    uint16_t ZeroExtend( uint16_t bitstring );
    uint16_t ZeroExtend( int16_t  bitstring );
    uint32_t ZeroExtend( uint32_t bitstring );
    uint32_t ZeroExtend( int32_t  bitstring );
    uint64_t ZeroExtend( uint64_t bitstring );
    uint64_t ZeroExtend( int64_t  bitstring );
    

    /**
     * If x is a bitstring and i is an integer, then SignExtend(x,i) is x
     * extended to a length of i bits, by adding sufficient copies of its
     * leftmost bit to its left.
     * oldSize is used to know the size of bitstring.
     * There is a limitation of 64 bits.
     * (I.5.3, p.2090)
     */
    uint64_t SignExtend( uint64_t bitstring, int newSize, int oldSize );

    /**
     * This function returns TRUE if the processor currently provides support
     * for unaligned memory accesses, or FALSE otherwise. This is always TRUE
     * in ARMv7.
     * (I.7.33, p.2102)
     */
    bool UnalignedSupport();

    /**
     * This function tests whether big-endian memory accesses are currently
     * selected.
     * (A2.5.3 p.51)
     */
    template< typename proc_type >
    bool BigEndian( proc_type& proc );

    /**
     * Bitwise inversion of a bitstring 
     * (I.5.3 p.2089)
     */
    template< typename T >
    T NOT( T val );

    /**
     * Number of zeros at left end of bitstring 
     * (I.5.3 p.2089)
     */
    uint32_t CountLeadingZeroBits( uint32_t x );

    /**
     * The BadMode() function tests whether a 5-bit mode number corresponds
     * to one of the permitted modes.
     * (B1.3 p.1158)
     */
    template< typename T >
    bool BadMode( T val );

    /**
     * (B1.3 p.1158)
     */
    template< typename proc_type >
    bool CurrentModeIsPrivileged( proc_type& proc );

    /**
     * The following function returns TRUE if the Security Extensions are not
     * implemented or the processor is in Secure state, and FALSE otherwise.
     * (B1.5.2 p.1178)
     */
    template< typename proc_type >
    bool IsSecure( proc_type& proc );

    /**
     * (B1.3.3 p.1171)
     */
    template< typename proc_type, typename mask_type, typename value_type >
    void CPSRWriteByInstr( value_type value, mask_type bytemask,
                                bool affect_execstate, proc_type& proc );

} // namespace arm


#endif // __ARMV7_FUNCTION_HPP__
