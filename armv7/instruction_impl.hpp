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

#ifndef __ARMV7_INSTRUCTION_IMPL_HPP__
#define __ARMV7_INSTRUCTION_IMPL_HPP__

#include "function.hpp"
#include "instruction.hpp"
#include <boost/cstdint.hpp>

/*
 * Emit a warning whenever the "UNPREDICTABLE" state is reached.
 */

#ifndef UNPREDICTABLE
#include <iostream>
#define UNPREDICTABLE                                                   \
{                                                                       \
    std::cerr << "Warning: arm::" << __func__                           \
              << "(): entering unpredictable state." << std::endl;      \
}
#endif


template< typename proc_type >
void arm::ADC_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.1, p.326)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        bool setflags  = (S == 1);
        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], imm32, (uint32_t)proc.CPSR.C,
                               carry, overflow );
        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADC_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.2, p.328)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)proc.CPSR.C,
                               carry, overflow );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADC_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.3, p.330)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;
        bool setflags  = (S == 1);
        SRType shift_t = DecodeRegShift( type );

        if( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;

        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)proc.CPSR.C,
                               carry, overflow );

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}

template< typename proc_type >
void arm::ADD_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.5, p.334)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rn == 15) && (S == 0) )
        {
            // SEE ADR
            // TODO: Call ADR.
        }

        if( Rn == 13 )
        {
            // SEE ADD (SP plus immediate)
            // TODO: Call ADD (SP plus immediate)
        }

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        bool setflags  = (S == 1);
        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], imm32, (uint32_t)0,
                               carry, overflow );
        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADD_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.6, p.336)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        if( Rn == 13 )
        {
            // SEE ADD (SP plus register)
            // TODO: Call ADD (SP plus register)
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)0,
                               carry, overflow );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADD_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.3, p.330)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;
        bool setflags  = (S == 1);
        SRType shift_t = DecodeRegShift( type );

        if( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)0,
                               carry, overflow );

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}

template< typename proc_type >
void arm::ADD_SP_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.8, p.340)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        bool setflags  = (S == 1);
        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result, carry, overflow;

        // FIXME: SP is not always register 13! 
        //        (depends on the proc active mode)
        result = AddWithCarry( (uint32_t)proc.R[13], imm32, (uint32_t)0,
                               carry, overflow );
        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADD_SP_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.9, p.340)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        // FIXME: SP is not always register 13! 
        //        (depends on the proc active mode)
        result = AddWithCarry( (uint32_t)proc.R[13], shifted, (uint32_t)0,
                               carry, overflow );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}

template< typename proc_type >
void arm::ADR_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.10, p.344)
        // Encoding-specific operations
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t d = Rd;
        bool add = true;
        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result;

        if( add )
        {
            result = Align( proc.PC, 4 ) + imm32;
        } 
        else
        {
            result = Align( proc.PC, 4 ) - imm32;
        }


        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
        }
    }
}

template< typename proc_type >
void arm::ADR_A2( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.10, p.344)
        // Encoding-specific operations
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t d = Rd;
        bool add = false;
        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result;

        if( add )
        {
            result = Align( proc.PC, 4 ) + imm32;
        } 
        else
        {
            result = Align( proc.PC, 4 ) - imm32;
        }


        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
        }
    }
}

template< typename proc_type >
void arm::AND_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.11, p.346)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        bool setflags  = (S == 1);
        UValueCarry c_ = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t imm32 = c_.value;
        bool carry     = c_.carry;

        uint32_t result = proc.R[n] & imm32;

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::AND_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.12, p.348)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        bool carry       = c_.carry;

        uint32_t result = proc.R[n] & shifted;

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::AND_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.3, p.330)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;
        bool setflags  = (S == 1);
        SRType shift_t = DecodeRegShift( type );

        if( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        uint32_t carry = c_.carry;

        uint32_t result = proc.R[n] & shifted;

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            //proc.CPSR.V = overflow; UNCHANGED
        }
    }
}

template< typename proc_type >
void arm::ASR_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.14, p.352)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( (uint32_t)0b10, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], arm::SRType_ASR, s_.shift_n, proc.CPSR.C );
        uint32_t result = c_.value;
        bool carry      = c_.carry;

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::ASR_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.15, p.354)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rm   = Bits( instr, 11,  8 );
        uint32_t Rn   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;

        bool setflags  = (S == 1);

        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[m], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[n], arm::SRType_ASR, shift_n, proc.CPSR.C );
        uint32_t result = c_.value;
        bool carry      = c_.carry;

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            //proc.CPSR.V = overflow; UNCHANGED
        }
    }
}

template< typename proc_type >
void arm::B_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.16, p.356)
        // Encoding-specific operations
        uint32_t imm24 = Bits( instr, 23, 0 );
        // Shift to the right to add two 0 bits.
        // Effective size is 26 bits.
        uint32_t imm32 = (uint32_t)SignExtend( (imm24 << 2), 32, 26 );
        BranchWritePC( proc, proc.PC + imm32 );        
    }
}

template< typename proc_type >
void arm::BFC_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.17, p.358)
        // Encoding-specific operations
        uint32_t msb = Bits( instr, 20, 16 );
        uint32_t Rd  = Bits( instr, 15, 12 );
        uint32_t lsb = Bits( instr, 11,  7 );

        uint32_t d = Rd;
        uint32_t msbit = msb;
        uint32_t lsbit = lsb;

        if( d == 15 ) UNPREDICTABLE;
        if( msbit >= lsbit )
        {
            for(uint32_t i = lsb; i <= msb; ++i )
            {
                CLEAR_BIT( proc.R[d], i );
            }
        }
        else
        {
            UNPREDICTABLE;
        }
    }
}

template< typename proc_type >
void arm::BFI_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.18, p.360)
        // Encoding-specific operations
        uint32_t msb = Bits( instr, 20, 16 );
        uint32_t Rd  = Bits( instr, 15, 12 );
        uint32_t lsb = Bits( instr, 11,  7 );
        uint32_t Rn  = Bits( instr,  3,  0 );
        
        if( Rn == 15 )
        {
            // SEE BFC.
            BFC_A1( proc, instr );
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t msbit = msb;
        uint32_t lsbit = lsb;


        if( d == 15 ) UNPREDICTABLE;
        if( msbit >= lsbit )
        {
            for(uint32_t i = lsb; i <= msb; ++i )
            {
                // Naive implementation of bit copy.
                if( BIT_IS_SET( proc.R[n], i ) )
                {
                    SET_BIT( proc.R[d], i );
                }
                else
                {
                    CLEAR_BIT( proc.R[d], i );
                }
            }
        }
        else
        {
            UNPREDICTABLE;
        }
    }
}

template< typename proc_type >
void arm::BIC_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.19, p.362)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        bool setflags  = (S == 1);
        UValueCarry c_ = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t imm32 = c_.value;
        bool carry     = c_.carry;

        uint32_t result = proc.R[n] & NOT( imm32 );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::BIC_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.20, p.364)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        uint32_t carry   = c_.carry;
        uint32_t result = proc.R[n] & NOT( shifted );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::BIC_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.21, p.366)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;
        bool setflags  = (S == 1);
        SRType shift_t = DecodeRegShift( type );

        if( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        uint32_t carry = c_.carry;

        uint32_t result = proc.R[n] & NOT( shifted );

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            //proc.CPSR.V = overflow; UNCHANGED
        }
    }
}

template< typename proc_type >
void arm::BL_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.23, p.370)
        // Encoding-specific operations
        uint32_t imm24 = Bits( instr, 23, 0 );

        // Shift to the right to add two 0 bits.
        // Effective size is 26 bits.
        uint32_t imm32 = (uint32_t)SignExtend( (imm24 << 2), 32, 26 );
        bool toARM = true;
        
        if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        {
            uint32_t next_instr_addr = proc.PC - 4;
            // FIXME: LR register is not always register 14!
            proc.R[14] = next_instr_addr;
        }
        else
        {
            uint32_t next_instr_addr = proc.PC;
            // FIXME: LR register is not always register 14!
            proc.R[14] = SET_BIT( next_instr_addr, 0 );
        }

        if( toARM )
        {
            SelectInstrSet( proc, arm::InstrSet_ARM );
            BranchWritePC( proc, Align( proc.PC, 4 ) + imm32 );
        }
        else
        {
            SelectInstrSet( proc, arm::InstrSet_Thumb );
            BranchWritePC( proc, proc.PC + imm32 );
        }
    }
}

template< typename proc_type >
void arm::BLX_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.23, p.370)
        // Encoding-specific operations
        uint32_t H     = Bits( instr, 24, 24 );
        uint32_t imm24 = Bits( instr, 23, 0 );

        // Shift to the right to add one 0 bits and the H bit.
        // Effective size is 26 bits.
        uint32_t imm32 = (uint32_t)SignExtend( ((imm24 << 2) | H << 1), 32, 26 );
        bool toARM = false;
        
        if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        {
            uint32_t next_instr_addr = proc.PC - 4;
            // FIXME: LR register is not always register 14!
            proc.R[14] = next_instr_addr;
        }
        else
        {
            uint32_t next_instr_addr = proc.PC;
            // FIXME: LR register is not always register 14!
            proc.R[14] = SET_BIT( next_instr_addr, 0 );
        }

        if( toARM )
        {
            SelectInstrSet( proc, arm::InstrSet_ARM );
            BranchWritePC( proc, Align( proc.PC, 4 ) + imm32 );
        }
        else
        {
            SelectInstrSet( proc, arm::InstrSet_Thumb );
            BranchWritePC( proc, proc.PC + imm32 );
        }
    }
}

template< typename proc_type >
void arm::BLX_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.24, p.372)
        // Encoding-specific operations
        uint32_t Rm = Bits( instr, 3, 0 );

        uint32_t m = Rm;

        if( m == 15 ) UNPREDICTABLE;        
        if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        {
            uint32_t next_instr_addr = proc.PC - 4;
            // FIXME: LR register is not always register 14!
            proc.R[14] = next_instr_addr;
        }
        else
        {
            uint32_t next_instr_addr = proc.PC - 2;
            // FIXME: LR register is not always register 14!
            proc.R[14] = SET_BIT( next_instr_addr, 0 );
        }

        BXWritePC( proc, proc.R[m] );
    }
}

template< typename proc_type >
void arm::BX_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.25, p.374)
        // Encoding-specific operations
        uint32_t Rm = Bits( instr, 3, 0 );

        uint32_t m = Rm;

        BXWritePC( proc, proc.R[m] );
    }
}

template< typename proc_type >
void arm::CLZ_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.31, p.384)
        // Encoding-specific operations
        uint32_t Rd = Bits( instr, 15, 12 );
        uint32_t Rm = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t m = Rm;

        if( d == 15 || m == 15 ) UNPREDICTABLE;
        uint32_t result = CountLeadingZeroBits( proc.R[m] );
        proc.R[d] = result;
    }
}

template< typename proc_type >
void arm::CMN_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.32, p.386)
        // Encoding-specific operations
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t n = Rn;

        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], imm32, (uint32_t)0,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}

template< typename proc_type >
void arm::CMN_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.33, p.388)
        // Encoding-specific operations
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t n = Rn;
        uint32_t m = Rm;

        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)0,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}

template< typename proc_type >
void arm::CMN_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.34, p.390)
        // Encoding-specific operations
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;

        SRType shift_t = DecodeRegShift( type );

        if( n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], shifted, (uint32_t)0,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}


template< typename proc_type >
void arm::CMP_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.35, p.392)
        // Encoding-specific operations
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t n = Rn;

        uint32_t imm32 = ARMExpandImm( proc, imm12 );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], NOT( imm32 ), (uint32_t)1,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}


template< typename proc_type >
void arm::CMP_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.36, p.394)
        // Encoding-specific operations
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t n = Rn;
        uint32_t m = Rm;

        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], NOT( shifted ), (uint32_t)1,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}

template< typename proc_type >
void arm::CMP_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.37, p.396)
        // Encoding-specific operations
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;

        SRType shift_t = DecodeRegShift( type );

        if( n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], NOT( shifted ), (uint32_t)1,
                               carry, overflow );

        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}

template< typename proc_type >
void arm::EOR_imm_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.44, p.406)
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        bool setflags  = (S == 1);
        UValueCarry c_ = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t imm32 = c_.value;
        bool carry     = c_.carry;

        uint32_t result = proc.R[n] ^ imm32;

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        } 
        else 
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::EOR_reg_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.45, p.432)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            return;
        }

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool setflags  = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );

        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        bool carry       = c_.carry;

        uint32_t result = proc.R[n] ^ shifted;

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                //proc.CPSR.V = overflow; UNCHANGED
            }
        }
    }
}

template< typename proc_type >
void arm::EOR_rsr_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.46, p.410)
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t Rn   = Bits( instr, 19, 16 );
        uint32_t Rd   = Bits( instr, 15, 12 );
        uint32_t Rs   = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t Rm   = Bits( instr,  3,  0 );

        uint32_t d = Rd;
        uint32_t n = Rn;
        uint32_t m = Rm;
        uint32_t s = Rs;
        bool setflags  = (S == 1);
        SRType shift_t = DecodeRegShift( type );

        if( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        uint32_t carry = c_.carry;

        uint32_t result = proc.R[n] ^ shifted;

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            //proc.CPSR.V = overflow; UNCHANGED
        }
    }
}

template< typename proc_type >
void arm::LDM_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.53, p.422)
        // Encoding-specific operations
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t Rn            = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        if( W == 1 && Rn == 13 
            && ( BitCount( register_list ) >= 2 ) )
        {
            // SEE POP
            // TODO: Call POP
        }
        
        uint32_t n = Rn;
        uint32_t registers = register_list;
        bool wback = (W == 1);

        if( n == 15 || BitCount( registers ) < 1) UNPREDICTABLE;
        if( wback && (Bits( registers, n, n ) == 1) && ArchVersion() >= 7 )
            UNPREDICTABLE;

        NullCheckIfThumbEE( proc, n );
        uint32_t address = proc.R[n];

        for(uint32_t i = 0; i <= 14; ++i)
        {
            if( Bits( registers, i, i ) == 1 )
            {
                // MemA
                proc.R[i] = proc.dMem.read_word( address );
                address += 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 )
        {
            LoadWritePC( proc, proc.dMem.read_word( address ) );
        }

        if( wback && (Bits( registers, n, n ) == 0) )
        {
            proc.R[n] += 4*BitCount( registers );
        }

        if( wback && (Bits( registers, n, n ) == 1) )
        {
            proc.R[n] = 0xDEADDEAD; // UNKNOWN
        }
    }
}

template< typename proc_type >
void arm::LDMDA_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.54, p.424)
        // Encoding-specific operations
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t Rn            = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t n = Rn;
        uint32_t registers = register_list;
        bool wback = (W == 1);

        if( n == 15 || BitCount( registers ) < 1) UNPREDICTABLE;
        if( wback && (Bits( registers, n, n ) == 1) && ArchVersion() >= 7 )
            UNPREDICTABLE;
        uint32_t address = proc.R[n] - 4*BitCount( registers ) + 4;

        for(uint32_t i = 0; i <= 14; ++i)
        {
            if( Bits( registers, i, i ) == 1 )
            {
                // MemA
                proc.R[i] = proc.dMem.read_word( address );
                address += 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 )
        {
            LoadWritePC( proc, proc.dMem.read_word( address ) );
        }

        if( wback && (Bits( registers, n, n ) == 0) )
        {
            proc.R[n] -= 4*BitCount( registers );
        }

        if( wback && (Bits( registers, n, n ) == 1) )
        {
            proc.R[n] = 0xDEADDEAD; // UNKNOWN
        }
    }
}


template< typename proc_type >
void arm::LDMDB_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.55, p.426)
        // Encoding-specific operations
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t Rn            = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t n = Rn;
        uint32_t registers = register_list;
        bool wback = (W == 1);

        if( n == 15 || BitCount( registers ) < 1) UNPREDICTABLE;
        if( wback && (Bits( registers, n, n ) == 1) && ArchVersion() >= 7 )
            UNPREDICTABLE;

        NullCheckIfThumbEE( proc, n );
        uint32_t address = proc.R[n] - 4*BitCount( registers );

        for(uint32_t i = 0; i <= 14; ++i)
        {
            if( Bits( registers, i, i ) == 1 )
            {
                // MemA
                proc.R[i] = proc.dMem.read_word( address );
                address += 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 )
        {
            LoadWritePC( proc, proc.dMem.read_word( address ) );
        }

        if( wback && (Bits( registers, n, n ) == 0) )
        {
            proc.R[n] -= 4*BitCount( registers );
        }

        if( wback && (Bits( registers, n, n ) == 1) )
        {
            proc.R[n] = 0xDEADDEAD; // UNKNOWN
        }
    }
}

template< typename proc_type >
void arm::LDMIB_A1( proc_type& proc, uint32_t instr )
{
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // (A8.6.56, p.428)
        // Encoding-specific operations
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t Rn            = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t n = Rn;
        uint32_t registers = register_list;
        bool wback = (W == 1);

        if( n == 15 || BitCount( registers ) < 1) UNPREDICTABLE;
        if( wback && (Bits( registers, n, n ) == 1) && ArchVersion() >= 7 )
            UNPREDICTABLE;

        uint32_t address = proc.R[n] + 4;

        for(uint32_t i = 0; i <= 14; ++i)
        {
            if( Bits( registers, i, i ) == 1 )
            {
                // MemA
                proc.R[i] = proc.dMem.read_word( address );
                address += 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 )
        {
            LoadWritePC( proc, proc.dMem.read_word( address ) );
        }

        if( wback && (Bits( registers, n, n ) == 0) )
        {
            proc.R[n] += 4*BitCount( registers );
        }

        if( wback && (Bits( registers, n, n ) == 1) )
        {
            proc.R[n] = 0xDEADDEAD; // UNKNOWN
        }
    }
}

template< typename proc_type >
void arm::LDR_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.58, p.432)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
        
        if( Rn == 0b1111 )
        {
            // SEE LDR (literal)
            arm::LDR_lit_A1( proc, instr );
            return;
        }
        
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRT
            if( Bits( instr, 25, 25 ) == 0 )
            {
                arm::LDRT_A1( proc, instr );
            }
            else
            {
                arm::LDRT_A2( proc, instr );
            }
            return;
        }
        
        if( (Rn == 0b1011) && (P == 0) && (U == 1) && (W == 0) &&
            (imm12 == 0b000000000100) )
        {
            // SEE POP
            //TODO
            //POP_A2( proc, instr );
            return;
        }
        
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( imm12 );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
        
        if( wback && (n == t) ) UNPREDICTABLE;
    
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_word( address );
        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( t == 15 )
        {
            if( Bits( address, 1, 0 ) == 0 )
            {
                LoadWritePC( proc, data );
            }
            else UNPREDICTABLE
        }
        else if( UnalignedSupport() || (Bits( address, 1, 0 ) == 0) )
        {
            proc.R[t] = data;
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = ROR( data, 8*Bits( address, 1, 0 ) );
        //}
    }
}

template< typename proc_type >
void arm::LDR_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.59, p.434)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t t     = Rt;
        uint32_t imm32 = ZeroExtend( imm12 );
        bool add       = (U == 1);
    
        NullCheckIfThumbEE( proc, 15 );

        uint32_t base = Align( proc.PC, 4 );
        uint32_t address;
        if( add )
        {
            address = base + imm32;
        }
        else
        {
            address = base - imm32;
        }

        // MemU
        uint32_t data = proc.dMem.read_word( address );

        if( t == 15 )
        {
            if( Bits( address, 1,  0 ) == 0 )
            {
                LoadWritePC( proc, data );
            }
            else UNPREDICTABLE
        }
        else if( UnalignedSupport() || (Bits( address, 1,  0 ) == 0) )
        {
            proc.R[t] = data;
        }
        //else // Can only apply before ARMv7
        //{
        //    if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        //    {
        //        proc.R[t] = ROR( data, 8*Bits( address, 1, 0 ) );
        //    }
        //    else
        //    {
        //        proc.R[t] = 0; // UNKNOWN
        //    }
        //}
    }
}

template< typename proc_type >
void arm::LDR_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.60, p.436)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRT
            if( Bits( instr, 25, 25 ) == 0 )
            {
                arm::LDRT_A1( proc, instr );
            }
            else
            {
                arm::LDRT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t = Rt;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool index = (P == 1);
        bool add   = (U == 1);
        bool wback = (P == 0) || (W == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );
    
        if( m == 15 ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
    
        NullCheckIfThumbEE( proc, n );

        uint32_t offset = Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_word( address );
        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( t == 15 )
        {
            if( Bits( address, 1,  0 ) == 0 )
            {
                LoadWritePC( proc, data );
            }
            else UNPREDICTABLE
        }
        else if( UnalignedSupport() || (Bits( address, 1,  0 ) == 0) )
        {
            proc.R[t] = data;
        }
        //else // Can only apply before ARMv7
        //{
        //    if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        //    {
        //        proc.R[t] = ROR( data, 8*Bits( address, 1, 0 ) );
        //    }
        //    else
        //    {
        //        proc.R[t] = 0; // UNKNOWN
        //    }
        //}
    }
}


template< typename proc_type >
void arm::LDRB_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.62, p.440)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        if( Rn == 0b1111 )
        {
            // SEE LDRB (literal)
            arm::LDRB_lit_A1( proc, instr );
            return;
        }
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRBT
            if( Bits( instr, 25, 25 ) == 0 )
            {
                arm::LDRBT_A1( proc, instr );
            }
            else
            {
                arm::LDRBT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( imm12 );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (t == 15) || (wback && (n == t)) ) UNPREDICTABLE;
        
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        proc.R[t] = ZeroExtend( proc.dMem.read_byte( address ) );
        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRB_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.63, p.442)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t t     = Rt;
        uint32_t imm32 = ZeroExtend( imm12 );
        bool add       = (U == 1);
    
        if( t == 15 ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, 15 );

        uint32_t base = Align( proc.PC, 4 );
        uint32_t address;
        if( add )
        {
            address = base + imm32;
        }
        else
        {
            address = base - imm32;
        }

        // MemU
        proc.R[t] = ZeroExtend( proc.dMem.read_byte( address ) );
    }
}


template< typename proc_type >
void arm::LDRB_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.64, p.444)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRBT
            if( Bits( instr, 25, 25 ) == 0 )
            {
                arm::LDRBT_A1( proc, instr );
            }
            else
            {
                arm::LDRBT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t = Rt;
        uint32_t n = Rn;
        uint32_t m = Rm;
        bool index = (P == 1);
        bool add   = (U == 1);
        bool wback = (P == 0) || (W == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );
    
        if( (t == 15) || (m == 15) ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset =
            Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        proc.R[t] = ZeroExtend( proc.dMem.read_byte( address ) );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRBT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.65, p.446)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = false;
        uint32_t imm32     = ZeroExtend( imm12 );
    
        if( (t == 15) || (n == 15) || (n == t) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            // always false
            //offset = Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        }
        else
        {
            offset = imm32;
        }

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        else
        {
            address = offset_addr;
        }

        // MemU_unpriv
        proc.R[t] = ZeroExtend( proc.dMem.read_byte( address ) );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRBT_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.65, p.446)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;
        ShiftUValue s_     = DecodeImmShift( type, imm5 );
    
        if( (t == 15) || (n == 15) || (n == t) || (m == 15) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && (m == n) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            offset = Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        }
        //else // never called for encoding A2
        //{
        //    offset = imm32;
        //}

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A2
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        proc.R[t] = ZeroExtend( proc.dMem.read_byte( address ) );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRD_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.66, p.448)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        if( Rn == 0b1111 )
        {
            // SEE LDRD (literal)
            arm::LDRD_lit_A1( proc, instr );
            return;
        }
    
        if( Bits( Rt, 0, 0 ) == 1 );    // UNDEFINED
    
        uint32_t t     = Rt;
        uint32_t t2    = t + 1;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (P == 0) && (W == 1) ) UNPREDICTABLE;
        if( wback && ((n == t) || (n == t2)) ) UNPREDICTABLE;
        if( (t2 == 15) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemA
        proc.R[t] = ZeroExtend( proc.dMem.read_word( address ) );
        proc.R[t2] = ZeroExtend( proc.dMem.read_word( address + 4 ) );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRD_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.67, p.450)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        if( Bits( Rt, 0, 0 ) == 1 );    // UNDEFINED
    
        uint32_t t     = Rt;
        uint32_t t2    = t + 1;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool add       = (U == 1);
    
        if( t2 == 15 ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, 15 );

        uint32_t address;
        if( add )
        {
            address = Align( proc.PC, 4 ) + imm32;
        }
        else
        {
            address = Align( proc.PC, 4 ) - imm32;
        }

        // MemA
        proc.R[t] = ZeroExtend( proc.dMem.read_word( address ) );
        proc.R[t2] = ZeroExtend( proc.dMem.read_word( address + 4 ) );
    }
}


template< typename proc_type >
void arm::LDRD_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.68, p.452)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( Bits( Rt, 0, 0 ) == 1 );    // UNDEFINED
    
        uint32_t t     = Rt;
        uint32_t t2    = t + 1;
        uint32_t n     = Rn;
        uint32_t m     = Rm;
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (P == 0) && (W == 1) ) UNPREDICTABLE;
        if( (t2 == 15) || (m == 15) || (m == t) || (m == t2) ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t) || (n == t2)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + proc.R[m];
        }
        else
        {
            offset_addr = proc.R[n] - proc.R[m];
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemA
        proc.R[t] = ZeroExtend( proc.dMem.read_word( address ) );
        proc.R[t2] = ZeroExtend( proc.dMem.read_word( address + 4 ) );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRH_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.74, p.464)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        if( Rn == 0b1111 )
        {
            // SEE LDRH (literal)
            arm::LDRH_lit_A1( proc, instr );
            return;
        }
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRHT
            if( Bits( instr,  22,  22 ) == 1 )
            {
                arm::LDRHT_A1( proc, instr );
            }
            else
            {
                arm::LDRHT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (t == 15) || (wback && (n == t)) ) UNPREDICTABLE;
        
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = ZeroExtend( data );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}

template< typename proc_type >
void arm::LDRH_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.75, p.466)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t     = Rt;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool add       = (U == 1);
    
        if( t == 15 ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, 15 );

        uint32_t base = Align( proc.PC, 4 );
        uint32_t address;
        if( add )
        {
            address = base + imm32;
        }
        else
        {
            address = base - imm32;
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = ZeroExtend( data );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRH_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.76, p.468)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRHT
            if( Bits( instr,  22,  22 ) == 1 )
            {
                arm::LDRHT_A1( proc, instr );
            }
            else
            {
                arm::LDRHT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t m     = Rm;
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
        ShiftUValue s_;
        s_.shift_t = arm::SRType_LSL;
        s_.shift_n = 0;
    
        if( (t == 15) || (m == 15) ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
    
        NullCheckIfThumbEE( proc, n );

        uint32_t offset = Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = ZeroExtend( data );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRHT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.77, p.470)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        bool postindex     = true;
        bool add           = (U == 1);
        //bool register_form = false;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
    
        if( (t == 15) || (n == 15) || (n == t) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        //if( register_form )   // never called for encoding A1
        //{
        //    offset = proc.R[m];
        //}
        //else
        {
            offset = imm32;
        }

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else// never called for encoding A1
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_half( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = ZeroExtend( data );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRHT_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.77, p.470)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;
    
        if( (t == 15) || (n == 15) || (n == t) || (m == 15) ) UNPREDICTABLE;
        
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            offset = proc.R[m];
        }
        //else // never called for encoding A2
        //{
        //    offset = imm32;
        //}

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A2
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_half( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = ZeroExtend( data );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRSB_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.78, p.472)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        if( Rn == 0b1111 )
        {
            // SEE LDRSB (literal)
            arm::LDRSB_lit_A1( proc, instr );
            return;
        }
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRSBT
            if( Bits( instr, 22, 22 ) == 0 )
            {
                arm::LDRSBT_A2( proc, instr );
            }
            else
            {
                arm::LDRSBT_A1( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (t == 15) || (wback && (n == t)) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU_unpriv
        proc.R[t] = SignExtend( proc.dMem.read_byte( address ), 32, 8 );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRSB_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.79, p.474)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t     = Rt;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool add       = (U == 1);
    
        if( t == 15 ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, 15 );

        uint32_t base = Align( proc.PC, 4 );
        uint32_t address;
        if( add )
        {
            address = base + imm32;
        }
        else
        {
            address = base - imm32;
        }

        // MemU
        proc.R[t] = SignExtend( proc.dMem.read_byte( address ), 32, 8 );
    }
}


template< typename proc_type >
void arm::LDRSB_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.80, p.476)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRSBT
            if( Bits( instr, 22, 22 ) == 0 )
            {
                arm::LDRSBT_A2( proc, instr );
            }
            else
            {
                arm::LDRSBT_A1( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t m     = Rm;
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
        ShiftUValue s_;
        s_.shift_t = arm::SRType_LSL;
        s_.shift_n = 0;
    
        if( (t == 15) || (m == 15) ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset =
            Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        proc.R[t] = SignExtend( proc.dMem.read_byte( address ), 32, 8 );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRSBT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.81, p.478)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        bool postindex     = true;
        bool add           = (U == 1);
        //bool register_form = false;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
    
        if( (t == 15) || (n == 15) || (n == t) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        //if( register_form )   // never called for encoding A1
        //{
        //    offset = proc.R[m];
        //}
        //else
        {
            offset = imm32;
        }

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A1
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        proc.R[t] = SignExtend( proc.dMem.read_byte( address ), 32, 8 );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRSBT_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.81, p.478)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;
    
        if( (t == 15) || (n == 15) || (n == t) || (m == 15) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            offset = proc.R[m];
        }
        //else // never called for encoding A2
        //{
        //    offset = imm32;
        //}

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A2
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        proc.R[t] = SignExtend( proc.dMem.read_byte( address ), 32, 8 );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }
    }
}


template< typename proc_type >
void arm::LDRSH_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.82, p.480)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        if( Rn == 0b1111 )
        {
            // SEE LDRSH (literal)
            arm::LDRSH_lit_A1( proc, instr );
            return;
        }
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRSHT
            if( Bits( instr, 22, 22 ) == 1 )
            {
                arm::LDRSHT_A1( proc, instr );
            }
            else
            {
                arm::LDRSHT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
    
        if( (t == 15) || (wback && (n == t)) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + imm32;
        }
        else
        {
            offset_addr = proc.R[n] - imm32;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = SignExtend( data, 32, 16 );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRSH_lit_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.83, p.482)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t     = Rt;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
        bool add       = (U == 1);
    
        if( t == 15 ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, 15 );

        uint32_t base = Align( proc.PC, 4 );
        uint32_t address;
        if( add )
        {
            address = base + imm32;
        }
        else
        {
            address = base - imm32;
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = SignExtend( data, 32, 16 );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRSH_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.84, p.484)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (P == 0) && (W == 1) )
        {
            // SEE LDRSHT
            if( Bits( instr, 22, 22 ) == 1 )
            {
                arm::LDRSHT_A1( proc, instr );
            }
            else
            {
                arm::LDRSHT_A2( proc, instr );
            }
            return;
        }
    
        uint32_t t     = Rt;
        uint32_t n     = Rn;
        uint32_t m     = Rm;
        bool index     = (P == 1);
        bool add       = (U == 1);
        bool wback     = (P == 0) || (W == 1);
        ShiftUValue s_;
        s_.shift_t = arm::SRType_LSL;
        s_.shift_n = 0;
    
        if( (t == 15) || (m == 15) ) UNPREDICTABLE;
        if( wback && ((n == 15) || (n == t)) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && wback && (m == n) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset =
            Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( index )
        {
            address = offset_addr;
        }
        else
        {
            address = proc.R[n];
        }

        // MemU
        uint32_t data = proc.dMem.read_half( address );

        if( wback )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = SignExtend( data, 32, 16 );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRSHT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.85, p.486)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        bool postindex     = true;
        bool add           = (U == 1);
        //bool register_form = false;
        uint32_t imm32 = ZeroExtend( (imm4H << 4) | imm4L );
    
        if( (t == 15) || (n == 15) || (n == t) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        //if( register_form ) // never called for encoding A1
        //{
        //    offset = proc.R[m];
        //}
        //else
        {
            offset = imm32;
        }

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_half( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = SignExtend( data, 32, 16 );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRSHT_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.85, p.486)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;
    
        if( (t == 15) || (n == 15) || (n == t) || (m == 15) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            offset = proc.R[m];
        }
        //else // never called for encoding A2
        //{
        //    offset = imm32;
        //}

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A1
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_half( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 0, 0 ) == 0) )
        {
            proc.R[t] = SignExtend( data, 32, 16 );
        }
        //else // Can only apply before ARMv7
        //{
        //    proc.R[t] = 0; // UNKNOWN
        //}
    }
}


template< typename proc_type >
void arm::LDRT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.86, p.488)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        bool postindex     = true;
        bool add           = (U == 1);
        //bool register_form = false;
        uint32_t imm32 = ZeroExtend( imm12 );
    
        if( (t == 15) || (n == 15) || (n == t) ) UNPREDICTABLE;
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        //if( register_form ) // never called for encoding A1
        //{
        //
        //}
        //else
        {
            offset = imm32;
        }

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A1
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_word( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 1, 0 ) == 0) )
        {
            proc.R[t] = data;
        }
        //else // Can only apply before ARMv7
        //{
        //    if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        //    {
        //        proc.R[t] = ROR( data, 8*Bits( address, 1, 0 ) );
        //    }
        //    else
        //    {
        //        proc.R[t] = 0; // UNKNOWN
        //    }
        //}
    }
}


template< typename proc_type >
void arm::LDRT_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.86, p.488)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t Rn    = Bits( instr, 19, 16 );
        uint32_t Rt    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t t         = Rt;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;
        ShiftUValue s_     = DecodeImmShift( type, imm5 );
    
        if( (t == 15) || (n == 15) || (n == t) || (m == 15) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && (m == n) ) UNPREDICTABLE;
        
        NullCheckIfThumbEE( proc, n );

        uint32_t offset;
        if( register_form )
        {
            offset = Shift( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        }
        //else // never called for encoding A2
        //{
        //
        //}

        uint32_t offset_addr;
        if( add )
        {
            offset_addr = proc.R[n] + offset;
        }
        else
        {
            offset_addr = proc.R[n] - offset;
        }

        uint32_t address;
        if( postindex )
        {
            address = proc.R[n];
        }
        //else // never called for encoding A2
        //{
        //    address = offset_addr;
        //}

        // MemU_unpriv
        uint32_t data = proc.dMem.read_word( address );

        if( postindex )
        {
            proc.R[n] = offset_addr;
        }

        if( UnalignedSupport() || (Bits( address, 1, 0 ) == 0) )
        {
            proc.R[t] = data;
        }
        //else // Can only apply before ARMv7
        //{
        //    if( CurrentInstrSet( proc ) == arm::InstrSet_ARM )
        //    {
        //        proc.R[t] = ROR( data, 8*Bits( address, 1, 0 ) );
        //    }
        //    else
        //    {
        //        proc.R[t] = 0; // UNKNOWN
        //    }
        //}
    }
}


template< typename proc_type >
void arm::LSL_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.88, p.490)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( imm5 == 0 )
        {
            arm::MOV_reg_A1( proc, instr );
        }
    
        uint32_t d         = Rd;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
        ShiftUValue s_     = DecodeImmShift( 0, imm5 );
        UValueCarry c_;
        c_ = Shift_C( proc.R[m], arm::SRType_LSL, s_.shift_n, proc.CPSR.C );

        if( d == 15 )   // Can only occur for ARM encoding
        {
            ALUWritePC( proc, c_.value ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = c_.value;
            if( setflags )
            {
                proc.CPSR.N = Bits( c_.value, 31, 31 );
                proc.CPSR.Z = IsZeroBit( c_.value );
                proc.CPSR.C = c_.carry;
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::LSL_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.89, p.492)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr, 11,  8 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
    
        if( (d == 15) || (n == 15) || (m == 15) ) UNPREDICTABLE;
        uint32_t shift_n = Bits( proc.R[m],  7,  0 );
        UValueCarry c_;
        c_ = Shift_C( proc.R[n], arm::SRType_LSL, shift_n, proc.CPSR.C );

        proc.R[d] = c_.value;

        if( setflags )
        {
            proc.CPSR.N = Bits( c_.value, 31, 31 );
            proc.CPSR.Z = IsZeroBit( c_.value );
            proc.CPSR.C = c_.carry;
            // proc.CPSR.V unchanged
        }
    }
}


template< typename proc_type >
void arm::LSR_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.90, p.494)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
        ShiftUValue s_     = DecodeImmShift( 1, imm5 );
        UValueCarry c_;
        c_ = Shift_C( proc.R[m], arm::SRType_LSR, s_.shift_n, proc.CPSR.C );

        if( d == 15 )   // Can only occur for ARM encoding
        {
            ALUWritePC( proc, c_.value ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = c_.value;
            if( setflags )
            {
                proc.CPSR.N = Bits( c_.value, 31, 31 );
                proc.CPSR.Z = IsZeroBit( c_.value );
                proc.CPSR.C = c_.carry;
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::LSR_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.91, p.496)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr, 11,  8 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
    
        if( (d == 15) || (n == 15) || (m == 15) ) UNPREDICTABLE;
    
        uint32_t shift_n = Bits( proc.R[m],  7,  0 );
        UValueCarry c_;
        c_ = Shift_C( proc.R[n], arm::SRType_LSR, shift_n, proc.CPSR.C );

        proc.R[d] = c_.value;

        if( setflags )
        {
            proc.CPSR.N = Bits( c_.value, 31, 31 );
            proc.CPSR.Z = IsZeroBit( c_.value );
            proc.CPSR.C = c_.carry;
            // proc.CPSR.V unchanged
        }
    }
}


template< typename proc_type >
void arm::MLA_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.94, p.502)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 19, 16 );
        uint32_t Ra    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr, 11,  8 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        uint32_t a         = Ra;
        bool setflags      = (S == 1);
    
        if( (d == 15) || (n == 15) || (m == 15) || (a == 15) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && (d == n) ) UNPREDICTABLE;
        uint32_t operand1 = SInt( proc.R[n] );
        uint32_t operand2 = SInt( proc.R[m] );
        uint32_t addend   = SInt( proc.R[a] );
        uint32_t result   = operand1 * operand2 + addend;

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            if( ArchVersion() == 4 )
            {
                proc.CPSR.C = 0; // bit UNKNOWN
            }
            // else proc.CPSR.C unchanged
            // proc.CPSR.V always unchanged
        }
    }
}


template< typename proc_type >
void arm::MLS_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.95, p.504)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t Rd    = Bits( instr, 19, 16 );
        uint32_t Ra    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr, 11,  8 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        uint32_t a         = Ra;
    
        if( (d == 15) || (n == 15) || (m == 15) || (a == 15) ) UNPREDICTABLE;
    
        uint32_t operand1 = SInt( proc.R[n] );
        uint32_t operand2 = SInt( proc.R[m] );
        uint32_t addend   = SInt( proc.R[a] );
        uint32_t result   = addend - operand1 * operand2;

        proc.R[d] = result;
    }
}


template< typename proc_type >
void arm::MOV_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.96, p.506)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        if( (Rd == 0b1111) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instruction
            // TODO
            return;
        }
    
        uint32_t d         = Rd;
        bool setflags      = (S == 1);
        UValueCarry c_     = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t imm32     = c_.value;
        bool carry         = c_.carry;
    
        uint32_t result = imm32;
        if( d == 15 )   // Can only occur for encoding A1
        {
            ALUWritePC( proc, result ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::MOV_imm_A2( proc_type& proc, uint32_t instr )
{
    // (A8.6.96, p.506)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t imm4  = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t d         = Rd;
        //bool setflags      = false;
        uint32_t imm32     = ZeroExtend( (imm4 << 12) | imm12 );
    
        if( d == 15 ) UNPREDICTABLE;
    
        uint32_t result = imm32;
        if( d == 15 )   // Can only occur for encoding A1
        {
            ALUWritePC( proc, result ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = result;

            //if( setflags )    // always false
            //{
            //    proc.CPSR.N = Bits( result, 31, 31 );
            //    proc.CPSR.Z = IsZeroBit( result );
            //    proc.CPSR.C = carry;
            //    // proc.CPSR.V unchanged
            //}
        }
    }
}


template< typename proc_type >
void arm::MOV_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.97, p.508)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (Rd == 15) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instructions
            // TODO
            return;
        }
    
        uint32_t d         = Rd;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
    
        uint32_t result = proc.R[m];
        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::MOVT_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.99, p.512)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t imm4  = Bits( instr, 19, 16 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        uint32_t d         = Rd;
        uint32_t imm16     = (imm4 << 12) | imm12;
    
        if( d == 15 ) UNPREDICTABLE;
    
        proc.R[d] = (imm16 << 16) | Bits( proc.R[d], 15,  0 );
    }
}


template< typename proc_type >
void arm::MRS_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.102, p.518)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t Rd    = Bits( instr, 15, 12 );
    
        uint32_t d         = Rd;
    
        if( d == 15 ) UNPREDICTABLE;
    
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
        proc.R[d] = cpsr;
    }
}


template< typename proc_type >
void arm::MSR_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.103, p.520)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t mask  = Bits( instr, 19, 18 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        if( mask == 0 )
        {
            // SEE related encoding
            // TODO
            return;
        }
    
        uint32_t imm32 = ARMExpandImm( proc, imm12 );
        bool write_nzcvq = (Bits( mask, 1,  1 ) == 1);
        bool write_g     = (Bits( mask, 0,  0 ) == 1);
    
        if( write_nzcvq )
        {
            proc.CPSR.N = Bits( imm32, 31, 31 );
            proc.CPSR.Z = Bits( imm32, 30, 30 );
            proc.CPSR.C = Bits( imm32, 29, 29 );
            proc.CPSR.V = Bits( imm32, 28, 28 );
            proc.CPSR.Q = Bits( imm32, 27, 27 );
        }

        if( write_g )
        {
            proc.CPSR.GE = Bits( imm32, 19, 16 );
        }
    }
}


template< typename proc_type >
void arm::MSR_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.104, p.522)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t mask  = Bits( instr, 19, 18 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t n         = Rn;
        bool write_nzcvq   = (Bits( mask, 1,  1 ) == 1);
        bool write_g       = (Bits( mask, 0,  0 ) == 1);
    
        if( mask == 0 ) UNPREDICTABLE;
        if( n == 15 ) UNPREDICTABLE;
    
        if( write_nzcvq )
        {
            proc.CPSR.N = Bits( proc.R[n], 31, 31 );
            proc.CPSR.Z = Bits( proc.R[n], 30, 30 );
            proc.CPSR.C = Bits( proc.R[n], 29, 29 );
            proc.CPSR.V = Bits( proc.R[n], 28, 28 );
            proc.CPSR.Q = Bits( proc.R[n], 27, 27 );
        }

        if( write_g )
        {
            proc.CPSR.GE = Bits( proc.R[n], 19, 16 );
        }
    }
}


template< typename proc_type >
void arm::MUL_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.105, p.524)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 19, 16 );
        uint32_t Rm    = Bits( instr, 11,  8 );
        uint32_t Rn    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t n         = Rn;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
    
        if( (d == 15) || (n == 15) || (m == 15) ) UNPREDICTABLE;
        if( (ArchVersion() < 6) && (d == n) ) UNPREDICTABLE;
        
        uint32_t operand1 = SInt( proc.R[n] );
        uint32_t operand2 = SInt( proc.R[m] );
        uint32_t result   = operand1 * operand2;

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            if( ArchVersion() == 4 )
            {
                proc.CPSR.C = 0; // bit UNKNOWN
            }
            // else proc.CPSR.C unchanged
            // proc.CPSR.V always unchanged
        }
    }
}


template< typename proc_type >
void arm::MVN_imm_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.106, p.526)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );
    
        if( (Rd == 0b1111) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instructions
            return;
        }
    
        uint32_t d         = Rd;
        bool setflags      = (S == 1);
        UValueCarry c_     = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t imm32     = c_.value;
        bool carry         = c_.carry;
    
        uint32_t result = NOT( imm32 );
        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;
            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::MVN_reg_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.107, p.528)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t imm5  = Bits( instr, 11,  7 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        if( (Rd == 0b1111) && (S == 1) )
        {
            // SEE SUBS PC, LR and related instructions
            // TODO
            return;
        }
    
        uint32_t d         = Rd;
        uint32_t m         = Rm;
        bool setflags      = (S == 1);
        ShiftUValue s_ = DecodeImmShift( type, imm5 );
    
        UValueCarry c_ =
            Shift_C( proc.R[m], s_.shift_t, s_.shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        bool carry = c_.carry;

        uint32_t result = NOT( shifted );

        if( d == 15 )
        {
            ALUWritePC( proc, result );
        }
        else
        {
            proc.R[d] = result;

            if( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                // proc.CPSR.V unchanged
            }
        }
    }
}


template< typename proc_type >
void arm::MVN_rsr_A1( proc_type& proc, uint32_t instr )
{
    // (A8.6.108, p.530)
    // Operation
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t Rd    = Bits( instr, 15, 12 );
        uint32_t Rs    = Bits( instr, 11,  8 );
        uint32_t type  = Bits( instr,  6,  5 );
        uint32_t Rm    = Bits( instr,  3,  0 );
    
        uint32_t d         = Rd;
        uint32_t m         = Rm;
        uint32_t s         = Rs;
        bool setflags      = (S == 1);
        SRType shift_t = DecodeRegShift( type );
    
        if( (d == 15) || (m == 15) || (s == 15) ) UNPREDICTABLE;
    
        uint32_t shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry c_ = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        uint32_t shifted = c_.value;
        bool carry = c_.carry;

        uint32_t result = NOT( shifted );

        proc.R[d] = result;

        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            // proc.CPSR.V unchanged
        }
    }
}


template< typename proc_type >
void arm::NOP_A1( proc_type& proc, uint32_t instr )
{
    // Do nothing
}


template< typename proc_type >
void arm::ORR_imm_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t    S        = Bits( instr, 20, 20 );
    const uint32_t    d        = Bits( instr, 15, 12 );
    const uint32_t    n        = Bits( instr, 19, 16 );
    const uint32_t    imm12    = Bits( instr, 11,  0 );
    const bool        setflags = ( S == 1 );
    const UValueCarry imm32    = ARMExpandImm_C( imm12, proc.CPSR.C );

    if( ( d == 0xF ) && ( S == 1 ) )
    {
        // FIXME: See SUBS PC, LR...
        return;
    }

    uint32_t result = proc.R[n] | imm32.value;

    // Instruction code
    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = imm32.carry;
        }
    }
}


template< typename proc_type >
void arm::ORR_reg_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }
    
    // Encoding-specific operations
    const uint32_t    S        = Bits( instr, 20, 20 );
    const uint32_t    d        = Bits( instr, 15, 12 );
    const uint32_t    m        = Bits( instr,  3,  0 );
    const uint32_t    n        = Bits( instr, 19, 16 );
    const bool        setflags = ( S == 1 );
    const uint32_t    type     = Bits( instr,  6,  5 );
    const uint32_t    imm5     = Bits( instr, 11,  7 );

    if( ( d == 0xF ) && ( S == 1 ) )
    {
        // FIXME: See SUBS PC, LR...
        return;
    }
    
    ShiftUValue shift = DecodeImmShift( type, imm5 );

    // Instruction code
    UValueCarry shifted = Shift_C( proc.R[m], shift.shift_t,
                                   shift.shift_n, proc.CPSR.C );

    uint32_t result = proc.R[n] | shifted.value;

    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = shifted.carry;
        }
    }
}


template< typename proc_type >
void arm::ORR_reg_shift_reg_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t    S        = Bits( instr, 20, 20 );
    const uint32_t    d        = Bits( instr, 15, 12 );
    const uint32_t    m        = Bits( instr,  3,  0 );
    const uint32_t    n        = Bits( instr, 19, 16 );
    const uint32_t    s        = Bits( instr, 11,  8 );
    const uint32_t    type     = Bits( instr,  6,  5 );

    const bool        setflags = ( S == 1 );
    const SRType      shift_t  = DecodeRegShift( type );

    if( ( d == 15 ) || ( m == 15 ) || ( n == 15 ) || ( s == 15 ) )
         UNPREDICTABLE;

    // Instruction code
    uint32_t shift_n = Bits( proc.R[s], 7, 0 );
    UValueCarry shifted = Shift_C( proc.R[m], shift_t,
                                   shift_n, proc.CPSR.C );

    uint32_t result = proc.R[n] | shifted.value;

    proc.R[d] = result;
    if( setflags )
    {
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = result;
    }
}


template< typename proc_type >
void arm::PKH_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    const uint32_t    d        = Bits( instr, 15, 12 );
    const uint32_t    m        = Bits( instr,  3,  0 );
    const uint32_t    n        = Bits( instr, 19, 16 );
    const uint32_t    imm5     = Bits( instr, 11,  7 );
    const uint32_t    tb       = Bits( instr,  6,  6 );
    const bool        tbform   = ( tb == 1 );

    ShiftUValue shift = DecodeImmShift( tb << 1 /*append 0*/, imm5 );
    
    if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

    // Instruction code
    uint32_t operand2 = Shift( proc.R[m], shift.shift_t,
                                shift.shift_n, proc.CPSR.C );

    proc.R[d] = ( tbform ? operand2 : (uint32_t)proc.R[n] )  & 0x0000FFFF;
    proc.R[d] |= ( tbform ? (uint32_t)proc.R[n] : operand2 ) & 0xFFFF0000;
}


template< typename proc_type >
void arm::PLD_imm_A1( proc_type& proc, uint32_t instr ) { return; }


template< typename proc_type >
void arm::PLD_lit_A1( proc_type& proc, uint32_t instr ) { return; }


template< typename proc_type >
void arm::PLD_reg_A1( proc_type& proc, uint32_t instr ) { return; }


template< typename proc_type >
void arm::PLI_imm_lit_A1( proc_type& proc, uint32_t instr ) { return; }


template< typename proc_type >
void arm::PLI_reg_A1( proc_type& proc, uint32_t instr ) { return; }


template< typename proc_type >
void arm::POP_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t register_list = Bits( instr, 15, 0 );

    if( BitCount( register_list ) < 2 )
    {
        // FIXME: SEE LDM/LDMA/LDMFD      
    }

    if( ( Bits( register_list, 13, 13 ) == 1 ) && ArchVersion() >= 7 )
         UNPREDICTABLE;

    // Instruction code
    if( !NullCheckIfThumbEE( proc, 13 ) )
    {
        return; // See NullCheckIfThumbEE's documentation
    }
    
    // FIXME : SP is not always register 13 (depending on the execution mode)
    uint32_t address = proc.R[13];
    
    for( int i = 0; i <= 14; ++i )
    {
        // Pop selected registers
        if( Bits( register_list, i, i ) == 1 )
        {
            proc.R[i] = proc.dMem.read_word( address );
            address += 4;
        }
    }
    
    // PC is handled separatly
    if( Bits( register_list, 15, 15 ) == 1 )
    {
        LoadWritePC( proc, address );
    }

    if( Bits( register_list , 13, 13 ) == 0 )
    {
        // FIXME : SP is not always register 13 (depending on the execution mode)
        proc.R[13] += 4*BitCount( register_list );
    }
    else
    {
        // SP is assigned an UNKWNOWN value
        // FIXME : SP is not always register 13 (depending on the execution mode)
        proc.R[13] = 0xDEADC0DE;
    }
}


template< typename proc_type >
void arm::POP_A2( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t t = Bits( instr, 15, 12 );
    uint32_t register_list = 0;
    register_list |= 1 << t;

    if( t == 13 ) UNPREDICTABLE;

    // Instruction code
    // FIXME : SP is not always register 13 (depending on the execution mode)
    uint32_t address = proc.R[13];
    
    for( int i = 0; i <= 14; ++i )
    {
        // Pop selected registers
        if( Bits( register_list, i, i ) == 1 )
        {
            proc.R[i] = proc.dMem.read_word( address );
            address += 4;
        }
    }
    
    // PC is handled separatly
    if( Bits( register_list, 15, 15 ) == 1 )
    {
        LoadWritePC( proc, address );
    }

    /* If registers<13> = 1, SP is unknown... thus it is set to the same value
     * as the "known" case.
     */
    if( Bits( register_list , 13, 13 ) == 0 )
    {
        // FIXME : SP is not always register 13 (depending on the execution mode)
        proc.R[13] += 4*BitCount( register_list );
    }
    else
    {
        // SP is assigned an UNKWNOWN value
        // FIXME : SP is not always register 13 (depending on the execution mode)
        proc.R[13] = 0xDEADC0DE;
    }
}


template< typename proc_type >
void arm::PUSH_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t register_list = Bits( instr, 15, 0 );

    if( BitCount( register_list ) < 2 )
    {
        // FIXME: SEE STMDB / STMFD
    }

    // Instruction code
    if( !NullCheckIfThumbEE( proc, 13 ) )
    {
        return;
    }

    // FIXME : SP is not always register 13 (depending on the execution mode)
    uint32_t address = proc.R[13] - 4 * BitCount( register_list );
    
    for( int i = 0; i <= 14; ++i )
    {
        if( Bits( register_list, i , i ) == 1 )
        {
            if( i == 13 && i != LowestSetBit( register_list ) )
            {
                // Write an UNKNOWN value
                proc.dMem.write_word( address, 0xC0DEBEEF );
            }
            else
            {
                proc.dMem.write_word( address, proc.R[i] );            
            }
            address += 4;        
        }
    }

    if( Bits( register_list, 15, 15 ) == 1 )
    {
        proc.dMem.write_word( address, PCStoreValue( proc ) );
    }

    // FIXME : SP is not always register 13 (depending on the execution mode)
    proc.R[13] = proc.R[13] - 4*BitCount( register_list );
}


template< typename proc_type >
void arm::PUSH_A2( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t t = Bits( instr, 15, 12 );    
    uint32_t register_list = 0;
    register_list |= 1 << t;

    if( t == 13 ) UNPREDICTABLE;

    // Instruction code
    if( !NullCheckIfThumbEE( proc, 13 ) )
    {
        return;
    }

    // FIXME : SP is not always register 13 (depending on the execution mode)
    uint32_t address = proc.R[13] - 4 * BitCount( register_list );
    
    for( int i = 0; i <= 14; ++i )
    {
        if( Bits( register_list, i , i ) == 1 )
        {
            if( i == 13 && i != LowestSetBit( register_list ) )
            {
                // Write UNKNOWN value
                proc.dMem.write_word( address, 0xC0DEBEEF );
            }
            else
            {
                proc.dMem.write_word( address, proc.R[i] );            
            }
            address += 4;        
        }
    }

    if( Bits( register_list, 15, 15 ) == 1 )
    {
        proc.dMem.write_word( address, PCStoreValue( proc ) );
    }

    // FIXME : SP is not always register 13 (depending on the execution mode)
    proc.R[13] = proc.R[13] - 4*BitCount( register_list );
}


template< typename proc_type >
void arm::QADD_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );
        
        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;
        
        // Instruction code
        ValueSat res = SignedSatQ( (int64_t)SInt( proc.R[m] ) +
                                   (int64_t)SInt( proc.R[n] ), 32 );
        proc.R[d] = res.value;
        
        if ( res.saturated )
        {
            proc.CPSR.Q = 1;
        }   
    }
}


template< typename proc_type >
void arm::QADD16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );

        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

        int32_t sum1
            = (int32_t)SInt( (uint16_t)Bits( proc.R[n], 15,  0 ) )
            + (int32_t)SInt( (uint16_t)Bits( proc.R[m], 15,  0 ) );
        int32_t sum2
            = (int32_t)SInt( (uint16_t)Bits( proc.R[n], 31, 16 ) )
            + (int32_t)SInt( (uint16_t)Bits( proc.R[m], 31, 16 ) );
    
        // Instruction code
        ValueSat res1 = SignedSatQ( sum1, 16 );
        ValueSat res2 = SignedSatQ( sum2, 16 );
        proc.R[d]  = (uint16_t)(res1.value);
        proc.R[d] |= (uint16_t)(res2.value) << 16;
    }
}


template< typename proc_type >
void arm::QADD8_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );

        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

        int16_t sum1
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n],  7,  0 ) )
            + (int16_t)SInt( (uint8_t)Bits( proc.R[m],  7,  0 ) );

        int16_t sum2
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 15,  8 ) )
            + (int16_t)SInt( (uint8_t)Bits( proc.R[m], 15,  8 ) );

        int16_t sum3
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 23, 16 ) )
            + (int16_t)SInt( (uint8_t)Bits( proc.R[m], 23, 16 ) );

        int16_t sum4
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 31, 24 ) )
            + (int16_t)SInt( (uint8_t)Bits( proc.R[m], 31, 24 ) );
    
        // Instruction code
        ValueSat res1 = SignedSatQ( sum1, 8 );
        ValueSat res2 = SignedSatQ( sum2, 8 );
        ValueSat res3 = SignedSatQ( sum3, 8 );
        ValueSat res4 = SignedSatQ( sum4, 8 );

        proc.R[d]  =  (uint8_t)(res1.value);
        proc.R[d] |=  (uint8_t)(res2.value) << 8;
        proc.R[d] |=  (uint8_t)(res3.value) << 16;
        proc.R[d] |=  (uint8_t)(res4.value) << 24;
    }
}


template< typename proc_type >
void arm::QASX_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );
    const uint32_t n = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    // Instruction code
    int32_t diff = SInt( (uint16_t)Bits( proc.R[n], 15,  0 ) )
        - SInt( (uint16_t)Bits( proc.R[m], 31, 16 ) );

    int32_t sum  = SInt( (uint16_t)Bits( proc.R[n], 31, 16 ) )
        + SInt( (uint16_t)Bits( proc.R[m], 15,  0 ) );

    ValueSat low  = SignedSatQ( diff, 16 );
    ValueSat high = SignedSatQ( sum,  16 );
    proc.R[d] =  (uint16_t)(low.value);
    proc.R[d] |= (uint16_t)(high.value) << 16;
}


template< typename proc_type >
void arm::QDADD_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );
    const uint32_t n = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    // Instruction code
    int64_t doubledInt = (int64_t)( (int32_t)proc.R[n] ) * 2;
    ValueSat doubled = SignedSatQ( doubledInt, 32 );
    ValueSat res     = SignedSatQ( SInt( (int32_t)proc.R[m] ) + doubled.value, 32 );

    proc.R[d] = res.value;

    if( doubled.saturated || res.saturated )
    {
        proc.CPSR.Q = 1;
    }   
}


template< typename proc_type >
void arm::QDSUB_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );
    const uint32_t n = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    // Instruction code
    int64_t doubledInt = (int64_t)( (int32_t)proc.R[n] ) * 2;
    ValueSat doubled = SignedSatQ( doubledInt, 32 );
    ValueSat res     = SignedSatQ( SInt( (int32_t)proc.R[m] ) - doubled.value, 32 );

    proc.R[d] = res.value;

    if( doubled.saturated || res.saturated )
    {
        proc.CPSR.Q = 1;
    }   
}


template< typename proc_type >
void arm::QSAX_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );
    const uint32_t n = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    // Instruction code
    int32_t sum  = SInt( (uint16_t)Bits( proc.R[n], 15, 0 ) )
        + SInt( (uint16_t)Bits( proc.R[m], 31, 16 ) );
    int32_t diff =  SInt( (uint16_t)Bits( proc.R[n], 31, 16 ) )
        - SInt( (uint16_t)Bits( proc.R[m], 15,  0 ) );
                
    proc.R[d] =  (uint16_t)(SignedSat( sum, 16 ));
    proc.R[d] |= (uint16_t)(SignedSat( diff, 16 )) << 16;
}


template< typename proc_type >
void arm::QSUB_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );

        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;
    
        // Instruction code
        ValueSat res = SignedSatQ( (int64_t)SInt( proc.R[m] ) -
                                   (int64_t)SInt( proc.R[n] ), 32  );

        proc.R[d] = res.value;

        if ( res.saturated )
        {
            proc.CPSR.Q = 1;
        }
    }
}


template< typename proc_type >
void arm::QSUB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );

        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;
    
        // Instruction code
        int32_t diff1
            = (int32_t)SInt( (uint16_t)Bits( proc.R[n], 15,  0 ) )
            - (int32_t)SInt( (uint16_t)Bits( proc.R[m], 15,  0 ) );

        int32_t diff2
            = (int32_t)SInt( (uint16_t)Bits( proc.R[n], 31, 16 ) )
            - (int32_t)SInt( (uint16_t)Bits( proc.R[m], 31, 16 ) );

        proc.R[d]  = (uint16_t)(SignedSat( diff1, 16 ));
        proc.R[d] |= (uint16_t)(SignedSat( diff2, 16 )) << 16;
    }
}


template< typename proc_type >
void arm::QSUB8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        const uint32_t d = Bits( instr, 15, 12 );
        const uint32_t m = Bits( instr,  3,  0 );
        const uint32_t n = Bits( instr, 19, 16 );

        if ( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;
    
        // Instruction code
        int16_t diff1
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n],  7,  0 ) )
            - (int16_t)SInt( (uint8_t)Bits( proc.R[m],  7,  0 ) );

        int16_t diff2
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 15,  8 ) )
            - (int16_t)SInt( (uint8_t)Bits( proc.R[m], 15,  8 ) );

        int16_t diff3
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 23, 16 ) )
            - (int16_t)SInt( (uint8_t)Bits( proc.R[m], 23, 16 ) );

        int16_t diff4
            = (int16_t)SInt( (uint8_t)Bits( proc.R[n], 31, 24 ) )
            - (int16_t)SInt( (uint8_t)Bits( proc.R[m], 31, 24 ) );

        proc.R[d]  = (uint8_t)(SignedSat( diff1,  8 ));
        proc.R[d] |= (uint8_t)(SignedSat( diff2,  8 )) << 8;
        proc.R[d] |= (uint8_t)(SignedSat( diff3,  8 )) << 16;
        proc.R[d] |= (uint8_t)(SignedSat( diff4,  8 )) << 24;
    }
}


template< typename proc_type >
void arm::RBIT_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );

    if( d == 15 || m == 15 ) UNPREDICTABLE;

    // Instruction code
    uint32_t result = 0;
    for( int i = 0; i < 32; ++i )
    {
        result |= Bits( proc.R[m], i, i ) << ( 31 - i );
    }
    proc.R[d] = result;
}


template< typename proc_type >
void arm::REV_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );

    if( d == 15 || m == 15 ) UNPREDICTABLE;

    // Instruction code
    proc.R[d]  = Bits( proc.R[m],  7,  0 ) << 24;
    proc.R[d] |= Bits( proc.R[m], 15,  8 ) << 16;
    proc.R[d] |= Bits( proc.R[m], 23, 16 ) << 8;
    proc.R[d] |= Bits( proc.R[m], 31, 24 );
}


template< typename proc_type >
void arm::REV16_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );

    if( d == 15 || m == 15 ) UNPREDICTABLE;
 
    // Instruction code
    proc.R[d]  = Bits( proc.R[m], 15,  8 );
    proc.R[d] |= Bits( proc.R[m],  7,  0 ) << 8;
    proc.R[d] |= Bits( proc.R[m], 31, 24 ) << 16;
    proc.R[d] |= Bits( proc.R[m], 23, 16 ) << 24;
}


template< typename proc_type >
void arm::REVSH_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d = Bits( instr, 15, 12 );
    const uint32_t m = Bits( instr,  3,  0 );

    if( d == 15 || m == 15 ) UNPREDICTABLE;
    
    // Instruction code
    int32_t result = SignExtend( Bits( proc.R[m], 7, 0 ), 24, 8 );
    result = result << 8;
    result |= Bits( proc.R[m], 15, 8 );
    proc.R[d] = result;
}


template< typename proc_type >
void arm::RFE_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const bool wback = Bits( instr, 21, 21 ) == 1;
    const bool inc   = Bits( instr, 23, 23 ) == 1;
    const bool P     = Bits( instr, 24, 24 ) == 1;
    const bool wordhigher = P == inc;
    const uint32_t n = Bits( instr, 19, 16 );

    if( n == 15 )
    {
        // Unpredictable
        return;
    }
    
    // Instruction code
    if( !CurrentModeIsPrivileged( proc ) || 
        CurrentInstrSet( proc ) == InstrSet_ThumbEE )
    {
        UNPREDICTABLE;
    }
    else
    {
        uint32_t address = inc ? proc.R[n] : proc.R[n] - 8;
        address += wordhigher ? 4 : 0;

        CPSRWriteByInstr( proc.dMem.read_word( address+4 ), 0xF, true, proc );
        
        BranchWritePC( proc, proc.dMem.read_word( address ) );
        
        if( wback )
        {
            proc.R[n] += inc ? 8 : -8;
        }
    }
}


template< typename proc_type >
void arm::ROR_IMM_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t imm5 = Bits( instr, 11,  7 );
    const bool setflags = Bits( instr, 20, 20 ) == 1;
    const uint32_t d    = Bits( instr, 15, 12 );
    const uint32_t m    = Bits( instr,  3,  0 );

    uint32_t shift_n = DecodeImmShift( 3, imm5 ).shift_n;
    SRType   shift_t = SRType_ROR;

    if( imm5 == 0 )
    {
        // RRX case
        shift_n = 1;
        shift_t = SRType_RRX;
    }

    // Instruction code
    UValueCarry result = Shift_C( proc.R[m], shift_t, shift_n, proc.CPSR.C );
    if( d == 15 )
    {
        ALUWritePC( proc, result.value );
    }
    else
    {
        proc.R[d] = result.value;
        if( setflags )
        {
            proc.CPSR.N = Bits( result.value, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result.value );
            proc.CPSR.C = result.carry;
        }
    }
}


template< typename proc_type >
void arm::ROR_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d    = Bits( instr, 15, 12 );
    const uint32_t m    = Bits( instr, 11,  8 );
    const uint32_t n    = Bits( instr,  3,  0 );
    const bool setflags = Bits( instr, 20, 20 ) == 1;

    if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

    // Instruction code
    uint32_t shift_n = Bits( proc.R[m],  7,  0 );
    UValueCarry result = Shift_C( proc.R[m], SRType_ROR, shift_n, proc.CPSR.C );

    proc.R[d] = result.value;
    if( setflags )
    {
        proc.CPSR.N = Bits( result.value, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result.value );
        proc.CPSR.C = result.carry;
    }    
}


template< typename proc_type >
void arm::RRX_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d    = Bits( instr, 15, 12 );
    const uint32_t m    = Bits( instr,  3,  0 );
    const bool setflags = Bits( instr, 20, 20 ) == 1;

    // Instruction code
    UValueCarry result = Shift_C( proc.R[m], SRType_RRX, 1, proc.CPSR.C );
    if( d == 15 )
    {
        ALUWritePC( proc, result.value );
    }
    else
    {
        proc.R[d] = result.value;
        if( setflags )
        {
            proc.CPSR.N = Bits( result.value, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result.value );
            proc.CPSR.C = result.carry;
        }
    }
    
}


template< typename proc_type >
void arm::RSB_IMM_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t imm12 = Bits( instr, 11,  0 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;
    const uint32_t imm32 = ARMExpandImm( proc, imm12 );

    if( d == 0xF && setflags )
    {
        // FIXME
        // SEE SUBS PC, LR
    }

    // Instruction code
    uint8_t carry;
    uint8_t overflow;
    uint32_t result = AddWithCarry( NOT(proc.R[n]), imm32, 1, carry, overflow );
    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}


template< typename proc_type >
void arm::RSB_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t imm5  = Bits( instr, 11,  7 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;
    const uint32_t type  = Bits( instr,  6,  5 );

    const ShiftUValue shift = DecodeImmShift( type, imm5 );

    if( d == 0xF && setflags )
    {
        // FIXME
        // SEE SUBS PC, LR
    }

    // Instruction code
    uint32_t shifted = Shift( proc.R[m], shift.shift_t,
                              shift.shift_n, proc.CPSR.C );

    uint8_t carry;
    uint8_t overflow;
    uint32_t result = AddWithCarry( NOT(proc.R[n]), shifted, 1, carry, overflow );
    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}


template< typename proc_type >
void arm::RSB_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t s     = Bits( instr, 11,  8 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;
    const uint32_t type  = Bits( instr,  6,  5 );

    const SRType shift_t = DecodeRegShift( type );

    if( d == 15 || m == 15 || n == 15 || s == 15 ) UNPREDICTABLE;

    // Instruction code
    const uint32_t shift_n = Bits( proc.R[s], 7, 0 );
    
    uint32_t shifted = Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );

    uint8_t carry;
    uint8_t overflow;
    uint32_t result = AddWithCarry( NOT(proc.R[n]), shifted, 1, carry, overflow );
    proc.R[d] = result;
    if( setflags )
    {
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}


template< typename proc_type >
void arm::RSC_IMM_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t imm12 = Bits( instr, 11,  0 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;
    const uint32_t imm32 = ARMExpandImm( proc, imm12 );

    if( d == 0xF && setflags )
    {
        // FIXME
        // SEE SUBS PC, LR
    }

    // Instruction code
    uint32_t result, carry, overflow;
    result = AddWithCarry( NOT(proc.R[n]), imm32, 1, carry, overflow );

    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}


template< typename proc_type >
void arm::RSC_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t imm5  = Bits( instr, 11,  7 );
    const uint32_t type  = Bits( instr,  6,  5 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;
    const ShiftUValue shift = DecodeImmShift( type, imm5 );

    if( d == 0xF && setflags )
    {
        // FIXME
        // SEE SUBS PC, LR
    }

    // Instruction code
    uint32_t shifted = Shift( proc.R[m]. shift.shift_t, 
                              shift.shift_n, proc.CPSR.C );

    uint32_t result, carry, overflow;
    result = AddWithCarry( NOT(proc.R[n]), shifted, proc.CPSR.C,
                           carry, overflow );

    if( d == 15 )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }
    }
}


template< typename proc_type >
void arm::RSC_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );
    const uint32_t s     = Bits( instr, 11,  8 );
    const uint32_t type  = Bits( instr,  6,  5 );
    const bool setflags  = Bits( instr, 20, 20 ) == 1;

    const SRType shift_t = DecodeRegShift( type );

    if( d == 15 || m == 15 || n == 15 || s == 15 ) UNPREDICTABLE;

    // Instruction code
    const uint32_t shift_n = Bits( proc.R[s], 7, 0 );
    uint32_t shifted = Shift( proc.R[m]. shift_t, 
                              shift_n, proc.CPSR.C );

    uint32_t result, carry, overflow;
    result = AddWithCarry( NOT(proc.R[n]), shifted, proc.CPSR.C,
                           carry, overflow );

    proc.R[d] = result;
    if( setflags )
    {
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        proc.CPSR.V = overflow;
    }
}


template< typename proc_type >
void arm::SADD16_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    int32_t sum1 = (int32_t)(SignExtend( Bits( proc.R[n], 15, 0 ), 32, 16 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 15, 0 ), 32, 16 ));

    int32_t sum2 = (int32_t)(SignExtend( Bits( proc.R[n], 31, 16 ), 32, 16 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 31, 16 ), 32, 16 ));

    // Pack results in the destination register
    proc.R[d]  = (int16_t)(Bits( sum1, 15, 0 ));
    proc.R[d] |= ((int16_t)(Bits( sum2, 15, 0 ))) << 16;

    // Set GE <1:0>
    proc.CPSR.GE  = sum1 >= 0 ? (0x3) : 0;
    // Set GE <3:2>
    proc.CPSR.GE |= ( sum2 >= 0 ? (0x3) : 0 ) << 2;
}


template< typename proc_type >
void arm::SADD8_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    int32_t sum1 = (int32_t)(SignExtend( Bits( proc.R[n], 7, 0 ), 32, 8 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 7, 0 ), 32, 8 ));

    int32_t sum2 = (int32_t)(SignExtend( Bits( proc.R[n], 15, 8 ), 32, 8 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 15, 8 ), 32, 8 ));

    int32_t sum3 = (int32_t)(SignExtend( Bits( proc.R[n], 23, 16 ), 32, 8 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 23, 16 ), 32, 8 ));

    int32_t sum4 = (int32_t)(SignExtend( Bits( proc.R[n], 31, 24 ), 32, 8 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 31, 24 ), 32, 8 ));

    // Pack results in the destination register
    proc.R[d]  =  (int8_t)(Bits( sum1, 7, 0 ));
    proc.R[d] |= ((int8_t)(Bits( sum2, 7, 0 ))) << 8;
    proc.R[d] |= ((int8_t)(Bits( sum3, 7, 0 ))) << 16;
    proc.R[d] |= ((int8_t)(Bits( sum4, 7, 0 ))) << 24;

    // Set GE values
    proc.CPSR.GE  = sum1 >= 0 ? 1 : 0;
    proc.CPSR.GE |= ( sum2 >= 0 ? 1 : 0 ) << 1;
    proc.CPSR.GE |= ( sum3 >= 0 ? 1 : 0 ) << 2;
    proc.CPSR.GE |= ( sum4 >= 0 ? 1 : 0 ) << 3;
}


template< typename proc_type >
void arm::SASX_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d     = Bits( instr, 15, 12 );
    const uint32_t m     = Bits( instr,  3,  0 );
    const uint32_t n     = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;

    int32_t diff = (int32_t)(SignExtend( Bits( proc.R[n], 15, 0 ), 32, 16 ))
        - (int32_t)(SignExtend( Bits( proc.R[m], 31, 16 ), 32, 16 ));

    int32_t sum = (int32_t)(SignExtend( Bits( proc.R[n], 31, 16 ), 32, 16 ))
        + (int32_t)(SignExtend( Bits( proc.R[m], 15, 0 ), 32, 16 ));

    // Pack results in the destination register
    proc.R[d]  =  (int16_t)(Bits( diff, 15, 0 ));
    proc.R[d] |= ((int16_t)(Bits( sum, 15, 0 ))) << 16;

    // Set GE values
    proc.CPSR.GE  = diff >= 0 ? (0x3) : 0;
    proc.CPSR.GE |= ( sum >= 0 ? (0x3) : 0 ) << 2;
}


template< typename proc_type >
void arm::SBC_IMM_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d        = Bits( instr, 15, 12 );
    const uint32_t n        = Bits( instr, 19, 16 );
    const uint32_t imm12    = Bits( instr, 11,  0 );
    const bool     setflags = Bits( instr, 20, 20 ) == 1;
    const uint32_t imm32    = ARMExpandImm( proc, imm12 );

    if( d == 0xF )
    {
        // FIXME : SEE SUBS PC, LR
    }

    uint32_t result, carry, overflow;
    result = AddWithCarry( (uint32_t)proc.R[n], NOT( imm32 ), proc.CPSR.C,
                           carry, overflow );

    if( d == 0xF )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;            
        }
    }
}


template< typename proc_type >
void arm::SBC_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d        = Bits( instr, 15, 12 );
    const uint32_t m        = Bits( instr,  3,  0 );
    const uint32_t n        = Bits( instr, 19, 16 );
    const uint32_t imm5     = Bits( instr, 11,  7 );
    const bool     setflags = Bits( instr, 20, 20 ) == 1;
    const uint32_t type     = Bits( instr,  6,  5 );

    ShiftUValue shift = DecodeImmShift( type, imm5 );

    if( d == 0xF )
    {
        // FIXME : SEE SUBS PC, LR
    }

    uint32_t shifted = Shift( proc.R[m], shift.shift_n, shift.shift_t,
                              proc.CPSR.C );

    uint32_t result, carry, overflow;
    result = AddWithCarry( (uint32_t)proc.R[n], NOT( shifted ), proc.CPSR.C,
                           carry, overflow );

    if( d == 0xF )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;            
        }
    }
}


template< typename proc_type >
void arm::SBC_REG_SHIFT_REG_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d        = Bits( instr, 15, 12 );
    const uint32_t m        = Bits( instr,  3,  0 );
    const uint32_t n        = Bits( instr, 19, 16 );
    const uint32_t s        = Bits( instr, 11,  8 );
    const bool     setflags = Bits( instr, 20, 20 ) == 1;
    const uint32_t type     = Bits( instr,  6,  5 );

    ShiftUValue shift;
    shift.shift_t = DecodeRegShift( type );
    shift.shift_n = Bits( proc.R[s], 7, 0 );

    if( d == 0xF )
    {
        // FIXME : SEE SUBS PC, LR
    }

    uint32_t shifted = Shift( proc.R[m], shift.shift_n, shift.shift_t,
                              proc.CPSR.C );

    uint32_t result, carry, overflow;
    result = AddWithCarry( (uint32_t)proc.R[n], NOT( shifted ), proc.CPSR.C,
                           carry, overflow );

    if( d == 0xF )
    {
        ALUWritePC( proc, result );
    }
    else
    {
        proc.R[d] = result;
        
        if( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;            
        }
    }
}


template< typename proc_type >
void arm::SBFX_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d            = Bits( instr, 15, 12 );
    const uint32_t n            = Bits( instr, 19, 16 );
    const uint32_t lsbit        = Bits( instr, 11,  7 );
    const uint32_t widthminus1  = Bits( instr, 20, 16 );

    if( d == 15 || n == 15 )
    {
         UNPREDICTABLE;
    }
    
    uint32_t msbit = lsbit + widthminus1;

    if( msbit <= 31 )
    {
        proc.R[d] = SignExtend( Bits( proc.R[n], msbit, lsbit ), 32, 
                                ( msbit-lsbit + 1 )/*old size*/ );
    }
    else
    {
         UNPREDICTABLE;
    }
}


template< typename proc_type >
void arm::SEL_A1( proc_type& proc, uint32_t instr )
{
    if( !ConditionPassed( proc, instr ) )
    {
        return;
    }

    // Encoding-specific operations
    const uint32_t d            = Bits( instr, 15, 12 );
    const uint32_t m            = Bits( instr,  3,  0 );
    const uint32_t n            = Bits( instr, 19, 16 );

    if( d == 15 || m == 15 || n == 15 ) UNPREDICTABLE;
    
    proc.R[d] =  ( proc.CPSR.GE & 0x1 ) == 1 ? Bits( proc.R[n], 7, 0 ) :
                                               Bits( proc.R[m], 7, 0 );

    proc.R[d] |= (( proc.CPSR.GE & 0x2 ) == 1 ? Bits( proc.R[n], 15, 8 ) :
                                                Bits( proc.R[m], 15, 8 )) << 8;

    proc.R[d] |= (( proc.CPSR.GE & 0x4 ) == 1 ? Bits( proc.R[n], 23, 16 ) :
                                                Bits( proc.R[m], 23, 16 )) << 16;

    proc.R[d] |= (( proc.CPSR.GE & 0x8 ) == 1 ? Bits( proc.R[n], 31, 24 ) :
                                                Bits( proc.R[m], 31, 24 )) << 24;
}


template< typename proc_type >
void arm::SETEND_A1( proc_type& proc, uint32_t instr )
{
    const uint32_t set_bigend = Bits( instr, 9, 9 );
    proc.CPSR.E = set_bigend;
}


template< typename proc_type >
void arm::SHADD16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t sum1
            = int16_t( Bits( proc.R[n], 15, 0 ) )
            + int16_t( Bits( proc.R[m], 15, 0 ) );
        int32_t sum2
            = int16_t( Bits( proc.R[n], 31, 16 ) )
            + int16_t( Bits( proc.R[m], 31, 16 ) );


        proc.R[d]  = 0x00000000;
        proc.R[d] |= Bits( sum1, 16, 1 );
        proc.R[d] |= Bits( sum2, 16, 1 ) << 16;
    }
}


template< typename proc_type >
void arm::SHADD8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t sum1
            = int8_t( Bits( proc.R[n], 7, 0 ) )
            + int8_t( Bits( proc.R[m], 7, 0 ) );
        int16_t sum2
            = int8_t( Bits( proc.R[n], 15, 8 ) )
            + int8_t( Bits( proc.R[m], 15, 8 ) );
        int16_t sum3
            = int8_t( Bits( proc.R[n], 23, 16 ) )
            + int8_t( Bits( proc.R[m], 23, 16 ) );
        int16_t sum4
            = int8_t( Bits( proc.R[n], 31, 24 ) )
            + int8_t( Bits( proc.R[m], 31, 24 ) );

        proc.R[d] = 0x0;
        proc.R[d] |= ( Bits( sum1, 8, 1 ) );
        proc.R[d] |= ( Bits( sum2, 8, 1 ) << 8  );
        proc.R[d] |= ( Bits( sum3, 8, 1 ) << 16 );
        proc.R[d] |= ( Bits( sum4, 8, 1 ) << 24 );
    }
}


template< typename proc_type >
void arm::SHASX_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int32_t diff = int16_t( Bits( proc.R[n], 15,  0 ) )
                     - int16_t( Bits( proc.R[m], 31, 16 ) );
                     
        int32_t sum = int16_t( Bits( proc.R[n], 31, 16 ) ) 
                    + int16_t( Bits( proc.R[m], 15,  0 ) );
        
        proc.R[d] = 0x0;
        proc.R[d] |= Bits( diff, 16, 1 );
        proc.R[d] |= Bits( sum, 16, 1 ) << 16;
    }
}


template< typename proc_type >
void arm::SHSAX_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t sum  = int16_t( Bits( proc.R[n], 15, 0  ) ) 
                     + int16_t( Bits( proc.R[m], 31, 16 ) );
                     
        int32_t diff = int16_t( Bits( proc.R[n], 31, 16 ) )
                     - int16_t( Bits( proc.R[m], 15, 0  ) );
        
        proc.R[d] = 0x0;
        proc.R[d] |= Bits( sum,  16, 1 );
        proc.R[d] |= Bits( diff, 16, 1 ) << 16;
    }
}


template< typename proc_type >
void arm::SHSUB16_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int32_t diff1 = int16_t( Bits( proc.R[n], 15, 0 ) )
                      - int16_t( Bits( proc.R[m], 15, 0 ) );
                      
        int32_t diff2 = int16_t( Bits( proc.R[n], 31, 16 ) )
                      - int16_t( Bits( proc.R[m], 31, 16 ) );
    
        proc.R[d] = 0x0;
        proc.R[d] |= Bits( diff1, 16, 1 );
        proc.R[d] |= Bits( diff2, 16, 1 ) << 16;
    }
}


template< typename proc_type >
void arm::SHSUB8_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t diff1 = int8_t( Bits( proc.R[n], 7 , 0 ) ) 
                     - int8_t( Bits( proc.R[m], 7 , 0 ) );
                     
        int16_t diff2 = int8_t( Bits( proc.R[n], 15, 8 ) )
                     - int8_t( Bits( proc.R[m], 15, 8 ) );
                     
        int16_t diff3 = int8_t( Bits( proc.R[n], 23, 16 ) ) 
                     - int8_t( Bits( proc.R[m], 23, 16 ) );
                     
        int16_t diff4 = int8_t( Bits( proc.R[n], 31, 24 ) )
                     - int8_t( Bits( proc.R[m], 31, 24 ) );
        
        proc.R[d] = 0x0;
        proc.R[d] |= Bits( diff1, 8 , 1 );
        proc.R[d] |= Bits( diff2, 8 , 1 ) << 8;
        proc.R[d] |= Bits( diff3, 8 , 1 ) << 16;
        proc.R[d] |= Bits( diff4, 8 , 1 ) << 24;
    }
}

template< typename proc_type >
void arm::SMLAxy_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  6,  6 );
        uint32_t N = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool n_high = ( N == 1 );
        bool m_high = ( M == 1 );
        
        if( d == 15 || n == 15 || m == 15 || a == 15 ) UNPREDICTABLE;
        
        // Operation
        int16_t operand1, operand2;

        if( n_high )
            operand1 = Bits( proc.R[n], 31, 16 );
        else
            operand1 = Bits( proc.R[n], 15,  0 );

        if( m_high )
            operand2 = Bits( proc.R[m], 31, 16 );
        else
            operand2 = Bits( proc.R[m], 15,  0 );
        
        int64_t result = int64_t( operand1 ) * operand2
                       + int32_t( proc.R[a] );

        proc.R[d] = Bits64( result, 31, 0 );
        
        if( result != int32_t( Bits64( result, 31, 0 )) ) // Signed overflow
            proc.CPSR.Q = 1;
    }
}


template< typename proc_type >
void arm::SMLAD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool m_swap = (M == 1);
        
        if( a == 15 ) 
        {
            arm::SMUAD_A1( proc, instr );
            return;
        }
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );
            
        int64_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) ) ) 
                         * int16_t( Bits( operand2,  15, 0 ) );
                            
        int64_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int16_t( Bits( operand2,  31, 16 ) );
        
        int64_t result = product1 + product2 + int32_t(proc.R[a]);
        
        proc.R[d] = Bits64( result, 31, 0 );
        
        if( result != int32_t( Bits64( result, 31, 0 ) ) ) // Signed overflow
            proc.CPSR.Q = 1;
    }
}


template< typename proc_type >
void arm::SMLAL_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S   = Bits( instr, 20, 20 );
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t n   = Bits( instr,  3,  0 );

        bool setflags = ( S == 0x1 );

        if ( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if ( dHi == dLo ) UNPREDICTABLE;
        if ( ArchVersion() < 6 && ( dHi == n || dLo == n ) ) UNPREDICTABLE;
        
        // Operation
        int64_t result = int64_t( int32_t( proc.R[n] ) ) 
                       * int64_t( int32_t( proc.R[m] ) )
                       + int64_t( ( int64_t( proc.R[dHi] ) << 32 )
                         | proc.R[dLo] );
        
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );
        
        if( setflags )
        {
            proc.CPSR.N = Bits64( result, 63, 63 );
            proc.CPSR.Z = IsZeroBit( result );
        }
    }
}


template< typename proc_type >
void arm::SMLALxy_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t M   = Bits( instr,  6,  6 );
        uint32_t N   = Bits( instr,  5,  5 );
        uint32_t n   = Bits( instr,  3,  0 );
        
        bool n_high = ( N == 1 );
        bool m_high = ( M == 1 );
        
        if( dHi == 15 || dLo == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if( dHi == dLo ) UNPREDICTABLE;
       
        // Operation
        int16_t operand1;
        int16_t operand2;
        
        if( n_high )
            operand1 = Bits( proc.R[n], 31, 16 );
        else
            operand1 = Bits( proc.R[n], 15,  0 );

        if( m_high )
            operand2 = Bits( proc.R[m], 31, 16 );
        else
            operand2 = Bits( proc.R[m], 15,  0 );
        
        int64_t result = int64_t( operand1 ) 
                       * int64_t( operand2 ) 
                       + int64_t( ( int64_t( proc.R[dHi] ) << 32 )
                         | proc.R[dLo] );
        
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );
    }
}


template< typename proc_type >
void arm::SMLALD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t M   = Bits( instr,  5,  5 );
        uint32_t n   = Bits( instr,  3,  0 );
         
        bool m_swap = (M == 1);
        
        if( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if( dHi == dLo ) UNPREDICTABLE;
        
        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );
            
        int32_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) )  )
                         * int32_t( int16_t( Bits( operand2,  15, 0 ) ) );
                         
        int32_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int32_t( int16_t( Bits( operand2,  31, 16 ) ) );
        
        int64_t result = int64_t( product1 )
                       + int64_t( product2 ) 
                       + int64_t( ( int64_t( proc.R[dHi] ) << 32 )
                         | proc.R[dLo]);
        
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );
    }
}


template< typename proc_type >
void arm::SMLAWx_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  6,  6 );
        uint32_t n = Bits( instr,  3,  0 );

        bool m_high = ( M == 1 );
        
        if( d == 15 || n == 15 || m == 15 || a == 15 ) UNPREDICTABLE;
        
        // Operation
        int16_t operand2;
        
        if( m_high )
            operand2 = Bits( proc.R[m], 31, 16 );
        else
            operand2 = Bits( proc.R[m], 15,  0 );
        
        int64_t result = int64_t( int32_t(proc.R[n]) ) 
                       * int64_t( operand2 )
                       + ( int64_t( int32_t( proc.R[a] ) ) << 16 );
        
        proc.R[d] = Bits64( result, 47, 16 );
        
        if( (result >> 16) != proc.R[d] ) // Signed overflow
            proc.CPSR.Q = 1;
    }
}


template< typename proc_type >
void arm::SMLSD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool m_swap = (M == 1);

        if( a == 15 ) 
        {
            arm::SMUSD_A1( proc, instr );
            return;
        }
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );
        
        int32_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) ) )
                         * int16_t( Bits( operand2,  15, 0 ) );

        int32_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int16_t( Bits( operand2,   31, 16 ) );
        
        int64_t result = int64_t( product1 ) 
                       - int64_t( product2 )
                       + int64_t( int32_t( proc.R[a] ) );
        
        proc.R[d] = Bits64( result, 31, 0 );
        
        if( result != int32_t( Bits64( result, 31, 0 ) ) ) // Signed overflow
            proc.CPSR.Q = 1;
    }
}


template< typename proc_type >
void arm::SMLSLD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t M   = Bits( instr,  5,  5 );
        uint32_t n   = Bits( instr,  3,  0 );
         
        bool m_swap = (M == 1);
        
        if( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if( dHi == dLo ) UNPREDICTABLE;
        
        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );
        
        int32_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) ) )
                         * int32_t( int16_t( Bits( operand2,  15, 0 ) ) );

        int32_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int32_t( int16_t( Bits( operand2,  31, 16 ) ) );
        
        int64_t result = int64_t( product1 )
                       - int64_t( product2 )
                       + int64_t( ( int64_t( proc.R[dHi] ) << 32 )
                         | proc.R[dLo] );
        
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 ); 
    }
}


template< typename proc_type >
void arm::SMMLA_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t R = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool round = (R == 1);

        if( a == 15 ) 
        {
            arm::SMMUL_A1( proc, instr );
            return;
        }

        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int64_t result = ( int64_t( proc.R[a] ) << 32 ) 
                       +   int64_t( int32_t( proc.R[n] ) )
                       *   int64_t( int32_t( proc.R[m] ) );
        
        if( round )
            result = result + 0x80000000;
        
        proc.R[d] = Bits64( result, 63, 32 );
    }
}


template< typename proc_type >
void arm::SMMLS_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t R = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool round = (R == 1);
        
        if( d == 15 || n == 15 || m == 15 || a == 15 ) UNPREDICTABLE;
    
        // Operation
        int64_t result = ( int64_t( proc.R[a] ) << 32 )
                       -   int64_t( int32_t( proc.R[n] ) )
                       *   int64_t( int32_t( proc.R[m] ) );
        
        if( round )
            result = result + 0x80000000;
            
        proc.R[d] = Bits64( result, 63, 32 );
    }
}


template< typename proc_type >
void arm::SMMUL_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t R = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool round = (R == 1);
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int64_t result = int64_t( int32_t( proc.R[n] ) )
                       * int64_t( int32_t( proc.R[m] ) );
        
        if( round )
            result = result + 0x80000000;
            
        proc.R[d] = Bits64( result, 63, 32 );
    }
}


template< typename proc_type >
void arm::SMUAD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool m_swap = (M == 1);
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );
            
        int32_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) ) )
                         * int32_t( int16_t( Bits( operand2,  15, 0 ) ) );
                            
        int32_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int32_t( int16_t( Bits( operand2,  31, 16 ) ) );
        
        int64_t result = int64_t( product1 ) + int64_t( product2 );
        
        proc.R[d] = Bits64( result, 31, 0 );
        
        if( result != int32_t(Bits64( result, 31, 0 )) ) // Signed overflow
            proc.CPSR.Q = 1;
    
    }
}


template< typename proc_type >
void arm::SMULxy_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  6,  6 );
        uint32_t N = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );

        bool n_high = ( N == 1 );
        bool m_high = ( M == 1 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t operand1;
        int16_t operand2;
        
        if( n_high )
            operand1 = Bits( proc.R[n], 31, 16 );
        else
            operand1 = Bits( proc.R[n], 15,  0 );

        if( m_high )
            operand2 = Bits( proc.R[m], 31, 16 );
        else
            operand2 = Bits( proc.R[m], 15,  0 );
        
        int64_t result = int64_t( operand1 ) * int64_t( operand2 );
        
        proc.R[d] = Bits64( result, 31, 0 ); 
        // Signed overflow cannot occur
    }
}


template< typename proc_type >
void arm::SMULL_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S   = Bits( instr, 20, 20 );
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t n   = Bits( instr,  3,  0 );

        bool setflags = ( S == 0x1 );

        if ( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if ( dHi == dLo ) UNPREDICTABLE;
        if ( ArchVersion() < 6 && ( dHi == n || dLo == n ) ) UNPREDICTABLE;
        
        // Operation        
        int64_t result = int64_t( int32_t( proc.R[n] ) )
                       * int64_t( int32_t( proc.R[m] ) );
        
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );
        
        if( setflags )
        {
            proc.CPSR.N = Bits64( result, 63, 63 );
            proc.CPSR.Z = IsZeroBit( result );
        }
    }
}


template< typename proc_type >
void arm::SMULWx_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  6,  6 );
        uint32_t n = Bits( instr,  3,  0 );

        bool m_high = ( M == 1 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        
        // Operation
        int16_t operand2;
        
        if( m_high )
            operand2 = Bits( proc.R[m], 31, 16 );
        else
            operand2 = Bits( proc.R[m], 15,  0 );

        int64_t product = int64_t( int32_t( proc.R[n] ) )
                        * int64_t( operand2 );

        proc.R[d] = Bits64( product, 47, 16 );
        // Signed overflow cannot occur
    }
}


template< typename proc_type >
void arm::SMUSD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t M = Bits( instr,  5,  5 );
        uint32_t n = Bits( instr,  3,  0 );
        
        bool m_swap = (M == 1);
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t operand2 = ( m_swap ? ROR(proc.R[m],16) : uint32_t( proc.R[m] ) );

        int32_t product1 = int32_t( int16_t( Bits( proc.R[n], 15, 0 ) ) )
                         * int32_t( int16_t( Bits( operand2,  15, 0 ) ) );

        int32_t product2 = int32_t( int16_t( Bits( proc.R[n], 31, 16 ) ) )
                         * int32_t( int16_t( Bits( operand2,  31, 16 ) ) );

        int64_t result = int64_t( product1 ) - int64_t( product2 );

        proc.R[d] = Bits( result, 31, 0 );
        // Signed overflow cannot occur

    }
}    


template< typename proc_type >
void arm::SSAT_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t sat_imm = Bits( instr, 20, 16 );
        uint32_t d       = Bits( instr, 15, 12 );
        uint32_t imm5    = Bits( instr, 11,  7 );
        uint32_t sh      = Bits( instr,  6,  6 );
        uint32_t n       = Bits( instr,  3,  0 );
        
        uint32_t saturate_to = UInt(sat_imm)+1;

        uint32_t type = sh << 1;

        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;
        
        if( d == 15 || n == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t operand = Shift( proc.R[n], shift_t, shift_n, proc.CPSR.C);
                                                        // CPSR.C ignored

        ValueSat valueSat = SignedSatQ( int32_t( operand ), saturate_to );
        int64_t result    = valueSat.value;
        bool    sat       = valueSat.saturated;

        proc.R[d] = int32_t( result );

        if( sat )
            proc.CPSR.Q = 1;
    }
}  


template< typename proc_type >
void arm::SSAT16_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t sat_imm = Bits( instr, 19, 16 );
        uint32_t d       = Bits( instr, 15, 12 );
        uint32_t n       = Bits( instr,  3,  0 );
        
        uint32_t saturate_to = UInt(sat_imm)+1;

        if( d == 15 || n == 15 ) UNPREDICTABLE;

        // Operation
        ValueSat valueSat1 = 
            SignedSatQ( int16_t( Bits( proc.R[n], 15, 0 ) ), saturate_to );

        int64_t result1    = valueSat1.value;
        bool    sat1       = valueSat1.saturated;

        ValueSat valueSat2 = 
            SignedSatQ( int16_t( Bits( proc.R[n], 31, 16 ) ), saturate_to );

        int64_t result2    = valueSat2.value;
        bool    sat2       = valueSat2.saturated;

        proc.R[d]  = 0x00000000;
        proc.R[d] |= int16_t( result1 );
        proc.R[d] |= int16_t( result2 ) << 16;

        if( sat1 || sat2 )
            proc.CPSR.Q = 1;
    }
}


template< typename proc_type >
void arm::SSAX_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t sum = int16_t( Bits( proc.R[n], 15,  0 ) ) 
                    + int16_t( Bits( proc.R[m], 31, 16 ) );

        int16_t diff = int16_t( Bits( proc.R[n], 31, 16 ) )
                     - int16_t( Bits( proc.R[m], 15,  0 ) );

        proc.R[d]  = 0x00000000;
        proc.R[d] |= Bits( sum,  15, 0 );
        proc.R[d] |= Bits( diff, 15, 0 ) << 16;

        proc.CPSR.GE  = ( sum  >= 0 ? 3 : 0 ) 
                    | ( ( diff >= 0 ? 3 : 0 ) << 2 );
    }
}


template< typename proc_type >
void arm::SSUB16_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t diff1 = int16_t( Bits( proc.R[n], 15,  0 ) )
                      - int16_t( Bits( proc.R[m], 15,  0 ) );

        int16_t diff2 = int16_t( Bits( proc.R[n], 31, 16 ) )
                      - int16_t( Bits( proc.R[m], 31, 16 ) );

        proc.R[d]  = 0x00000000;
        proc.R[d] |= Bits( diff1, 15, 0 );
        proc.R[d] |= Bits( diff2, 15, 0 ) << 16;

        proc.CPSR.GE  = ( diff1 >= 0 ? 3 : 0 )
                    | ( ( diff2 >= 0 ? 3 : 0 ) << 2 );
    }
}


template< typename proc_type >
void arm::SSUB8_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );
        
        if( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int8_t diff1 = int8_t( Bits( proc.R[n],  7,  0 ) ) 
                     - int8_t( Bits( proc.R[m],  7,  0 ) );

        int8_t diff2 = int8_t( Bits( proc.R[n], 15,  8 ) )
                     - int8_t( Bits( proc.R[m], 15,  8 ) );

        int8_t diff3 = int8_t( Bits( proc.R[n], 23, 16 ) ) 
                     - int8_t( Bits( proc.R[m], 23, 16 ) );

        int8_t diff4 = int8_t( Bits( proc.R[n], 31, 24 ) )
                     - int8_t( Bits( proc.R[m], 31, 24 ) );

        proc.R[d]  = 0x00000000;
        proc.R[d] |= Bits( diff1, 7, 0 );
        proc.R[d] |= Bits( diff2, 7, 0 ) << 8;
        proc.R[d] |= Bits( diff3, 7, 0 ) << 16;
        proc.R[d] |= Bits( diff4, 7, 0 ) << 24;

        proc.CPSR.GE = ( diff1 >= 0 ? 1 : 0 )
                     | (( diff2 >= 0 ? 1 : 0 ) << 1)
                     | (( diff3 >= 0 ? 1 : 0 ) << 2)
                     | (( diff4 >= 0 ? 1 : 0 ) << 3);
    }
}


template< typename proc_type >
void arm::STM_STMIA_STMEA_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t n             = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t registers = register_list;
        bool wback = ( W == 1 );
        
        if( n == 15 || BitCount( registers ) < 1 ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t address = proc.R[n];

        for( int i = 0; i <= 14; ++i )
        {
            if( Bits( registers, i, i ) == 1 )
            {
                if( (uint32_t)i == n && wback && i != LowestSetBit( registers ) )
                    // Only possible for encodings T1 and A1
                    proc.dMem.write_word( address, 0x000000000 );// UNKNOWN;
                else
                    proc.dMem.write_word( address, proc.R[i] );

                address = address + 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 ) // Only possible for encoding A1
            proc.dMem.write_word( address, PCStoreValue( proc ) );

        if( wback )
            proc.R[n] = proc.R[n] + 4 * BitCount( registers );
    }
}


template< typename proc_type >
void arm::STMDA_STMED_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t n             = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t registers = register_list;
        bool wback = ( W == 1 );
        
        if( n == 15 || BitCount( registers ) < 1 ) UNPREDICTABLE;

        // Operation
        uint32_t address = proc.R[n] - 4 * BitCount( registers ) + 4;

        for( int i = 0; i <= 14; ++i )
        {
            if( Bits( registers, i, i ) == 1 )
            {
                if( (uint32_t)i == n && wback && i != LowestSetBit( registers ) )
                    // Only possible for encodings T1 and A1
                    proc.dMem.write_word( address, 0x000000000 );// UNKNOWN;
                else
                    proc.dMem.write_word( address, proc.R[i] );

                address = address + 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 ) // Only possible for encoding A1
            proc.dMem.write_word( address, PCStoreValue( proc ) );

        if( wback )
            proc.R[n] = proc.R[n] - 4 * BitCount( registers );     
    }
}


template< typename proc_type >
void arm::STMDB_STMFD_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t n             = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        if( W == 1 && n == 13 && BitCount(register_list) >= 2 )
        {
            //PUSH_A1( proc, instr ); //FIXME
            return;
        }

        uint32_t registers = register_list;
        bool wback = ( W == 1 );
        
        if( n == 15 || BitCount( registers ) < 1 ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t address = proc.R[n] - 4 * BitCount( registers );
        for( int i = 0; i <= 14; ++i )
        {
            if( Bits( registers, i, i ) == 1 )
            {
                if( (uint32_t)i == n && wback && i != LowestSetBit( registers ) )
                    // Only possible for encodings T1 and A1
                    proc.dMem.write_word( address, 0x000000000 );// UNKNOWN;
                else
                    proc.dMem.write_word( address, proc.R[i] );

                address = address + 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 ) // Only possible for encoding A1
            proc.dMem.write_word( address, PCStoreValue( proc ) );

        if( wback )
            proc.R[n] = proc.R[n] - 4 * BitCount( registers );   
    }
}


template< typename proc_type >
void arm::STMIB_STMFA_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t W             = Bits( instr, 21, 21 );
        uint32_t n             = Bits( instr, 19, 16 );
        uint32_t register_list = Bits( instr, 15,  0 );

        uint32_t registers = register_list;
        bool wback = ( W == 1 );
        
        if( n == 15 || BitCount( registers ) < 1 ) UNPREDICTABLE;

        // Operation
        uint32_t address = proc.R[n] + 4;

        for( int i = 0; i <= 14; ++i )
        {
            if( Bits( registers, i, i ) == 1 )
            {
                if( (uint32_t)i == n && wback && i != LowestSetBit( registers ) )
                    // Only possible for encodings T1 and A1
                    proc.dMem.write_word( address, 0x000000000 );// UNKNOWN;
                else
                    proc.dMem.write_word( address, proc.R[i] );

                address = address + 4;
            }
        }

        if( Bits( registers, 15, 15 ) == 1 ) // Only possible for encoding A1
            proc.dMem.write_word( address, PCStoreValue( proc ) );

        if( wback )
            proc.R[n] = proc.R[n] + 4 * BitCount( registers );   
    }
}


template< typename proc_type >
void arm::STR_imm_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t imm32 = ZeroExtend(imm12); 
        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 )
        {
            STRT_A1( proc, instr );
            return;
        }
    
        if( n == 13 && P == 1 && U == 0 && W == 1 && imm12 == 0x004 )
        {
            //PUSH_A1( proc, instr );//FIXME
            return;
        }
        
        if( wback && ( n == 15 || n == t ) ) UNPREDICTABLE;

        // Operation
        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + imm32;
        else
            offset_addr = proc.R[n] - imm32;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        uint32_t storeValue;

        if( t == 15 )
            storeValue = PCStoreValue( proc );
        else
            storeValue = proc.R[t];

        proc.dMem.write_word( address, storeValue );

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STR_reg_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t P    = Bits( instr, 24, 24 );
        uint32_t U    = Bits( instr, 23, 23 );
        uint32_t W    = Bits( instr, 21, 21 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t t    = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 )
        {
            STRT_A2( proc, instr );
            return;
        }

        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        if( m == 15 ) UNPREDICTABLE;
        if( wback && (n == 15 || n == t) ) UNPREDICTABLE;
        if( ArchVersion() < 6 && wback && m == n ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset =
             Shift(proc.R[m], shift_t, shift_n, proc.CPSR.C);

        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        uint32_t data;

        if( t == 15 ) // Only possible for encoding A1
            data = PCStoreValue( proc );
        else
            data = proc.R[t];

        if( UnalignedSupport() || Bits( address, 1, 0 ) == 0 
            || CurrentInstrSet( proc ) == InstrSet_ARM )
            proc.dMem.write_word( address, data );
        else // Can only occur before ARMv7
            proc.dMem.write_word( address, 0x0 ); // UNKNOWN;

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRB_imm_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        uint32_t imm32 = ZeroExtend(imm12); 
        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 )
        {
            STRBT_A1( proc, instr );
            return;
        }
        
        if( t == 15 ) UNPREDICTABLE;
        if( wback && ( n == 15 || n == t ) ) UNPREDICTABLE;

        // Operation
        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + imm32;
        else
            offset_addr = proc.R[n] - imm32;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        proc.dMem.write_byte( address, Bits( proc.R[t], 7, 0 ) );

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRB_reg_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t P    = Bits( instr, 24, 24 );
        uint32_t U    = Bits( instr, 23, 23 );
        uint32_t W    = Bits( instr, 21, 21 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t t    = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 )
        {
            STRBT_A2( proc, instr );
            return;
        }

        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        if( t == 15 || m == 15 ) UNPREDICTABLE;
        if( wback && (n == 15 || n == t) ) UNPREDICTABLE;
        if( ArchVersion() < 6 && wback && m == n ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset =
             Shift(proc.R[m], shift_t, shift_n, proc.CPSR.C);

        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        proc.dMem.write_byte( address, Bits( proc.R[t], 7, 0 ) );

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRBT_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = false;

        uint32_t imm32 = ZeroExtend( imm12 );

        SRType      shift_t = (SRType)0;
        uint32_t    shift_n = 0;
        uint32_t    m       = 0;

        if( t == 15 || n == 15 || n == t) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;

        if( register_form )
            offset = Shift(proc.R[m], shift_t, shift_n, proc.CPSR.C);
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        proc.dMem.write_byte( address, Bits( proc.R[t], 7, 0 ) );

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRBT_A2( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U    = Bits( instr, 23, 23 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t t    = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;

        uint32_t imm32 = 0;

        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        if( t == 15 || n == 15 || n == t || m == 15 ) UNPREDICTABLE;
        if( ArchVersion() < 6 && m == n ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;

        if( register_form )
            offset = Shift(proc.R[m], shift_t, shift_n, proc.CPSR.C);
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        proc.dMem.write_byte( address, Bits( proc.R[t], 7, 0 ) );

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRD_imm_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );

        if( Bits( t, 0, 0 ) == 1 ); // UNDEFINED;

        uint32_t t2    = t + 1;
        uint32_t imm32 = ZeroExtend( imm4H << 4 | imm4L );

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 ) UNPREDICTABLE;
        if( wback && ( n == 15 || n == t || n == t2 ) ) UNPREDICTABLE;
        if( t2 == 15 ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + imm32;
        else
            offset_addr = proc.R[n] - imm32;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        proc.dMem.write_word( address,     proc.R[t]  );
        proc.dMem.write_word( address + 4, proc.R[t2] );

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRD_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P  = Bits( instr, 24, 24 );
        uint32_t U  = Bits( instr, 23, 23 );
        uint32_t W  = Bits( instr, 21, 21 );
        uint32_t n  = Bits( instr, 19, 16 );
        uint32_t t  = Bits( instr, 15, 12 );
        uint32_t m  = Bits( instr,  3,  0 );

        if( Bits( t, 0, 0 ) == 1 ); // UNDEFINED;

        uint32_t t2    = t + 1;

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( P == 0 && W == 1 ) UNPREDICTABLE;
        if( t2 == 15 || m == 15 ) UNPREDICTABLE;
        if( wback && ( n == 15 || n == t || n == t2 ) ) UNPREDICTABLE;
        if( ArchVersion() < 6 && wback && m == n ) UNPREDICTABLE;

        // Operation
        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + proc.R[m];
        else
            offset_addr = proc.R[n] - proc.R[m];

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        proc.dMem.write_word( address,     proc.R[t]  );
        proc.dMem.write_word( address + 4, proc.R[t2] );

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRH_imm_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P     = Bits( instr, 24, 24 );
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t W     = Bits( instr, 21, 21 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );

        if( P == 0 && W == 1 )
        {
            STRHT_A1( proc, instr );
            return;
        }

        uint32_t imm32 = ZeroExtend( imm4H << 4 | imm4L );

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        if( t == 15 ) UNPREDICTABLE;
        if( wback && ( n == 15 || n == t ) ) UNPREDICTABLE;

        // Operation
        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + imm32;
        else
            offset_addr = proc.R[n] - imm32;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 )
            proc.dMem.write_half( address, Bits( proc.R[t], 15, 0 ) );
        else // Can only occur before ARMv7
            proc.dMem.write_half( address, 0x0000 ); //UNKNOWN;

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRH_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t P  = Bits( instr, 24, 24 );
        uint32_t U  = Bits( instr, 23, 23 );
        uint32_t W  = Bits( instr, 21, 21 );
        uint32_t n  = Bits( instr, 19, 16 );
        uint32_t t  = Bits( instr, 15, 12 );
        uint32_t m  = Bits( instr,  3,  0 );

        if( P == 0 && W == 1 )
        {
            STRHT_A2( proc, instr );
            return;
        }

        bool     index = (P == 1);
        bool     add   = (U == 1);
        bool     wback = (P == 0) || (W == 1);

        SRType      shift_t = SRType_LSL;
        uint32_t    shift_n = 0;

        if( t == 15 || m == 15 ) UNPREDICTABLE;
        if( wback && ( n == 15 || n == t ) ) UNPREDICTABLE;
        if( ArchVersion() < 6 && wback && m == n ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset = 
            Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );

        uint32_t offset_addr;
        uint32_t address;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( index )
            address = offset_addr;
        else
            address = proc.R[n];

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 )
            proc.dMem.write_half( address, Bits( proc.R[t], 15, 0 ) );
        else // Can only occur before ARMv7
            proc.dMem.write_half( address, 0x0000 ); //UNKNOWN;

        if( wback )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRHT_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm4H = Bits( instr, 11,  8 );
        uint32_t imm4L = Bits( instr,  3,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = false;

        uint32_t imm32 = ZeroExtend( imm4H << 4 | imm4L );
        uint32_t m     = 0;

        if( t == 15 || n == 15 || n == t) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;

        if( register_form )
            offset = proc.R[m];
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 )
            proc.dMem.write_half( address, Bits( proc.R[t], 15, 0 ) );
        else // Can only occur before ARMv7
            proc.dMem.write_half( address, 0x0000 ); //UNKNOWN;

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRHT_A2( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U    = Bits( instr, 23, 23 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t t    = Bits( instr, 15, 12 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;

        uint32_t imm32 = 0;

        if( t == 15 || n == 15 || n == t || m == 15 ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;

        if( register_form )
            offset = proc.R[m];
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 )
            proc.dMem.write_half( address, Bits( proc.R[t], 15, 0 ) );
        else // Can only occur before ARMv7
            proc.dMem.write_half( address, 0x0000 ); //UNKNOWN;

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRT_A1( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U     = Bits( instr, 23, 23 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t t     = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = false;

        uint32_t imm32 = ZeroExtend( imm12 );

        SRType      shift_t = (SRType)0;
        uint32_t    shift_n = 0;
        uint32_t    m       = 0;

        if( n == 15 || n == t) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;
        uint32_t data;

        if( register_form )
            offset = Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        if( t == 15 ) // Only possible for encodings A1 and A2
            data = PCStoreValue( proc );
        else
            data = proc.R[t];

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 
            || CurrentInstrSet( proc ) == InstrSet_ARM )
            proc.dMem.write_word( address, data );
        else // Can only occur before ARMv7
            proc.dMem.write_word( address, 0x00000000 ); //UNKNOWN;

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::STRT_A2( proc_type& proc, uint32_t instr )
{
    if( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operation
        uint32_t U    = Bits( instr, 23, 23 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t t    = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool postindex     = true;
        bool add           = (U == 1);
        bool register_form = true;

        uint32_t imm32 = 0;

        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        if( n == 15 || n == t || m == 15 ) UNPREDICTABLE;
        if( ArchVersion() < 6 && m == n ) UNPREDICTABLE;

        // Operation
        if( !NullCheckIfThumbEE( proc, n ) ) return;

        uint32_t offset;
        uint32_t offset_addr;
        uint32_t address;
        uint32_t data;

        if( register_form )
            offset = Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );
        else
            offset = imm32;

        if( add )
            offset_addr = proc.R[n] + offset;
        else
            offset_addr = proc.R[n] - offset;

        if( postindex )
            address = proc.R[n];
        else
            address = offset_addr;

        if( t == 15 ) // Only possible for encodings A1 and A2
            data = PCStoreValue( proc );
        else
            data = proc.R[t];

        if( UnalignedSupport() || Bits( address, 0, 0 ) == 0 
            || CurrentInstrSet( proc ) == InstrSet_ARM )
            proc.dMem.write_word( address, data );
        else // Can only occur before ARMv7
            proc.dMem.write_word( address, 0x00000000 ); //UNKNOWN;

        if( postindex )
            proc.R[n] = offset_addr;
    }
}


template< typename proc_type >
void arm::SUB_imm_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S     = Bits( instr, 20, 20 );
        uint32_t n     = Bits( instr, 19, 16 );
        uint32_t d     = Bits( instr, 15, 12 );
        uint32_t imm12 = Bits( instr, 11, 0 );
        
        if ( n == 0xF && S == 0x0 ) ; // SEE ADR
        if ( n == 0xD ) ;             // SEE SUB (SP minus immediate)
        if ( d == 0xF && S == 0x1 ) ; // SEE SUBS PC, LR and related
                                      // instructions

        bool     setflags = ( S == 0x1 );
        uint32_t imm32    = ARMExpandImm( proc, imm12 );

        // Operation
        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)( proc.R[n] ), ~imm32, (uint32_t)1,
                               carry, overflow );
        if ( d == 15 )
        {
            ALUWritePC( proc, result ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = result;
            if ( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}


template< typename proc_type >
void arm::SUB_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) ) // FIXME
    {
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t d    = Bits( instr, 15, 12 );
        uint32_t imm5 = Bits( instr, 11,  7 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool        setflags = ( S == 1 );
        ShiftUValue shift    = DecodeImmShift( type, imm5 );
        SRType      shift_t  = shift.shift_t;
        uint32_t    shift_n  = shift.shift_n;

        // Operation
        uint32_t shifted;
        shifted = Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], ~shifted, (uint32_t)1,
                               carry, overflow );
        if ( d == 15 )
        {
            ALUWritePC( proc, result ); // setflags is always FALSE here
        }
        else
        {
            proc.R[d] = result;
            if ( setflags )
            {
                proc.CPSR.N = Bits( result, 31, 31 );
                proc.CPSR.Z = IsZeroBit( result );
                proc.CPSR.C = carry;
                proc.CPSR.V = overflow;
            }
        }
    }
}


template< typename proc_type >
void arm::SUB_sh_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S    = Bits( instr, 20, 20 );
        uint32_t n    = Bits( instr, 19, 16 );
        uint32_t d    = Bits( instr, 15, 12 );
        uint32_t s    = Bits( instr, 11,  8 );
        uint32_t type = Bits( instr,  6,  5 );
        uint32_t m    = Bits( instr,  3,  0 );

        bool   setflags = ( S == 1 );
        SRType shift_t  = DecodeRegShift( type );

        if ( d == 15 || n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t shift_n = UInt( Bits( proc.R[s], 7, 0 ) );
        uint32_t shifted = Shift( proc.R[m], shift_t, shift_n, proc.CPSR.C );

        uint32_t result, carry, overflow;
        result = AddWithCarry( (uint32_t)proc.R[n], ~shifted, (uint32_t)1,
                               carry, overflow );
        
        proc.R[d] = result;
        if ( setflags )
        {
            proc.CPSR.N = Bits( result, 31, 31 );
            proc.CPSR.Z = IsZeroBit( result );
            proc.CPSR.C = carry;
            proc.CPSR.V = overflow;
        }

    }
}


template< typename proc_type >
void arm::SXTAB_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );
        if ( n == 0xF ) ;           // SEE SXTB
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = proc.R[n] + SignExtend( Bits( rotated, 7, 0 ), 32, 8 );
    }
}


template< typename proc_type >
void arm::SXTAB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );
        if ( n == 0xF ) ;           // SEE SXTB16
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        uint32_t rn_h    = Bits( proc.R[n], 31, 16 );
        uint32_t rn_l    = Bits( proc.R[n], 15,  0 );

        uint32_t rd_l = rn_l + SignExtend( Bits( rotated,  7,  0 ), 16, 8 );
        uint32_t rd_h = rn_h + SignExtend( Bits( rotated, 23, 16 ), 16, 8 );
        proc.R[d] = ( rd_h << 16 ) + rd_l;
    }    
}


template< typename proc_type >
void arm::SXTAH_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( n == 0xF ) ;           // SEE SXTH
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = proc.R[n] + SignExtend( Bits( rotated, 15, 0 ), 32, 16 );
    }    
}


template< typename proc_type >
void arm::SXTB_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = SignExtend( Bits( rotated, 7, 0 ), 32, 8 );
    }    
}


template< typename proc_type >
void arm::SXTB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        uint32_t rd_l = SignExtend( Bits( rotated,  7,  0 ), 16, 8 );
        uint32_t rd_h = SignExtend( Bits( rotated, 23, 16 ), 16, 8 );
        proc.R[d] = ( rd_h << 16 ) + rd_l;
    }    
}


template< typename proc_type >
void arm::SXTH_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = SignExtend( Bits( rotated, 15, 0 ), 32, 16 );
    }
}


template< typename proc_type >
void arm::TEQ_imm_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n     = Bits( instr, 19, 16 );
        uint32_t    imm12 = Bits( instr, 11,  0 );
        UValueCarry value = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t    imm32 = value.value;
        uint32_t    carry = value.carry ? 1 : 0;

        // Operation
        uint32_t result = proc.R[n] ^ imm32;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged
    }
}


template< typename proc_type >
void arm::TEQ_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n       = Bits( instr, 19, 16 );
        uint32_t    imm5    = Bits( instr, 11,  7 );
        uint32_t    type    = Bits( instr,  6,  5 );
        uint32_t    m       = Bits( instr,  3,  0 );
        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        // Operation
        UValueCarry value   = Shift_C( proc.R[m], shift_t, shift_n,
                                       proc.CPSR.C );
        uint32_t    shifted = value.value;
        uint32_t    carry   = value.carry ? 1 : 0;

        uint32_t result = proc.R[n] ^ shifted;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged
    }
}


template< typename proc_type >
void arm::TEQ_sh_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n       = Bits( instr, 19, 16 );
        uint32_t    s       = Bits( instr, 11,  8 );
        uint32_t    type    = Bits( instr,  6,  5 );
        uint32_t    m       = Bits( instr,  3,  0 );
        SRType      shift_t = DecodeRegShift( type );

        if ( n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t    shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry value   = Shift_C( proc.R[m], shift_t, shift_n,
                                       proc.CPSR.C );
        uint32_t    shifted = value.value;
        uint32_t    carry   = value.carry ? 1 : 0;

        uint32_t result = proc.R[n] ^ shifted;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged        
    }    
}


template< typename proc_type >
void arm::TST_imm_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n     = Bits( instr, 19, 16 );
        uint32_t    imm12 = Bits( instr, 11,  0 );
        UValueCarry value = ARMExpandImm_C( imm12, proc.CPSR.C );
        uint32_t    imm32 = value.value;
        uint32_t    carry = value.carry ? 1 : 0;

        // Operation
        uint32_t result = proc.R[n] & imm32;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged        
    }    
}


template< typename proc_type >
void arm::TST_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n       = Bits( instr, 19, 16 );
        uint32_t    imm5    = Bits( instr, 11,  7 );
        uint32_t    type    = Bits( instr,  6,  5 );
        uint32_t    m       = Bits( instr,  3,  0 );
        ShiftUValue shift   = DecodeImmShift( type, imm5 );
        SRType      shift_t = shift.shift_t;
        uint32_t    shift_n = shift.shift_n;

        // Operation
        UValueCarry value   = Shift_C( proc.R[m], shift_t, shift_n,
                                       proc.CPSR.C );
        uint32_t    shifted = value.value;
        uint32_t    carry   = value.carry ? 1 : 0;
        uint32_t    result  = proc.R[n] & shifted;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged
    }    
}


template< typename proc_type >
void arm::TST_sh_reg_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t    n       = Bits( instr, 19, 16 );
        uint32_t    s       = Bits( instr, 11,  8 );
        uint32_t    type    = Bits( instr,  6,  5 );
        uint32_t    m       = Bits( instr,  3,  0 );
        SRType      shift_t = DecodeRegShift( type );

        if ( n == 15 || m == 15 || s == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t    shift_n = Bits( proc.R[s], 7, 0 );
        UValueCarry value   = Shift_C( proc.R[m], shift_t, shift_n,
                                       proc.CPSR.C );
        uint32_t    shifted = value.value;
        uint32_t    carry   = value.carry ? 1 : 0;
        uint32_t    result  = proc.R[n] & shifted;
        proc.CPSR.N = Bits( result, 31, 31 );
        proc.CPSR.Z = IsZeroBit( result );
        proc.CPSR.C = carry;
        // CPSR.V unchanged
    }    
}


template< typename proc_type >
void arm::UADD16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t sum1 = Bits( proc.R[n], 15,  0 ) + Bits( proc.R[m], 15,  0 );
        uint32_t sum2 = Bits( proc.R[n], 31, 16 ) + Bits( proc.R[m], 31, 16 );
        proc.R[d] = Bits( sum1, 15, 0 ) + ( Bits( sum2, 15, 0 ) << 16 );
        
        proc.CPSR.GE = (sum1 >= 0x10000 ? 0x3 : 0x0) 
                     | (sum2 >= 0x10000 ? 0xC : 0x0);
    }
}


template< typename proc_type >
void arm::UADD8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t sum1 = Bits( proc.R[n],  7,  0  ) + Bits( proc.R[m],  7,  0  );
        uint32_t sum2 = Bits( proc.R[n], 15,  8  ) + Bits( proc.R[m], 15,  8  );
        uint32_t sum3 = Bits( proc.R[n], 23,  16 ) + Bits( proc.R[m], 23,  16 );
        uint32_t sum4 = Bits( proc.R[n], 31,  24 ) + Bits( proc.R[m], 31,  24 );

        proc.R[d]  = 0x00000000;
        proc.R[d] += Bits( sum1, 7, 0 );
        proc.R[d] += ( Bits( sum2, 7, 0 ) << 8 );
        proc.R[d] += ( Bits( sum3, 7, 0 ) << 16 );
        proc.R[d] += ( Bits( sum4, 7, 0 ) << 24 );

        proc.CPSR.GE = (sum1 >= 0x100 ? 0x1 : 0x0)
                     | (sum2 >= 0x100 ? 0x2 : 0x0)
                     | (sum3 >= 0x100 ? 0x4 : 0x0)
                     | (sum4 >= 0x100 ? 0x8 : 0x0);
    }    
}


template< typename proc_type >
void arm::UASX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t diff = Bits( proc.R[n], 15,  0 ) - Bits( proc.R[m], 31, 16 );
        uint32_t sum  = Bits( proc.R[n], 31, 16 ) + Bits( proc.R[m], 15,  0 );
        proc.R[d] = Bits( diff, 15, 0 ) + ( Bits( sum, 15, 0 ) << 16 );
        
        proc.CPSR.GE = (SInt( (uint16_t)diff ) >= 0x0 ? 0x3 : 0x0)
                     | (sum  >= 0x10000 ? 0xC : 0x0);
    }
}


template< typename proc_type >
void arm::UBFX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t widthminus1 = Bits( instr, 20, 16 );
        uint32_t d           = Bits( instr, 15, 12 );
        uint32_t lsbit       = Bits( instr, 11,  7 );
        uint32_t n           = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t msbit = lsbit + widthminus1;
        if ( msbit <= 31 )
        {
            proc.R[d] = ZeroExtend( Bits( proc.R[n], msbit, lsbit ) );
        }
        else
             UNPREDICTABLE;
    }
}


template< typename proc_type >
void arm::UHADD16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t sum1 = Bits( proc.R[n], 15,  0 ) + Bits( proc.R[m], 15,  0 );
        uint32_t sum2 = Bits( proc.R[n], 31, 16 ) + Bits( proc.R[m], 31, 16 );

        sum1 = Bits( sum1, 16, 1 );
        sum2 = Bits( sum2, 16, 1 );

        proc.R[d] = sum1 + ( sum2 << 16 );
    }    
}


template< typename proc_type >
void arm::UHADD8_A1( proc_type& proc, uint32_t instr )

{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t sum1 = Bits( proc.R[n],  7,  0 ) + Bits( proc.R[m],  7,  0 );
        uint32_t sum2 = Bits( proc.R[n], 15,  8 ) + Bits( proc.R[m], 15,  8 );
        uint32_t sum3 = Bits( proc.R[n], 23, 16 ) + Bits( proc.R[m], 23, 16 );
        uint32_t sum4 = Bits( proc.R[n], 31, 24 ) + Bits( proc.R[m], 31, 24 );
        
        sum1 = Bits( sum1, 8, 1 );
        sum2 = Bits( sum2, 8, 1 );
        sum3 = Bits( sum3, 8, 1 );
        sum4 = Bits( sum4, 8, 1 );

        proc.R[d] = sum1 + ( sum2 << 8 ) + ( sum3 << 16 ) + ( sum4 << 24 );
    }
}


template< typename proc_type >
void arm::UHASX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t diff = Bits( proc.R[n], 15,  0 ) - Bits( proc.R[m], 31, 16 );
        uint32_t sum  = Bits( proc.R[n], 31, 16 ) + Bits( proc.R[m], 15,  0 );

        diff = Bits( diff, 16, 1 );
        sum  = Bits( sum , 16, 1 );

        proc.R[d] = diff + ( sum << 16 );
    }
}


template< typename proc_type >
void arm::UHSAX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t sum  = Bits( proc.R[n], 15,  0 ) + Bits( proc.R[m], 31, 16 );
        uint32_t diff = Bits( proc.R[n], 31, 16 ) - Bits( proc.R[m], 15,  0 );

        diff = Bits( diff, 16, 1 );
        sum  = Bits( sum , 16, 1 );

        proc.R[d] = sum + ( diff << 16 );
    }
}


template< typename proc_type >
void arm::UHSUB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t diff1 = Bits( proc.R[n], 15,  0 ) - Bits( proc.R[m], 15,  0 );
        uint32_t diff2 = Bits( proc.R[n], 31, 16 ) - Bits( proc.R[m], 31, 16 );

        diff1 = Bits( diff1, 16, 1 );
        diff2 = Bits( diff2, 16, 1 );

        proc.R[d] = diff1 + ( diff2 << 16 );
    }
}


template< typename proc_type >
void arm::UHSUB8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t diff1 = Bits( proc.R[n],  7,  0 ) - Bits( proc.R[m],  7,  0 );
        uint32_t diff2 = Bits( proc.R[n], 15,  8 ) - Bits( proc.R[m], 15,  8 );
        uint32_t diff3 = Bits( proc.R[n], 23, 16 ) - Bits( proc.R[m], 23, 16 );
        uint32_t diff4 = Bits( proc.R[n], 31, 24 ) - Bits( proc.R[m], 31, 24 );
        
        diff1 = Bits( diff1, 8, 1 );
        diff2 = Bits( diff2, 8, 1 );
        diff3 = Bits( diff3, 8, 1 );
        diff4 = Bits( diff4, 8, 1 );

        proc.R[d] = diff1 + ( diff2 << 8 ) + ( diff3 << 16 ) + ( diff4 << 24 );
    }
}


template< typename proc_type >
void arm::UMAAL_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t n   = Bits( instr,  3,  0 );

        if ( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if ( dHi == dLo ) UNPREDICTABLE;

        // Operation
        uint64_t result
            = (uint64_t)UInt( proc.R[n] )   * (uint64_t)UInt( proc.R[m] )
            + (uint64_t)UInt( proc.R[dHi] ) + (uint64_t)UInt( proc.R[dLo] );
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );
    }
}


template< typename proc_type >
void arm::UMLAL_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S   = Bits( instr, 20, 20 );
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t n   = Bits( instr,  3,  0 );

        if ( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if ( dHi == dLo ) UNPREDICTABLE;
        if ( ArchVersion() < 6 && ( dHi == n || dLo == n ) ) UNPREDICTABLE;

        bool setflags = ( S == 0x1 );

        // Operation
        uint64_t result
            =   (uint64_t)( proc.R[n] )           * (uint64_t)( proc.R[m] )
            + ( (uint64_t)( proc.R[dHi] ) << 32 ) + (uint64_t)( proc.R[dLo] );
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );

        if ( setflags )
        {
            proc.CPSR.N = Bits64( result, 63, 63 );
            proc.CPSR.Z = IsZeroBit( result );
            // CPSR.C, CPSR.V unchanged
        }
    }
}


template< typename proc_type >
void arm::UMULL_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t S   = Bits( instr, 20, 20 );
        uint32_t dHi = Bits( instr, 19, 16 );
        uint32_t dLo = Bits( instr, 15, 12 );
        uint32_t m   = Bits( instr, 11,  8 );
        uint32_t n   = Bits( instr,  3,  0 );

        if ( dLo == 15 || dHi == 15 || n == 15 || m == 15 ) UNPREDICTABLE;
        if ( dHi == dLo ) UNPREDICTABLE;
        if ( ArchVersion() < 6 && ( dHi == n || dLo == n ) ) UNPREDICTABLE;

        bool setflags = ( S == 0x1 );

        // Operation
        uint64_t result = (uint64_t)( proc.R[n] ) * (uint64_t)( proc.R[m] );
        proc.R[dHi] = Bits64( result, 63, 32 );
        proc.R[dLo] = Bits64( result, 31,  0 );

        if ( setflags )
        {
            proc.CPSR.N = Bits64( result, 63, 63 );
            proc.CPSR.Z = IsZeroBit( result );
            // CPSR.C, CPSR.V unchanged
        }
    }
}


template< typename proc_type >
void arm::UQADD16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t sum1 = Bits( proc.R[n], 15,  0 ) + Bits( proc.R[m], 15,  0 );
        int64_t sum2 = Bits( proc.R[n], 31, 16 ) + Bits( proc.R[m], 31, 16 );
        proc.R[d] = UnsignedSat( sum1, 16 ) + ( UnsignedSat( sum2, 16 ) << 16 );
    }
}


template< typename proc_type >
void arm::UQADD8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t sum1 = Bits( proc.R[n],  7,   0 ) + Bits( proc.R[m],  7,  0 );
        int64_t sum2 = Bits( proc.R[n], 15,   8 ) + Bits( proc.R[m], 15,  8 );
        int64_t sum3 = Bits( proc.R[n], 23,  16 ) + Bits( proc.R[m], 23, 16 );
        int64_t sum4 = Bits( proc.R[n], 31,  24 ) + Bits( proc.R[m], 31, 24 );

        sum1 = UnsignedSat( sum1, 8 );
        sum2 = UnsignedSat( sum2, 8 );
        sum3 = UnsignedSat( sum3, 8 );
        sum4 = UnsignedSat( sum4, 8 );

        proc.R[d] = sum1 + ( sum2 << 8 ) + ( sum3 << 16 ) + ( sum4 << 24 );
    }
}


template< typename proc_type >
void arm::UQASX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t diff
            = (int64_t)Bits( proc.R[n], 15,  0 )
            - (int64_t)Bits( proc.R[m], 31, 16 );
        int64_t sum
            = (int64_t)Bits( proc.R[n], 31, 16 )
            + (int64_t)Bits( proc.R[m], 15,  0 );

        proc.R[d] = UnsignedSat( diff, 16 ) + ( UnsignedSat( sum, 16 ) << 16 );
    }
}


template< typename proc_type >
void arm::UQSAX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t sum
            = (int64_t)Bits( proc.R[n], 15,  0 )
            + (int64_t)Bits( proc.R[m], 31, 16 );
        int64_t diff
            = (int64_t)Bits( proc.R[n], 31, 16 )
            - (int64_t)Bits( proc.R[m], 15,  0 );

        proc.R[d] = UnsignedSat( sum, 16 ) + ( UnsignedSat( diff, 16 ) << 16 );
    }
}


template< typename proc_type >
void arm::UQSUB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t diff1
            = (int64_t)Bits( proc.R[n], 15,  0 )
            - (int64_t)Bits( proc.R[m], 15,  0 );
        int64_t diff2
            = (int64_t)Bits( proc.R[n], 31, 16 )
            - (int64_t)Bits( proc.R[m], 31, 16 );
        proc.R[d]
            = UnsignedSat( diff1, 16 ) + ( UnsignedSat( diff2, 16 ) << 16 );
    }
}


template< typename proc_type >
void arm::UQSUB8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int64_t diff1
            = (int64_t)Bits( proc.R[n],  7,  0 )
            - (int64_t)Bits( proc.R[m],  7,  0 );
        int64_t diff2
            = (int64_t)Bits( proc.R[n], 15,  8 )
            - (int64_t)Bits( proc.R[m], 15,  8 );
        int64_t diff3
            = (int64_t)Bits( proc.R[n], 23, 16 )
            - (int64_t)Bits( proc.R[m], 23, 16 );
        int64_t diff4
            = (int64_t)Bits( proc.R[n], 31, 24 )
            - (int64_t)Bits( proc.R[m], 31, 24 );

        diff1 = UnsignedSat( diff1, 8 );
        diff2 = UnsignedSat( diff2, 8 );
        diff3 = UnsignedSat( diff3, 8 );
        diff4 = UnsignedSat( diff4, 8 );

        proc.R[d] = diff1 + ( diff2 << 8 ) + ( diff3 << 16 ) + ( diff4 << 24 );
    }
}


template< typename proc_type >
void arm::USAD8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t n = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int16_t diff1
            = (int16_t)Bits( proc.R[n],  7,  0 )
            - (int16_t)Bits( proc.R[m],  7,  0 );
        int16_t diff2
            = (int16_t)Bits( proc.R[n], 15,  8 )
            - (int16_t)Bits( proc.R[m], 15,  8 );
        int16_t diff3
            = (int16_t)Bits( proc.R[n], 23, 16 )
            - (int16_t)Bits( proc.R[m], 23, 16 );
        int16_t diff4
            = (int16_t)Bits( proc.R[n], 31, 24 )
            - (int16_t)Bits( proc.R[m], 31, 24 );

        uint32_t absdiff1 = Abs( diff1 );
        uint32_t absdiff2 = Abs( diff2 );
        uint32_t absdiff3 = Abs( diff3 );
        uint32_t absdiff4 = Abs( diff4 );

        uint32_t result = absdiff1 + absdiff2 + absdiff3 + absdiff4;
        proc.R[d] = result;
    }
}


template< typename proc_type >
void arm::USADA8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t d = Bits( instr, 19, 16 );
        uint32_t a = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr, 11,  8 );
        uint32_t n = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 || a == 15 ) UNPREDICTABLE;

        // Operation
        int16_t diff1
            = (int16_t)Bits( proc.R[n],  7,  0 )
            - (int16_t)Bits( proc.R[m],  7,  0 );
        int16_t diff2
            = (int16_t)Bits( proc.R[n], 15,  8 )
            - (int16_t)Bits( proc.R[m], 15,  8 );
        int16_t diff3
            = (int16_t)Bits( proc.R[n], 23, 16 )
            - (int16_t)Bits( proc.R[m], 23, 16 );
        int16_t diff4
            = (int16_t)Bits( proc.R[n], 31, 24 )
            - (int16_t)Bits( proc.R[m], 31, 24 );

        uint32_t absdiff1 = Abs( diff1 );
        uint32_t absdiff2 = Abs( diff2 );
        uint32_t absdiff3 = Abs( diff3 );
        uint32_t absdiff4 = Abs( diff4 );

        uint32_t result
            = UInt( proc.R[a] ) + absdiff1 + absdiff2 + absdiff3 + absdiff4;
        proc.R[d] = result;
    }
}


template< typename proc_type >
void arm::USAT_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t saturate_to = Bits( instr, 20, 16 );
        uint32_t d           = Bits( instr, 15, 12 );
        uint32_t imm5        = Bits( instr, 11,  7 );
        uint32_t sh          = Bits( instr,  6,  6 );
        uint32_t n           = Bits( instr,  3,  0 );

        ShiftUValue shift    = DecodeImmShift( sh << 1, imm5 );
        SRType      shift_t  = shift.shift_t;
        uint32_t    shift_n  = shift.shift_n;

        if ( d == 15 || n == 15 ) UNPREDICTABLE;

        // Operation
        int64_t operand
            = SInt( Shift( proc.R[n], shift_t, shift_n, proc.CPSR.C ) );
        UValueSat value   = UnsignedSatQ( operand, saturate_to );
        uint64_t  result  = value.value;
        uint32_t  sat     = value.saturated ? 1 : 0;
        
        proc.R[d] = ZeroExtend( result );
        if ( sat )
        {
            proc.CPSR.Q = 1;
        }
    }
}


template< typename proc_type >
void arm::USAT16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t saturate_to = Bits( instr, 19, 16 );
        uint32_t d           = Bits( instr, 15, 12 );
        uint32_t n           = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 ) UNPREDICTABLE;

        // Operation
        uint16_t operand1 = Bits( proc.R[n], 15,  0 );
        uint16_t operand2 = Bits( proc.R[n], 31, 16 );

        UValueSat v1 = UnsignedSatQ( SInt( operand1 ), saturate_to );
        uint64_t  result1 = v1.value;
        uint32_t  sat1    = v1.saturated ? 1 : 0;

        UValueSat v2 = UnsignedSatQ( SInt( operand2 ), saturate_to );
        uint64_t  result2 = v2.value;
        uint32_t  sat2    = v2.saturated ? 1 : 0;

        proc.R[d] = ZeroExtend( result1 ) + ( ZeroExtend( result2 ) << 16 );

        if ( sat1 || sat2 )
        {
            proc.CPSR.Q = 1;
        }
    }
}


template< typename proc_type >
void arm::USAX_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t sum  = Bits( proc.R[n], 15,  0 ) + Bits( proc.R[m], 31, 16 );
        int32_t diff = Bits( proc.R[n], 31, 16 ) - Bits( proc.R[m], 15,  0 );

        proc.R[d] = Bits( sum, 15, 0 ) + ( Bits( diff, 15, 0 ) << 16 );

        proc.CPSR.GE = (sum  >= 0x10000 ? 0x3 : 0x0)
                     | (diff >= 0x0     ? 0xC : 0x0);
    }
}


template< typename proc_type >
void arm::USUB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t diff1 = Bits( proc.R[n], 15,  0 ) - Bits( proc.R[m], 15,  0 );
        int32_t diff2 = Bits( proc.R[n], 31, 16 ) - Bits( proc.R[m], 31, 16 );
        proc.R[d] = Bits( diff1, 15, 0 ) + ( Bits( diff2, 15, 0 ) << 16 );

        proc.CPSR.GE = (diff1 >= 0x0 ? 0x3 : 0x0)
                     | (diff2 >= 0x0 ? 0xC : 0x0);
    }
}


template< typename proc_type >
void arm::USUB8_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n = Bits( instr, 19, 16 );
        uint32_t d = Bits( instr, 15, 12 );
        uint32_t m = Bits( instr,  3,  0 );

        if ( d == 15 || n == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        int32_t diff1 = Bits( proc.R[n],  7,  0 ) - Bits( proc.R[m],  7,  0 );
        int32_t diff2 = Bits( proc.R[n], 15,  8 ) - Bits( proc.R[m], 15,  8 );
        int32_t diff3 = Bits( proc.R[n], 23, 16 ) - Bits( proc.R[m], 23, 16 );
        int32_t diff4 = Bits( proc.R[n], 31, 24 ) - Bits( proc.R[m], 31, 24 );

        proc.R[d]
            = Bits( diff1, 7, 0 )
            + ( Bits( diff2, 7, 0 ) << 8 )
            + ( Bits( diff3, 7, 0 ) << 16 )
            + ( Bits( diff4, 7, 0 ) << 24 );

        proc.CPSR.GE = (diff1 >= 0x0 ? 0x1 : 0x0)
                     | (diff2 >= 0x0 ? 0x2 : 0x0)
                     | (diff3 >= 0x0 ? 0x4 : 0x0)
                     | (diff4 >= 0x0 ? 0x8 : 0x0);
    }
}


template< typename proc_type >
void arm::UXTAB_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );
        if ( n == 0xF ) ;           // SEE UXTB
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = proc.R[n] + ZeroExtend( Bits( rotated, 7, 0 ) );
    }
}


template< typename proc_type >
void arm::UXTAB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );
        if ( n == 0xF ) ;           // SEE UXTB16
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        uint32_t rn_h    = Bits( proc.R[n], 31, 16 );
        uint32_t rn_l    = Bits( proc.R[n], 15,  0 );

        uint32_t rd_l = rn_l + ZeroExtend( Bits( rotated, 7, 0 ) );
        uint32_t rd_h = rn_h + ZeroExtend( Bits( rotated, 23, 16 ) );
        proc.R[d] = ( Bits( rd_h, 15, 0 ) << 16 ) + Bits( rd_l, 15, 0 );
    }
}


template< typename proc_type >
void arm::UXTAH_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t n        = Bits( instr, 19, 16 );
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( n == 0xF ) ;           // SEE UXTH
        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = proc.R[n] + ZeroExtend( Bits( rotated, 15, 0 ) );
    }
}


template< typename proc_type >
void arm::UXTB_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = ZeroExtend( Bits( rotated, 7, 0 ) );
    }
}


template< typename proc_type >
void arm::UXTB16_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        uint32_t rd_l = ZeroExtend( Bits( rotated, 7, 0 ) );
        uint32_t rd_h = ZeroExtend( Bits( rotated, 23, 16 ) );
        proc.R[d] = ( Bits( rd_h, 15, 0 ) << 16 ) + Bits( rd_l, 15, 0 );
    }
}


template< typename proc_type >
void arm::UXTH_A1( proc_type& proc, uint32_t instr )
{
    if ( ConditionPassed( proc, instr ) )
    {    
        // Encoding-specific operations
        uint32_t d        = Bits( instr, 15, 12 );
        uint32_t rotate   = Bits( instr, 11, 10 );
        uint32_t m        = Bits( instr,  3,  0 );
        uint32_t rotation = UInt( rotate << 3 );

        if ( d == 15 || m == 15 ) UNPREDICTABLE;

        // Operation
        uint32_t rotated = ROR( proc.R[m], rotation );
        proc.R[d] = ZeroExtend( Bits( rotated, 15, 0 ) );
    }
}


#endif // __ARMV7_INSTRUCTION_IMPL_HPP__
