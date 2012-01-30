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
 * Unit tests for ARMv7 instructions.
 */

#ifndef __ARMV7_INSTRUCTION_TEST_HPP__
#define __ARMV7_INSTRUCTION_TEST_HPP__

#include "armv7_test_proc.hpp"

#include <armv7/isa.hpp>
#include <armv7/function.hpp>
#include <armv7/types.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/cstdint.hpp>
#include <cstring>


/**
 * Convenience typedef for pointers to functions implementing
 * instruction behavior.
 */

typedef void ( *BehaviorFunc )( test_proc&, uint32_t );


/*
 * Processor setup and test macros. Ugly macros are used instead of
 * pretty functions to avoid all errors to be reported at the same
 * line.
 */

#define SETUP_TEST                                      \
    test_cpsr CPSR;                                     \
    uint32_t  R[16];                                    \
    memset( &CPSR, 0, sizeof( CPSR ) );                 \
    memset(     R, 0, sizeof( uint32_t ) * 16 );        \
    test_proc proc = { CPSR, 0, R, {}, {} };
    
#define CHECK_RD( expected )                    \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( R[d], expected )

#define CHECK_RD64( expected )                                          \
    func( proc, instr );                                                \
    BOOST_CHECK_EQUAL( R[dHi], arm::Bits64( expected, 63, 32 ) );       \
    BOOST_CHECK_EQUAL( R[dLo], arm::Bits64( expected, 31,  0 ) );

#define CHECK_GE( expected )                    \
    proc.CPSR.GE = 0;                           \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( proc.CPSR.GE, expected )

#define CHECK_RT( expected )                    \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( R[t], expected )

#define CHECK_RTRN( exp_t, exp_n )              \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( R[t], exp_t );           \
    BOOST_CHECK_EQUAL( R[n], exp_n );

#define CHECK_CPSR( n, z, c, v )                \
    proc.CPSR.N = 0;                            \
    proc.CPSR.Z = 0;                            \
    proc.CPSR.C = 0;                            \
    proc.CPSR.V = 0;                            \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( proc.CPSR.N, n );        \
    BOOST_CHECK_EQUAL( proc.CPSR.Z, z );        \
    BOOST_CHECK_EQUAL( proc.CPSR.C, c );        \
    BOOST_CHECK_EQUAL( proc.CPSR.V, v );

#define CHECK_CPSR_EXT( n, z, c, v, q, ge )     \
    proc.CPSR.N = 0;                            \
    proc.CPSR.Z = 0;                            \
    proc.CPSR.C = 0;                            \
    proc.CPSR.V = 0;                            \
    proc.CPSR.Q = 0;                            \
    proc.CPSR.GE= 0;                            \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( proc.CPSR.N, n );        \
    BOOST_CHECK_EQUAL( proc.CPSR.Z, z );        \
    BOOST_CHECK_EQUAL( proc.CPSR.C, c );        \
    BOOST_CHECK_EQUAL( proc.CPSR.C, v );        \
    BOOST_CHECK_EQUAL( proc.CPSR.Q, q );        \
    BOOST_CHECK_EQUAL( proc.CPSR.GE,ge);

#define CHECK_PC( expected )                    \
    func( proc, instr );                        \
    BOOST_CHECK_EQUAL( proc.PC, expected )

/*
 * Actual tests.
 */


/*
 * Actual tests.
 */

BOOST_AUTO_TEST_CASE( ADC_imm_A1_test )
{
    // (A8.6.1, p.326)
    SETUP_TEST;
    const uint32_t op = 0xF2A01000;
    const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;  
    BehaviorFunc func = arm::ADC_imm_A1;
    uint32_t instr;

    R[n] = 0x00000000; instr = op | 0x00F; CHECK_RD( 0x0000000F ); 
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_RD( 0xFFFFFFFF );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_RD( 0x00000002 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_RD( 0xFFFFFFFF );
    // TODO: Test rotation imm12.
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_RD( 0xDEADDFAC );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; instr = op | S | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 0, 1, 1, 0 );

    // Test add with carry flag set by previous test.
    R[n] = 0x00000001; instr = op | 0x001; CHECK_RD( 0x00000003 ); 

    // No flags
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | S | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADC_reg_A1_test )
{
    // (A8.6.2, p.328)
    SETUP_TEST;
    const uint32_t op = 0xF0A01002;
    const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::ADC_reg_A1;
    uint32_t instr;

    R[n] = 0x00000000; R[m] = 0x00000001; instr = op ; CHECK_RD( 0x00000001 ); 
    // Shift test.
    uint32_t imm5 = 1;    // 1 bit shift
    uint32_t type = 0x00; // LSL
    R[n] = 0x00000001; R[m] = 0x00000001; instr = op | imm5 << 7 | type << 5; 
    CHECK_RD( 0x00000003 ); 
    R[n] = 0xDEAD0000; R[m] = 0x0000DEAD; instr = op ; CHECK_RD( 0xDEADDEAD );
    R[n] = 0x02546011; R[m] = 0xDC597E9C; instr = op ; CHECK_RD( 0xDEADDEAD );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op | S; 
    CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 0, 1, 1, 0 );

    // Test add with carry flag set by previous test.
    R[n] = 0x00000001; R[m] = 0x00000001; instr = op; CHECK_RD( 0x00000003 ); 

    // No flags
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );


    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | S | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADC_rsr_A1_test )
{
    // (A8.6.3, p.323)
    SETUP_TEST;
    const uint32_t op = 0xF0A01312;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2, s = 3;  
    BehaviorFunc func = arm::ADC_rsr_A1;
    uint32_t instr;

    R[n] = 0x00000000; R[m] = 0x00000001; R[s] = 0x00000000; instr = op;
    CHECK_RD( 0x00000001 ); 
}


BOOST_AUTO_TEST_CASE( ADD_imm_A1_test )
{
    // (A8.6.5, p.334)
    SETUP_TEST;
    const uint32_t op = 0xF2801000;
    const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;  
    BehaviorFunc func = arm::ADD_imm_A1;
    uint32_t instr;


    R[n] = 0x00000000; instr = op | 0x00F; CHECK_RD( 0x0000000F ); 
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_RD( 0xFFFFFFFF );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_RD( 0x00000002 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_RD( 0xFFFFFFFF );
    // TODO: Test rotation imm12.
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_RD( 0xDEADDFAC );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; instr = op | S | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 0, 1, 1, 0 );

    // No flags
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | S | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADD_reg_A1_test )
{
    // (A8.6.6, p.336)
    SETUP_TEST;
    const uint32_t op = 0xF0801002;
    const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::ADD_reg_A1;
    uint32_t instr;

    R[n] = 0x00000000; R[m] = 0x00000001; instr = op ; CHECK_RD( 0x00000001 ); 
    // Shift test.
    uint32_t imm5 = 1;    // 1 bit shift
    uint32_t type = 0x00; // LSL
    R[n] = 0x00000001; R[m] = 0x00000001; instr = op | imm5 << 7 | type << 5; 
    CHECK_RD( 0x00000003 ); 
    R[n] = 0xDEAD0000; R[m] = 0x0000DEAD; instr = op ; CHECK_RD( 0xDEADDEAD );
    R[n] = 0x02546011; R[m] = 0xDC597E9C; instr = op ; CHECK_RD( 0xDEADDEAD );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op | S; 
    CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 0, 1, 1, 0 );

    // No flags
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );


    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | S | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADD_rsr_A1_test )
{
    // (A8.6.7, p.338)
    SETUP_TEST;
    const uint32_t op = 0xF0001312;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2, s = 3;  
    BehaviorFunc func = arm::ADD_rsr_A1;
    uint32_t instr;

    R[n] = 0x00000000; R[m] = 0x00000001; R[s] = 0x00000000; instr = op; 
    CHECK_RD( 0x00000001 ); 
}

BOOST_AUTO_TEST_CASE( ADD_SP_imm_A1_test )
{
    // (A8.6.8, p.340)
    SETUP_TEST;
    const uint32_t op = 0xF28D1000;
    const uint32_t S  = 0x1 << 20;
    // SP = 13
    const uint32_t n = 13, d = 1;  
    BehaviorFunc func = arm::ADD_SP_imm_A1;
    uint32_t instr;

    R[n] = 0x00000000; instr = op | 0x00F; CHECK_RD( 0x0000000F ); 
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_RD( 0xFFFFFFFF );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_RD( 0x00000002 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_RD( 0xFFFFFFFF );
    // TODO: Test rotation imm12.
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_RD( 0xDEADDFAC );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; instr = op | S | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; instr = op | S | 0x001; CHECK_CPSR( 0, 1, 1, 0 );

    // No flags
    R[n] = 0xFFFFFFFF; instr = op | 0x000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000001; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFE; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xDEADDEAD; instr = op | 0x0FF; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; instr = op | S | 0xF << 12 | 0x004; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADD_SP_reg_A1_test )
{
    // (A8.6.9, p.342)
    SETUP_TEST;
    const uint32_t op = 0xF08D1002;
    const uint32_t S  = 0x1 << 20;
    // SP = 13
    const uint32_t n  = 13, d = 1, m = 2;  
    BehaviorFunc func = arm::ADD_SP_reg_A1;
    uint32_t instr;

    R[n] = 0x00000000; R[m] = 0x00000001; instr = op ; CHECK_RD( 0x00000001 ); 
    // Shift test.
    uint32_t imm5 = 1;    // 1 bit shift
    uint32_t type = 0x00; // LSL
    R[n] = 0x00000001; R[m] = 0x00000001; instr = op | imm5 << 7 | type << 5; 
    CHECK_RD( 0x00000003 ); 
    R[n] = 0xDEAD0000; R[m] = 0x0000DEAD; instr = op ; CHECK_RD( 0xDEADDEAD );
    R[n] = 0x02546011; R[m] = 0xDC597E9C; instr = op ; CHECK_RD( 0xDEADDEAD );

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op | S; 
    CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op | S; 
    CHECK_CPSR( 0, 1, 1, 0 );

    // No flags
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    // Watch out, not all address are valid. Adding wrong values can produce
    // undefined results.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000104 );

    // Test PC with flags, should do nothing.
    proc.PC = 0x00000000;
    R[n] = 0x00000100; R[m] = 0x00000004; instr = op | S | 0xF << 12; 
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0x00000000 );
}

BOOST_AUTO_TEST_CASE( ADR_A1_test )
{
    // (A8.6.10, p.344)
    SETUP_TEST;
    const uint32_t op = 0xF2AF1000;
    const uint32_t d = 1;  
    BehaviorFunc func = arm::ADR_A1;
    uint32_t instr;

    proc.PC = 0xDEAD0000; instr = op | 0x004; CHECK_RD( 0xDEAD0004 );
}

BOOST_AUTO_TEST_CASE( ADR_A2_test )
{
    // (A8.6.10, p.344)
    SETUP_TEST;
    const uint32_t op = 0xF24F1000;
    const uint32_t d = 1;  
    BehaviorFunc func = arm::ADR_A2;
    uint32_t instr;

    proc.PC = 0x00000004; instr = op | 0x004; CHECK_RD( 0x00000000 );
}

BOOST_AUTO_TEST_CASE( AND_imm_A1_test )
{
    // (A8.6.10, p.346)
    SETUP_TEST;
    const uint32_t op = 0xF2001000;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;  
    BehaviorFunc func = arm::AND_imm_A1;
    uint32_t instr;

    R[n] = 0x0000000F; instr = op | 0x00F; CHECK_RD( 0x0000000F ); 
}

BOOST_AUTO_TEST_CASE( AND_reg_A1_test )
{
    // (A8.6.12, p.348)
    SETUP_TEST;
    const uint32_t op = 0xF0001002;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::AND_reg_A1;
    uint32_t instr;

    R[n] = 0x0000000F; R[m] = 0x0000000F; instr = op; CHECK_RD( 0x0000000F );
}

BOOST_AUTO_TEST_CASE( AND_rsr_A1_test )
{
    // (A8.6.13, p.350)
    SETUP_TEST;
    const uint32_t op = 0xF0001312;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2, s = 3;  
    BehaviorFunc func = arm::AND_rsr_A1;
    uint32_t instr;

    R[n] = 0xF0F0F0F0; R[m] = 0x0F0F0F0F; R[s] = 0x00000000; instr = op; 
    CHECK_RD( 0x00000000 ); 
}

BOOST_AUTO_TEST_CASE( ASR_imm_A1_test )
{
    // (A8.6.14, p.352)
    SETUP_TEST;
    const uint32_t op = 0xF1A01042;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t d = 1, m = 2;  
    BehaviorFunc func = arm::ASR_imm_A1;
    uint32_t instr;

    uint32_t imm5 = 1;    // 1 bit shift
    R[m] = 0x00000002; instr = op | imm5 << 7; CHECK_RD( 0x00000001 ); 
}

BOOST_AUTO_TEST_CASE( ASR_reg_A1_test )
{
    // (A8.6.15, p.354)
    SETUP_TEST;
    const uint32_t op = 0xF1A01250;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::ASR_reg_A1;
    uint32_t instr;

    R[n] = 0x0000002; R[m] = 0x00000001; instr = op; CHECK_RD( 0x00000001 ); 
}

BOOST_AUTO_TEST_CASE( B_A1_test )
{
    // (A8.6.16, p.356)
    SETUP_TEST;
    const uint32_t op = 0xFA000000;
    BehaviorFunc func = arm::B_A1;
    uint32_t instr;

    proc.PC = 0x00000000;
    uint32_t imm24 = 0x00FFFFFF;
    instr = op | imm24; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xFFFFFFFC );
}

BOOST_AUTO_TEST_CASE( BFC_A1_test )
{
    // (A8.6.17, p.358)
    SETUP_TEST;
    const uint32_t op = 0xF7C0101F;
    const uint32_t d = 1;  
    BehaviorFunc func = arm::BFC_A1;
    uint32_t instr;

    // Clear low byte, second nibble
    uint32_t msb = 7;
    uint32_t lsb = 4;

    R[d] = 0xFFFFFFFF; instr = op | msb << 16 | lsb << 7; 
    CHECK_RD( 0xFFFFFF0F ); 
}

BOOST_AUTO_TEST_CASE( BFI_A1_test )
{
    // (A8.6.18, p.360)
    SETUP_TEST;
    const uint32_t op = 0xF7C01010;
    const uint32_t n  = 0, d = 1;  
    BehaviorFunc func = arm::BFI_A1;
    uint32_t instr;

    uint32_t msb = 19;
    uint32_t lsb = 12;

    R[n] = 0xFFF00FFF; R[d] = 0x0FFFFFF0; instr = op | msb << 16 | lsb << 7; 
    CHECK_RD( 0x0FF00FF0 ); 
}

BOOST_AUTO_TEST_CASE( BIC_imm_A1_test )
{
    // (A8.6.19, p.362)
    SETUP_TEST;
    const uint32_t op = 0xF3C01000;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;  
    BehaviorFunc func = arm::BIC_imm_A1;
    uint32_t instr;

    R[n] = 0x0000000F; instr = op | 0x000; CHECK_RD( 0x0000000F ); 
}

BOOST_AUTO_TEST_CASE( BIC_reg_A1_test )
{
    // (A8.6.20, p.364)
    SETUP_TEST;
    const uint32_t op = 0xF1C01002;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::BIC_reg_A1;
    uint32_t instr;

    R[n] = 0x0000000F; R[m] = 0x00000000; instr = op; CHECK_RD( 0x0000000F );
}

BOOST_AUTO_TEST_CASE( BIC_rsr_A1_test )
{
    // (A8.6.21, p.366)
    SETUP_TEST;
    const uint32_t op = 0xF1C01312;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1, m = 2, s = 3;  
    BehaviorFunc func = arm::BIC_rsr_A1;
    uint32_t instr;

    R[n] = 0x0000000F; R[m] = 0x00000000; R[s] = 0x00000000; instr = op; 
    CHECK_RD( 0x0000000F ); 
}

BOOST_AUTO_TEST_CASE( BL_A1_test )
{
    // (A8.6.23, p.370)
    SETUP_TEST;
    const uint32_t op = 0xFB000000;

    BehaviorFunc func = arm::BL_A1;
    uint32_t instr;

    proc.PC = 0x00000000;
    uint32_t imm24 = 0x00FFFFFF;
    instr = op | imm24; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xFFFFFFFC );

}

BOOST_AUTO_TEST_CASE( BLX_imm_A1_test )
{
    // (A8.6.23, p.370)
    SETUP_TEST;
    const uint32_t op = 0xFA000000;

    BehaviorFunc func = arm::BLX_imm_A1;
    uint32_t instr;

    proc.PC = 0x00000000;
    uint32_t H = 0x1 << 24;
    uint32_t imm24 = 0x00FFFFFF;
    instr = op | H | imm24; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xFFFFFFFE );

}

BOOST_AUTO_TEST_CASE( BLX_reg_A1_test )
{
    // (A8.6.24, p.372)
    SETUP_TEST;
    const uint32_t op = 0xF12FFF32;

    const uint32_t m = 2; 
    BehaviorFunc func = arm::BLX_reg_A1;
    uint32_t instr;

    proc.PC = 0x00000000;
    R[m] = 0xDEAD0000;
    instr = op; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xDEAD0000 );
}

BOOST_AUTO_TEST_CASE( BX_A1_test )
{
    // (A8.6.25, p.374)
    SETUP_TEST;
    const uint32_t op = 0xF12FFF12;

    const uint32_t m = 2;  
    BehaviorFunc func = arm::BX_A1;
    uint32_t instr;

    proc.PC = 0x00000000;
    R[m] = 0xDEAD0000;
    instr = op; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xDEAD0000 );
}

BOOST_AUTO_TEST_CASE( CLZ_A1_test )
{
    // (A8.6.31, p.384)
    SETUP_TEST;
    const uint32_t op = 0xF16F1F12;
    
    const uint32_t d = 1, m = 2;
    BehaviorFunc func = arm::CLZ_A1;
    uint32_t instr;

    R[m] = 0x0000000F; instr = op; CHECK_RD( 0x0000001C );
}

BOOST_AUTO_TEST_CASE( CMN_imm_A1_test )
{
    // (A8.6.32, p.386)
    SETUP_TEST;
    const uint32_t op = 0xF3700000;

    const uint32_t n = 0;  
    BehaviorFunc func = arm::CMN_imm_A1;
    uint32_t instr;

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; instr = op | 0x001; CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; instr = op | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; instr = op | 0x001; CHECK_CPSR( 0, 1, 1, 0 );
}

BOOST_AUTO_TEST_CASE( CMN_reg_A1_test )
{
    // (A8.6.33, p.388)
    SETUP_TEST;
    const uint32_t op = 0xF1700002;

    const uint32_t n = 0, m = 2;  
    BehaviorFunc func = arm::CMN_reg_A1;
    uint32_t instr;

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; instr = op; 
    CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; R[m] = 0x00000000; instr = op; 
    CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; instr = op; 
    CHECK_CPSR( 0, 1, 1, 0 );

}

BOOST_AUTO_TEST_CASE( CMN_rsr_A1_test )
{
    // (A8.6.34, p.390)
    SETUP_TEST;
    const uint32_t op = 0xF1700312;

    const uint32_t n = 0, m = 2, s = 3;  
    BehaviorFunc func = arm::CMN_rsr_A1;
    uint32_t instr;

    // Flags
    // Negative and overflow
    R[n] = 0x7FFFFFFF; R[m] = 0x00000001; R[s] = 0x00000000; instr = op;
    CHECK_CPSR( 1, 0, 0, 1 );
    // Zero
    R[n] = 0x00000000; R[m] = 0x00000000; R[s] = 0x00000000; instr = op;
    CHECK_CPSR( 0, 1, 0, 0 );
    // Carry
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; R[s] = 0x00000000; instr = op; 
    CHECK_CPSR( 0, 1, 1, 0 );
}

BOOST_AUTO_TEST_CASE( CMP_imm_A1_test )
{
    // (A8.6.35, p.392)
    SETUP_TEST;
    const uint32_t op = 0xF3500000;

    const uint32_t n = 0;  
    BehaviorFunc func = arm::CMP_imm_A1;
    uint32_t instr;

    // Flags
    // Negative
    R[n] = 0x00000000; instr = op | 0x001; CHECK_CPSR( 1, 0, 0, 0 );
    // Zero and carry
    R[n] = 0x00000001; instr = op | 0x001; CHECK_CPSR( 0, 1, 1, 0 );


}

BOOST_AUTO_TEST_CASE( CMP_reg_A1_test )
{
    // (A8.6.36, p.394)
    SETUP_TEST;
    const uint32_t op = 0xF1500001;

    const uint32_t n = 0, m = 1;  
    BehaviorFunc func = arm::CMP_reg_A1;
    uint32_t instr;

    // Flags
    // Negative and overflow
    R[n] = 0x00000000; R[m] = 0x00000001; instr = op; CHECK_CPSR( 1, 0, 0, 0 );
    // Zero and carry
    R[n] = 0x00000001; R[m] = 0x00000001; instr = op; CHECK_CPSR( 0, 1, 1, 0 );
}

BOOST_AUTO_TEST_CASE( CMP_rsr_A1_test )
{
    // (A8.6.37, p.396)
    SETUP_TEST;
    const uint32_t op = 0xF1500211;
    const uint32_t n  = 0, m = 1, s = 2;  
    BehaviorFunc func = arm::CMP_rsr_A1;
    uint32_t instr;


    // Flags
    // Negative and overflow
    R[n] = 0x00000000; R[m] = 0x00000001; R[s] = 0x00000000; instr = op; 
    CHECK_CPSR( 1, 0, 0, 0 );
    // Zero and carry
    R[n] = 0x00000001; R[m] = 0x00000001; R[s] = 0x00000000; instr = op; 
    CHECK_CPSR( 0, 1, 1, 0 );

}

BOOST_AUTO_TEST_CASE( EOR_imm_A1_test )
{
    // (A8.6.44, p.406)
    SETUP_TEST;
    const uint32_t op = 0xF2201000;
    //const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;  

    BehaviorFunc func = arm::EOR_imm_A1;
    uint32_t instr;

    R[n] = 0x000000F0; instr = op | 0x00F; CHECK_RD( 0x000000FF ); 
}

BOOST_AUTO_TEST_CASE( EOR_reg_A1_test )
{
    // (A8.6.45, p.408)
    SETUP_TEST;
    const uint32_t op = 0xF0201002;
    //const uint32_t S  = 0x1 << 20;

    const uint32_t n  = 0, d = 1, m = 2;  
    BehaviorFunc func = arm::EOR_reg_A1;
    uint32_t instr;

    R[n] = 0xF0F0F0F0; R[m] = 0x0F0F0F0F; instr = op; CHECK_RD( 0xFFFFFFFF );
}


BOOST_AUTO_TEST_CASE( EOR_rsr_A1_test )
{
    // (A8.6.46, p.410)
    SETUP_TEST;
    const uint32_t op = 0xF0201312;
    //const uint32_t S  = 0x1 << 20;

    const uint32_t n  = 0, d = 1, m = 2, s = 3;  
    BehaviorFunc func = arm::EOR_rsr_A1;
    uint32_t instr;


    R[n] = 0xF0F0F0F0; R[m] = 0x0F0F0F0F; R[s] = 0x00000000; instr = op; 
    CHECK_RD( 0xFFFFFFFF ); 
}

BOOST_AUTO_TEST_CASE( LDM_A1_test )
{
    // (A8.6.53, p.422)
    SETUP_TEST;
    const uint32_t op = 0xF8900000;
    //const uint32_t W  = 0x1 << 21;
    const uint32_t n = 0;
    BehaviorFunc func = arm::LDM_A1;
    uint32_t instr;

    // Base address
    uint32_t address = 0x00000000;
    proc.R[n] = address;
    // Load even registers only.
    uint32_t register_list = 0x00005555;

    uint32_t current_address = address;

    // Write memory
    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            proc.dMem.write_word( current_address, i );
            current_address += 4;
        }
    }

    instr = op | register_list;    

    func( proc, instr );

    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            BOOST_CHECK_EQUAL( proc.R[i], i );
        }
        else
        {
            BOOST_CHECK_EQUAL( proc.R[i], 0x0 );
        }
    }
}

BOOST_AUTO_TEST_CASE( LDMDA_A1_test )
{
    // (A8.6.54, p.424)
    SETUP_TEST;
    const uint32_t op = 0xF8100000;
    //const uint32_t W  = 0x1 << 21;
    const uint32_t n = 0;  
    BehaviorFunc func = arm::LDMDA_A1;
    uint32_t instr;

    // Base address
    uint32_t address = 0x0000001C;
    proc.R[n] = address;
    // Load even registers only.
    uint32_t register_list = 0x00005555;

    uint32_t current_address = 0x0;

    // Write memory
    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            proc.dMem.write_word( current_address, i );
            current_address += 4;
        }
    }

    instr = op | register_list;    

    func( proc, instr );

    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            BOOST_CHECK_EQUAL( proc.R[i], i );
        }
        else
        {
            BOOST_CHECK_EQUAL( proc.R[i], 0x0 );
        }
    }
}

BOOST_AUTO_TEST_CASE( LDMDB_B1_test )
{
    // (A8.6.55, p.426)
    SETUP_TEST;
    const uint32_t op = 0xF9100000;
    //const uint32_t W  = 0x1 << 21;
    const uint32_t n = 0;  
    BehaviorFunc func = arm::LDMDB_A1;
    uint32_t instr;

    // Base address
    uint32_t address = 0x00000020;
    proc.R[n] = address;
    // Load even registers only.
    uint32_t register_list = 0x00005555;

    uint32_t current_address = 0x0;

    // Write memory
    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            proc.dMem.write_word( current_address, i );
            current_address += 4;
        }
    }

    instr = op | register_list;    

    func( proc, instr );

    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            BOOST_CHECK_EQUAL( proc.R[i], i );
        }
        else
        {
            BOOST_CHECK_EQUAL( proc.R[i], 0x0 );
        }
    }
}

BOOST_AUTO_TEST_CASE( LDMIB_B1_test )
{
    // (A8.6.56, p.428)
    SETUP_TEST;
    const uint32_t op = 0xF9900000;
    //const uint32_t W  = 0x1 << 21;
    const uint32_t n = 0;  
    BehaviorFunc func = arm::LDMIB_A1;
    uint32_t instr;


    // Base address
    uint32_t address = 0x00000000;
    proc.R[n] = address;
    // Load even registers only.
    uint32_t register_list = 0x00005555;

    uint32_t current_address = address + 4;

    // Write memory
    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            proc.dMem.write_word( current_address, i );
            current_address += 4;
        }
    }

    instr = op | register_list;    

    func( proc, instr );

    for(uint32_t i = 0; i < 16; ++i)
    {
        if( (i % 2) == 0 )
        {
            BOOST_CHECK_EQUAL( proc.R[i], i );
        }
        else
        {
            BOOST_CHECK_EQUAL( proc.R[i], 0x0 );
        }
    }
}


BOOST_AUTO_TEST_CASE( LDR_imm_A1_test )
{
    // (A8.6.58, p.432)
    SETUP_TEST;
    const uint32_t op = 0xF4101000;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDR_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0xBEBEC0DE );

    // Load register immediate
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xBEBEC0DE, 0x0006 );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xBEBEC0DE, 0x0002 );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xBEBEC0DE, 0x02 );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xBEBEC0DE, 0x06 );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x02; CHECK_RTRN( 0xBEBEC0DE, 0x04 );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x06; CHECK_RTRN( 0xBEBEC0DE, 0x04 );

    // test call to LDR_lit_A1 (A8.6.59, p.434)
    const uint32_t op2 = 0xF51F0000;
    instr = op2 | U | address; CHECK_RT( 0xBEBEC0DE );

    // test call to LDRT_A1 (A8.6.86, p.488)
    const uint32_t op3 = 0xF4301000;
    instr = op3 | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xBEBEC0DE, 0x0006 );

    // test call to LDRT_A2 (A8.6.86, p.488)
    const uint32_t op4 = 0xF6301002;
    const uint32_t m = 2;
    instr = op4 | U | (0x01 << 7); R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0xBEBEC0DE, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDR_lit_A1_test )
{
    // (A8.6.59, p.434)
    SETUP_TEST;
    const uint32_t op = 0xF51F0000;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDR_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0xCAFEC0DE );

    // Load register literal
    instr = op | U | address; CHECK_RT( 0xCAFEC0DE );
}

BOOST_AUTO_TEST_CASE( LDR_reg_A1_test )
{
    // (A8.6.60, p.436)
    SETUP_TEST;
    const uint32_t op = 0xF6101002;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDR_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0xCAFE );
    proc.dMem.write_word( address+4, 0xAF0E );

    // Load register register
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xCAFE, 0x0003 );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xCAFE, 0x0005 );
    // !index and add and imm5 (type = LSL), wback
    instr = op | U | (0x02 << 7);
    R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xCAFE, 0x0008 );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xCAFE, 0x0005 );
    // index and add
    instr = op | U | P;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0xAF0E, 0x0004 );

    // index and !add, wback
    instr = op | P | W;
    R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xCAFE, 0x0004 );
    // index and add, wback
    instr = op | U | P | W;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0xAF0E, 0x0008 );
}

BOOST_AUTO_TEST_CASE( LDRB_imm_A1_test )
{
    // (A8.6.62, p.440)
    SETUP_TEST;
    const uint32_t op = 0xF4501000;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRB_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xDA );

    // Load register byte
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xDA, 0x0006 );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xDA, 0x0002 );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xDA, 0x02 );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xDA, 0x06 );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x02; CHECK_RTRN( 0xDA, 0x04 );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x06; CHECK_RTRN( 0xDA, 0x04 );
}

BOOST_AUTO_TEST_CASE( LDRB_lit_A1_test )
{
    // (A8.6.63, p.442)
    SETUP_TEST;
    const uint32_t op = 0xF55F0000;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRB_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xB0 );

    // Load register byte
    instr = op | U | address; CHECK_RT( 0xB0 );
}

BOOST_AUTO_TEST_CASE( LDRB_reg_A1_test )
{
    // (A8.6.64, p.444)
    SETUP_TEST;
    const uint32_t op = 0xF6501002;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRB_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xAF );
    proc.dMem.write_byte( address+4, 0xF0 );

    // Load register byte
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xAF, 0x0003 );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xAF, 0x0005 );
    // !index and add and imm5 (type = LSL), wback
    instr = op | U | (0x02 << 7);
    R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xAF, 0x0008 );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xAF, 0x0005 );
    // index and add
    instr = op | U | P;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0xF0, 0x0004 );

    // index and !add, wback
    instr = op | P | W;
    R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xAF, 0x0004 );
    // index and add, wback
    instr = op | U | P | W;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0xF0, 0x0008 );
}

BOOST_AUTO_TEST_CASE( LDRBT_A1_test )
{
    // (A8.6.65, p.446)
    SETUP_TEST;
    const uint32_t op = 0xF4701000;
    const uint32_t n = 0, t = 1;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRBT_A1;
    uint32_t instr;

    proc.dMem.write_byte( address, 0x15 );

    // Load register
    // add
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x15, 0x0006 );
    // !add
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x15, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LDRBT_A2_test )
{
    // (A8.6.65, p.446)
    SETUP_TEST;
    const uint32_t op = 0xF6701002;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRBT_A2;
    uint32_t instr;

    proc.dMem.write_byte( address, 0x15 );

    // Load register, type = LSL = 0b00
    // add
    instr = op | U | (0x01 << 7); R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x15, 0x0006 );
    // !add
    instr = op     | (0x01 << 7); R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x15, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LDRD_imm_A1_test )
{
    // (A8.6.66, p.448)
    SETUP_TEST;
    const uint32_t op = 0xF04010D0;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRD_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0xBEBE1A1A );
    proc.dMem.write_word( address+4, 0xDEADC0DE );

    // Load register immediate
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xBEBE1A1A, 0x0006 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xBEBE1A1A, 0x0002 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xBEBE1A1A, 0x02 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xBEBE1A1A, 0x06 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x02; CHECK_RTRN( 0xBEBE1A1A, 0x04 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x06; CHECK_RTRN( 0xBEBE1A1A, 0x04 );
    BOOST_CHECK_EQUAL( R[t+1], 0xDEADC0DE );
}

BOOST_AUTO_TEST_CASE( LDRD_lit_A1_test )
{
    // (A8.6.67, p.450)
    SETUP_TEST;
    const uint32_t op = 0xF14F00D0;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRD_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0xFAC0DADE );
    proc.dMem.write_word( address+4, 0xFAC0DEDA );

    // Load register literal
    instr = op | U | address; CHECK_RT( 0xFAC0DADE );
    BOOST_CHECK_EQUAL( R[t+1], 0xFAC0DEDA );
}

BOOST_AUTO_TEST_CASE( LDRD_reg_A1_test )
{
    // (A8.6.68, p.452)
    SETUP_TEST;
    const uint32_t op = 0xF00010D3;
    const uint32_t n = 0, t = 1, m = 3;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRD_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0x01234567 );
    proc.dMem.write_word( address+4, 0x89ABCDEF );
    proc.dMem.write_word( address+8, 0x03040506 );

    // Load register register
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x01234567, 0x0003 ); BOOST_CHECK_EQUAL( R[t+1], 0x89ABCDEF );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x01234567, 0x0005 ); BOOST_CHECK_EQUAL( R[t+1], 0x89ABCDEF );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001;
    CHECK_RTRN( 0x01234567, 0x0005 ); BOOST_CHECK_EQUAL( R[t+1], 0x89ABCDEF );
    // index and add
    instr = op | U | P; R[n] = 0x0004; R[m] = 0x0004;
    CHECK_RTRN( 0x89ABCDEF, 0x0004 ); BOOST_CHECK_EQUAL( R[t+1], 0x03040506 );

    // index and !add, wback
    instr = op | P | W; R[n] = 0x0005; R[m] = 0x0001;
    CHECK_RTRN( 0x01234567, 0x0004 ); BOOST_CHECK_EQUAL( R[t+1], 0x89ABCDEF );
    // index and add, wback
    instr = op | U | P | W; R[n] = 0x0000; R[m] = 0x0004;
    CHECK_RTRN( 0x01234567, 0x0004 ); BOOST_CHECK_EQUAL( R[t+1], 0x89ABCDEF );
}

BOOST_AUTO_TEST_CASE( LDRH_imm_A1_test )
{
    // (A8.6.74, p.464)
    SETUP_TEST;
    const uint32_t op = 0xF05010B0;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRH_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register immediate
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xC0DE, 0x0006 );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xC0DE, 0x0002 );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xC0DE, 0x02 );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xC0DE, 0x06 );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x04; CHECK_RTRN( 0x1337, 0x06 );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x08; CHECK_RTRN( 0x1337, 0x06 );
}

BOOST_AUTO_TEST_CASE( LDRH_lit_A1_test )
{
    // (A8.6.75, p.466)
    SETUP_TEST;
    const uint32_t op = 0xF15F00B0;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRH_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_half( address, 0xC0DE );

    // Load register literal
    instr = op | U | address; CHECK_RT( 0xC0DE );
}

BOOST_AUTO_TEST_CASE( LDRH_reg_A1_test )
{
    // (A8.6.76, p.468)
    SETUP_TEST;
    const uint32_t op = 0xF01010B2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRH_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register register
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xC0DE, 0x0003 );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xC0DE, 0x0005 );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xC0DE, 0x0005 );
    // index and add
    instr = op | U | P;
    R[n] = 0x0000; R[m] = 0x0004; CHECK_RTRN( 0xC0DE, 0x0000 );

    // index and !add, wback
    instr = op | P | W;
    R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xC0DE, 0x0004 );
    // index and add, wback
    instr = op | U | P | W;
    R[n] = 0x0002; R[m] = 0x0004; CHECK_RTRN( 0x1337, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRHT_A1_test )
{
    // (A8.6.77, p.470)
    SETUP_TEST;
    const uint32_t op = 0xF07010B0;
    const uint32_t n = 0, t = 1;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRHT_A1;
    uint32_t instr;

    proc.dMem.write_half( address, 0x1337 );

    // Load register
    // add
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x1337, 0x0006 );
    // !add
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x1337, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LDRHT_A2_test )
{
    // (A8.6.77, p.470)
    SETUP_TEST;
    const uint32_t op = 0xF03010B2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRHT_A2;
    uint32_t instr;

    proc.dMem.write_half( address, 0x1337 );

    // Load register
    // add
    instr = op | U; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x1337, 0x0006 );
    // !add
    instr = op    ; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x1337, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LDRSB_imm_A1_test )
{
    // (A8.6.78, p.472)
    SETUP_TEST;
    const uint32_t op = 0xF05010D0;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSB_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xDA );
    proc.dMem.write_byte( address+4, 0x1F );

    // Load register signed byte
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xFFFFFFDA, 0x0006 );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xFFFFFFDA, 0x0002 );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xFFFFFFDA, 0x02 );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xFFFFFFDA, 0x06 );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x02; CHECK_RTRN( 0xFFFFFFDA, 0x04 );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x0A; CHECK_RTRN( 0x1F, 0x08 );
}

BOOST_AUTO_TEST_CASE( LDRSB_lit_A1_test )
{
    // (A8.6.79, p.474)
    SETUP_TEST;
    const uint32_t op = 0xF15F00D0;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSB_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xB0 );
    proc.dMem.write_byte( address+4, 0x0B );

    // Load register byte
    instr = op | U | address;   CHECK_RT( 0xFFFFFFB0 );
    instr = op | U | (address+4); CHECK_RT( 0x0B );
}

BOOST_AUTO_TEST_CASE( LDRSB_reg_A1_test )
{
    // (A8.6.80, p.476)
    SETUP_TEST;
    const uint32_t op = 0xF01010D2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSB_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_byte( address, 0xF0 );
    proc.dMem.write_byte( address+4, 0x0F );

    // Load register byte
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xFFFFFFF0, 0x0003 );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xFFFFFFF0, 0x0005 );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xFFFFFFF0, 0x0005 );
    // index and add
    instr = op | U | P;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0x0F, 0x0004 );

    // index and !add, wback
    instr = op | P | W;
    R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xFFFFFFF0, 0x0004 );
    // index and add, wback
    instr = op | U | P | W;
    R[n] = 0x0004; R[m] = 0x0004; CHECK_RTRN( 0x0F, 0x0008 );
}

BOOST_AUTO_TEST_CASE( LDRSBT_A1_test )
{
    // (A8.6.81, p.478)
    SETUP_TEST;
    const uint32_t op = 0xF07010D0;
    const uint32_t n = 0, t = 1;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSBT_A1;
    uint32_t instr;

    proc.dMem.write_byte( address, 0x15 );
    proc.dMem.write_byte( address+4, 0x81 );

    // Load register
    // add
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x00000015, 0x0006 );
    instr = op | U | 0x002; R[n] = 0x0008; CHECK_RTRN( 0xFFFFFF81, 0x000A );
    // !add
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x00000015, 0x0002 );
    instr = op     | 0x002; R[n] = 0x0008; CHECK_RTRN( 0xFFFFFF81, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRSBT_A2_test )
{
    // (A8.6.81, p.478)
    SETUP_TEST;
    const uint32_t op = 0xF03010D2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSBT_A2;
    uint32_t instr;

    proc.dMem.write_byte( address, 0x15 );
    proc.dMem.write_byte( address+4, 0x81 );

    // Load register
    // add
    instr = op | U; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x00000015, 0x0006 );
    instr = op | U; R[n] = 0x0008; R[m] = 0x0002;
    CHECK_RTRN( 0xFFFFFF81, 0x000A );
    // !add
    instr = op    ; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x00000015, 0x0002 );
    instr = op    ; R[n] = 0x0008; R[m] = 0x0002;
    CHECK_RTRN( 0xFFFFFF81, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRSH_imm_A1_test )
{
    // (A8.6.82, p.480)
    SETUP_TEST;
    const uint32_t op = 0xF05010F0;
    const uint32_t n = 0, t = 1;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSH_imm_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register immediate
    // !index and add, wback
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xFFFFC0DE, 0x0006 );
    // !index and !add, wback
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0xFFFFC0DE, 0x0002 );

    // index and add
    instr = op | P|U | 0x002; R[n] = 0x02; CHECK_RTRN( 0xFFFFC0DE, 0x02 );
    // index and !add
    instr = op | P   | 0x002; R[n] = 0x06; CHECK_RTRN( 0xFFFFC0DE, 0x06 );

    // index and add, wback
    instr = op | P|U|W | 0x002; R[n] = 0x04; CHECK_RTRN( 0x1337, 0x06 );
    // index and !add, wback
    instr = op | P  |W | 0x002; R[n] = 0x08; CHECK_RTRN( 0x1337, 0x06 );
}

BOOST_AUTO_TEST_CASE( LDRSH_lit_A1_test )
{
    // (A8.6.83, p.482)
    SETUP_TEST;
    const uint32_t op = 0xF15F00F0;
    const uint32_t t = 0;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSH_lit_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_half( address, 0xC0DE );

    // Load register literal
    instr = op | U | address; CHECK_RT( 0xFFFFC0DE );
}

BOOST_AUTO_TEST_CASE( LDRSH_reg_A1_test )
{
    // (A8.6.84, p.484)
    SETUP_TEST;
    const uint32_t op = 0xF01010F2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t P = 0x1 << 24;
    const uint32_t U = 0x1 << 23;
    const uint32_t W = 0x1 << 21;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSH_reg_A1;
    uint32_t instr;

    // write value in memory to test
    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register register
    // !index and !add, wback
    instr = op; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xFFFFC0DE, 0x0003 );
    // !index and add, wback
    instr = op | U; R[n] = 0x0004; R[m] = 0x0001; CHECK_RTRN( 0xFFFFC0DE, 0x0005 );

    // index and !add
    instr = op | P; R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xFFFFC0DE, 0x0005 );
    // index and add
    instr = op | U | P;
    R[n] = 0x0000; R[m] = 0x0004; CHECK_RTRN( 0xFFFFC0DE, 0x0000 );

    // index and !add, wback
    instr = op | P | W;
    R[n] = 0x0005; R[m] = 0x0001; CHECK_RTRN( 0xFFFFC0DE, 0x0004 );
    // index and add, wback
    instr = op | U | P | W;
    R[n] = 0x0002; R[m] = 0x0004; CHECK_RTRN( 0x1337, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRSHT_A1_test )
{
    // (A8.6.85, p.486)
    SETUP_TEST;
    const uint32_t op = 0xF07010F0;
    const uint32_t n = 0, t = 1;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSHT_A1;
    uint32_t instr;

    proc.dMem.write_half( address, 0x1337 );
    proc.dMem.write_half( address+4, 0xECC1 );

    // Load register
    // add
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x00001337, 0x0006 );
    instr = op | U | 0x002; R[n] = 0x0008; CHECK_RTRN( 0xFFFFECC1, 0x000A );
    // !add
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x00001337, 0x0002 );
    instr = op     | 0x002; R[n] = 0x0008; CHECK_RTRN( 0xFFFFECC1, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRSHT_A2_test )
{
    // (A8.6.85, p.486)
    SETUP_TEST;
    const uint32_t op = 0xF03010F2;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRSHT_A2;
    uint32_t instr;

    proc.dMem.write_half( address, 0x1337 );
    proc.dMem.write_half( address+4, 0xECC1 );

    // Load register
    // add
    instr = op | U; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x00001337, 0x0006 );
    instr = op | U; R[n] = 0x0008; R[m] = 0x0002;
    CHECK_RTRN( 0xFFFFECC1, 0x000A );
    // !add
    instr = op    ; R[n] = 0x0004; R[m] = 0x0002;
    CHECK_RTRN( 0x00001337, 0x0002 );
    instr = op    ; R[n] = 0x0008; R[m] = 0x0002;
    CHECK_RTRN( 0xFFFFECC1, 0x0006 );
}

BOOST_AUTO_TEST_CASE( LDRT_A1_test )
{
    // (A8.6.86, p.488)
    SETUP_TEST;
    const uint32_t op = 0xF4301000;
    const uint32_t n = 0, t = 1;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRT_A1;
    uint32_t instr;

    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register
    // add
    instr = op | U | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x1337C0DE, 0x0006 );
    // !add
    instr = op     | 0x002; R[n] = 0x0004; CHECK_RTRN( 0x1337C0DE, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LDRT_A2_test )
{
    // (A8.6.86, p.488)
    SETUP_TEST;
    const uint32_t op = 0xF6301002;
    const uint32_t n = 0, t = 1, m = 2;
    const uint32_t U = 0x1 << 23;
    const uint32_t address = 0x00000004;
    BehaviorFunc func = arm::LDRT_A2;
    uint32_t instr;

    proc.dMem.write_word( address, 0x1337C0DE );

    // Load register, type = LSL = 0b00
    // add
    instr = op | U | (0x01 << 7); R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x1337C0DE, 0x0006 );
    // !add
    instr = op     | (0x01 << 7); R[n] = 0x0004; R[m] = 0x0001;
    CHECK_RTRN( 0x1337C0DE, 0x0002 );
}

BOOST_AUTO_TEST_CASE( LSL_imm_A1_test )
{
    // (A8.6.88, p.490)
    SETUP_TEST;
    const uint32_t op = 0xF1A00001;
    const uint32_t d = 0, m = 1;
    BehaviorFunc func = arm::LSL_imm_A1;
    uint32_t instr;

    // MOV_reg_A1 (A8.6.97, p.508)
    instr = op;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFFFFFF );

    // Logical Shift Left
    instr = op | (0x01 << 7); R[m] = 0x33333333; CHECK_RD( 0x66666666 );
    instr = op | (0x04 << 7); R[m] = 0x12345678; CHECK_RD( 0x23456780 );
    instr = op | (0x1F << 7); R[m] = 0xFFFFFFFF; CHECK_RD( 0x80000000 );

    // set flags
    instr = op | (0x1 << 20) | (0x1 << 7);
    R[m] = 0x40000000; CHECK_CPSR( 1, 0, 0, 0 );
    R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );
    R[m] = 0x80000001; CHECK_CPSR( 0, 0, 1, 0 );
    R[m] = 0x80000000; CHECK_CPSR( 0, 1, 1, 0 );
    R[m] = 0xC0000000; CHECK_CPSR( 1, 0, 1, 0 );

    // !set flags
    instr = op | (0x1 << 7);
    R[m] = 0x40000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x80000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x80000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0xC0000000; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( LSL_reg_A1_test )
{
    // (A8.6.89, p.492)
    SETUP_TEST;
    const uint32_t op = 0xF1A00112;
    const uint32_t d = 0, m = 1, n = 2;
    BehaviorFunc func = arm::LSL_reg_A1;
    uint32_t instr;

    // Logical Shift Left
    instr = op;
    R[m] = 0x00000000; R[n] = 0x00000001; CHECK_RD( 0x00000001 );
    R[m] = 0x0000001F; R[n] = 0x00000001; CHECK_RD( 0x80000000 );
    R[m] = 0x00000004; R[n] = 0x87654321; CHECK_RD( 0x76543210 );

    // set flags
    instr = op | (0x1 << 20);
    R[m] = 0x00000001; R[n] = 0x70000000; CHECK_CPSR( 1, 0, 0, 0 );
    R[m] = 0x00000001; R[n] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );
    R[m] = 0x000000FF; R[n] = 0x80000000; CHECK_CPSR( 0, 1, 0, 0 );
    R[m] = 0x00000001; R[n] = 0x80000001; CHECK_CPSR( 0, 0, 1, 0 );
    R[m] = 0x00000001; R[n] = 0x80000000; CHECK_CPSR( 0, 1, 1, 0 );
    R[m] = 0x00000001; R[n] = 0xC0000000; CHECK_CPSR( 1, 0, 1, 0 );

    // !set flags
    instr = op;
    R[m] = 0x00000001; R[n] = 0x70000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x00000001; R[n] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x000000FF; R[n] = 0x80000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x00000001; R[n] = 0x80000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x00000001; R[n] = 0x80000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x00000001; R[n] = 0xC0000000; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( LSR_imm_A1_test )
{
    // (A8.6.90, p.494)
    SETUP_TEST;
    const uint32_t op = 0xF1A00021;
    const uint32_t d = 0, m = 1;
    BehaviorFunc func = arm::LSR_imm_A1;
    uint32_t instr;

    // Logical Shift Right
    instr = op | (0x01 << 7); R[m] = 0x00000002; CHECK_RD( 0x00000001 );
    instr = op | (0x1F << 7); R[m] = 0x80000000; CHECK_RD( 0x00000001 );
    instr = op | (0x0C << 7); R[m] = 0x12345678; CHECK_RD( 0x00012345 );

    // set flags
    uint32_t S = 0x1 << 20;
    instr = op | S | (0x01 << 7); R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );

    // !set flags
    instr = op | (0x01 << 7); R[m] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );

    // check ALUWritePC
    uint32_t Rd = (15 << 12);
    instr = op | Rd | (0x01 << 7); R[m] = 0x0000000F; CHECK_PC( 0x00000006 );
}

BOOST_AUTO_TEST_CASE( LSR_reg_A1_test )
{
    // (A8.6.91, p.496)
    SETUP_TEST;
    const uint32_t op = 0xF1A00132;
    const uint32_t d = 0, m = 1, n = 2;
    BehaviorFunc func = arm::LSR_reg_A1;
    uint32_t instr;

    // Logical Shift Right
    instr = op;
    R[m] = 0x00000000; R[n] = 0x00000001; CHECK_RD( 0x00000001 );
    R[m] = 0x0000001F; R[n] = 0x80000000; CHECK_RD( 0x00000001 );
    R[m] = 0x00000004; R[n] = 0x12345678; CHECK_RD( 0x01234567 );

    // set flags
    instr = op | (0x1 << 20);
    R[m] = 0x00000001; R[n] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );

    // !set flags
    instr = op;
    R[m] = 0x00000001; R[n] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( MLA_A1_test )
{
    // (A8.6.94, p.502)
    SETUP_TEST;
    const uint32_t op = 0xF0201293;
    const uint32_t d = 0, a = 1, m = 2, n = 3;
    BehaviorFunc func = arm::MLA_A1;
    uint32_t instr;

    // Multiply Accumulate, n * m + a = d
    instr = op;
    R[n] =  0; R[m] = 42; R[a] =  0; CHECK_RD( 0 );
    R[n] =  0; R[m] = 42; R[a] = 17; CHECK_RD( 17 );
    R[n] =  5; R[m] =  3; R[a] =  0; CHECK_RD( 15 );
    R[n] =  2; R[m] = 81; R[a] = 38; CHECK_RD( 200 );

    // set flags
    instr = op | (0x1 << 20);
    R[n] =  1; R[m] =          1; R[a] = 0x80000000; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] =  0; R[m] =         42; R[a] =          0; CHECK_CPSR( 0, 1, 0, 0 );

    // !set flags
    instr = op;
    R[n] =  1; R[m] =          1; R[a] = 0x80000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] =  0; R[m] =         42; R[a] =          0; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( MLS_A1_test )
{
    // (A8.6.95, p.504)
    SETUP_TEST;
    const uint32_t op = 0xF0601293;
    const uint32_t d = 0, a = 1, m = 2, n = 3;
    BehaviorFunc func = arm::MLS_A1;
    uint32_t instr;

    // Multiply and Subtract, a - n * m = d
    instr = op;
    R[a] =  0; R[n] =  0; R[m] =  0; CHECK_RD(  0 );
    R[a] =  0; R[n] =  0; R[m] =  1; CHECK_RD(  0 );
    R[a] =  0; R[n] =  1; R[m] =  0; CHECK_RD(  0 );
    R[a] =  1; R[n] =  0; R[m] =  0; CHECK_RD(  1 );
    R[a] =  1; R[n] =  1; R[m] =  1; CHECK_RD(  0 );
    R[a] = 91; R[n] =  7; R[m] =  7; CHECK_RD( 42 );
}

BOOST_AUTO_TEST_CASE( MOV_imm_A1_test )
{
    // (A8.6.96, p.506)
    SETUP_TEST;
    const uint32_t op = 0xF3A00000;
    const uint32_t d = 0;
    BehaviorFunc func = arm::MOV_imm_A1;
    uint32_t instr;

    // Move
    instr = op | 0x000; CHECK_RD( 0x00000000 );
    instr = op | 0x042; CHECK_RD( 0x00000042 );
    instr = op | 0x0FF; CHECK_RD( 0x000000FF );

    // set flags
    instr = op | (0x1 << 20) | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    instr = op | (0x1 << 20) | 0x1FF; CHECK_CPSR( 1, 0, 1, 0 );

    // !set flags
    instr = op | 0x000; CHECK_CPSR( 0, 0, 0, 0 );
    instr = op | 0x1FF; CHECK_CPSR( 0, 0, 0, 0 );

    // check ALUWritePC
    uint32_t Rd = (15 << 12);
    instr = op | Rd | 0x00F; CHECK_PC( 0x0000000E );
}

BOOST_AUTO_TEST_CASE( MOV_imm_A2_test )
{
    // (A8.6.96, p.506)
    SETUP_TEST;
    const uint32_t op = 0xF3000000;
    const uint32_t d = 0;
    BehaviorFunc func = arm::MOV_imm_A2;
    uint32_t instr;

    // Move
    instr = op;                         CHECK_RD( 0x00000000 );
    instr = op | 0xB0B;                 CHECK_RD( 0x00000B0B );
    instr = op | (0xC << 16) | 0x0DE;   CHECK_RD( 0x0000C0DE );
    instr = op | (0xA << 16);           CHECK_RD( 0x0000A000 );
}

BOOST_AUTO_TEST_CASE( MOV_reg_A1_test )
{
    // (A8.6.97, p.508)
    SETUP_TEST;
    const uint32_t op = 0xF1A00001;
    const uint32_t d = 0, m = 1;
    BehaviorFunc func = arm::MOV_reg_A1;
    uint32_t instr;

    // Move
    instr = op;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0x43215678; CHECK_RD( 0x43215678 );

    // set flags
    instr = op | (0x1 << 20);
    R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );
    R[m] = 0x80000000; CHECK_CPSR( 1, 0, 0, 0 );

    // !set flags
    instr = op;
    R[m] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0x80000000; CHECK_CPSR( 0, 0, 0, 0 );

    // check ALUWritePC
    instr = op | (15 << 12);
    R[m] = 0x0000000F; CHECK_PC( 0x0000000E );
}

BOOST_AUTO_TEST_CASE( MOVT_A1_test )
{
    // (A8.6.99, p.512)
    SETUP_TEST;
    const uint32_t op = 0xF3400000;
    const uint32_t d = 0;
    BehaviorFunc func = arm::MOVT_A1;
    uint32_t instr;

    // Move Top
    instr = op;                         CHECK_RD( 0x00000000 );
    instr = op | 0xB0B;                 CHECK_RD( 0x0B0B0000 );
    instr = op | (0xC << 16) | 0x0DE;   CHECK_RD( 0xC0DE0000 );
    instr = op | (0xA << 16);           CHECK_RD( 0xA0000000 );

    // Lower bits in RD do not change
    R[d] = 0x0000C0DE;
    instr = op | (0xC << 16) | 0xAFE;   CHECK_RD( 0xCAFEC0DE );
}

BOOST_AUTO_TEST_CASE( MRS_A1_test )
{
    // (A8.6.102, p.518)
    SETUP_TEST;
    const uint32_t op = 0xF10F0000;
    const uint32_t d = 0;
    BehaviorFunc func = arm::MRS_A1;
    uint32_t instr;

    uint32_t cpsr = (proc.CPSR.N        << 31) |
                    (proc.CPSR.Z        << 30) |
                    (proc.CPSR.C        << 29) |
                    (proc.CPSR.V        << 28) |
                    (proc.CPSR.Q        << 27) |
                    (proc.CPSR.IT_L     << 25) |
                    (proc.CPSR.J        << 24) |
                    (proc.CPSR.reserved << 20) |
                    (proc.CPSR.GE       << 16) |
                    (proc.CPSR.IT_H     << 10) |
                    (proc.CPSR.E        <<  9) |
                    (proc.CPSR.A        <<  8) |
                    (proc.CPSR.I        <<  7) |
                    (proc.CPSR.F        <<  6) |
                    (proc.CPSR.T        <<  5) |
                    proc.CPSR.M;

    // Move to register from special register
    instr = op; CHECK_RD( cpsr );
}

BOOST_AUTO_TEST_CASE( MSR_imm_A1_test )
{
    // (A8.6.103, p.520)
    SETUP_TEST;
    const uint32_t op = 0xF320F000;
    BehaviorFunc func = arm::MSR_imm_A1;
    uint32_t instr;

    // Move immediate value to special register
    instr = op | (0x0 << 18) | 0x4FF; CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    instr = op | (0x0 << 18) | 0x8FF; CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    instr = op | (0x1 << 18) | 0x8FF; CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 15 );
    instr = op | (0x2 << 18) | 0x4FF; CHECK_CPSR_EXT( 1, 1, 1, 1, 1, 0 );
}

BOOST_AUTO_TEST_CASE( MSR_reg_A1_test )
{
    // (A8.6.104, p.522)
    SETUP_TEST;
    const uint32_t op = 0xF120F000;
    const uint32_t n = 0;
    BehaviorFunc func = arm::MSR_reg_A1;
    uint32_t instr;

    // Move to special register
    instr = op | (0x0 << 18); R[n] = 0xFFFFFFFF; CHECK_CPSR_EXT(0,0,0,0,0,0);
    instr = op | (0x1 << 18); R[n] = 0xFFFFFFFF; CHECK_CPSR_EXT(0,0,0,0,0,15);
    instr = op | (0x2 << 18); R[n] = 0xFFFFFFFF; CHECK_CPSR_EXT(1,1,1,1,1,0);
    instr = op | (0x3 << 18); R[n] = 0xFFFFFFFF; CHECK_CPSR_EXT(1,1,1,1,1,15);
}

BOOST_AUTO_TEST_CASE( MUL_A1_test )
{
    // (A8.6.105, p.524)
    SETUP_TEST;
    const uint32_t op = 0xF0000192;
    const uint32_t d = 0, m = 1, n = 2;
    BehaviorFunc func = arm::MUL_A1;
    uint32_t instr;

    // Multiplication n * m = d
    instr = op;
    R[n] =  0; R[m] =  0; CHECK_RD(  0 );
    R[n] =  0; R[m] =  7; CHECK_RD(  0 );
    R[n] =  6; R[m] =  0; CHECK_RD(  0 );
    R[n] =  6; R[m] =  7; CHECK_RD( 42 );

    // check set flags
    instr = op | (0x1 << 20);
    R[n] = 0x80000000; R[m] = 0x00000001; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000000; R[m] = 0x00000001; CHECK_CPSR( 0, 1, 0, 0 );

    // check !set flags
    instr = op;
    R[n] = 0x80000000; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000000; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( MVN_imm_A1_test )
{
    // (A8.6.106, p.526)
    SETUP_TEST;
    const uint32_t op = 0xF3E00000;
    const uint32_t d = 0;
    BehaviorFunc func = arm::MVN_imm_A1;
    uint32_t instr;

    // Bitwise NOT
    instr = op;                         CHECK_RD( 0xFFFFFFFF );
    instr = op | 0x0FF;                 CHECK_RD( 0xFFFFFF00 );

    // check set flags
    instr = op | (0x1 << 20);         CHECK_CPSR( 1, 0, 0, 0 );

    // check !set flags
    instr = op; CHECK_CPSR( 0, 0, 0, 0 );

    // check ALUWritePC
    instr = op | (15 << 12); CHECK_PC( 0xFFFFFFFE );
}

BOOST_AUTO_TEST_CASE( MVN_reg_A1_test )
{
    // (A8.6.107, p.528)
    SETUP_TEST;
    const uint32_t op = 0xF1E00001;
    const uint32_t d = 0, m = 1;
    BehaviorFunc func = arm::MVN_reg_A1;
    uint32_t instr;
    
    // type = 0b00 = SRType_LSL, imm5
    // Bitwise NOT
    instr = op;
    R[m] = 0x00000000; CHECK_RD( 0xFFFFFFFF );
    R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
    instr = op | (0x00004 << 7); R[m] = 0x0000FFFF; CHECK_RD( 0xFFF0000F );

    // check set flags
    instr = op | (0x1 << 20);
    R[m] = 0x00000000; CHECK_CPSR( 1, 0, 0, 0 );
    R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 1, 0, 0 );

    // check !set flags
    instr = op;
    R[m] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );

    // check ALUWritePC
    instr = op | (15 << 12); R[m] = 0x00000000; CHECK_PC( 0xFFFFFFFE );
}

BOOST_AUTO_TEST_CASE( MVN_rsr_A1_test )
{
    // (A8.6.108, p.530)
    SETUP_TEST;
    const uint32_t op = 0xF1E00112;
    const uint32_t d = 0, s = 1, m = 2;
    BehaviorFunc func = arm::MVN_rsr_A1;
    uint32_t instr;

    // type = 0b00 = SRType_LSL, R[s]
    // Bitwise NOT, register-shifted register
    instr = op;
    R[s] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0xFFFFFFFF );
    R[s] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
    R[s] = 0x00000004; R[m] = 0x0000FFFF; CHECK_RD( 0xFFF0000F );

    // check set flags
    instr = op | (0x1 << 20);
    R[s] = 0x00000000; R[m] = 0x00000000; CHECK_CPSR( 1, 0, 0, 0 );
    R[s] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 1, 0, 0 );

    // check !set flags
    instr = op;
    R[s] = 0x00000000; R[m] = 0x00000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[s] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( NOP_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF320F000;
    BehaviorFunc func = arm::NOP_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( ORR_imm_A1_test )
{
    SETUP_TEST;

    // Testing flags and no-flags versions
    const uint32_t instr_flags   = 0xF3934E5B;
    const uint32_t instr_noflags = 0xF3834E5B;

    // Test write to PC (d = 15)
    const uint32_t instr_wPC     = 0xF383F000;
    const uint32_t n = 3, d = 4, imm12 = 0xE5B;

    arm::UValueCarry expanded = arm::ARMExpandImm_C( imm12, false );

    BehaviorFunc func = arm::ORR_imm_A1;
    uint32_t instr = instr_flags;
    R[n] = 0xF00DBEEF; CHECK_RD( 0xF00DBEEF | expanded.value );
    CHECK_CPSR( 1, 0, expanded.carry, 0 );

    instr = instr_noflags;
    R[n] = 0xF00DBEEF; CHECK_RD( 0xF00DBEEF | expanded.value );
    CHECK_CPSR( 0, 0, 0, 0 );

    instr = instr_wPC;
    R[n] = 0xF00D0000; CHECK_PC( (0xF00D0000) );
    CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( ORR_reg_A1_test )
{
    SETUP_TEST;

    // Testing flags and no-flags versions
    const uint32_t instr_flags   = 0xF1934446;
    const uint32_t instr_noflags = 0xF1834446;
    const uint32_t m = 6, n = 3, d = 4, imm5 = 0x8, type = 0x2;
    const uint32_t op1 = 0xFFF0F0F0;
    const uint32_t op2 = 0xDEADC080;

    arm::ShiftUValue shift = arm::DecodeImmShift( type, imm5 );
    arm::UValueCarry shifted = arm::Shift_C( op2, shift.shift_t,
                                             shift.shift_n, false );

    BehaviorFunc func = arm::ORR_reg_A1;
    uint32_t instr = instr_flags;
    R[m] = op2; R[n] = op1; CHECK_RD( shifted.value | R[n] );
    CHECK_CPSR( 1, 0, shifted.carry, 0 );

    instr = instr_noflags;
    R[m] = op2; R[n] = op1; CHECK_RD( shifted.value | R[n] );
    CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( ORR_reg_shift_reg_A1_test )
{
    SETUP_TEST;

    // Testing update flags and don't update flags versions
    const uint32_t instr_flags   = 0xF1934236;
    const uint32_t instr_noflags = 0xF1834236;
    const uint32_t m = 6, n = 3, d = 4, s = 2, type = 0x1;
    const uint32_t op1 = 0xF0000000;
    const uint32_t op2 = 0xDEADC0DE;
    const uint32_t shift_n = 4;
    const arm::SRType shift_t = arm::DecodeRegShift( type );

    const arm::UValueCarry shifted = arm::Shift_C( op2, shift_t,
                                                   shift_n, false );

    BehaviorFunc func = arm::ORR_reg_A1;

    uint32_t instr = instr_flags;
    R[s] = shift_n | 0xF << 9; // The added bits should be ignored
    R[m] = op2; R[n] = op1;

    CHECK_RD( shifted.value | R[n] );
    CHECK_CPSR( 1, 0, shifted.carry, 0 );

    instr = instr_noflags;
    R[s] = shift_n | 0xF << 9;  // The added bits should be ignored
    R[m] = op2; R[n] = op1;

    CHECK_RD( shifted.value | R[n] );
    CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( PKH_A1_test )
{
    SETUP_TEST;

    const uint32_t m = 1, n = 2, d = 3, imm5 = 8,
        cst1 = 0xDDDD0000, cst2 = 0x0000AAAA;

    // tb = 0
    uint32_t instr = 0xF6823411;
    arm::ShiftUValue shift = arm::DecodeImmShift( 0, imm5 );
    uint32_t operand2 = Shift( cst1, shift.shift_t, shift.shift_n, false );

    BehaviorFunc func = arm::PKH_A1;
    R[m] = cst1; R[n] = cst2;
    CHECK_RD( (0xFFFF & cst2) | ((0xFFFF0000 & operand2)) );
    CHECK_CPSR( 0, 0, 0, 0 );

    // tb = 1
    instr = 0xF6823451;
    shift = arm::DecodeImmShift( 2, imm5 );
    operand2 = Shift( cst1, shift.shift_t, shift.shift_n, false );
    R[m] = cst1; R[n] = cst2;
    CHECK_RD( (0xFFFF & operand2) | ((0xFFFF0000 & cst2)) );
    CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( PLD_imm_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF510FAAA;
    BehaviorFunc func = arm::PLD_imm_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( PLD_lit_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF55FF555;
    BehaviorFunc func = arm::PLD_lit_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( PLD_reg_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF55FF555;
    BehaviorFunc func = arm::PLD_reg_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( PLI_imm_lit_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF450FAAA;
    BehaviorFunc func = arm::PLI_imm_lit_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( PLI_reg_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF65FF50A;
    BehaviorFunc func = arm::PLI_reg_A1;

    // Confirm no registers where changed
    for( int d = 0; d < 16; ++d )
    {
        CHECK_RD( proc.R[d] );
    }

    // Confirm processor state registers are not modified
    CHECK_GE( 0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    CHECK_PC( proc.PC );
}

BOOST_AUTO_TEST_CASE( POP_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF8BD0036;
    BehaviorFunc func = arm::POP_A1;

    // Pop regs {1, 2, 4, 5}
    proc.dMem.write_word( 0,  0xCAFEC0DE ); // Reg 1
    proc.dMem.write_word( 4,  0xA407BABE ); // Reg 2
    proc.dMem.write_word( 8,  0x800081E5 ); // Reg 4
    proc.dMem.write_word( 12, 0x1337C0DE ); // Reg 5

    func( proc, instr );
    BOOST_CHECK_EQUAL( 0xCAFEC0DE, R[1] );
    BOOST_CHECK_EQUAL( 0xA407BABE, R[2] );
    BOOST_CHECK_EQUAL( 0x800081E5, R[4] );
    BOOST_CHECK_EQUAL( 0x1337C0DE, R[5] );
}

BOOST_AUTO_TEST_CASE( POP_A2_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF49DA004;
    const uint32_t t = 10;
    BehaviorFunc func = arm::POP_A2;

    // Pop register 10
    proc.dMem.write_word( 0,  0x7E577E57 ); // Reg 10

    func( proc, instr );
    BOOST_CHECK_EQUAL( 0x7E577E57, R[t] );
}

BOOST_AUTO_TEST_CASE( PUSH_A1_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF92D0016;
    BehaviorFunc func = arm::PUSH_A1;
    const uint32_t r1 = 42, r2 = 24, r4 = 84;

    // Push regs {1, 2 and 4}
    R[13] = 12;
    R[1] = r1; R[2] = r2; R[4] = r4;

    func( proc, instr );

    BOOST_CHECK_EQUAL( proc.dMem.read_word( 0 ) /* Reg 1 */, r1 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( 4 ) /* Reg 2 */, r2 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( 8 ) /* Reg 4 */, r4 );
}

BOOST_AUTO_TEST_CASE( PUSH_A2_test )
{
    SETUP_TEST;

    const uint32_t instr = 0xF52DA000;
    const uint32_t r10 = 0x44444199;
    BehaviorFunc func = arm::PUSH_A2;

    // Push register 10
    R[10] = r10; R[13] = 4;

    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( 0 ) /* Reg 10 */, r10 );
}

BOOST_AUTO_TEST_CASE( QADD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1001052;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QADD_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0x0000000B; R[m] = 0xFFFFFFFE; CHECK_RD( 0x00000009 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFC; R[m] = 0xFFFFFFFD; CHECK_RD( 0xFFFFFFF9 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0x00000004; R[m] = 0xFFFFFFFB; CHECK_RD( 0xFFFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0x7FFFFFFE; R[m] = 0x00000002; CHECK_RD( 0x7FFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[n] = 0x7FFFFFFD; R[m] = 0x00000100; CHECK_RD( 0x7FFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[n] = 0x80000000; R[m] = 0xFFFFFFFE; CHECK_RD( 0x80000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( QADD16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201F12;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QADD16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A000B; R[m] = 0xFFFFFFFE; CHECK_RD( 0x00090009 );
    R[n] = 0xFF00FFFC; R[m] = 0x0000FFFD; CHECK_RD( 0xFF00FFF9 );
    R[n] = 0xFF000004; R[m] = 0x0000FFFB; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFF000004; R[m] = 0x00FF0000; CHECK_RD( 0xFFFF0004 );
    R[n] = 0x7FFE7FFD; R[m] = 0x00010002; CHECK_RD( 0x7FFF7FFF );
    R[n] = 0x7FFE7FFD; R[m] = 0x00100100; CHECK_RD( 0x7FFF7FFF );
    R[n] = 0x80008000; R[m] = 0xFFFFFFFE; CHECK_RD( 0x80008000 );
    R[n] = 0xFFFFFFFF; R[m] = 0x00010001; CHECK_RD( 0x00000000 );
}

BOOST_AUTO_TEST_CASE( QADD8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201F92;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QADD8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0xFFFEFDFC; CHECK_RD( 0x09090909 );
    R[n] = 0x01020304; R[m] = 0x02030405; CHECK_RD( 0x03050709 );
    R[n] = 0xA004B005; R[m] = 0x00FB00FA; CHECK_RD( 0xA0FFB0FF );
    R[n] = 0x80808080; R[m] = 0xFFFEFDFC; CHECK_RD( 0x80808080 );
    R[n] = 0x7F7E7D7C; R[m] = 0x01020304; CHECK_RD( 0x7F7F7F7F );
    R[n] = 0x7F7E7D7C; R[m] = 0x10101010; CHECK_RD( 0x7F7F7F7F );
    R[n] = 0xFFFFFFFF; R[m] = 0x01010101; CHECK_RD( 0x00000000 );
}

BOOST_AUTO_TEST_CASE( QASX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201F32;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QASX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    
    R[n] = 0xFBCA0148 /*-1078,328*/; R[m] = 0xFFF400B2 /*-12,178*/;
    CHECK_RD( 0xFC7C0154 );
}

BOOST_AUTO_TEST_CASE( QDADD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1401052;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QDADD_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );

    R[n] = 0x7FFFFFFF; R[m] = 0x7FFFFFFF; CHECK_RD( 0x7FFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );

    R[n] = /*-950*/ 0xFFFFFC4A; R[m] = 18; CHECK_RD( 0xFFFFF8A6 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( QDSUB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1601052;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QDSUB_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );

    R[n] = 0x7FFFFFFF; R[m] = 0x7FFFFFFF; CHECK_RD( 0x0 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );

    R[n] = /*-950*/ 0xFFFFFC4A; R[m] = 18; CHECK_RD( 0x77E );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( QSAX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201F52;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QSAX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );

    R[n] = /*64,-1882*/ 0x0040F8A6; R[m] = /*18,55*/0x00120037;
    CHECK_RD( 0x0009F8B8 );
}

BOOST_AUTO_TEST_CASE( QSUB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1201052;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QSUB_A1;
    R[m] = 0x00000000; R[n] = 0x00000000; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[m] = 0x0000000B; R[n] = 0x00000002; CHECK_RD( 0x00000009 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[m] = 0xFFFFFFFC; R[n] = 0x00000003; CHECK_RD( 0xFFFFFFF9 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[m] = 0x00000004; R[n] = 0x00000005; CHECK_RD( 0xFFFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[m] = 0x7FFFFFFE; R[n] = 0xFFFFFFFE; CHECK_RD( 0x7FFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[m] = 0x7FFFFFFD; R[n] = 0xFFFFFF00; CHECK_RD( 0x7FFFFFFF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[m] = 0x80000000; R[n] = 0x00000002; CHECK_RD( 0x80000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
    R[m] = 0xFFFFFFFF; R[n] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( QSUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201F72;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QSUB16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A000B; R[m] = 0x00010002; CHECK_RD( 0x00090009 );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0xFF00FFF9 );
    R[n] = 0xFF000004; R[m] = 0x00000005; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFF000004; R[m] = 0xFF010000; CHECK_RD( 0xFFFF0004 );
    R[n] = 0x7FFE7FFD; R[m] = 0xFFFEFFFD; CHECK_RD( 0x7FFF7FFF );
    R[n] = 0x7FFE7FFD; R[m] = 0xFFF0FF00; CHECK_RD( 0x7FFF7FFF );
    R[n] = 0x80008000; R[m] = 0x00010002; CHECK_RD( 0x80008000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}

BOOST_AUTO_TEST_CASE( QSUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6201FF2;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::QSUB8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0x09090909 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0x03050709 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0xA0FFB0FF );
    R[n] = 0x80808080; R[m] = 0x01020304; CHECK_RD( 0x80808080 );
    R[n] = 0x7F7E7D7C; R[m] = 0xFFFEFDFC; CHECK_RD( 0x7F7F7F7F );
    R[n] = 0x7F7E7D7C; R[m] = 0xF0F0F0F0; CHECK_RD( 0x7F7F7F7F );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}

BOOST_AUTO_TEST_CASE( RBIT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6FF1F32;
    uint32_t d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::RBIT_A1;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0xCAFE2011; CHECK_RD( 0x88047F53 );
}

BOOST_AUTO_TEST_CASE( REV_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6BF1F32;
    uint32_t d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::REV_A1;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0xCAFE2011; CHECK_RD( 0x1120FECA );
}

BOOST_AUTO_TEST_CASE( REV16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6BF1FB2;
    uint32_t d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::REV16_A1;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0xCAFE2011; CHECK_RD( 0xFECA1120 );
}

BOOST_AUTO_TEST_CASE( REVSH_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6FF1FB2;
    uint32_t d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::REVSH_A1;
    R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[m] = 0xFAFABE98 ; CHECK_RD( 0xFFFF98BE );
}

BOOST_AUTO_TEST_CASE( SEL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6801FB2;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::SEL_A1;
    R[m] = 0xAABBCCDD; R[n] = 0xEEFF8899;
    proc.CPSR.GE = 0xD;
    CHECK_RD( 0xAABBCC99 );        
}

BOOST_AUTO_TEST_CASE( SHADD16_A1_test )
{
    SETUP_TEST;
    const uint32_t instr = 0xF6301F12;
    const uint32_t n = 0, d = 1, m = 2;

    BehaviorFunc func = arm::SHADD16_A1;
    R[n] =         15; R[m] =         -2; CHECK_RD( 0xffff0006 );
    R[n] =         -1; R[m] =         -5; CHECK_RD( 0xfffffffd );
    R[n] =   10100101; R[m] =    3333333; CHECK_RD( 0x0066fd2d );
    R[n] =         15; R[m] =         22; CHECK_RD(         18 );
    R[n] = 0x00000014; R[m] = 0x00000016; CHECK_RD( 0x00000015 );
    R[n] = 0xffffffff; R[m] = 0x00002222; CHECK_RD( 0xffff1110 );
    R[n] = 0x7FFF0014; R[m] = 0x7FFF0016; CHECK_RD( 0x7FFF0015 );
}

BOOST_AUTO_TEST_CASE( SHADD8_A1_test )
{
    SETUP_TEST;
    const uint32_t instr = 0xF6301F92;
    const uint32_t n = 0, d = 1, m = 2;

    BehaviorFunc func = arm::SHADD8_A1;
    R[n] = 0x00000002; R[m] = 0x00000002; CHECK_RD( 0x00000002 );
    R[n] = 0x00020000; R[m] = 0x00020000; CHECK_RD( 0x00020000 );
    R[n] = 0x00020002; R[m] = 0x00020002; CHECK_RD( 0x00020002 );
    R[n] = 0x0f020002; R[m] = 0x01020002; CHECK_RD( 0x08020002 );
    R[n] = 0xffffffff; R[m] = 0x00000022; CHECK_RD( 0xffffff10 );
    R[n] =         20; R[m] =         22; CHECK_RD(         21 );
}

BOOST_AUTO_TEST_CASE( SHASX_A1_test )
{
    // (A8.6.161, p.634)
    SETUP_TEST;
    const uint32_t op = 0xF6302F31;
    const uint32_t n = 0, m = 1, d = 2;
    BehaviorFunc func = arm::SHASX_A1;
    uint32_t instr = op;
    uint32_t result;

    result  = ((0x00000003 + 0x00000007)/2) << 16;
    result |= (0x00000005 - 0x00000001)/2 & 0x0000FFFF;
    R[n] = 0x00030005; R[m] = 0x00010007; CHECK_RD( result );

    result  = ((0xFFFFFFFC + 0x00000006)/2) << 16;
    result |= (0x00000000 - 0x00000000)/2 & 0x0000FFFF;
    R[n] = 0xFFFC0000; R[m] = 0x00000006; CHECK_RD( result );

    result  = ((0xFFFFF111 + 0x00001006)/2) << 16;
    result |= (0x00000000 - 0x00000000)/2 & 0x0000FFFF;
    R[n] = 0xF1110000; R[m] = 0x00001006; CHECK_RD( result );

    result  = ((0xFFFFFFFE + 0xFFFFFFCC)/2) << 16;
    result |= (0x00000005 - 0x00000001)/2 & 0x0000FFFF;
    R[n] = 0xFFFE0005; R[m] = 0x0001FFCC; CHECK_RD( result );

    result  = ((0xFFFFFFFE + 0xFFFFFFCC)/2) << 16;
    result |= (0xFFFFFFAA - 0xFFFFABB1)/2 & 0x0000FFFF;
    R[n] = 0xFFFEFFAA; R[m] = 0xABB1FFCC; CHECK_RD( result );
}

BOOST_AUTO_TEST_CASE( SHSAX_A1_test )
{
    // (A8.6.161, p.634)
    SETUP_TEST;
    const uint32_t op = 0xF6302F51;
    const uint32_t n = 0, m = 1, d = 2;
    BehaviorFunc func = arm::SHSAX_A1;
    uint32_t instr = op;
    uint32_t result;

    result  = ((0x00000003 - 0x00000007)/2) << 16;
    result |= ((0x00000005 + 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0x00030005; R[m] = 0x00010007; CHECK_RD( result );

    result  = ((0x00000000 - 0x00000000)/2) << 16;
    result |= ((0x00000005 + 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0x00000005; R[m] = 0x00010000; CHECK_RD( result );

    result  = ((0x0000000A - 0x00000007)/2) << 16;
    result |= ((0x00000005 + 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0x000A0005; R[m] = 0x00010007; CHECK_RD( result );

    result  = ((0xFFFFF0AA - 0x00000007)/2) << 16;
    result |= ((0x00000005 + 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0xF0AA0005; R[m] = 0x00010007; CHECK_RD( result );

    result  = ((0xFFFFF0AA - 0xFFFFFF07)/2) << 16;
    result |= ((0xFFFFF005 + 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0xF0AAF005; R[m] = 0x0001FF07; CHECK_RD( result );

    result  = ((0xFFFFF0AA - 0xFFFFFF07)/2) << 16;
    result |= ((0xFFFFF005 + 0xFFFFFFFC)/2) & 0x0000FFFF;
    R[n] = 0xF0AAF005; R[m] = 0xFFFCFF07; CHECK_RD( result );
}

BOOST_AUTO_TEST_CASE( SHSUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6347F72;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SHSUB16_A1;
    uint32_t instr = op;
    uint32_t result;

    result  = ((0x00000003 - 0x00000007)/2) << 16;
    result |= ((0x00000005 - 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0x00030005; R[m] = 0x00070001; CHECK_RD( result );

    result  = ((0xFFFFFF03 - 0x00000007)/2) << 16;
    result |= ((0x00000005 - 0xFFFFA001)/2) & 0x0000FFFF;
    R[n] = 0xFF030005; R[m] = 0x0007A001; CHECK_RD( result );

    result  = ((0x00007003 - 0x00000A07)/2) << 16;
    result |= ((0x00000005 - 0x00000001)/2) & 0x0000FFFF;
    R[n] = 0x70030005; R[m] = 0x0A070001; CHECK_RD( result );
}

BOOST_AUTO_TEST_CASE( SHSUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6347FF2;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SHSUB8_A1;
    uint32_t instr = op;
    uint32_t result;

    result  = (((0x00000000 - 0x00000000)/2) & 0x000000FF) << 24;
    result |= (((0x00000003 - 0x00000007)/2) & 0x000000FF) << 16;
    result |= (((0x00000000 - 0x00000000)/2) & 0x000000FF) <<  8;
    result |= (((0x00000005 - 0x00000001)/2) & 0x000000FF);
    R[n] = 0x00030005; R[m] = 0x00070001; CHECK_RD( result );

    result  = (((0xFFFFFFFF - 0x00000000)/2) & 0x000000FF) << 24;
    result |= (((0x00000003 - 0x00000007)/2) & 0x000000FF) << 16;
    result |= (((0x00000000 - 0x00000000)/2) & 0x000000FF) <<  8;
    result |= (((0xFFFFFFF5 - 0x00000001)/2) & 0x000000FF);
    R[n] = 0xFF0300F5; R[m] = 0x00070001; CHECK_RD( result );

    result  = (((0xFFFFFFFF - 0x00000000)/2) & 0x000000FF) << 24;
    result |= (((0x00000003 - 0x00000007)/2) & 0x000000FF) << 16;
    result |= (((0x00000000 - 0x00000000)/2) & 0x000000FF) <<  8;
    result |= (((0xFFFFFFF5 - 0xFFFFFFF1)/2) & 0x000000FF);
    R[n] = 0xFF0300F5; R[m] = 0x000700F1; CHECK_RD( result );

    // NB: The rounding of negative numbers is not the same with /2
    //     or >>1. Therefor, here some of /2 has been replaced by >>1
    //     to fit with instruction's rounding implementation.
    result  = (((0xFFFFFFFF - 0x00000000)/2) & 0x000000FF) << 24;
    result |= (((0x00000003 - 0x00000007)/2) & 0x000000FF) << 16;
    result |= (((0x00000000 - 0x0000000F)>>1) & 0x000000FF) << 8;
    result |= (((0xFFFFFFF5 - 0x00000001)/2) & 0x000000FF);
    R[n] = 0xFF0300F5; R[m] = 0x00070F01; CHECK_RD( result );

    result  = (((0xFFFFFFFF - 0x00000000)/2) & 0x000000FF) << 24;
    result |= (((0x00000003 - 0x00000007)/2) & 0x000000FF) << 16;
    result |= (((0x00000000 - 0x0000000F)>>1) & 0x000000FF) << 8;
    result |= (((0xFFFFFFF5 - 0xFFFFFFF1)/2) & 0x000000FF);
    R[n] = 0xFF0300F5; R[m] = 0x00070FF1; CHECK_RD( result );
}

BOOST_AUTO_TEST_CASE( SMLAxy_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1071284;
    const uint32_t n = 4, m = 2, d = 7, a = 1;
    BehaviorFunc func = arm::SMLAxy_A1;
    uint8_t  N;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x00070001; R[n] = 0x00030005;  R[a] = 0x00070001; 
    M = 0; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0x00070006 );
    M = 0; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0x00070004 );
    M = 1; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0x00070024 );
    M = 1; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0x00070016 );

    R[m] = 0xF0070001; R[n] = 0x00030F05;  R[a] = 0x80070001; 
    M = 0; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0x80070F06 );
    M = 0; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0x80070004 );
    M = 1; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0x7F171924 );
    M = 1; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0x8006D016 );

    M = 1; N = 1; instr = op | M << 6 | N << 5; 
    R[m] = 0xF0070001; R[n] = 0x00030F05;  R[a] = 0x80070001; proc.CPSR.Q = 0;
    func( proc, instr ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 ); 

    R[m] = 0x7FFF7FFF; R[n] = 0x7FFF7FFF;  R[a] = 0x7FFFFFFF; proc.CPSR.Q = 0; 
    func( proc, instr ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 ); 
}

BOOST_AUTO_TEST_CASE( SMLAD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7070214;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SMLAD_A1;
    uint8_t  M;
    uint32_t instr;
    uint32_t a;

    a = 1;
    R[m] = 0xF0070001; R[n] = 0x00030F05; R[a] = 0x80070001; 
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x8006DF1B );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0x7F171927 );

    R[m] = 0xF0070001; R[n] = 0x00030F05; R[a] = 0x10F70001; 
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x10F6DF1B );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0x10071927 );

    R[m] = 0xF0070001; R[n] = 0x0003FF05; R[a] = 0x10F70001; 
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x10F6CF1B );

    a = 15; // Special case, R[a] should not be used
    R[m] = 0xF0070001; R[n] = 0x00030F05; R[a] = 0x10F70001; 
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0xFFFFDF1A );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0xFF101926 );
}

BOOST_AUTO_TEST_CASE( SMLAL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0E76294;
    const uint32_t n = 4, m = 2, dHi = 7, dLo = 6;
    BehaviorFunc func = arm::SMLAL_A1;
    uint8_t  S;
    uint32_t instr;

    // !setflags
    S = 0; instr = op | S << 20; 

    R[m] = 0x00000002; R[n] = 0x00030F05;
    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x00000000000E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    R[m] = 0x4F000002; R[n] = 0x00030F05;
    R[dHi] = 0x00001000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x000101A28B0E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    R[m] = 0x4F000002; R[n] = 0x00030F05;
    R[dHi] = 0x07001000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x070101A28B0E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    R[m] = 0xFF000002; R[n] = 0x00030F05;
    R[dHi] = 0x07001000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x07000CF0FB0E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    R[m] = 0xFF000002; R[n] = 0xF0030F05;
    R[dHi] = 0x07001000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x7100CF0DB0E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    // setflags
    S = 1; instr = op | S << 20; 

    R[m] = 0x00000002; R[n] = 0x00030F05;
    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    CHECK_RD64( 0x00000000000E1E0ALL ); CHECK_CPSR( 0, 0, 0, 0 );

    R[m] = 0x00000000; R[n] = 0x00030F05;
    R[dHi] = 0xF0001000; R[dLo] = 0x00080000;
    CHECK_RD64( 0xF000100000080000LL ); CHECK_CPSR( 1, 0, 0, 0 );

    R[m] = 0x00000002; R[n] = 0x00000000;
    R[dHi] = 0x00000000; R[dLo] = 0x00000000;
    CHECK_RD64( 0x0000000000000000LL ); CHECK_CPSR( 0, 1, 0, 0 );
}

BOOST_AUTO_TEST_CASE( SMLALxy_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1476284;
    const uint32_t n = 4, m = 2, dHi = 7, dLo = 6;
    BehaviorFunc func = arm::SMLALxy_A1;
    uint8_t  M;
    uint8_t  N;
    uint32_t instr;

    R[m] = 0x00010002; R[n] = 0x00030F05;

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 0; N = 0; instr = op | M << 6 | N << 5;
    CHECK_RD64( 0x0000000000081E0ALL );

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 0; N = 1; instr = op | M << 6 | N << 5;
    CHECK_RD64( 0x0000000000080006LL );

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 1; N = 0; instr = op | M << 6 | N << 5;
    CHECK_RD64( 0x0000000000080F05LL );

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 1; N = 1; instr = op | M << 6 | N << 5;
    CHECK_RD64( 0x0000000000080003LL );

    // With negative operands
    R[m] = 0xF001F002; R[n] = 0x0003FF05;
    M = 0; N = 0; instr = op | M << 6 | N << 5;

    R[dHi] = 0x00200004; R[dLo] = 0x00080000;
    CHECK_RD64( 0x002000040017AE0ALL );

    R[dHi] = 0xF0200004; R[dLo] = 0x00080000;
    CHECK_RD64( 0xF02000040017AE0ALL );
}

BOOST_AUTO_TEST_CASE( SMLALD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7476214;
    const uint32_t n = 4, m = 2, dHi = 7, dLo = 6;
    BehaviorFunc func = arm::SMLALD_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x00010002; R[n] = 0x00030F05;

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 0; instr = op | M << 5; CHECK_RD64( 0x0000000000081E0DLL );
    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 1; instr = op | M << 5; CHECK_RD64( 0x0000000000080F0BLL );

    R[m] = 0x7FFFF002; R[n] = 0x7FFFFF05;

    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 0; instr = op | M << 5; CHECK_RD64( 0x000000004016AE0BLL );
    R[dHi] = 0x00000000; R[dLo] = 0x00080000;
    M = 1; instr = op | M << 5; CHECK_RD64( 0xFFFFFFFFF78B90F9LL );
}

BOOST_AUTO_TEST_CASE( SMLAWx_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1267284;
    const uint32_t n = 4, m = 2, d = 6, a = 7;
    BehaviorFunc func = arm::SMLAWx_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x00010002; R[n] = 0x00030F05; R[a] = 0x02010F05;
    M = 0; instr = op | M << 6; CHECK_RD( 0x02010F0B );
    M = 1; instr = op | M << 6; CHECK_RD( 0x02010F08 );

    R[m] = 0x02012002; R[n] = 0x08F30F05; R[a] = 0x02010F05;
    M = 0; instr = op | M << 6; CHECK_RD( 0x031F82CB );
    M = 1; instr = op | M << 6; CHECK_RD( 0x0212FE16 );

    R[m] = 0x1201F002; R[n] = 0xF8F30F05; R[a] = 0xF2010F05;
    M = 0; instr = op | M << 6; CHECK_RD( 0xF271CFFA );
    M = 1; instr = op | M << 6; CHECK_RD( 0xF1821F06 );
}

BOOST_AUTO_TEST_CASE( SMLSD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7060254;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SMLSD_A1;
    uint8_t  M;
    uint32_t instr, a;
    
    // Check results in normal operation
    a = 7;

    R[m] = 0x00010002; R[n] = 0x00030F05; R[a] = 0x02010F05;
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x02012D0C );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0x02011E04 );

    R[m] = 0x02012002; R[n] = 0x08F30F05; R[a] = 0x02010F05;
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x03CFDE1C );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0x0100B624 );

    R[m] = 0x1201F002; R[n] = 0xF8F30F05; R[a] = 0xF2010F05;
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0xF18FCE1C );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0xF29EB624 );

    // Check if Q flag is set when accumulate operation overflows
    M = 0; instr = op | a << 12 | M << 5;

    proc.CPSR.Q = 0;
    R[m] = 0x1201F002; R[n] = 0xF8F30F05; R[a] = 0xF2010F05;
    func( proc, instr ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0;
    R[m] = 0x80007FFF; R[n] = 0x7FFF7FFF; R[a] = 0x7FFFFFFF;
    func( proc, instr ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    // Special case, when a == 15, R[a] is ignored
    a = 15;

    R[m] = 0x00010002; R[n] = 0x00030F05; R[a] = 0x02010F05;
    M = 0; instr = op | a << 12 | M << 5; CHECK_RD( 0x00001E07 );
    M = 1; instr = op | a << 12 | M << 5; CHECK_RD( 0x00000EFF );
}

BOOST_AUTO_TEST_CASE( SMLSLD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7476254;
    const uint32_t n = 4, m = 2, dHi = 7, dLo = 6;
    BehaviorFunc func = arm::SMLSLD_A1;
    uint8_t  M;
    uint32_t instr;
    
    R[m] = 0x00010002; R[n] = 0x00030F05;
    R[dHi] = 0x00000000; R[dLo] = 0x02010F05;
    M = 0; instr = op | M << 5; CHECK_RD64( 0x0000000002012D0CLL );
    R[dHi] = 0x00000000; R[dLo] = 0x02010F05;
    M = 1; instr = op | M << 5; CHECK_RD64( 0x0000000002011E04LL );

    R[m] = 0x02012002; R[n] = 0x08F30F05;
    R[dHi] = 0x00000000; R[dLo] = 0x02010F05;
    M = 0; instr = op | M << 5; CHECK_RD64( 0x0000000003CFDE1CLL );
    R[dHi] = 0x00000000; R[dLo] = 0x02010F05;
    M = 1; instr = op | M << 5; CHECK_RD64( 0x000000000100B624LL );

    R[m] = 0xF0077FFF; R[n] = 0x00037FFF;
    R[dHi] = 0xF0001000; R[dLo] = 0x00080000;
    M = 0; instr = op | M << 5; CHECK_RD64( 0xF000100040072FECLL );
    R[dHi] = 0xF0001000; R[dLo] = 0x00080000;
    M = 1; instr = op | M << 5; CHECK_RD64( 0xF0000FFFF80A0FFCLL );
}

BOOST_AUTO_TEST_CASE( SMMLA_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7570214;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SMMLA_A1;
    uint8_t  Ro;
    uint32_t instr, a;

    // Check results in normal operation
    a = 5;

    R[m] = 0xF0070001; R[n] = 0x00030F05; R[a] = 0x00030F05;
    Ro = 0; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x0002DE2A );
    Ro = 1; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x0002DE2A );

    R[m] = 0x70077FFF; R[n] = 0x70037FFF; R[a] = 0x00030F05;
    Ro = 0; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x3107DF1E );
    Ro = 1; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x3107DF1E );

    
    // Special case, R[a] must be ignored
    a = 15;

    R[m] = 0x70077FFF; R[n] = 0x70037FFF; R[a] = 0x00030F05;
    Ro = 0; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x3104D019 );
    Ro = 1; instr = op | a << 12 | Ro << 5; CHECK_RD( 0x3104D019 );
}

BOOST_AUTO_TEST_CASE( SMMLS_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF75762D4;
    const uint32_t n = 4, m = 2, d = 7, a = 6;
    BehaviorFunc func = arm::SMMLS_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x01070001; R[n] = 0x00030F05; R[a] = 0x02010F05;
    M = 0; instr = op | M << 5; CHECK_RD( 0x02010BE0 );
    M = 1; instr = op | M << 5; CHECK_RD( 0x02010BE1 );

    R[m] = 0xF0077FFF; R[n] = 0x00037FFF; R[a] = 0x02010F05;
    M = 0; instr = op | M << 5; CHECK_RD( 0x020146EA );
    M = 1; instr = op | M << 5; CHECK_RD( 0x020146EB );
}

BOOST_AUTO_TEST_CASE( SMMUL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF757F214;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SMMUL_A1;
    uint8_t  Ro;
    uint32_t instr;

    R[m] = 0x70077FFF; R[n] = 0x70037FFF;
    Ro = 0; instr = op | Ro << 5; CHECK_RD( 0x3104D019 );
    Ro = 1; instr = op | Ro << 5; CHECK_RD( 0x3104D019 );

    R[m] = 0xF0077FFF; R[n] = 0x70037FFF;
    Ro = 0; instr = op | Ro << 5; CHECK_RD( 0xF9031019 );
    Ro = 1; instr = op | Ro << 5; CHECK_RD( 0xF903101A );
}

BOOST_AUTO_TEST_CASE( SMUAD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF707F214;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SMUAD_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0xF0070001; R[n] = 0x00030F05;
    M = 0; instr = op | M << 5; CHECK_RD( 0xFFFFDF1A );
    M = 1; instr = op | M << 5; CHECK_RD( 0xFF101926 );

    R[m] = 0xF0077FFF; R[n] = 0x00037FFF; proc.CPSR.Q = 0;
    M = 0; instr = op | M << 5; CHECK_RD( 0x3FFED016 );
    BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    R[m] = 0x80008000; R[n] = 0x80008000; proc.CPSR.Q = 0;
    M = 0; instr = op | M << 5; CHECK_RD( 0x80000000 );
    BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );
}

BOOST_AUTO_TEST_CASE( SMULxy_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1670284;
    const uint32_t n = 4, m = 2, d = 7;
    BehaviorFunc func = arm::SMULxy_A1;
    uint8_t  M, N;
    uint32_t instr;

    R[m] = 0xF0070001; R[n] = 0x00030F05;
    M = 0; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0x00000F05 );
    M = 0; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0x00000003 );
    M = 1; N = 0; instr = op | M << 6 | N << 5; CHECK_RD( 0xFF101923 );
    M = 1; N = 1; instr = op | M << 6 | N << 5; CHECK_RD( 0xFFFFD015 );
}

BOOST_AUTO_TEST_CASE( SMULL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0C76294;
    const uint32_t n = 4, m = 2, dHi = 7, dLo = 6;
    BehaviorFunc func = arm::SMULL_A1;
    uint8_t  S;
    uint32_t instr;

    // !setflags
    S = 0; instr = op | S << 20; 

    R[m] = 0xF0070001; R[n] = 0x00030F05;
    CHECK_RD64( 0xFFFFCF2519260F05LL ); CHECK_CPSR( 0, 0, 0, 0 ); 

    // setflags
    S = 1; instr = op | S << 20;
    
    R[m] = 0xF0070001; R[n] = 0x00030F05;
    CHECK_RD64( 0xFFFFCF2519260F05LL ); CHECK_CPSR( 1, 0, 0, 0 );  

    R[m] = 0x70070001; R[n] = 0x01030F05;
    CHECK_RD64( 0x00715DA79A260F05LL ); CHECK_CPSR( 0, 0, 0, 0 ); 

    R[m] = 0x70070001; R[n] = 0x00000000;
    CHECK_RD64( 0x0000000000000000LL ); CHECK_CPSR( 0, 1, 0, 0 ); 
}

BOOST_AUTO_TEST_CASE( SMULWx_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF12602A4;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SMULWx_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x70010002; R[n] = 0x00030F05;
    M = 0; instr = op | M << 6; CHECK_RD( 0x00000006 );
    M = 1; instr = op | M << 6; CHECK_RD( 0x00015695 );

    R[m] = 0x1201F002; R[n] = 0xF8F30F05;
    M = 0; instr = op | M << 6; CHECK_RD( 0x0070C0F5 );
    M = 1; instr = op | M << 6; CHECK_RD( 0xFF811001 );
}

BOOST_AUTO_TEST_CASE( SMUSD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7060254;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SMUSD_A1;
    uint8_t  M;
    uint32_t instr;

    R[m] = 0x00010002; R[n] = 0x00030F05;
    M = 0; instr = op | M << 5; CHECK_RD( 0x00001E07 );
    M = 1; instr = op | M << 5; CHECK_RD( 0x00000EFF );

    R[m] = 0x1201F002; R[n] = 0xF8F30F05;
    M = 0; instr = op | M << 5; CHECK_RD( 0xFF8EBF17 );
    M = 1; instr = op | M << 5; CHECK_RD( 0x009DA71F );
}

BOOST_AUTO_TEST_CASE( SSAT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6A06014;
    const uint32_t n = 4, d = 6;
    BehaviorFunc func = arm::SSAT_A1;
    uint8_t  sat_imm, imm5, sh;
    uint32_t instr;

    proc.CPSR.Q = 0; R[n] = 0x00010002;
    sat_imm = 15; sh = 0; imm5 = 0;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x00007FFF ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    proc.CPSR.Q = 0; R[n] = 0x00010002;
    sat_imm = 25; sh = 0; imm5 = 0;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x00010002 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0x00010002;
    sat_imm = 15; sh = 1; imm5 = 0;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x00000000 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0x00010002;
    sat_imm = 30; sh = 0; imm5 = 2;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x00040008 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0xB450DEAD;
    sat_imm = 31; sh = 1; imm5 = 8;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0xFFB450DE ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0xB450DEAD;
    sat_imm = 16; sh = 1; imm5 = 8;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0xFFFF0000 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    proc.CPSR.Q = 0; R[n] = 0x1FFFFFFF;
    sat_imm = 30; sh = 1; imm5 = 5;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x00FFFFFF ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0x1FFFFFFF;
    sat_imm = 16; sh = 1; imm5 = 5;
    instr = op | sat_imm << 16 | imm5 << 7 | sh << 6; 
    CHECK_RD( 0x0000FFFF ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );
}

BOOST_AUTO_TEST_CASE( SSAT16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6A06F34;
    const uint32_t n = 4, d = 6;
    BehaviorFunc func = arm::SSAT16_A1;
    uint8_t  sat_imm;
    uint32_t instr;

    proc.CPSR.Q = 0; R[n] = 0x01010101;
    sat_imm = 9; instr = op | sat_imm << 16; 
    CHECK_RD( 0x01010101 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0x01010301;
    sat_imm = 9; instr = op | sat_imm << 16; 
    CHECK_RD( 0x010101FF ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    proc.CPSR.Q = 0; R[n] = 0x03010101;
    sat_imm = 9; instr = op | sat_imm << 16; 
    CHECK_RD( 0x01FF0101 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    proc.CPSR.Q = 0; R[n] = 0x03010301;
    sat_imm = 9; instr = op | sat_imm << 16; 
    CHECK_RD( 0x01FF01FF ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );

    proc.CPSR.Q = 0; R[n] = 0xF0010002;
    sat_imm = 12; instr = op | sat_imm << 16; 
    CHECK_RD( 0xF0010002 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 0 );

    proc.CPSR.Q = 0; R[n] = 0x80010002;
    sat_imm = 12; instr = op | sat_imm << 16; 
    CHECK_RD( 0xF0000002 ); BOOST_CHECK_EQUAL( proc.CPSR.Q, 1 );
}

BOOST_AUTO_TEST_CASE( SSAX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6146F52;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SSAX_A1;
    uint32_t instr = op;

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x00030F05;
    CHECK_RD( 0x00010F06 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xF );

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x0003FF05;
    CHECK_RD( 0x0001FF06 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xC );

    proc.CPSR.GE = 0; R[m] = 0x00010102; R[n] = 0x00030005;
    CHECK_RD( 0xFF010006 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x3 );

    proc.CPSR.GE = 0; R[m] = 0x00010102; R[n] = 0x0003FF05;
    CHECK_RD( 0xFF01FF06 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x0 );
}

BOOST_AUTO_TEST_CASE( SSUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6146F72;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SSUB16_A1;
    uint32_t instr = op;

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x00030F05;
    CHECK_RD( 0x00020F03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xF );

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x0003FF05;
    CHECK_RD( 0x0002FF03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xC );

    proc.CPSR.GE = 0; R[m] = 0xFFF10002; R[n] = 0xFFE20F05;
    CHECK_RD( 0xFFF10F03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x3 );

    proc.CPSR.GE = 0; R[m] = 0x10010102; R[n] = 0xF003FF05;
    CHECK_RD( 0xE002FE03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x0 );
}

BOOST_AUTO_TEST_CASE( SSUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6146FF2;
    const uint32_t n = 4, m = 2, d = 6;
    BehaviorFunc func = arm::SSUB8_A1;
    uint32_t instr = op;

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x00030F05;
    CHECK_RD( 0x00020F03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xF );

    proc.CPSR.GE = 0; R[m] = 0x00010002; R[n] = 0x0003FF05;
    CHECK_RD( 0x0002FF03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xD );

    proc.CPSR.GE = 0; R[m] = 0xFFF10002; R[n] = 0xFFE20F05;
    CHECK_RD( 0x00F10F03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0xB );

    proc.CPSR.GE = 0; R[m] = 0x10010102; R[n] = 0xF003FF05;
    CHECK_RD( 0xE002FE03 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x5 );

    proc.CPSR.GE = 0; R[m] = 0x10010102; R[n] = 0xF003FFF5;
    CHECK_RD( 0xE002FEF3 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x4 );

    proc.CPSR.GE = 0; R[m] = 0x10110102; R[n] = 0xF003FFF5;
    CHECK_RD( 0xE0F2FEF3 ); BOOST_CHECK_EQUAL( proc.CPSR.GE, 0x0 );
}

BOOST_AUTO_TEST_CASE( STM_STMIA_STMEA_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF8840000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STM_STMIA_STMEA_A1;
    uint8_t  W;
    uint16_t register_list;
    uint32_t instr;
    uint32_t address = 0x00000021;

    R[0] = 0x12345678;
    R[2] = 0x00000028;
    R[4] = 0x10010008;
    R[8] = 0x10000008;
    proc.PC = 0x00001000;
    register_list = 0x8115;

    W = 0; instr = op | W << 21 | register_list; R[n] = address;
    
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 8 ), R[4] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 12 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 16 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address );
    
    W = 1; instr = op | W << 21 | register_list; R[n] = address;

    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 8 ), 0x0 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 12 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 16 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address + 20 );
}

BOOST_AUTO_TEST_CASE( STMDA_STMED_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF8040000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STMDA_STMED_A1;
    uint8_t  W;
    uint16_t register_list;
    uint32_t instr;
    uint32_t address = 0x00000021;

    R[0] = 0x12345678;
    R[2] = 0x00000028;
    R[4] = 0x10100008;
    R[8] = 0x10000008;
    proc.PC = 0x00001000;
    register_list = 0x8115;

    W = 0; instr = op | W << 21 | register_list; R[n] = address;
    
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 16 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 12 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 8 ), R[4] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 4), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address );
    
    W = 1; instr = op | W << 21 | register_list; R[n] = address;

    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 16 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 12 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 8 ), 0x0 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 4 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address - 20 );
}

BOOST_AUTO_TEST_CASE( STMDB_STMFD_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF9040000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STMDB_STMFD_A1;
    uint8_t  W;
    uint16_t register_list;
    uint32_t instr;
    uint32_t address = 0x00000021;

    R[0] = 0x12345678;
    R[2] = 0x00000028;
    R[4] = 0x10010008;
    R[8] = 0x10000008;
    proc.PC = 0x00001000;
    register_list = 0x8115;

    W = 0; instr = op | W << 21 | register_list; R[n] = address;
    
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 20 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 16 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 12 ), R[4] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 8 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 4 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], 0x00000021 );
    
    W = 1; instr = op | W << 21 | register_list; R[n] = address;

    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 20 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 16 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 12 ), 0x0 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 8 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - 4 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address - 20 );

    // Special case: branch to PUSH
    //FIXME
}

BOOST_AUTO_TEST_CASE( STMIB_STMFA_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF9840000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STMIB_STMFA_A1;
    uint8_t  W;
    uint16_t register_list;
    uint32_t instr;
    uint32_t address = 0x00000021;

    R[0] = 0x12345678;
    R[2] = 0x00000028;
    R[4] = 0x10010008;
    R[8] = 0x10000008;
    proc.PC = 0x00001000;
    register_list = 0x8115;

    W = 0; instr = op | W << 21 | register_list; R[n] = address;
    
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 8 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 12 ), R[4] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 16 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 20 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address );
    
    W = 1; instr = op | W << 21 | register_list; R[n] = address;

    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[0] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 8 ), R[2] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 12 ), 0x0 );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 16 ), R[8] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 20 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address + 20 );
}

BOOST_AUTO_TEST_CASE( STR_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF4040000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STR_imm_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint8_t  t;
    uint32_t instr;
    uint32_t address = 0x00000533;
    uint16_t imm12 = 0x122;

    t = 5; R[t] = 0x08080808;
    
    R[n] = address; P = 0; U = 0; W = 0;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    // Special case: branch to STRT
    R[n] = address; P = 0; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    R[n] = address; P = 1; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - imm12 ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    R[n] = address; P = 1; U = 1; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + imm12 ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );

    // Special case: store PC instead of R[t]
    t = 15; proc.PC = 0x01010101;
    
    R[n] = address; P = 1; U = 1; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + imm12 ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );

    // Special case: branch to PUSH
    //FIXME
}

BOOST_AUTO_TEST_CASE( STR_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6040005;
    const uint32_t n = 4, m = 5;
    BehaviorFunc func = arm::STR_reg_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint8_t  t;
    uint8_t imm5;
    uint8_t type;
    uint32_t instr;
    uint32_t address = 0x00000533;

    P = 0; U = 0; W = 0; t = 6; imm5 = 0; type = 0;
    R[t] = 0x08080808; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    P = 0; U = 0; W = 0; t = 6; imm5 = 2; type = 0;
    R[t] = 0x08080808; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );

    P = 1; U = 0; W = 0; t = 6; imm5 = 2; type = 0;
    R[t] = 0x08080808; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address - ( R[m] << 2 ) ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address );

    // Special case: store PC instead of R[t]
    P = 1; U = 1; W = 0; t = 15; imm5 = 2; type = 1;
    proc.PC = 0x08080808; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + ( R[m] >> 2 ) ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address );

    // Special case: branch to STRT
    P = 0; U = 0; W = 1; t = 6; imm5 = 2; type = 0;
    R[t] = 0x08080808; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );
}

BOOST_AUTO_TEST_CASE( STRB_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF4440000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STRB_imm_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint8_t  t;
    uint32_t instr;
    uint32_t address = 0x00000533;
    uint16_t imm12 = 0x122;

    t = 5; R[t] = 0x12345678;
    
    R[n] = address; P = 0; U = 0; W = 0;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    // Special case: branch to STRBT
    R[n] = address; P = 0; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    R[n] = address; P = 1; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address - imm12 ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );
    
    R[n] = address; P = 1; U = 1; W = 1;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address + imm12 ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );
}

BOOST_AUTO_TEST_CASE( STRB_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6440005;
    const uint32_t n = 4, m = 5;
    BehaviorFunc func = arm::STRB_reg_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint8_t  t;
    uint8_t imm5;
    uint8_t type;
    uint32_t instr;
    uint32_t address = 0x00000533;

    P = 0; U = 0; W = 0; t = 6; imm5 = 0; type = 0;
    R[t] = 0x12345678; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    P = 0; U = 0; W = 0; t = 6; imm5 = 2; type = 0;
    R[t] = 0x12345678; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );

    P = 1; U = 1; W = 0; t = 6; imm5 = 2; type = 0;
    R[t] = 0x12345678; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address + ( R[m] << 2 ) ),  0x78 );
    BOOST_CHECK_EQUAL( R[n], address );

    // Special case: branch to STRBT
    P = 0; U = 0; W = 1; t = 6; imm5 = 2; type = 0;
    R[t] = 0x12345678; R[m] = 15; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );
}

BOOST_AUTO_TEST_CASE( STRBT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF4645000;
    const uint32_t n = 4, t = 5;
    BehaviorFunc func = arm::STRBT_A1;
    uint8_t  U;
    uint32_t instr;
    uint32_t address = 0x00000533;
    uint16_t imm12 = 0x122;

    R[t] = 0x12345678; 

    R[n] = address; U = 0; instr = op | U << 23 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );

    R[n] = address; U = 1; instr = op | U << 23 | imm12;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );
}

BOOST_AUTO_TEST_CASE( STRBT_A2_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6447005;
    const uint32_t n = 4, t = 7, m = 5;
    BehaviorFunc func = arm::STRBT_A2;
    uint8_t  U;
    uint8_t imm5;
    uint8_t type;
    uint32_t instr;
    uint32_t address = 0x00000533;

    R[t] = 0x12345678; R[m] = 15; 

    U = 0; imm5 = 2; type = 0; R[n] = address;
    instr = op | U << 23 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );

    U = 1; imm5 = 2; type = 0; R[n] = address;
    instr = op | U << 23 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_byte( address ), 0x78 );
    BOOST_CHECK_EQUAL( R[n], address + ( R[m] << 2 ) );
}

BOOST_AUTO_TEST_CASE( STRD_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF04470F0;
    const uint32_t n = 4, t = 7;
    BehaviorFunc func = arm::STRD_imm_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint8_t imm4H = 0x2;
    uint8_t imm4L = 0x1;
    uint32_t instr;
    uint32_t address = 0x00000533;

    R[t] = 0x12345678; R[t+1] = 0x9ABCDEF0; 

    P = 0; U = 0; W = 0; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | imm4H << 8 | imm4L;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address - 0x21 );

    P = 1; U = 1; W = 0; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | imm4H << 8 | imm4L;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 0x21 ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 0x21 + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address );

    P = 1; U = 1; W = 1; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 | imm4H << 8 | imm4L;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 0x21 ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 0x21 + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address + 0x21 );
}

BOOST_AUTO_TEST_CASE( STRD_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF00470F5;
    const uint32_t n = 4, t = 7, m = 5;
    BehaviorFunc func = arm::STRD_reg_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint32_t instr;
    uint32_t address = 0x00000533;

    R[m] = 15; R[t] = 0x12345678; R[t+1] = 0x9ABCDEF0; 

    P = 0; U = 0; W = 0; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21 ;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    P = 1; U = 1; W = 0; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + R[m] ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + R[m] + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address );

    P = 1; U = 1; W = 1; R[n] = address;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + R[m] ), R[t] );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address + R[m] + 4 ), R[t+1] );
    BOOST_CHECK_EQUAL( R[n], address + R[m] );
}

BOOST_AUTO_TEST_CASE( STRH_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0445ABC;
    const uint32_t n = 4, t = 5, imm4H = 0xA, imm4L = 0xC; 
    BehaviorFunc func = arm::STRH_imm_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint32_t instr;
    uint32_t address = 0x00000349;
    uint16_t imm8 = imm4H << 4 | imm4L;

    R[t] = 0x12345678;
    
    R[n] = address; P = 0; U = 0; W = 0;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - imm8 );
    
    // Special case: branch to STRHT
    R[n] = address; P = 0; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - imm8 );
    
    R[n] = address; P = 1; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address - imm8 ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - imm8 );
    
    R[n] = address; P = 1; U = 1; W = 1;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address + imm8 ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address + imm8 );
}

BOOST_AUTO_TEST_CASE( STRH_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF00470B5;
    const uint32_t n = 4, t = 7, m = 5;
    BehaviorFunc func = arm::STRH_reg_A1;
    uint8_t  P;
    uint8_t  U;
    uint8_t  W;
    uint32_t instr;
    uint32_t address = 0x00000533;
    R[t] = 0x12345678; R[m] = 24;

    R[n] = address; P = 0; U = 0; W = 0;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    R[n] = address; P = 0; U = 0; W = 0;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    R[n] = address; P = 1; U = 1; W = 0;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address + R[m] ),  0x5678 );
    BOOST_CHECK_EQUAL( R[n], address );

    // Special case: branch to STRHT
    R[n] = address; P = 0; U = 0; W = 1;
    instr = op | P << 24 | U << 23 | W << 21;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );
}

BOOST_AUTO_TEST_CASE( STRHT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0645ABC;
    const uint32_t n = 4, t = 5, imm4H = 0xA, imm4L = 0xC; 
    BehaviorFunc func = arm::STRHT_A1;
    uint8_t  U;
    uint32_t instr;
    uint32_t address = 0x00000349;
    uint16_t imm8 = imm4H << 4 | imm4L;

    R[t] = 0x12345678; 

    R[n] = address; U = 0; instr = op | U << 23;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - imm8 );

    R[n] = address; U = 1; instr = op | U << 23;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address + imm8 );
}

BOOST_AUTO_TEST_CASE( STRHT_A2_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF04470B5;
    const uint32_t n = 4, t = 7, m = 5;
    BehaviorFunc func = arm::STRHT_A2;
    uint8_t  U;
    uint32_t instr;
    uint32_t address = 0x00000533;

    R[t] = 0x12345678; R[m] = 0x123; 

    R[n] = address; U = 0;
    instr = op | U << 23; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address - R[m] );

    R[n] = address; U = 1;
    instr = op | U << 23; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_half( address ), 0x5678 );
    BOOST_CHECK_EQUAL( R[n], address + R[m] );
}

BOOST_AUTO_TEST_CASE( STRT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF4240000;
    const uint32_t n = 4;
    BehaviorFunc func = arm::STRT_A1;
    uint8_t  U;
    uint8_t  t;
    uint32_t instr;
    uint32_t address = 0x00000533;
    uint16_t imm12 = 0x122;

    //Normal operation: t != 15
    t = 7;
    R[t] = 0x3456789A;

    R[n] = address; U = 0;
    instr = op | U << 23 | t << 12 | imm12; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - imm12 );

    R[n] = address; U = 1;
    instr = op | U << 23 | t << 12 | imm12; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );

    // Special case: t == 15 -> store PC
    t = 15;

    R[n] = address; U = 1; proc.PC = 0x11223344;
    instr = op | U << 23 | t << 12 | imm12; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address + imm12 );
}

BOOST_AUTO_TEST_CASE( STRT_A2_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6247005;
    const uint32_t n = 4, m = 5;
    BehaviorFunc func = arm::STRT_A2;
    uint8_t  U;
    uint8_t  t;
    uint8_t  imm5;
    uint8_t  type;
    uint32_t instr;
    uint32_t address = 0x00000533;

    //Normal operation: t != 15
    t = 7;
    R[t] = 0x3456789A;

    R[n] = address; U = 0; imm5 = 2; type = 0;
    instr = op | U << 23 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address - ( R[m] << 2 ) );

    R[n] = address; U = 1; imm5 = 2; type = 0;
    instr = op | U << 23 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address + ( R[m] << 2 ) );

    R[n] = address; U = 1; imm5 = 3; type = 1;
    instr = op | U << 23 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), R[t] );
    BOOST_CHECK_EQUAL( R[n], address + ( R[m] >> 3 ) );

    // Special case: t == 15 -> store PC
    t = 15;

    R[n] = address; U = 1; imm5 = 3; type = 1; proc.PC = 0x11442233;
    instr = op | U << 23 | t << 12 | imm5 << 7 | type << 5;
    func( proc, instr );
    BOOST_CHECK_EQUAL( proc.dMem.read_word( address ), proc.PC );
    BOOST_CHECK_EQUAL( R[n], address + ( R[m] >> 3 ) );
    
}

BOOST_AUTO_TEST_CASE( SUB_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF2401000;
    const uint32_t S  = 0x1 << 20;
    const uint32_t n  = 0, d = 1;

    uint32_t instr;
    BehaviorFunc func = arm::SUB_imm_A1;
    R[n] = 0x0000000F; instr = op | 0x00F; CHECK_RD( 0x00000000 );
    R[n] = 0xFFFFFFFF; instr = op | 0x00F; CHECK_RD( 0xFFFFFFF0 );
    R[n] = 0xFFFFFFFF; instr = op | 0x4FF; CHECK_RD( 0x00FFFFFF );
    R[n] = 0xFFFFFFFF; instr = op | 0x8FF; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xABCDEF01; instr = op | 0xCEF; CHECK_RD( 0xABCD0001 );

    // setflags
    R[n] = 0xE0000000; instr = op | 0x21E | S; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; instr = op | 0x20E | S; CHECK_CPSR( 0, 0, 1, 0 );
    R[n] = 0xF0000000; instr = op | 0x20F | S; CHECK_CPSR( 0, 1, 1, 0 );
    R[n] = 0x7FFFFFFF; instr = op | 0x4FF | S; CHECK_CPSR( 1, 0, 0, 1 );
    R[n] = 0xFFFFFFFF; instr = op | 0x00F | S; CHECK_CPSR( 1, 0, 1, 0 );

    // !setflags
    R[n] = 0xE0000000; instr = op | 0x21E; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; instr = op | 0x20E; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF0000000; instr = op | 0x20F; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x7FFFFFFF; instr = op | 0x4FF; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; instr = op | 0x00F; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    proc.PC = 0x00000000;
    R[n] = 0xCAFEC0DE; instr = op | 0xF << 12 | 0x0DE; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xCAFEC000 );
}

BOOST_AUTO_TEST_CASE( SUB_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0401002;
    uint32_t n = 0, d = 1, m = 2;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func = arm::SUB_reg_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0000000F; R[m] = 0x0000000F; CHECK_RD( 0x00000000 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_RD( 0xFFFFFFF0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );

    // With shift
    R[n] = 0xFFFFFFFF;
    R[m] = 0x87654321;
    instr = op | 0x00 << 7 | 0x0 << 5; CHECK_RD( 0x789ABCDE ); // No shift
    instr = op | 0x04 << 7 | 0x0 << 5; CHECK_RD( 0x89ABCDEF ); // LSL(4)
    instr = op | 0x00 << 7 | 0x1 << 5; CHECK_RD( 0xFFFFFFFF ); // LSR(32)
    instr = op | 0x08 << 7 | 0x1 << 5; CHECK_RD( 0xFF789ABC ); // LSR(8)
    instr = op | 0x00 << 7 | 0x2 << 5; CHECK_RD( 0x00000000 ); // ASR(32)
    instr = op | 0x0C << 7 | 0x2 << 5; CHECK_RD( 0x000789AB ); // ASR(12)
    instr = op | 0x00 << 7 | 0x3 << 5; CHECK_RD( 0xBC4D5E6F ); // RRX(1)
    instr = op | 0x10 << 7 | 0x3 << 5; CHECK_RD( 0xBCDE789A ); // ROR(16)

    // setflags
    instr = op | 0x1 << 20;
    R[n] = 0xE0000000; R[m] = 0xE0000001; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; R[m] = 0xE0000000; CHECK_CPSR( 0, 0, 1, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 1, 1, 0 );
    R[n] = 0x7FFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 1, 0, 0, 1 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_CPSR( 1, 0, 1, 0 );

    // !setflags
    instr = op;
    R[n] = 0xE0000000; R[m] = 0xE0000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; R[m] = 0xE0000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x7FFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_CPSR( 0, 0, 0, 0 );

    // Write to PC
    instr = op | 0xF << 12;
    proc.PC = 0x00000000;
    R[n] = 0xCAFEC0DE; R[m] = 0x0000C0DE; func( proc, instr );
    BOOST_CHECK_EQUAL( proc.PC, 0xCAFE0000 );
}

BOOST_AUTO_TEST_CASE( SUB_sh_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0401312;;
    const uint32_t n = 0, d = 1, m = 2, s = 3;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func = arm::SUB_sh_reg_A1;
    R[s] = 0x00000000;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0000000F; R[m] = 0x0000000F; CHECK_RD( 0x00000000 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_RD( 0xFFFFFFF0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );

    // With shift
    R[n] = 0xFFFFFFFF;
    R[m] = 0x87654321;
    instr = op | 0x0 << 5; R[s] = 0x00; CHECK_RD( 0x789ABCDE ); // No shift
    instr = op | 0x0 << 5; R[s] = 0x04; CHECK_RD( 0x89ABCDEF ); // LSL(4)
    instr = op | 0x1 << 5; R[s] = 0x08; CHECK_RD( 0xFF789ABC ); // LSR(8)
    instr = op | 0x2 << 5; R[s] = 0x0C; CHECK_RD( 0x000789AB ); // ASR(12)
    instr = op | 0x3 << 5; R[s] = 0x10; CHECK_RD( 0xBCDE789A ); // ROR(16)    

    // setflags
    instr = op | 0x1 << 20;
    R[s] = 0x00000000;
    R[n] = 0xE0000000; R[m] = 0xE0000001; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; R[m] = 0xE0000000; CHECK_CPSR( 0, 0, 1, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 1, 1, 0 );
    R[n] = 0x7FFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 1, 0, 0, 1 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_CPSR( 1, 0, 1, 0 );

    // !setflags
    instr = op;
    R[n] = 0xE0000000; R[m] = 0xE0000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xEFFFFFFF; R[m] = 0xE0000000; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x7FFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0x0000000F; CHECK_CPSR( 0, 0, 0, 0 );
}

BOOST_AUTO_TEST_CASE( SXTAB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6A01072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::SXTAB_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11111189 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11111167 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11111145 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11111123 );

    // Subtract
    R[n] = 0x00000001;
    R[m] = 0xFBFCFDFE;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFFFFFFF );
    instr = op | 0x1 << 10; CHECK_RD( 0xFFFFFFFE );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFFFFFFD );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFFFFFFC );
}


BOOST_AUTO_TEST_CASE( SXTAB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF5801072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::SXTAB16_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11451189 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11231167 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11891145 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11671123 );

    // Subtract
    R[n] = 0x00010001;
    R[m] = 0xFBFCFDFE;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFFDFFFF );
    instr = op | 0x1 << 10; CHECK_RD( 0xFFFCFFFE );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFFFFFFD );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFFEFFFC );
}


BOOST_AUTO_TEST_CASE( SXTAH_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF5801072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::SXTAH_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11116789 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11114567 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11112345 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11118923 );

    // Subtract
    R[n] = 0x00000001;
    R[m] = 0xFFFDFFFE;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFFFFFFF );
    instr = op | 0x1 << 10; CHECK_RD( 0xFFFFFE00 );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFFFFFFE );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFFFFF00 );
}


BOOST_AUTO_TEST_CASE( SXTB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6AF0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::SXTB_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00000078 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00000056 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00000034 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00000012 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFFFFFF8 );
    instr = op | 0x1 << 10; CHECK_RD( 0xFFFFFFD6 );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFFFFFB4 );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFFFFF92 );
}


BOOST_AUTO_TEST_CASE( SXTB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF68F0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::SXTB16_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00340078 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00120056 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00780034 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00560012 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFB4FFF8 );
    instr = op | 0x1 << 10; CHECK_RD( 0xFF92FFD6 );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFF8FFB4 );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFD6FF92 );
}


BOOST_AUTO_TEST_CASE( SXTH_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6BF0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::SXTH_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00005678 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00003456 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00001234 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00007812 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0xFFFFD6F8 );
    instr = op | 0x1 << 10; CHECK_RD( 0xFFFFB4D6 );
    instr = op | 0x2 << 10; CHECK_RD( 0xFFFF92B4 );
    instr = op | 0x3 << 10; CHECK_RD( 0xFFFFF892 );
}


BOOST_AUTO_TEST_CASE( TEQ_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF3300000;
    const uint32_t n = 0;

    uint32_t instr = op;
    BehaviorFunc func  = arm::TEQ_imm_A1;
    R[n] = 0x00000000; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; instr = op | 0x0FF; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000042; instr = op | 0x042; CHECK_CPSR( 0, 1, 0, 0 );
    R[n] = 0xFF000001; instr = op | 0x1FF; CHECK_CPSR( 0, 0, 1, 0 );
    R[n] = 0x00000000; instr = op | 0x1FF; CHECK_CPSR( 1, 0, 1, 0 );
}


BOOST_AUTO_TEST_CASE( TEQ_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1300001;
    const uint32_t n = 0, m = 1;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func  = arm::TEQ_reg_A1;
    R[n] = 0x00000000; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; R[m] = 0x000000FF; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000042; R[m] = 0x00000042; CHECK_CPSR( 0, 1, 0, 0 );

    // With shift
    instr = op | 0x3 << 5 | 0x01 << 7;
    R[m] = 0x000000FF;
    R[n] = 0xFF000001; CHECK_CPSR( 0, 0, 1, 0 ); // ROR(1)
    R[n] = 0x00000000; CHECK_CPSR( 1, 0, 1, 0 ); // ROR(1)
}


BOOST_AUTO_TEST_CASE( TEQ_sh_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1300201;
    const uint32_t n = 0, m = 1, s = 2;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func  = arm::TEQ_sh_reg_A1;
    R[s] = 0x00000000;
    R[n] = 0x00000000; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; R[m] = 0x000000FF; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000042; R[m] = 0x00000042; CHECK_CPSR( 0, 1, 0, 0 );

    // With shift
    instr = op | 0x3 << 5; R[s] = 0x00000001;
    R[m] = 0x000000FF;
    R[n] = 0xFF000001; CHECK_CPSR( 0, 0, 1, 0 ); // ROR(1)
    R[n] = 0x00000000; CHECK_CPSR( 1, 0, 1, 0 ); // ROR(1)
}


BOOST_AUTO_TEST_CASE( TST_imm_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF3100000;
    const uint32_t n = 0;

    uint32_t instr = op;
    BehaviorFunc func  = arm::TST_imm_A1;
    R[n] = 0x00000001; instr = op | 0x001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; instr = op | 0x2FF; CHECK_CPSR( 1, 0, 1, 0 );
    R[n] = 0x00000042; instr = op | 0x000; CHECK_CPSR( 0, 1, 0, 0 );
    R[n] = 0x00000001; instr = op | 0x1FF; CHECK_CPSR( 0, 0, 1, 0 );
}


BOOST_AUTO_TEST_CASE( TST_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1100001;
    const uint32_t n = 0, m = 1;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func  = arm::TST_reg_A1;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; R[m] = 0xF000000F; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000042; R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );

    // With shift
    instr = op | 0x3 << 5 | 0x01 << 7;
    R[m] = 0x000000FF;
    R[n] = 0xC000003F; CHECK_CPSR( 1, 0, 1, 0 ); // ROR(1)
}


BOOST_AUTO_TEST_CASE( TST_sh_reg_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF1100201;
    const uint32_t n = 0, m = 1, s = 2;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func  = arm::TST_sh_reg_A1;
    R[s] = 0x00000000;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0xF00000FF; R[m] = 0xF000000F; CHECK_CPSR( 1, 0, 0, 0 );
    R[n] = 0x00000042; R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );

    // With shift
    instr = op | 0x3 << 5; R[s] = 0x00000001;
    R[m] = 0x000000FF;
    R[n] = 0xC000003F; CHECK_CPSR( 1, 0, 1, 0 ); // ROR(1)
}


BOOST_AUTO_TEST_CASE( UADD16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501F12;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UADD16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x00010002; R[m] = 0x000A000B; CHECK_RD( 0x000B000D );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFF00FFFC; R[m] = 0x00000004; CHECK_RD( 0xFF000000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFEFFFE );

    // GE flag
    R[n] = 0x000000FF; R[m] = 0xFFFFFF00; CHECK_GE( 0x0 );
    R[n] = 0xFFFF0015; R[m] = 0x00010015; CHECK_GE( 0xC );
    R[n] = 0xFFFF0015; R[m] = 0x0000FFF0; CHECK_GE( 0x3 );
    R[n] = 0xFFFF0015; R[m] = 0x0001FFFF; CHECK_GE( 0xF );
}


BOOST_AUTO_TEST_CASE( UADD8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501F92;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UADD8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x01020304; R[m] = 0x0A0B0C0D; CHECK_RD( 0x0B0D0F11 );
    R[n] = 0xFEFDFCFB; R[m] = 0x01020304; CHECK_RD( 0xFFFFFFFF );
    R[n] = 0xA0FFB0FF; R[m] = 0x00010001; CHECK_RD( 0xA000B000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFEFEFEFE );

    // GE flag
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F0F0F0; CHECK_GE( 0x0 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F0F0F1; CHECK_GE( 0x1 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F0F1F0; CHECK_GE( 0x2 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F0F1F1; CHECK_GE( 0x3 );

    R[n] = 0x0F0F0F0F; R[m] = 0xF0F1F0F0; CHECK_GE( 0x4 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F1F0F1; CHECK_GE( 0x5 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F1F1F0; CHECK_GE( 0x6 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF0F1F1F1; CHECK_GE( 0x7 );

    R[n] = 0x0F0F0F0F; R[m] = 0xF1F0F0F0; CHECK_GE( 0x8 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F0F0F1; CHECK_GE( 0x9 );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F0F1F0; CHECK_GE( 0xA );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F0F1F1; CHECK_GE( 0xB );

    R[n] = 0x0F0F0F0F; R[m] = 0xF1F1F0F0; CHECK_GE( 0xC );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F1F0F1; CHECK_GE( 0xD );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F1F1F0; CHECK_GE( 0xE );
    R[n] = 0x0F0F0F0F; R[m] = 0xF1F1F1F1; CHECK_GE( 0xF );
}


BOOST_AUTO_TEST_CASE( UASX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501F32;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UASX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A0002; R[m] = 0x00020001; CHECK_RD( 0x000B0000 );
    R[n] = 0xFFF0FFF0; R[m] = 0xFFF0000F; CHECK_RD( 0xFFFF0000 );
    R[n] = 0xFFFCFF00; R[m] = 0xFF010004; CHECK_RD( 0x0000FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFE0000 );

    // GE flag
    R[n] = 0x00000000; R[m] = 0x0001FF00; CHECK_GE( 0x0 );
    R[n] = 0xFFFF0000; R[m] = 0x00010001; CHECK_GE( 0xC );
    R[n] = 0xFFFE0001; R[m] = 0x00010001; CHECK_GE( 0x3 );
    R[n] = 0xFFFF0042; R[m] = 0x0042FFFF; CHECK_GE( 0xF );
}


BOOST_AUTO_TEST_CASE( UBFX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7E01050;
    uint32_t n = 0, d = 1;

    uint32_t instr = op;
    BehaviorFunc func = arm::UBFX_A1;
    R[n] = 0xABCDEF42;
    instr = op | 0x00 << 7 | 0x00 << 16; CHECK_RD( 0x00000000 );
    instr = op | 0x00 << 7 | 0x03 << 16; CHECK_RD( 0x00000002 );
    instr = op | 0x00 << 7 | 0x07 << 16; CHECK_RD( 0x00000042 );
    instr = op | 0x00 << 7 | 0x0F << 16; CHECK_RD( 0x0000EF42 );
    instr = op | 0x00 << 7 | 0x1F << 16; CHECK_RD( 0xABCDEF42 );
    instr = op | 0x10 << 7 | 0x0F << 16; CHECK_RD( 0x0000ABCD );
    instr = op | 0x1C << 7 | 0x03 << 16; CHECK_RD( 0x0000000A );
}


BOOST_AUTO_TEST_CASE( UHADD16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701F12;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::UHADD16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x00010002; R[m] = 0x000A000B; CHECK_RD( 0x00050006 );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0x7F807FFF );
    R[n] = 0xFF00FFFC; R[m] = 0x00000004; CHECK_RD( 0x7F808000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFFFFFF );
}


BOOST_AUTO_TEST_CASE( UHADD8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701F92;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UHADD8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x01020304; R[m] = 0x0A0B0C0D; CHECK_RD( 0x05060708 );
    R[n] = 0xFEFDFCFB; R[m] = 0x01020304; CHECK_RD( 0x7F7F7F7F );
    R[n] = 0xA0FFB0FF; R[m] = 0x00010001; CHECK_RD( 0x50805880 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFFFFFF );
}


BOOST_AUTO_TEST_CASE( UHASX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701F32;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UHASX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A0002; R[m] = 0x00020001; CHECK_RD( 0x00050000 );
    R[n] = 0xFFF0FFF0; R[m] = 0xFFF0000F; CHECK_RD( 0x7FFF0000 );
    R[n] = 0xFFFCFF00; R[m] = 0xFF010004; CHECK_RD( 0x8000FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFF0000 );
}


BOOST_AUTO_TEST_CASE( UHSAX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701F52;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UHSAX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0002000A; R[m] = 0x00010002; CHECK_RD( 0x00000005 );
    R[n] = 0xFFF0FFF0; R[m] = 0x000FFFF0; CHECK_RD( 0x00007FFF );
    R[n] = 0xFF00FFFC; R[m] = 0x0004FF01; CHECK_RD( 0xFFFF8000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x0000FFFF );
}


BOOST_AUTO_TEST_CASE( UHSUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701F72;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::UHSUB16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A000B; R[m] = 0x00010002; CHECK_RD( 0x00040004 );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0x7F807FFC );
    R[n] = 0xFF000004; R[m] = 0x00000005; CHECK_RD( 0x7F80FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}


BOOST_AUTO_TEST_CASE( UHSUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6701FF2;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UHSUB8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0x04040404 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0x81828384 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0x50FF58FF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}


BOOST_AUTO_TEST_CASE( UMAAL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0401293;
    uint32_t dHi = 0, dLo = 1, m = 2, n = 3;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UMAAL_A1;
    R[dHi] = 0x00000000;
    R[dLo] = 0x00000000;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_RD64( 0x0000000000000001LL );
    R[n] = 0x0000CAFE; R[m] = 0x01000000; CHECK_RD64( 0x000000CAFE000001LL );

    R[dHi] = 0x00000000;
    R[dLo] = 0x00000001;
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; CHECK_RD64( 0x0000000100000000LL );
    R[n] = 0xFFFFFFFF; R[m] = 0x10000000; CHECK_RD64( 0x0FFFFFFFF0000001LL );

    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFF;
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD64( 0xFFFFFFFFFFFFFFFFLL );
}


BOOST_AUTO_TEST_CASE( UMLAL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0A01293;
    uint32_t dHi = 0, dLo = 1, m = 2, n = 3;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UMLAL_A1;
    R[dHi] = 0x00000000;
    R[dLo] = 0x00000000;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_RD64( 0x0000000000000001LL );
    R[n] = 0x0000CAFE; R[m] = 0x01000000; CHECK_RD64( 0x000000CAFE000001LL );

    R[dHi] = 0x00000000;
    R[dLo] = 0x00000001;
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; CHECK_RD64( 0x0000000100000000LL );
    R[n] = 0xFFFFFFFF; R[m] = 0x10000000; CHECK_RD64( 0x10000000F0000000LL );

    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFF;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_RD64( 0x0000000000000000LL );
    CHECK_CPSR( 0, 0, 0, 0 );

    // setflags
    instr = op | 0x1 << 20;
    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFF;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_CPSR( 0, 1, 0, 0 );

    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFE;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_CPSR( 1, 0, 0, 0 );
}


BOOST_AUTO_TEST_CASE( UMULL_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF0801293;
    uint32_t dHi = 0, dLo = 1, m = 2, n = 3;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UMULL_A1;
    R[dHi] = 0x00000000;
    R[dLo] = 0x00000000;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000000; R[m] = 0xFFFFFFFF; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0xFFFFFFFF; R[m] = 0x00000000; CHECK_RD64( 0x0000000000000000LL );
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_RD64( 0x0000000000000001LL );
    R[n] = 0x0000CAFE; R[m] = 0x01000000; CHECK_RD64( 0x000000CAFE000000LL );

    R[dHi] = 0x00000000;
    R[dLo] = 0x00000001;
    R[n] = 0xFFFFFFFF; R[m] = 0x00000001; CHECK_RD64( 0x00000000FFFFFFFFLL );
    R[n] = 0xFFFFFFFF; R[m] = 0x10000000; CHECK_RD64( 0x0FFFFFFFF0000000LL );

    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFF;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_RD64( 0x0000000000000001LL );
    CHECK_CPSR( 0, 0, 0, 0 );

    // setflags
    instr = op | 0x1 << 20;
    R[dHi] = 0xFFFFFFFF;
    R[dLo] = 0xFFFFFFFF;
    R[n] = 0x00000001; R[m] = 0x00000001; CHECK_CPSR( 0, 0, 0, 0 );
    R[n] = 0x00000001; R[m] = 0x00000000; CHECK_CPSR( 0, 1, 0, 0 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_CPSR( 1, 0, 0, 0 );
}


BOOST_AUTO_TEST_CASE( UQADD16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601F12;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UQADD16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x00010002; R[m] = 0x000A000B; CHECK_RD( 0x000B000D );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFF00FFFC; R[m] = 0x00000004; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFFFFFF );
}


BOOST_AUTO_TEST_CASE( UQADD8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601F92;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UQADD8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x01020304; R[m] = 0x0A0B0C0D; CHECK_RD( 0x0B0D0F11 );
    R[n] = 0xFEFDFCFB; R[m] = 0x01020304; CHECK_RD( 0xFFFFFFFF );
    R[n] = 0xA0FFB0FF; R[m] = 0x00010001; CHECK_RD( 0xA0FFB0FF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFFFFFF );
}


BOOST_AUTO_TEST_CASE( UQASX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601F32;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UQASX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A0002; R[m] = 0x00020001; CHECK_RD( 0x000B0000 );
    R[n] = 0xFFF0FFF0; R[m] = 0xFFF0000F; CHECK_RD( 0xFFFF0000 );
    R[n] = 0xFFFCFF00; R[m] = 0xFF010004; CHECK_RD( 0xFFFF0000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xFFFF0000 );
}


BOOST_AUTO_TEST_CASE( UQSAX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601F52;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UQSAX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0002000A; R[m] = 0x00010002; CHECK_RD( 0x0000000B );
    R[n] = 0xFFF0FFF0; R[m] = 0x000FFFF0; CHECK_RD( 0x0000FFFF );
    R[n] = 0xFF00FFFC; R[m] = 0x0004FF01; CHECK_RD( 0x0000FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x0000FFFF );
}


BOOST_AUTO_TEST_CASE( UQSUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601F72;
    uint32_t n = 0, d = 1, m = 2;

    uint32_t instr = op;
    BehaviorFunc func = arm::UQSUB16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A000B; R[m] = 0x00010002; CHECK_RD( 0x00090009 );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0xFF00FFF9 );
    R[n] = 0xFF000004; R[m] = 0x00000005; CHECK_RD( 0xFF000000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}


BOOST_AUTO_TEST_CASE( UQSUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6601FF2;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::UQSUB8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0x09090909 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0x00000000 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0xA000B000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}


BOOST_AUTO_TEST_CASE( USAD8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF781F210;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::USAD8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0x00000024 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0x000003E8 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0x00000152 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );
}


BOOST_AUTO_TEST_CASE( USADA8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF7813210;
    uint32_t n = 0, d = 1, m = 2, a = 3;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::USADA8_A1;
    R[a] = 0xBADC0FFE;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0xBADC0FFE );

    R[a] = 0xEBADC0DE;    
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0xEBADC102 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0xEBADC4C6 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0xEBADC230 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0xEBADC0DE );
}


BOOST_AUTO_TEST_CASE( USAT_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6E01010;
    uint32_t n = 0, d = 1;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func = arm::USAT_A1;
    R[n] = 0xBADDECAF; instr = op | 0x14 << 16; CHECK_RD( 0x00000000 );
    R[n] = 0x7ADDECAF; instr = op | 0x14 << 16; CHECK_RD( 0x000FFFFF );
    R[n] = 0x000DECAF; instr = op | 0x14 << 16; CHECK_RD( 0x000DECAF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0xBADDECAF; instr = op | 0x00 << 16; CHECK_RD( 0x00000000 );
    R[n] = 0xFFFFFFFF; instr = op | 0x1F << 16; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );

    // With shift
    R[n] = 0x12345678;
    instr = op | 0x0 << 6 | 0x0C << 7 | 0x1F << 16; CHECK_RD( 0x45678000 );
    instr = op | 0x1 << 6 | 0x0C << 7 | 0x1F << 16; CHECK_RD( 0x00012345 );
    instr = op | 0x1 << 6 | 0x00 << 7 | 0x1F << 16; CHECK_RD( 0x00000000 );

    R[n] = 0x87654321;
    instr = op | 0x0 << 6 | 0x0C << 7 | 0x1F << 16; CHECK_RD( 0x54321000 );
    instr = op | 0x1 << 6 | 0x0C << 7 | 0x1F << 16; CHECK_RD( 0x00000000 );
    instr = op | 0x1 << 6 | 0x00 << 7 | 0x1F << 16; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
}


BOOST_AUTO_TEST_CASE( USAT16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6E01F30;
    uint32_t n = 0, d = 1;

    // Without shift
    uint32_t instr = op;
    BehaviorFunc func = arm::USAT16_A1;
    R[n] = 0xBADDECAF; instr = op | 0xC << 16; CHECK_RD( 0x00000000 );
    R[n] = 0x7ADDECAF; instr = op | 0xC << 16; CHECK_RD( 0x0FFF0000 );
    R[n] = 0x000D0CAF; instr = op | 0xC << 16; CHECK_RD( 0x000D0CAF );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 0, 0 );
    R[n] = 0xBADDECAF; instr = op | 0x0 << 16; CHECK_RD( 0x00000000 );
    R[n] = 0xFFFFFFFF; instr = op | 0xF << 16; CHECK_RD( 0x00000000 );
    CHECK_CPSR_EXT( 0, 0, 0, 0, 1, 0 );
}


BOOST_AUTO_TEST_CASE( USAX_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501F52;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::USAX_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0002000A; R[m] = 0x00010002; CHECK_RD( 0x0000000B );
    R[n] = 0xFFF0FFF0; R[m] = 0x000FFFF0; CHECK_RD( 0x0000FFFF );
    R[n] = 0xFF00FFFC; R[m] = 0x0004FF01; CHECK_RD( 0xFFFF0000 );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x0000FFFE );

    // GE flag
    R[n] = 0x00000000; R[m] = 0xFF000001; CHECK_GE( 0x0 );
    R[n] = 0x0000FFFF; R[m] = 0x00010001; CHECK_GE( 0x3 );
    R[n] = 0x0001FFFE; R[m] = 0x00010001; CHECK_GE( 0xC );
    R[n] = 0x0042FFFF; R[m] = 0xFFFF0042; CHECK_GE( 0xF );
}


BOOST_AUTO_TEST_CASE( USUB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501F72;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::USUB16_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x000A000B; R[m] = 0x00010002; CHECK_RD( 0x00090009 );
    R[n] = 0xFF00FFFC; R[m] = 0x00000003; CHECK_RD( 0xFF00FFF9 );
    R[n] = 0xFF000004; R[m] = 0x00000005; CHECK_RD( 0xFF00FFFF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );

    // GE flag
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_GE( 0xF );
    R[n] = 0x00000000; R[m] = 0x00000001; CHECK_GE( 0xC );
    R[n] = 0x00000000; R[m] = 0x00010000; CHECK_GE( 0x3 );
    R[n] = 0x00000000; R[m] = 0x00010001; CHECK_GE( 0x0 );
}


BOOST_AUTO_TEST_CASE( USUB8_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6501FF2;
    uint32_t n = 0, d = 1, m = 2;

    // Results
    uint32_t instr = op;
    BehaviorFunc func = arm::USUB8_A1;
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_RD( 0x00000000 );
    R[n] = 0x0A0B0C0D; R[m] = 0x01020304; CHECK_RD( 0x09090909 );
    R[n] = 0x01020304; R[m] = 0xFEFDFCFB; CHECK_RD( 0x03050709 );
    R[n] = 0xA004B005; R[m] = 0x00050006; CHECK_RD( 0xA0FFB0FF );
    R[n] = 0xFFFFFFFF; R[m] = 0xFFFFFFFF; CHECK_RD( 0x00000000 );

    // GE flag
    R[n] = 0x00000000; R[m] = 0x00000000; CHECK_GE( 0xF );
    R[n] = 0x01010101; R[m] = 0x00000000; CHECK_GE( 0xF );
    R[n] = 0x01020304; R[m] = 0x05060708; CHECK_GE( 0x0 );
    R[n] = 0x00000001; R[m] = 0x00000002; CHECK_GE( 0xE );
    R[n] = 0x00000200; R[m] = 0x00000300; CHECK_GE( 0xD );
    R[n] = 0x00030000; R[m] = 0x00040000; CHECK_GE( 0xB );
    R[n] = 0x06000000; R[m] = 0x08000000; CHECK_GE( 0x7 );
}


BOOST_AUTO_TEST_CASE( UXTAB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6E01072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::UXTAB_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11111189 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11111167 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11111145 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11111123 );

    // Subtract
    R[n] = 0x00000001;
    R[m] = 0xFBFCFDFE;
    instr = op | 0x0 << 10; CHECK_RD( 0x000000FF );
    instr = op | 0x1 << 10; CHECK_RD( 0x000000FE );
    instr = op | 0x2 << 10; CHECK_RD( 0x000000FD );
    instr = op | 0x3 << 10; CHECK_RD( 0x000000FC );
}


BOOST_AUTO_TEST_CASE( UXTAB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6C01072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::UXTAB16_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11451189 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11231167 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11891145 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11671123 );

    // Subtract
    R[n] = 0x00010001;
    R[m] = 0xFBFCFDFE;
    instr = op | 0x0 << 10; CHECK_RD( 0x00FD00FF );
    instr = op | 0x1 << 10; CHECK_RD( 0x00FC00FE );
    instr = op | 0x2 << 10; CHECK_RD( 0x00FF00FD );
    instr = op | 0x3 << 10; CHECK_RD( 0x00FE00FC );
}


BOOST_AUTO_TEST_CASE( UXTAH_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6F01072;
    const uint32_t n = 0, d = 1, m = 2;

    // Add
    uint32_t instr = op;
    BehaviorFunc func = arm::UXTAH_A1;
    R[n] = 0x11111111;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x11116789 );
    instr = op | 0x1 << 10; CHECK_RD( 0x11114567 );
    instr = op | 0x2 << 10; CHECK_RD( 0x11112345 );
    instr = op | 0x3 << 10; CHECK_RD( 0x11118923 );

    // Subtract
    R[n] = 0x00000001;
    R[m] = 0xFFFDFFFE;
    instr = op | 0x0 << 10; CHECK_RD( 0x0000FFFF );
    instr = op | 0x1 << 10; CHECK_RD( 0x0000FE00 );
    instr = op | 0x2 << 10; CHECK_RD( 0x0000FFFE );
    instr = op | 0x3 << 10; CHECK_RD( 0x0000FF00 );
}


BOOST_AUTO_TEST_CASE( UXTB_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6FF0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::UXTB_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00000078 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00000056 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00000034 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00000012 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0x000000F8 );
    instr = op | 0x1 << 10; CHECK_RD( 0x000000D6 );
    instr = op | 0x2 << 10; CHECK_RD( 0x000000B4 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00000092 );
}


BOOST_AUTO_TEST_CASE( UXTB16_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6CF0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::UXTB16_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00340078 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00120056 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00780034 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00560012 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0x00B400F8 );
    instr = op | 0x1 << 10; CHECK_RD( 0x009200D6 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00F800B4 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00D60092 );
}


BOOST_AUTO_TEST_CASE( UXTH_A1_test )
{
    SETUP_TEST;
    const uint32_t op = 0xF6FF0071;
    const uint32_t d = 0, m = 1;

    // > 0
    uint32_t instr = op;
    BehaviorFunc func  = arm::UXTH_A1;
    R[m] = 0x12345678;
    instr = op | 0x0 << 10; CHECK_RD( 0x00005678 );
    instr = op | 0x1 << 10; CHECK_RD( 0x00003456 );
    instr = op | 0x2 << 10; CHECK_RD( 0x00001234 );
    instr = op | 0x3 << 10; CHECK_RD( 0x00007812 );

    // < 0
    R[m] = 0x92B4D6F8;
    instr = op | 0x0 << 10; CHECK_RD( 0x0000D6F8 );
    instr = op | 0x1 << 10; CHECK_RD( 0x0000B4D6 );
    instr = op | 0x2 << 10; CHECK_RD( 0x000092B4 );
    instr = op | 0x3 << 10; CHECK_RD( 0x0000F892 );
}


#endif // __ARMV7_INSTRUCTION_TEST_HPP__
