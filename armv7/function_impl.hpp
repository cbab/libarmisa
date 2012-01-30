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

#ifndef __ARMV7_FUNCTION_IMPL_HPP__
#define __ARMV7_FUNCTION_IMPL_HPP__

#include "function.hpp"
#include "types.hpp"

#include <boost/cstdint.hpp>
#include <cassert>
#include <cmath>



template< typename proc_type >
uint32_t arm::ARMExpandImm( proc_type& proc, uint32_t imm12 )
{
    return ARMExpandImm_C( imm12, proc.CPSR.C ).value;
}


template< typename proc_type >
bool arm::ConditionPassed( proc_type& proc, uint32_t instr )
{
    bool result = false;
    uint32_t cond = CurrentCond( instr );

    switch ( Bits( cond, 3, 1 ) )
    {

    case 0x0:
        // EQ or NE
        result = (proc.CPSR.Z == 1);
        break;
    case 0x1:
        // CS or CC
        result = (proc.CPSR.C == 1);
        break;
    case 0x2:
        // MI or PL
        result = (proc.CPSR.N == 1);
        break;
    case 0x3:
        // VS or VC
        result = (proc.CPSR.V == 1);
        break;
    case 0x4:
        // HI or LS
        result = (proc.CPSR.C == 1) && (proc.CPSR.Z == 0);
        break;
    case 0x5:
        // GE or LT
        result = (proc.CPSR.N == proc.CPSR.V);
        break;
    case 0x6:
        // GT or LE
        result = (proc.CPSR.N == proc.CPSR.V) && (proc.CPSR.Z == 0);
        break;
    case 0x7:
        // AL
        result = true;
        break;
    default:
        // This is bad
        assert( false && "unknown condition in instruction" );
        break;
    }

    // Check the lowest bit only.
    int cond_l = cond & 0x1;

    // Condition bits ‘111x’ indicate the instruction is always executed. 
    // Otherwise, invert condition if necessary.
    if (cond_l == 1 && cond != 0xF)
        result = !result;

    return result;
}
    

template< typename value_type, typename flag_type >
value_type arm::AddWithCarry( value_type x, value_type y, flag_type carry_in,
                              flag_type& carry_out, flag_type& overflow )
{
    uint64_t ux = (uint64_t)UInt( x );
    uint64_t uy = (uint64_t)UInt( y );
    uint64_t uc = (uint64_t)UInt( carry_in );
    int64_t  sx = (int64_t) SInt( x );
    int64_t  sy = (int64_t) SInt( y );
    int64_t  sc = (int64_t) SInt( carry_in );

    uint64_t   unsigned_sum = ux + uy + uc;
    int64_t    signed_sum   = sx + sy + sc;
    uint32_t   N            = sizeof( value_type ) * 8;
    value_type result       = Bits64( unsigned_sum, N - 1, 0 );

    carry_out = UInt( result ) == unsigned_sum ? 0 : 1;
    overflow  = SInt( result ) == signed_sum   ? 0 : 1;

    return result;
}



template< typename proc_type >
arm::InstrSet arm::CurrentInstrSet( proc_type& proc )
{    
    InstrSet result;

    uint32_t state = proc.CPSR.J << 1 | proc.CPSR.T;

    switch ( state )
    {
    case 0:
        result = InstrSet_ARM;
        break;
    case 1:
        result = InstrSet_Thumb;
        break;
    case 2:
        result = InstrSet_Jazelle;
        break;
    case 3:
        result = InstrSet_ThumbEE;
        break;
    }
        
    return result;
}

template< typename proc_type >
void arm::SelectInstrSet( proc_type& proc, InstrSet iset )
{
    switch (iset)
    {
    case InstrSet_ARM:
        if(CurrentInstrSet( proc ) == InstrSet_ThumbEE)
        {        
            ;// UNPREDICTABLE
        }
        else
        {
            proc.CPSR.J = 0;
            proc.CPSR.T = 0;
        }
        break;
    case InstrSet_Thumb:
        proc.CPSR.J = 0;
        proc.CPSR.T = 1;
        break;
    case InstrSet_Jazelle:
        proc.CPSR.J = 1;
        proc.CPSR.T = 0;
        break;
    case InstrSet_ThumbEE:
        proc.CPSR.J = 1;
        proc.CPSR.T = 1;
        break;
    }
}


template< typename proc_type >
void arm::ALUWritePC( proc_type& proc, uint32_t address )
{
    if ( ArchVersion() >= 7 && CurrentInstrSet( proc ) == InstrSet_ARM )
    {
        BXWritePC( proc, address );
    }
    else
    {
        BranchWritePC( proc, address );
    }
}

template< typename proc_type >
void arm::LoadWritePC( proc_type& proc, uint32_t address)
{
    if ( ArchVersion() >= 5 )
    {
        BXWritePC( proc, address );
    }
    else
    {
        BranchWritePC( proc, address );
    }
}


template< typename value_type >
value_type arm::Abs( value_type value )
{
    return value < 0 ? -value : value;
}


template< typename proc_type >
void arm::BXWritePC( proc_type& proc, uint32_t address )
{
    if( CurrentInstrSet( proc ) == InstrSet_ThumbEE )
    {
        if( address & 0x1 )
        {
            // Force lowest address bit to 0.
            uint32_t branch = address & (0xFFFFFFFE);
            BranchTo( proc, branch ); // Remaining in ThumbEE state
        } 
        else
        {
            // UNPREDICTABLE
        }
    }
    else
    {
        if( address & 0x1 )
        {
            SelectInstrSet( proc, InstrSet_Thumb );
            // Force lowest address bit to 0.
            uint32_t branch = address & (0xFFFFFFFE);
            BranchTo( proc, branch );
        } 
        else if( !((address >> 1) & 0x1) ) 
        {
            SelectInstrSet( proc, InstrSet_ARM );
            BranchTo( proc, address );
        }
        else
        {
            // UNPREDICTABLE
        }

    }
}

template< typename proc_type >
void arm::BranchWritePC( proc_type& proc, uint32_t address )
{
    if( CurrentInstrSet( proc ) == InstrSet_ARM )
    {
        // Second condition is wrong.
        /*
        if( ArchVersion( proc ) < 6 && ((address & 0x3) != 0) )
        {
            // UNPREDICTABLE
        }
        */

        // Force lowest two bits to 0.
        CLEAR_BIT( address, 0 );
        CLEAR_BIT( address, 1 );
        uint32_t branch = address;
        BranchTo( proc, branch );
    }
    else
    {
        // Force lowest address bit to 0.
        CLEAR_BIT( address, 0 );
        uint32_t branch = address;
        BranchTo( proc, branch );
    }

}

template< typename proc_type >
void arm::BranchTo( proc_type& proc, uint32_t address )
{
    proc.PC = address;
}

template< typename proc_type >
int arm::PCStoreValue( proc_type& proc )
{
    // This function returns the PC value. On architecture versions
    // before ARMv7, it is permitted to instead return PC+4, provided
    // it does so consistently. It is used only to describe ARM
    // instructions, so it returns the address of the current
    // instruction plus 8 (normally) or 12 (when the alternative is
    // permitted).
        
    return proc.PC;
}
    
template< typename proc_type >
bool arm::NullCheckIfThumbEE( proc_type& proc, int n )
{
    //ThumbEE is not implemented
    return true;
}


template< typename proc_type >
bool arm::BigEndian( proc_type& proc )
{
    return proc.CPSR.E == 0x1;
}

template< typename T >
T arm::NOT( T val )
{
    return ~val;
}

template< typename T >
bool arm::BadMode( T val )
{
    switch( val )
    {
    case 0x10:
        // User mode
        return false;
    case 0x11:
        // FIQ mode
        return false;
    case 0x12:
        // IRQ mode
        return false;
    case 0x13:
        // Supervisor mode
        return false;
    case 0x16:
        // Monitor mode
        return !HaveSecurityExt();
    case 0x17:
        // Abort mode
        return false;
    case 0x18:
        // Undefined mode
        return false;
    case 0x1F:
        // System mode
        return false;
    default:
        return true;
    }
}

template< typename proc_type >
bool arm::CurrentModeIsPrivileged( proc_type& proc )
{
    if( BadMode( proc.CPSR.M ) )
    {
        // Unpredictable
        return proc.Z;
    }

    if( proc.CPSR.M == 0x10 || proc.CPSR.M == 0x1F )
    {
        return true;
    }
    else
    {
        return false;
    }
}

template< typename proc_type >
bool arm::IsSecure( proc_type& proc )
{
    return !HaveSecurityExt() || proc.SCR.NS == 0 || proc.CPSR.M == 0x16;
}

template< typename proc_type, typename mask_type, typename value_type >
void arm::CPSRWriteByInstr( value_type value, mask_type bytemask,
                            bool affect_execstate, proc_type& proc )
{
    const bool privileged = CurrentModeIsPrivileged( proc );
    const bool nmfi = proc.SCTLR.NMFI == 1;

    if( Bits( bytemask, 3, 3 ) == 1 )
    {
        proc.CPSR &= ~(0xF0000000);
        proc.CPSR |= Bits( value, 31, 27 ) << 28;
        
        if( affect_execstate )
        {
            proc.CPSR &= ~(0x0F000000);
            proc.CPSR |= Bits( value, 26, 24 ) << 24;
        }
    }

    if( Bits( bytemask, 2, 2 ) == 1 )
    {
        proc.CPSR &= ~(0x000F0000);
        proc.CPSR |= Bits( value, 19, 16 ) << 16;
    }

    if( Bits( bytemask, 1, 1 ) == 1 )
    {
        if( affect_execstate )
        {
            proc.CPSR &= ~(0xFC00);
            proc.CPSR |= Bits( value, 15 , 10 ) << 10;
        }
        proc.CPSR &= ~(0x200);
        proc.CPSR |= Bits( value, 9, 9 ) << 9;
        
        if( privileged && ( IsSecure( proc ) || proc.SCR.AW == 1 ) )
        {
            proc.CPSR &= ~(0x100);
            proc.CPSR |= Bits( value, 8, 8 ) << 8;
        }
    }

    if( Bits( bytemask, 0, 0 ) == 1 )
    {
        if( privileged )
        {
            proc.CPSR &= ~(0x80);
            proc.CPSR |= Bits( value, 7, 7 ) << 7;
        }

        if( privileged && ( IsSecure( proc ) || proc.SCR.FW == 1 ) &&
            ( !nmfi || Bits( value, 6, 6 ) == 0 ) )
        {
            proc.CPSR &= ~(0x40);
            proc.CPSR |= Bits( value, 6, 6 ) << 6;
        }

        if( affect_execstate )
        {
            proc.CPSR &= ~(0x20);
            proc.CPSR |= Bits( value, 5, 5 ) << 5;
        }

        if( privileged )
        {
            if( BadMode( Bits( value, 4, 0 ) ) )
            {
                // Unpredictable
                return;
            }
            else
            {
                if( !IsSecure( proc ) && Bits( value, 4, 0 ) == 0x16 )
                {
                    // Unpredictable
                    return;
                }

                if( !IsSecure( proc ) && Bits( value, 4, 0 ) == 0x11
                    && proc.NSACR.RFR == 1 )
                {
                    // Unpredictable
                    return;
                }
                proc.CPSR &= ~(0x1F);
                proc.CPSR |= Bits( value, 4, 0 );
            }
        }
    }
}

#endif // __ARMV7_FUNCTION_IMPL_HPP__
