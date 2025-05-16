#pragma once

#include "Delegates/DelegateCombinations.h" // 델리게이트 매크로가 정의된 헤더

//-----------------------------------------------------------------------------------
// 단일 오브젝트 바인딩용

// 기본 델리게이트
DECLARE_DELEGATE(FDelegateSignature)
DECLARE_DELEGATE(FDS_SkillLCooldown)

// 기본 델리게이트에 인자 1개를 전달
DECLARE_DELEGATE_OneParam(FDelegateSignatureParam1, FLinearColor)
// 기본 델리게이트에 인자 2개를 전달
DECLARE_DELEGATE_TwoParams(FDelegateSignatureParam2, FLinearColor, int32)

//-----------------------------------------------------------------------------------
// 멀티 오브젝트 바인딩용

// 멀티캐스트 델리게이트
DECLARE_MULTICAST_DELEGATE(FMulticastDelegateSignature)
// 멀티캐스트 델리게이트에 인자 1개를 전달
DECLARE_MULTICAST_DELEGATE_OneParam(FMulticastDelegateSignature1, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FMDS1, FHitResult)
// 멀티캐스트 델리게이트에 인자 2개를 전달
DECLARE_MULTICAST_DELEGATE_TwoParams(FMulticastDelegateSignature2, FLinearColor, int32)