// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
//*****************************************************************************
// File: CordbRegisterSet.cpp
// 

//
//*****************************************************************************
#include "primitives.h"


HRESULT CordbRegisterSet::GetRegistersAvailable(ULONG64* pAvailable)
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT(pAvailable, ULONG64 *);

    *pAvailable = SETBITULONG64(REGISTER_ARM64_PC)
                | SETBITULONG64(REGISTER_ARM64_SP)
                | SETBITULONG64(REGISTER_ARM64_X0)
                | SETBITULONG64(REGISTER_ARM64_X1)
                | SETBITULONG64(REGISTER_ARM64_X2)
                | SETBITULONG64(REGISTER_ARM64_X3)
                | SETBITULONG64(REGISTER_ARM64_X4)
                | SETBITULONG64(REGISTER_ARM64_X5)
                | SETBITULONG64(REGISTER_ARM64_X6)
                | SETBITULONG64(REGISTER_ARM64_X7)
                | SETBITULONG64(REGISTER_ARM64_X8)
                | SETBITULONG64(REGISTER_ARM64_X9)
                | SETBITULONG64(REGISTER_ARM64_X10)
                | SETBITULONG64(REGISTER_ARM64_X11)
                | SETBITULONG64(REGISTER_ARM64_X12)
                | SETBITULONG64(REGISTER_ARM64_X13)
                | SETBITULONG64(REGISTER_ARM64_X14)
                | SETBITULONG64(REGISTER_ARM64_X15)
                | SETBITULONG64(REGISTER_ARM64_X16)
                | SETBITULONG64(REGISTER_ARM64_X17)
                | SETBITULONG64(REGISTER_ARM64_X18)
                | SETBITULONG64(REGISTER_ARM64_X19)
                | SETBITULONG64(REGISTER_ARM64_X20)
                | SETBITULONG64(REGISTER_ARM64_X21)
                | SETBITULONG64(REGISTER_ARM64_X22)
                | SETBITULONG64(REGISTER_ARM64_X23)
                | SETBITULONG64(REGISTER_ARM64_X24)
                | SETBITULONG64(REGISTER_ARM64_X25)
                | SETBITULONG64(REGISTER_ARM64_X26)
                | SETBITULONG64(REGISTER_ARM64_X27)
                | SETBITULONG64(REGISTER_ARM64_X28)
                | SETBITULONG64(REGISTER_ARM64_FP)
                | SETBITULONG64(REGISTER_ARM64_LR);

    return S_OK;
}

HRESULT CordbRegisterSet::GetRegisters(ULONG64 mask, ULONG32 regCount,
                                       CORDB_REGISTER regBuffer[])
{ 
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    ATT_REQUIRE_STOPPED_MAY_FAIL(GetProcess());

    UINT iRegister = 0;

    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);
    
    // @ARM64TODO: floating point support

    for (int i = REGISTER_ARM64_X0;
         i <= REGISTER_ARM64_PC && iRegister < regCount;
         i++)
    {
        if (mask &  SETBITULONG64(i))
        {
            if ((i >= REGISTER_ARM64_X0) && (i <= REGISTER_ARM64_X28))
            {
                regBuffer[iRegister++] = m_rd->X[i - REGISTER_ARM64_X0];
                continue;
            }

            switch (i)
            {
            case REGISTER_ARM64_PC: 
                regBuffer[iRegister++] = m_rd->PC; break;
            case REGISTER_ARM64_SP:
                regBuffer[iRegister++] = m_rd->SP; break;
            case REGISTER_ARM64_FP:
                regBuffer[iRegister++] = m_rd->FP; break;
            case REGISTER_ARM64_LR:
                regBuffer[iRegister++] = m_rd->LR; break;
            default:
                _ASSERTE(false); break;
            }
        }
    }

    _ASSERTE (iRegister <= regCount);
    return S_OK;
}


HRESULT CordbRegisterSet::GetRegistersAvailable(ULONG32 regCount, 
                                                BYTE    pAvailable[])
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(pAvailable, CORDB_REGISTER, regCount, true, true);
    
    // Defer to adapter for v1.0 interface
    return GetRegistersAvailableAdapter(regCount, pAvailable);
}


HRESULT CordbRegisterSet::GetRegisters(ULONG32 maskCount, BYTE mask[], 
                                       ULONG32 regCount, CORDB_REGISTER regBuffer[])
{
    FAIL_IF_NEUTERED(this);
    VALIDATE_POINTER_TO_OBJECT_ARRAY(regBuffer, CORDB_REGISTER, regCount, true, true);

    // Defer to adapter for v1.0 interface
    return GetRegistersAdapter(maskCount, mask, regCount, regBuffer);
}


// This is just a convenience function to convert a regdisplay into a Context.
// Since a context has more info than a regdisplay, the conversion isn't perfect
// and the context can't be fully accurate.
void CordbRegisterSet::InternalCopyRDToContext(DT_CONTEXT *pInputContext)
{    INTERNAL_SYNC_API_ENTRY(GetProcess());
    _ASSERTE(pInputContext);

    if ((pInputContext->ContextFlags & DT_CONTEXT_INTEGER) == DT_CONTEXT_INTEGER)
    {
        for (int i = 0 ; i < 29 ; ++i)
        {
            pInputContext->X[i] = m_rd->X[i];
        }
    }

    if ((pInputContext->ContextFlags & DT_CONTEXT_CONTROL) == DT_CONTEXT_CONTROL)
    {
        pInputContext->Sp = m_rd->SP;
        pInputContext->Lr = m_rd->LR;
        pInputContext->Pc = m_rd->PC;
        pInputContext->Fp = m_rd->FP;
    }
}
