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

#include "function.hpp"
#include "function_impl.hpp"
#include "types.hpp"

#include <boost/cstdint.hpp>
#include <cassert>
#include <cmath>



uint32_t arm::Bits( uint32_t s, uint32_t b1, uint32_t b0 )
{
    assert( b1 >= b0 );
    assert( b1 <= 31 );
    return ( s & ( 0xFFFFFFFF >> ( 31 - b1 ) ) ) >> b0;
}


uint64_t arm::Bits64( uint64_t s, uint64_t b1, uint64_t b0 )
{
    assert( b1 >= b0 );
    assert( b1 <= 63 );
    return ( s & ( 0xFFFFFFFFFFFFFFFFLL >> ( 63 - b1 ) ) ) >> b0;
}


arm::UValueCarry arm::ARMExpandImm_C( uint32_t imm12, bool carry_in )
{
    uint64_t unrotated_value = ZeroExtend( (uint64_t)( Bits( imm12, 7, 0 ) ) );
    UValueCarry value = Shift_C( (uint32_t) unrotated_value, SRType_ROR,
                                 2*UInt( Bits( imm12, 11, 8 ) ), carry_in );
    return value;
}


int arm::CurrentCond( uint32_t instr )
{
    return Bits( instr, 31, 28 );
}


arm::ShiftUValue arm::DecodeImmShift( uint32_t type, uint32_t imm5 )
{
    SRType   shift_t;
    uint32_t shift_n;

    switch ( type )
    {
    case 0x00000000:
        shift_t = SRType_LSL;
        shift_n = UInt( imm5 );
        break;

    case 0x00000001:
        shift_t = SRType_LSR;
        if ( imm5 == 0x00000000 )
        {
            shift_n = 32;
        }
        else
        {
            shift_n = UInt( imm5 );
        }
        break;

    case 0x00000002:
        shift_t = SRType_ASR;
        if ( imm5 == 0x00000000 )
        {
            shift_n = 32;
        }
        else
        {
            shift_n = UInt( imm5 );
        }
        break;

    case 0x00000003:
        if ( imm5 == 0x00000000 )
        {
            shift_t = SRType_RRX;
            shift_n = 1;
        }
        else
        {
            shift_t = SRType_ROR;
            shift_n = UInt( imm5 );
        }
        break;

    default:
        assert( false && "unknown shift type" );
    };

    ShiftUValue result;
    result.shift_t = shift_t;
    result.shift_n = shift_n;
    return result;
}


arm::SRType arm::DecodeRegShift( uint32_t type )
{
    SRType shift_t;

    switch ( type )
    {
    case 0x00000000: shift_t = SRType_LSL; break;
    case 0x00000001: shift_t = SRType_LSR; break;
    case 0x00000002: shift_t = SRType_ASR; break;
    case 0x00000003: shift_t = SRType_ROR; break;
    default: assert( false && "unknown shift type" );
    };

    return shift_t;
}


bool arm::HaveSecurityExt()
{
    return false;
}


bool arm::HaveMPExt()
{
    return false;
}


int arm::BitCount( uint64_t bitstring )
{
    int count = 0;

    // Use GCC intrinsincs if available
#ifdef __GNUC__ 
    count = __builtin_popcount( (unsigned int)(bitstring) );
    count += __builtin_popcount( (unsigned int)(bitstring >> 32) );
    return count;
#endif
    
    // Naive algorithm
    uint64_t mask = 0x1;
    for ( int i = 0; i < 64; ++i )
    {
        count += ( bitstring & ( mask << i ) ) != 0 ? 1 : 0;
    }
    return count;
}


bool arm::IsZeroBit( uint64_t bitstring )
{
    return bitstring == 0;
}


bool arm::IsZero( uint64_t x )
{
    return (BitCount(x) == 0);
}


/* 
 * Shift operations are performed on 32 bits, if 64 bit versions are
 * necessary, they can be added here.
 */

// ARM's "pseudo-C" implementations of "_C" shift functions are not
// closely followed here since they make use of a lot of non-C
// constructs.

arm::UValueCarry arm::LSL_C( uint32_t value, int amount )
{
    assert( amount != 0 );

    const uint32_t highestBitMask = 0x80000000;
    UValueCarry result;
    result.value = value << ( amount - 1 );
    result.carry = ( result.value & highestBitMask ) != 0;
    result.value = result.value << 1;
    return result;
}


arm::UValueCarry arm::LSR_C( uint32_t value, int amount )
{
    assert( amount != 0 );

    const uint32_t lowestBitMask = 0x1;
    UValueCarry result;
    result.value = value >> ( amount - 1 );
    result.carry = ( result.value & lowestBitMask ) != 0;
    result.value = result.value >> 1;
    return result;
}
    

arm::UValueCarry arm::ASR_C( uint32_t value, int amount )
{
    assert( amount != 0 );

    UValueCarry result;
    bool signBit = Bits( value, 31, 31 ) == 1;

    // Build a mask to set all "new" leftmost bits to the initial sign
    // bit value.
    uint32_t shiftMask = 0;
    for ( int i = 0; i < amount; ++i )
    {
        shiftMask |= ( 0x80000000 >> i );
    }

    shiftMask = signBit ? shiftMask : (~shiftMask);

    result.value = value >> ( amount - 1 );

    result.carry = Bits( result.value, 0, 0 ) == 1;
    result.value = result.value >> 1;

    if( signBit )
    {
        result.value |= shiftMask;
    }

    return result;
}


arm::UValueCarry arm::ROR_C( uint32_t value, int amount )
{
    assert( amount != 0 );

    const uint32_t N = sizeof( value ) * 8;
    const uint32_t m = amount % N;

    UValueCarry result;
    result.value = LSR( value, m ) | LSL ( value, N - m );
    result.carry = Bits( result.value, N-1, N-1 );

    return result;
}


arm::UValueCarry arm::RRX_C( uint32_t value, bool carry_in )
{
    const uint32_t lowestBitMask = 0x1;
    const uint32_t highestBitMask = 0x80000000;

    UValueCarry result;
    result.carry = ( lowestBitMask & value ) != 0;
    result.value = value >> 1;

    if ( carry_in )
    {
        result.value |= highestBitMask;
    }
                     
    return result;
}


arm::UValueCarry arm::Shift_C( uint32_t value, SRType type,
                               int amount, bool carry_in )
{
    assert( !( type == SRType_RRX && amount != 1) && (amount >= 0) );

    if( amount == 0 )
    {
        UValueCarry result;
        result.value = value;
        return result;
    }
        
    switch ( type )
    {
    case SRType_LSL:
        return LSL_C( value, amount );
    case SRType_LSR:
        return LSR_C( value, amount );
    case SRType_ASR:
        return ASR_C( value, amount );
    case SRType_ROR:
        return ROR_C( value, amount );
    case SRType_RRX:
        return RRX_C( value, carry_in );
    default:
        assert( "Invalid shift register type in shift operation" && false );
    }
}


uint32_t arm::Shift( uint32_t value, SRType type, int amount, bool carry_in )
{
    return ( Shift_C( value, type, amount, carry_in ) ).value;
}


uint32_t arm::LSL( uint32_t value, int amount )
{
    assert ( amount >= 0 );
    if ( amount == 0 )
    {
        return value;
    }
    else
    {
        return ( LSL_C( value, amount ) ).value;
    }
}


uint32_t arm::LSR( uint32_t value, int amount )
{
    assert ( amount >= 0 );
    if ( amount == 0 )
    {
        return value;
    }
    else
    {
        return ( LSR_C( value, amount ) ).value;
    }
}


uint32_t arm::ASR( uint32_t value, int amount )
{
    assert ( amount >= 0 );
    if ( amount == 0 )
    {
        return value;
    }
    else
    {
        return ( ASR_C( value, amount ) ).value;
    }
}


uint32_t arm::ROR( uint32_t value, int amount )
{
    assert ( amount >= 0 );
    if ( amount == 0 )
    {
        return value;
    }
    else
    {
        return ( ROR_C( value, amount ) ).value;
    }
}


uint32_t arm::RRX( uint32_t value, bool carry_in )
{
    return ( RRX_C( value, carry_in ) ).value;
}


uint32_t arm::Align( uint32_t value, uint32_t align )
{
    assert( align != 0 && ( align % 2 == 0 ) && "Invalid align value" );

    int n = (int)(log2(align));
    uint32_t mask = 0xFFFFFFFF << n;
    return value & mask;
}


arm::ValueSat arm::SignedSatQ( int64_t i, unsigned int N )
{
    assert( N < 64 && "Saturation of 64+ bit values is not supported" );
    int64_t maxValue = pow( 2, N-1 );

    ValueSat result;

    if ( i > maxValue - 1 )
    {
        result.value = maxValue - 1;
        result.saturated = true;
    }
    else if ( i < -maxValue )
    {
        result.value = -maxValue;
        result.saturated = true;
    }
    else
    {
        result.value = i;
    }
    return result;
}


arm::UValueSat arm::UnsignedSatQ( int64_t i, unsigned int N )
{
    assert( N < 63 &&
            "Unsigned saturation of 63+ bit values is not supported" );
    int64_t maxValue = pow( 2, N ) - 1;

    UValueSat result;

    if ( i > maxValue )
    {
        result.value = (uint64_t)maxValue;
        result.saturated = true;
    }
    else if ( i < 0 )
    {
        result.value = (uint64_t)0;
        result.saturated = true;
    }
    else
    {
        result.value = (uint64_t)i;
    }
    return result;
}


int64_t arm::SignedSat( int64_t i, unsigned int N )
{
    return ( SignedSatQ( i, N ) ).value;
}
    

uint64_t arm::UnsignedSat( int64_t i, unsigned int N )
{
    return ( UnsignedSatQ( i, N ) ).value;
}

// While uint => uint conversion is a no-op, int => uint conversion
// must be done before any sign extension happens (such as when
// converting a signed value to a wider data type).
uint8_t  arm::UInt( int8_t   x ) { return static_cast< uint8_t > ( x ); }
uint8_t  arm::UInt( uint8_t  x ) { return static_cast< uint8_t > ( x ); }
uint16_t arm::UInt( int16_t  x ) { return static_cast< uint16_t >( x ); }
uint16_t arm::UInt( uint16_t x ) { return static_cast< uint16_t >( x ); }
uint32_t arm::UInt( int32_t  x ) { return static_cast< uint32_t >( x ); }
uint32_t arm::UInt( uint32_t x ) { return static_cast< uint32_t >( x ); }
uint64_t arm::UInt( int64_t  x ) { return static_cast< uint64_t >( x ); }
uint64_t arm::UInt( uint64_t x ) { return static_cast< uint64_t >( x ); }

int8_t  arm::SInt( int8_t   x ) { return static_cast< int8_t > ( x ); }
int8_t  arm::SInt( uint8_t  x ) { return static_cast< int8_t > ( x ); }
int16_t arm::SInt( int16_t  x ) { return static_cast< int16_t >( x ); }
int16_t arm::SInt( uint16_t x ) { return static_cast< int16_t >( x ); }
int32_t arm::SInt( int32_t  x ) { return static_cast< int32_t >( x ); }
int32_t arm::SInt( uint32_t x ) { return static_cast< int32_t >( x ); }
int64_t arm::SInt( int64_t  x ) { return static_cast< int64_t >( x ); }
int64_t arm::SInt( uint64_t x ) { return static_cast< int64_t >( x ); }


int arm::LowestSetBit( unsigned int x )
{
    int N = sizeof(x) * 8;
    unsigned int toCompare = 0x1;
        
    for (int i = 0; i < N; ++i)
    {
        if( (x & (toCompare << i)) !=0 )
        {
            return i;
        }
    }
        
    return N;
}


int arm::ArchVersion()
{
    return 7;
}

    
arm::MemArch arm::MemorySystemArchitecture()
{
    // ARMv7-A, the A profile, requires the inclusion of a Virtual
    // Memory System Architecture (VMSA)
    // (B2.1.1, p.1236)
        
    return MemArch_VMSA;
}

// In C++, unsigned values are automatically zero-extended. Therefore,
// ZeroExtend() does the same as UInt().
uint8_t  arm::ZeroExtend( uint8_t  bitstring ) { return UInt( bitstring ); }
uint8_t  arm::ZeroExtend( int8_t   bitstring ) { return UInt( bitstring ); }
uint16_t arm::ZeroExtend( uint16_t bitstring ) { return UInt( bitstring ); }
uint16_t arm::ZeroExtend( int16_t  bitstring ) { return UInt( bitstring ); }
uint32_t arm::ZeroExtend( uint32_t bitstring ) { return UInt( bitstring ); }
uint32_t arm::ZeroExtend( int32_t  bitstring ) { return UInt( bitstring ); }
uint64_t arm::ZeroExtend( uint64_t bitstring ) { return UInt( bitstring ); }
uint64_t arm::ZeroExtend( int64_t  bitstring ) { return UInt( bitstring ); }


uint64_t arm::SignExtend( uint64_t bitstring, int newSize, int oldSize  )
{
    assert( newSize <= 64 && "More than 64 bit values is not supported" );
    assert( newSize >= oldSize && "SignExtend error : (oldSize > newSize)." );
    assert( oldSize > 0 && "SignExtend error : Minimum size is 1." );

    if( newSize == oldSize )
    {
        return bitstring;
    }

    unsigned int topBit = bitstring >> (oldSize - 1);
    if( (topBit & 0x1) == 0 )
    {
        return bitstring;
    }

    // from here topBit is '1'.
    uint64_t result = bitstring;
    unsigned int d = newSize - oldSize;
    for( unsigned int i = 0; i < d; ++i )
    {
        result |= (1 << (oldSize + i));
    }

    return result;
}


bool arm::UnalignedSupport()
{
    return true;
}

uint32_t arm::CountLeadingZeroBits(uint32_t x)
{
    // http://hackers-delight.org.ua/040.htm
    uint32_t n;

    if (x == 0) return(32);

    n = 1;

    if ((x >> 16) == 0) {n = n +16; x = x <<16;}
    if ((x >> 24) == 0) {n = n + 8; x = x << 8;}
    if ((x >> 28) == 0) {n = n + 4; x = x << 4;}
    if ((x >> 30) == 0) {n = n + 2; x = x << 2;}

    n = n - (x >> 31);

    return n;
}

