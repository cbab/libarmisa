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
 * Unit tests for ARMv7 utility functions.
 */

#ifndef __ARMV7_FUNCTION_TEST_HPP__
#define __ARMV7_FUNCTION_TEST_HPP__

#include "armv7_test_proc.hpp"

#include <armv7/isa.hpp>
#include <armv7/types.hpp>
#include <boost/test/unit_test.hpp>
#include <cstring>


BOOST_AUTO_TEST_CASE( Mem_test )
{
    // Test of the test memory structure. Accesses are done with host
    // endianness.

    test_mem<256> mem;
    mem.write_dword( 0x00, 0x0123456789ABCDEFll );
    BOOST_CHECK_EQUAL( mem.read_dword( 0x00 ), 0x0123456789ABCDEFLL );

    mem.write_word ( 0x08, 0xDEADBEEF );
    BOOST_CHECK_EQUAL( mem.read_word ( 0x08 ), 0xDEADBEEF );

    mem.write_half ( 0x0C, 0xCAFE );
    BOOST_CHECK_EQUAL( mem.read_half ( 0x0C ), 0xCAFE );

    mem.write_byte ( 0xFD, 0x42 );
    BOOST_CHECK_EQUAL( mem.read_byte ( 0xFD ), 0x42 );
}

BOOST_AUTO_TEST_CASE( Bits_test )
{
    const uint32_t s = 0x42C0FFEE;
    BOOST_CHECK_EQUAL( arm::Bits( s, 31, 24 ), 0x00000042 );
    BOOST_CHECK_EQUAL( arm::Bits( s, 23, 0  ), 0x00C0FFEE );
}

BOOST_AUTO_TEST_CASE( ARMExpandImm_C_test )
{
    arm::UValueCarry result;

    result = arm::ARMExpandImm_C( 0xDEADD0AB, false );
    BOOST_CHECK_EQUAL( result.value, 0x000000AB );
    BOOST_CHECK_EQUAL( result.carry, false );

    result = arm::ARMExpandImm_C( 0xDEADD2AB, false );
    BOOST_CHECK_EQUAL( result.value, 0xB000000A );
    BOOST_CHECK_EQUAL( result.carry, true );

    result = arm::ARMExpandImm_C( 0xDEADD2C0, false );
    BOOST_CHECK_EQUAL( result.value, 0x0000000C );
    BOOST_CHECK_EQUAL( result.carry, false );
}

BOOST_AUTO_TEST_CASE( DecodeImmShift_test )
{
    arm::ShiftUValue result;

    // type == {0..3}, imm5 != 0
    result = arm::DecodeImmShift( 0x00000000, 0x0000000F );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_LSL );
    BOOST_CHECK_EQUAL( result.shift_n, 0x0000000F );

    result = arm::DecodeImmShift( 0x00000001, 0x00000010 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_LSR );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000010 );

    result = arm::DecodeImmShift( 0x00000002, 0x00000011 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_ASR );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000011 );

    result = arm::DecodeImmShift( 0x00000003, 0x0000001F );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_ROR );
    BOOST_CHECK_EQUAL( result.shift_n, 0x0000001F );

    // type == {0..3}, imm5 0= 0
    result = arm::DecodeImmShift( 0x00000000, 0x00000000 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_LSL );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000000 );

    result = arm::DecodeImmShift( 0x00000001, 0x00000000 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_LSR );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000020 );

    result = arm::DecodeImmShift( 0x00000002, 0x00000000 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_ASR );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000020 );

    result = arm::DecodeImmShift( 0x00000003, 0x00000000 );
    BOOST_CHECK_EQUAL( result.shift_t, arm::SRType_RRX );
    BOOST_CHECK_EQUAL( result.shift_n, 0x00000001 );
}

BOOST_AUTO_TEST_CASE( DecodeRegShift_test )
{
    BOOST_CHECK_EQUAL( arm::DecodeRegShift( 0x00000000 ), arm::SRType_LSL );
    BOOST_CHECK_EQUAL( arm::DecodeRegShift( 0x00000001 ), arm::SRType_LSR );
    BOOST_CHECK_EQUAL( arm::DecodeRegShift( 0x00000002 ), arm::SRType_ASR );
    BOOST_CHECK_EQUAL( arm::DecodeRegShift( 0x00000003 ), arm::SRType_ROR );
}

BOOST_AUTO_TEST_CASE( IsZeroBit_test )
{
    BOOST_CHECK( arm::IsZeroBit( 0 ) );
    BOOST_CHECK( !arm::IsZeroBit( 0x42L ) );
}

BOOST_AUTO_TEST_CASE( ROR_test )
{
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 0  ), 0x000000AC );
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 4  ), 0xC000000A );
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 8  ), 0xAC000000 );
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 16 ), 0x00AC0000 );
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 24 ), 0x0000AC00 );
    BOOST_CHECK_EQUAL( arm::ROR( 0x000000AC, 32 ), 0x000000AC );
}

BOOST_AUTO_TEST_CASE( Align_test )
{
    const uint32_t base = 51535493;
    // Test align to 2
    BOOST_CHECK( arm::Align( base, 2 ) % 2 == 0 );

    // Test align to 4
    BOOST_CHECK( arm::Align( base, 4 ) % 4 == 0 );

    // Test align to 32
    BOOST_CHECK( arm::Align( base, 32 ) % 32 == 0 );
}

BOOST_AUTO_TEST_CASE( BitCount_test )
{
    BOOST_CHECK_EQUAL( arm::BitCount( 0xF0F0CAD335B28E7ALL ), 34 );
}

// Testing SignedSatQ implicitly tests SignedSat
BOOST_AUTO_TEST_CASE( SignedSatQ_test )
{
    // Compute 32-bit signed bounds
    int64_t positive_32 = (int64_t)( pow( 2, 31 ) ) - 1;
    int64_t negative_32 = -positive_32 - 1;

    // Compute 64-bit signed bounds
    int64_t positive_64 = (int64_t)( pow( 2, 63 ) ) - 1;
    int64_t negative_64 = -positive_64 - 1;

    arm::ValueSat res( arm::SignedSatQ( positive_64, 32 ) );
    int64_t intResult = arm::SignedSat( positive_64, 32 );
    BOOST_CHECK_EQUAL( res.value, positive_32 );
    BOOST_CHECK_EQUAL( intResult, positive_32 );
    BOOST_CHECK( res.saturated );

    res = arm::SignedSatQ( negative_64, 32 );
    intResult = arm::SignedSat( negative_64, 32 );
    BOOST_CHECK_EQUAL( res.value, negative_32 );
    BOOST_CHECK_EQUAL( intResult, negative_32 );
    BOOST_CHECK( res.saturated );

    res = arm::SignedSatQ( negative_32, 55 );
    intResult = arm::SignedSat( negative_32, 55 );
    BOOST_CHECK_EQUAL( res.value, negative_32 );
    BOOST_CHECK_EQUAL( intResult, negative_32 );
    BOOST_CHECK( !res.saturated );

    res = arm::SignedSatQ( positive_32, 42 );
    intResult = arm::SignedSat( positive_32, 55 );
    BOOST_CHECK_EQUAL( res.value, positive_32 );
    BOOST_CHECK_EQUAL( intResult, positive_32 );
    BOOST_CHECK( !res.saturated );
}

// Testing UnsignedSatQ implicitly tests UnsignedSat
BOOST_AUTO_TEST_CASE( UnsignedSatQ_test )
{
    // Define expected bounds
    const int64_t upper_32 = 0x00000000FFFFFFFFLL;
    const int64_t upper_63 = 0x7FFFFFFFFFFFFFFFLL;
    const int64_t lower    = 0x0000000000000000LL;

    arm::UValueSat res;

    res = ( arm::UnsignedSatQ( upper_63, 32 ) );
    BOOST_CHECK_EQUAL( res.value, (uint64_t)upper_32 );
    BOOST_CHECK( res.saturated );

    res = arm::UnsignedSatQ( 0xF000000000000000LL, 32 );
    BOOST_CHECK_EQUAL( res.value, (uint64_t)lower );
    BOOST_CHECK( res.saturated );

    res = arm::UnsignedSatQ( upper_32, 33 );
    BOOST_CHECK_EQUAL( res.value, (uint64_t)upper_32 );
    BOOST_CHECK( !res.saturated );
}

BOOST_AUTO_TEST_CASE( LowestSetBit_test )
{
    BOOST_CHECK_EQUAL( arm::LowestSetBit( 0 ), 32 );
    BOOST_CHECK_EQUAL( arm::LowestSetBit( 0x00000422 ), 1 );
    BOOST_CHECK_EQUAL( arm::LowestSetBit( 0x100420 ),  5 );
    BOOST_CHECK_EQUAL( arm::LowestSetBit( 0x80000000 ), 31 );
}

BOOST_AUTO_TEST_CASE( ArchVersion_test )
{
    BOOST_CHECK_EQUAL( arm::ArchVersion(), 7 );
}

BOOST_AUTO_TEST_CASE( IsZero_test )
{
    BOOST_CHECK( !arm::IsZero( 0x24 ) );
    BOOST_CHECK( arm::IsZero( 0 ) );
}

BOOST_AUTO_TEST_CASE( Shift_LSL )
{
    // Logical Shift Left
    const uint32_t LSL_original = 0xB450DEAD;
    const uint32_t LSL_shifted = 0x450DEAD0;
    const int LSL_amount = 4;
    arm::UValueCarry res;

    BOOST_CHECK( arm::Shift(LSL_original, arm::SRType_LSL, LSL_amount, true ) ==
                 LSL_shifted );
    res = arm::Shift_C( LSL_original, arm::SRType_LSL, LSL_amount, true );
    BOOST_CHECK_EQUAL( res.value, LSL_shifted );
    BOOST_CHECK( res.carry );

    res = arm::LSL_C( LSL_original, LSL_amount );
    BOOST_CHECK_EQUAL( res.value, LSL_shifted );
    BOOST_CHECK( res.carry );
    BOOST_CHECK_EQUAL( arm::LSL( LSL_original, LSL_amount ), LSL_shifted );
}

BOOST_AUTO_TEST_CASE( Shift_LSR )
{
    // Logical Shift Right
    const uint32_t LSR_original = 0xB450DEAD;
    const uint32_t LSR_shifted = 0x00B450DE;
    const int LSR_amount = 8;
    arm::UValueCarry res;

    BOOST_CHECK( arm::Shift(LSR_original, arm::SRType_LSR, LSR_amount, true ) ==
                 LSR_shifted );
    res = arm::Shift_C( LSR_original, arm::SRType_LSR, LSR_amount, true );
    BOOST_CHECK_EQUAL( res.value, LSR_shifted );
    BOOST_CHECK( res.carry );

    res = arm::LSR_C( LSR_original, LSR_amount );
    BOOST_CHECK_EQUAL( res.value, LSR_shifted );
    BOOST_CHECK( res.carry );
    BOOST_CHECK_EQUAL( arm::LSR( LSR_original, LSR_amount ), LSR_shifted );
}

BOOST_AUTO_TEST_CASE( Shift_ASR )
{
    // Arithmetic Shift Right
    const uint32_t ASR_original = 0xB450DEAD;
    const uint32_t ASR_shifted = 0xFFB450DE;
    const int ASR_amount = 8;
    arm::UValueCarry res;

    BOOST_CHECK_EQUAL( arm::Shift(ASR_original, arm::SRType_ASR, ASR_amount, true ),
                 ASR_shifted );
    res = arm::Shift_C( ASR_original, arm::SRType_ASR, ASR_amount, true );
    BOOST_CHECK_EQUAL( res.value, ASR_shifted );
    BOOST_CHECK( res.carry );

    res = arm::ASR_C( ASR_original, ASR_amount );
    BOOST_CHECK_EQUAL( res.value, ASR_shifted );
    BOOST_CHECK( res.carry );
    BOOST_CHECK_EQUAL( arm::ASR( ASR_original, ASR_amount ), ASR_shifted );

    BOOST_CHECK_EQUAL( arm:: Shift(0x1FFFFFFF, arm:: SRType_ASR, 8, true ), 0x001FFFFF);
}

BOOST_AUTO_TEST_CASE( Shift_ROR )
{
    // Rotate Right
    const uint32_t ROR_original = 0xB450DEAD;
    const uint32_t ROR_shifted = 0xDEADB450;
    const int ROR_amount = 16;
    arm::UValueCarry res;

    BOOST_CHECK( arm::Shift(ROR_original, arm::SRType_ROR, ROR_amount, true ) ==
                 ROR_shifted );
    res = arm::Shift_C( ROR_original, arm::SRType_ROR, ROR_amount, true );
    BOOST_CHECK_EQUAL( res.value, ROR_shifted );
    BOOST_CHECK( res.carry );

    res = arm::ROR_C( ROR_original, ROR_amount );
    BOOST_CHECK_EQUAL( res.value, ROR_shifted );
    BOOST_CHECK( res.carry );
    BOOST_CHECK_EQUAL( arm::ROR( ROR_original, ROR_amount ), ROR_shifted );
}

BOOST_AUTO_TEST_CASE( Shift_RRX )
{
    // Rotate Right with extend
    const uint32_t RRX_original = 0xB450DEAD;
    const uint32_t RRX_shifted = 0xDA286F56;
    const int RRX_amount = 1;
    arm::UValueCarry res;

    BOOST_CHECK( arm::Shift(RRX_original, arm::SRType_RRX, RRX_amount, true ) ==
                 RRX_shifted );
    res = arm::Shift_C( RRX_original, arm::SRType_RRX, RRX_amount, true );
    BOOST_CHECK_EQUAL( res.value, RRX_shifted );
    BOOST_CHECK( res.carry );

    res = arm::RRX_C( RRX_original, RRX_amount );
    BOOST_CHECK_EQUAL( res.value, RRX_shifted );
    BOOST_CHECK( res.carry );
    BOOST_CHECK_EQUAL( arm::RRX( RRX_original, RRX_amount ), RRX_shifted );
}

BOOST_AUTO_TEST_CASE( CountLeadingZeroBits_test )
{
    // 42 == 0x0000002A, 26 zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 42 ), 26 );

    // 0, 32 zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 0 ), 32 );

    // 1, 31 zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 1 ), 31 );

    // 0xFFFFFFFF, 0 zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 0xFFFFFFFF ), 0 );

    // 0xF0FFFFFF, 0 leading zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 0xF0FFFFFF ), 0 );

    // 0x0FFFFFFF, 4 leading zeros.
    BOOST_CHECK_EQUAL( arm::CountLeadingZeroBits( 0x0FFFFFFF ), 4 );
}


BOOST_AUTO_TEST_CASE( NOT_test )
{
    // Complement of the maximum value on 32 bits should be 0.
    BOOST_CHECK_EQUAL( arm::NOT( 0xFFFFFFFFU ), 0x0U );

    // Complement of 0 should be the maximum value on 32 bits.
    BOOST_CHECK_EQUAL( arm::NOT( 0x0U ), 0xFFFFFFFFU );

    // Inversion pattern.
    BOOST_CHECK_EQUAL( arm::NOT( 0xFFFF0000U ), 0x0000FFFFU );

    // Inversion pattern.
    BOOST_CHECK_EQUAL( arm::NOT( 0xF0F0F0F0U ), 0x0F0F0F0FU );
}

BOOST_AUTO_TEST_CASE( SignExtend_test )
{
    BOOST_CHECK_EQUAL( arm::SignExtend( 0xCAFEC0DE, 32, 32 ),
        0xCAFEC0DE );
    BOOST_CHECK_EQUAL( arm::SignExtend( 0x0000000000000001LL, 64, 2 ),
        0x0000000000000001LL );
    BOOST_CHECK_EQUAL( arm::SignExtend( 0x0000000000000003LL, 64, 2 ),
        0xFFFFFFFFFFFFFFFFLL );
}

BOOST_AUTO_TEST_CASE( ConditionPassed_test )
{
    // Setup a test processor
    test_cpsr CPSR;
    memset( &CPSR, 0, sizeof( CPSR ) );
    test_proc proc = { CPSR, 0, NULL, {}, {} };

    // Instruction always executed (AL).
    BOOST_CHECK( arm::ConditionPassed( proc, 0xE0000000 ) == true );

    BOOST_CHECK( arm::ConditionPassed( proc, 0xF0000000 ) == true );

    // Equal (EQ).
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x00000000 ) == true );
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x00000000 ) == false );

    // Not equal (NE).
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x10000000 ) == true );
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x10000000 ) == false );

    // Carry set (CS).
    proc.CPSR.C = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x20000000 ) == true );
    proc.CPSR.C = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x20000000 ) == false );

    // Carry clear (CC).
    proc.CPSR.C = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x30000000 ) == true );
    proc.CPSR.C = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x30000000 ) == false );

    // Minus, negative (MI).
    proc.CPSR.N = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x40000000 ) == true );
    proc.CPSR.N = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x40000000 ) == false );

    // Plus, positive or zero (PL).
    proc.CPSR.N = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x50000000 ) == true );
    proc.CPSR.N = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x50000000 ) == false );

    // Overflow (VS).
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x60000000 ) == true );
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x60000000 ) == false );

    // No overflow (VC).
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x70000000 ) == true );
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x70000000 ) == false );

    // Unsigned higher (HI).
    proc.CPSR.C = 1;
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x80000000 ) == true );
    proc.CPSR.C = 0;
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x80000000 ) == false );
    proc.CPSR.C = 0;
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x80000000 ) == false );
    proc.CPSR.C = 1;
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x80000000 ) == false );

    // Unsigned lower or same (LS).
    proc.CPSR.C = 0;
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x90000000 ) == true );
    proc.CPSR.C = 1;
    proc.CPSR.Z = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x90000000 ) == false );
    proc.CPSR.C = 0;
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x90000000 ) == true );
    proc.CPSR.C = 1;
    proc.CPSR.Z = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0x90000000 ) == true );

    // Signed greater than or equal (GE).
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xA0000000 ) == true );
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xA0000000 ) == false );
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xA0000000 ) == false );
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xA0000000 ) == true );

    // Signed less than (LT).
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xB0000000 ) == false );
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xB0000000 ) == true );
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xB0000000 ) == true );
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xB0000000 ) == false );

    // Signed greater than (GT).
    proc.CPSR.Z = 0;
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == true );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == true );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xC0000000 ) == false );

    // Signed less than or equal (LE).
    proc.CPSR.Z = 0;
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == false );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == false );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );
    proc.CPSR.Z = 0;
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 0;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 1;
    proc.CPSR.V = 0;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 0;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );
    proc.CPSR.Z = 1;
    proc.CPSR.N = 1;
    proc.CPSR.V = 1;    
    BOOST_CHECK( arm::ConditionPassed( proc, 0xD0000000 ) == true );

}

#define ADD_WITH_CARRY_TEST( value_type )                               \
{                                                                       \
    uint64_t   max64 = 0xFFFFFFFFFFFFFFFFLL;                            \
    uint64_t   N     = sizeof( value_type ) * 8 - 1;                    \
    value_type max   = (value_type)arm::Bits64( max64, N, 0 );          \
                                                                        \
    value_type x, y, carry_in;                                          \
    value_type carry_out = 0;                                           \
    value_type overflow  = 0;                                           \
                                                                        \
    /* Add with carry in test. */                                       \
    x        = 20;                                                      \
    y        = 21;                                                      \
    carry_in = 1;                                                       \
    int32_t result = arm::AddWithCarry( x, y, carry_in,                 \
                                        carry_out, overflow );          \
                                                                        \
    BOOST_CHECK_EQUAL( result,    42 );                                 \
    BOOST_CHECK_EQUAL( carry_out, 0 );                                  \
    BOOST_CHECK_EQUAL( overflow,  0 );                                  \
                                                                        \
    /* Carry out test. */                                               \
    x        = max;                                                     \
    y        = max;                                                     \
    carry_in = 0;                                                       \
    result = arm::AddWithCarry( x, y, carry_in, carry_out, overflow );  \
                                                                        \
    BOOST_CHECK( carry_out == 1 );                                      \
    BOOST_CHECK( overflow == 0 );                                       \
                                                                        \
    /* Overflow test. */                                                \
    x        = max ^ ( 1 << N );                                        \
    y        = 1;                                                       \
    carry_in = 0;                                                       \
    result = arm::AddWithCarry( x, y, carry_in, carry_out, overflow );  \
                                                                        \
    BOOST_CHECK( carry_out == 0 );                                      \
    BOOST_CHECK( overflow == 1 );                                       \
}

BOOST_AUTO_TEST_CASE( AddWithCarry_test )
{
    ADD_WITH_CARRY_TEST( uint8_t  );
    ADD_WITH_CARRY_TEST( int8_t   );
    ADD_WITH_CARRY_TEST( uint16_t );
    ADD_WITH_CARRY_TEST( int16_t  );
    ADD_WITH_CARRY_TEST( uint32_t );
    ADD_WITH_CARRY_TEST( int32_t  );

    // AddWithCarry does not work with 64-bit data types
    //ADD_WITH_CARRY_TEST( uint64_t );
    //ADD_WITH_CARRY_TEST( int64_t  );
}

BOOST_AUTO_TEST_CASE( SelectInstrSet_test )
{
    // Setup a test processor
    test_cpsr CPSR;
    memset( &CPSR, 0, sizeof( CPSR ) );
    test_proc proc = { CPSR, 0, NULL, {}, {} };

    // Switch to ThumbEE.
    arm::InstrSet currentInstr = arm::InstrSet_ThumbEE;
    arm::SelectInstrSet( proc, currentInstr );

    arm::InstrSet state = arm::CurrentInstrSet( proc );
    BOOST_CHECK( state == arm::InstrSet_ThumbEE );
    
    // Switch to Thumb.
    currentInstr = arm::InstrSet_Thumb;
    arm::SelectInstrSet( proc, currentInstr );
    
    state = arm::CurrentInstrSet( proc );
    BOOST_CHECK( state == arm::InstrSet_Thumb );

    // Switch to Jazelle.
    currentInstr = arm::InstrSet_Jazelle;
    arm::SelectInstrSet( proc, currentInstr );
    
    state = arm::CurrentInstrSet( proc );
    BOOST_CHECK( state == arm::InstrSet_Jazelle );

    // Switch to ARM.
    currentInstr = arm::InstrSet_ARM;
    arm::SelectInstrSet( proc, currentInstr );
    
    state = arm::CurrentInstrSet( proc );
    BOOST_CHECK( state == arm::InstrSet_ARM );

}

BOOST_AUTO_TEST_CASE( LoadWritePC_test )
{
    // TODO
}

BOOST_AUTO_TEST_CASE( BXWritePC_test )
{
    // TODO
}

BOOST_AUTO_TEST_CASE( BranchWritePC_test )
{
    // TODO
}

BOOST_AUTO_TEST_CASE( BranchTo_test )
{
    // TODO
}

BOOST_AUTO_TEST_CASE( HaveMPExt_test )
{
    BOOST_CHECK( arm::HaveMPExt( ) == false );
}

BOOST_AUTO_TEST_CASE( HaveSecurityExt_test )
{
    BOOST_CHECK( arm::HaveSecurityExt( ) == false );
}

BOOST_AUTO_TEST_CASE( MemorySystemArchitecture_test )
{
    // Cortex-A uses VMSA memory architecture
    BOOST_CHECK( arm::MemorySystemArchitecture( ) == arm::MemArch_VMSA );
}

BOOST_AUTO_TEST_CASE( ZeroExtend_test )
{
    BOOST_CHECK_EQUAL( arm::ZeroExtend( (uint8_t)0xFF ), (uint64_t)0xFF );
    BOOST_CHECK_EQUAL( arm::ZeroExtend( (int8_t) 0xFF ), (uint64_t)0xFF );

    BOOST_CHECK_EQUAL( arm::ZeroExtend( (uint16_t)0xFFFF ), (uint64_t)0xFFFF );
    BOOST_CHECK_EQUAL( arm::ZeroExtend( (int16_t) 0xFFFF ), (uint64_t)0xFFFF );

    BOOST_CHECK_EQUAL( arm::ZeroExtend( (uint32_t)0xFFFFFFFF ),
                       (uint64_t)0xFFFFFFFF );
    BOOST_CHECK_EQUAL( arm::ZeroExtend( (int32_t) 0xFFFFFFFF ),
                       (uint64_t)0xFFFFFFFF );

    BOOST_CHECK_EQUAL( arm::ZeroExtend( (uint64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (uint64_t)0xFFFFFFFFFFFFFFFFLL );
    BOOST_CHECK_EQUAL( arm::ZeroExtend( (uint64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (uint64_t)0xFFFFFFFFFFFFFFFFLL );
}

BOOST_AUTO_TEST_CASE( NullCheckIfThumbEE_test )
{
    // Setup a test processor
    test_cpsr CPSR;
    memset( &CPSR, 0, sizeof( CPSR ) );
    test_proc proc = { CPSR, 0, NULL, {}, {} };
    
    // ThumEE is not implemented, so this function should return true
    BOOST_CHECK( arm::NullCheckIfThumbEE( proc, 5 ) == true );
}

BOOST_AUTO_TEST_CASE( PCStoreValue_test )
{
    // Setup a test processor
    test_cpsr CPSR;
    memset( &CPSR, 0, sizeof( CPSR ) );
    test_proc proc = { CPSR, 0, NULL, {}, {} };
    
    BOOST_CHECK_EQUAL( arm::PCStoreValue( proc ), (int32_t) proc.PC );
    
    proc.PC += 4; 
    BOOST_CHECK_EQUAL( arm::PCStoreValue( proc ), (int32_t) proc.PC );
}

BOOST_AUTO_TEST_CASE( UInt_test )
{
    BOOST_CHECK_EQUAL( arm::UInt( (uint8_t)0xFF ), (uint8_t)0xFF );
    BOOST_CHECK_EQUAL( arm::UInt( (int8_t) 0xFF ), (uint8_t)0xFF );

    BOOST_CHECK_EQUAL( arm::UInt( (uint16_t)0xFFFF ), (uint16_t)0xFFFF );
    BOOST_CHECK_EQUAL( arm::UInt( (int16_t) 0xFFFF ), (uint16_t)0xFFFF );

    BOOST_CHECK_EQUAL( arm::UInt( (uint32_t)0xFFFFFFFF ),
                       (uint32_t)0xFFFFFFFF );
    BOOST_CHECK_EQUAL( arm::UInt( (int32_t) 0xFFFFFFFF ),
                       (uint32_t)0xFFFFFFFF );

    BOOST_CHECK_EQUAL( arm::UInt( (uint64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (uint64_t)0xFFFFFFFFFFFFFFFFLL );
    BOOST_CHECK_EQUAL( arm::UInt( (int64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (uint64_t)0xFFFFFFFFFFFFFFFFLL );
}

BOOST_AUTO_TEST_CASE( SInt_test )
{
    BOOST_CHECK_EQUAL( arm::SInt( (uint8_t)0xFF ), (int8_t)0xFF );
    BOOST_CHECK_EQUAL( arm::SInt( (int8_t) 0xFF ), (int8_t)0xFF );

    BOOST_CHECK_EQUAL( arm::SInt( (uint16_t)0xFFFF ), (int16_t)0xFFFF );
    BOOST_CHECK_EQUAL( arm::SInt( (int16_t) 0xFFFF ), (int16_t)0xFFFF );

    BOOST_CHECK_EQUAL( arm::SInt( (uint32_t)0xFFFFFFFF ),
                       (int32_t)0xFFFFFFFF );
    BOOST_CHECK_EQUAL( arm::SInt( (int32_t) 0xFFFFFFFF ),
                       (int32_t)0xFFFFFFFF );

    BOOST_CHECK_EQUAL( arm::SInt( (uint64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (int64_t)0xFFFFFFFFFFFFFFFFLL );
    BOOST_CHECK_EQUAL( arm::SInt( (int64_t)0xFFFFFFFFFFFFFFFFLL ),
                       (int64_t)0xFFFFFFFFFFFFFFFFLL );
}

#endif // __ARMV7_FUNCTION_TEST_HPP__
