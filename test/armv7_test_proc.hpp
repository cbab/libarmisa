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
 * Test structures and convenience typedefs for armv7 function and
 * instruction tests.
 */

#ifndef __ARMV7_TEST_PROC_HPP__
#define __ARMV7_TEST_PROC_HPP__

#include <armv7/isa.hpp>
#include <boost/cstdint.hpp>


template< uint32_t mem_size >
struct test_mem
{
    union
    {
        uint64_t dwords[mem_size/8];
        uint32_t words [mem_size/4];
        uint16_t halves[mem_size/2];
        uint8_t  bytes [mem_size];
    };

    uint64_t read_dword( uint32_t addr ) const { return dwords[addr/8]; }
    uint32_t read_word ( uint32_t addr ) const { return  words[addr/4]; }
    uint16_t read_half ( uint32_t addr ) const { return halves[addr/2]; }
    uint8_t  read_byte ( uint32_t addr ) const { return  bytes[addr];   }

    void write_dword( uint32_t addr, uint64_t data ) { dwords[addr/8] = data; }
    void write_word ( uint32_t addr, uint32_t data ) {  words[addr/4] = data; }
    void write_half ( uint32_t addr, uint16_t data ) { halves[addr/2] = data; }
    void write_byte ( uint32_t addr,  uint8_t data ) {  bytes[addr]   = data; }
};


typedef uint32_t  test_field;
typedef uint32_t  test_reg;
typedef uint32_t* test_bank;
typedef arm::cpsr_adaptor< test_field > test_cpsr;
typedef arm::armv7_core  < test_cpsr, test_reg,
                           test_bank, test_mem<1024> > test_proc;

#endif // __ARMV7_TEST_PROC_HPP__

