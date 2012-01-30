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
 * This file implements types defined in the ARM Architecture
 * Reference Manual (ARM v7-A and ARM v7-R edition). All section and
 * page numbers refer to that manual unless otherwise noted.
 */

#ifndef __ARMV7_TYPES_HPP__
#define __ARMV7_TYPES_HPP__

#include <boost/cstdint.hpp>

namespace arm {



    /**
     * Shift register types
     * (A8.4.3, p.323)
     */
    enum SRType {
        SRType_LSL,
        SRType_LSR,
        SRType_ASR,
        SRType_ROR,
        SRType_RRX
    };


    /**
     * Instruction set flavors
     * (A2.5.1, p.48)
     */
    enum InstrSet {
        InstrSet_ARM,
        InstrSet_Thumb,
        InstrSet_Jazelle,
        InstrSet_ThumbEE
    };


    /**    
     * Types of memory architectures
     * (I.7.28, p.2102)
     */
    enum MemArch {
        MemArch_VMSA,
        MemArch_PMSA
    };


    /**
     * Types of memory
     * (B2.4.1, p.1263)
     */
    enum MemType {
        MemType_Normal,
        MemType_Device,
        MemType_StronglyOrdered
    };


    /**
     * Types of TLB entry
     * (B3.13.5, p.1432)
     */
    enum TLBRecType {
        TLBRecType_SmallPage,
        TLBRecType_LargePage,
        TLBRecType_Section,
        TLBRecType_Supersection,
        TLBRecType_MMUDisabled
    };


    /**    
     * Data abort types.
     * (B2.4.10, p.1273)
     */
    enum DAbort {
        DAbort_AccessFlag,
        DAbort_Alignment,
        DAbort_Background,
        DAbort_Domain,
        DAbort_Permission,
        DAbort_Translation
    };


    /**
     * Value-carry tuple often used in the ARMv7 documentation
     */
    struct ValueCarry
    {
        ValueCarry() : value( 0 ), carry( false ) {}
        int32_t value;
        bool    carry;
    };


    /**
     * Unsigned value-carry tuple often used in the ARMv7 documentation
     */
    struct UValueCarry
    {
        UValueCarry() : value( 0 ), carry( false ) {}
        uint32_t value;
        bool     carry;
    };


    /**
     * Value-saturation tuple often used in the ARMv7 documentation
     */
    struct ValueSat
    {
        ValueSat() : value( 0 ), saturated( false ) {}
        int64_t value;
        bool    saturated;
    };

    
    /**
     * Unsigned value-saturation tuple often used in the ARMv7 documentation
     */
    struct UValueSat
    {
        UValueSat() : value( 0 ), saturated( false ) {}
        uint64_t value;
        bool     saturated;
    };
    

    /**
     * Shift type-unsigned value tuple used in immediate shift decoding.
     */
    struct ShiftUValue
    {
        SRType   shift_t;
        uint32_t shift_n;
    };


    /**
     * Memory attributes descriptor
     * (B2.4.1, p.1263)
     */
    struct MemoryAttributes
    {
        MemType type;

        /**
         * ‘00’ = Non-cacheable;
         * ‘01’ = WBWA;
         * ‘10’ = WT;
         * ‘11’ = WBnWA.
         */
        bool innerattrs[2];

        /**
         * ‘00’ = Non-cacheable;
         * ‘01’ = WBWA;
         * ‘10’ = WT;
         * ‘11’ = WBnWA.
         */
        bool outerattrs[2];

        bool shareable;
        bool outershareable;
    };


    /**
     * Physical address type, with extra bits used by some VMSA features
     * (B2.4.1, p.1263)
     */
    struct FullAddress 
    {
        uint32_t physicaladdress;
        uint8_t  physicaladdressext;
        bool     NS; // ‘0’ = Secure, ‘1’ = Non-secure
    };


    /**
     * Descriptor used to access the underlying memory array
     * (B2.4.1, p.1263)
     */
    struct AddressDescriptor
    {
        MemoryAttributes memattrs;
        FullAddress      paddress;
    };


    /**
     * Access permissions descriptor
     * (B2.4.1, p.1264)
     */
    struct Permissions
    {
        uint8_t ap; // Access Permission bits
        bool    xn; // Execute Never bit
    };


    struct TLBRecord
    {
        Permissions perms;
        bool nG; // ‘0’ = Global, ‘1’ = not Global
        int domain;
        bool sectionnotpage;
        TLBRecType type;
        AddressDescriptor addrdesc;
    };


    struct CheckTLBRecord
    {
        bool tlbhit; //false if tblrecord is UNKNOWN
        TLBRecord tlbrecord;
    };



} // namespace arm

#endif // __ARMV7_TYPES_HPP__
