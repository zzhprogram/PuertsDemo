/*
 * Tencent is pleased to support the open source community by making Puerts available.
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
 * Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may
 * be subject to their corresponding license terms. This file is subject to the terms and conditions defined in file 'LICENSE',
 * which is part of this source code package.
 */

#include "TypeScriptGeneratedClass.h"
#include "PropertyMacros.h"
#include "JSGeneratedFunction.h"
#include "JSLogger.h"

DEFINE_FUNCTION(UTypeScriptGeneratedClass::execCallJS)
{
    UFunction* Func = Stack.CurrentNativeFunction ? Stack.CurrentNativeFunction : Stack.Node;
    check(Func);
    // UE_LOG(LogTemp, Warning, TEXT("overrided function called, %s(%p)"), *Func->GetName(), Func);

    UTypeScriptGeneratedClass* Class = Cast<UTypeScriptGeneratedClass>(Func->GetOuter());
    if (Class)
    {
        Class->ProcessPendingConstructJob();
        auto PinedDynamicInvoker = Class->DynamicInvoker.Pin();
        if (PinedDynamicInvoker)
        {
            PinedDynamicInvoker->InvokeTsMethod(Context, Func, Stack, RESULT_PARAM);
        }
        else
        {
            UE_LOG(Puerts, Error, TEXT("call %s::%s fail!, DynamicInvoker invalid"), *Class->GetName(), *Func->GetName());
        }
    }
    else
    {
        UE_LOG(Puerts, Error, TEXT("calling a not ts class method %s::%s"), *Func->GetOuter()->GetName(), *Func->GetName());
    }
}

DEFINE_FUNCTION(UTypeScriptGeneratedClass::execLazyLoadCallJS)
{
    UFunction* Function = Stack.CurrentNativeFunction ? Stack.CurrentNativeFunction : Stack.Node;
    check(Function);

    auto Class = Cast<UTypeScriptGeneratedClass>(Function->GetOuterUClassUnchecked());
    if (Class && Class->IsChildOf(UBlueprintFunctionLibrary::StaticClass()))
    {
        auto PinedDynamicInvoker = Class->DynamicInvoker.Pin();
        if (PinedDynamicInvoker)
        {
            PinedDynamicInvoker->NotifyReBind(Class);
            execCallJS(Context, Stack, RESULT_PARAM);
        }
    }
#if WITH_EDITOR
    else
    {
        NotifyRebind(Class);
        execCallJS(Context, Stack, RESULT_PARAM);
    }
#endif
}

#if WITH_EDITOR
void UTypeScriptGeneratedClass::NotifyRebind(UClass* Class)
{
    if (Class->ClassConstructor == &UTypeScriptGeneratedClass::StaticConstructor)
    {
        while (Class)
        {
            if (UTypeScriptGeneratedClass* TsClass = Cast<UTypeScriptGeneratedClass>(Class))
            {
                if (TsClass->NeedReBind && TsClass->DynamicInvoker.IsValid())
                {
                    TsClass->NeedReBind = false;
                    UTypeScriptGeneratedClass* CachedClass = TsClass;
                    Class = Class->GetSuperClass();
                    while (Class)
                    {
                        if (UTypeScriptGeneratedClass* SuperTsClass = Cast<UTypeScriptGeneratedClass>(Class))
                        {
                            SuperTsClass->NeedReBind = false;
                        }
                        Class = Class->GetSuperClass();
                    }
                    CachedClass->DynamicInvoker.Pin()->NotifyReBind(CachedClass);
                }
                return;
            }
            Class = Class->GetSuperClass();
        }
    }
}

void UTypeScriptGeneratedClass::LazyLoadRedirect()
{
    for (TFieldIterator<UFunction> FuncIt(this, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
    {
        auto Function = *FuncIt;
        if (!FunctionToRedirect.Contains(Function->GetFName()))
        {
            continue;
        }
        Function->FunctionFlags |= FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public;
        Function->SetNativeFunc(&UTypeScriptGeneratedClass::execLazyLoadCallJS);
        AddNativeFunction(*Function->GetName(), &UTypeScriptGeneratedClass::execLazyLoadCallJS);
    }
}
#endif

void UTypeScriptGeneratedClass::ProcessPendingConstructJob()
{
    FScopeLock ScopeLock(&PendingConstructJobMutex);
    if (!IsProcessingPendingConstructJob && PendingConstructInfos.Num() > 0)
    {
        IsProcessingPendingConstructJob = true;
        for (int Index = 0; Index < PendingConstructInfos.Num(); Index++)
        {
            PendingConstructInfos[Index].Ref.Reset();
            auto ExecFunction = PendingConstructInfos[Index].Func;
            if (!ExecFunction)
                continue;
            PendingConstructInfos[Index].Func = nullptr;
            ExecFunction();
        }
        PendingConstructInfos.Empty();
        IsProcessingPendingConstructJob = false;
    }
}

void UTypeScriptGeneratedClass::StaticConstructor(const FObjectInitializer& ObjectInitializer)
{
    auto Class = ObjectInitializer.GetClass();

    //????????????ts????????????????????????????????????????????????ts???????????????????????????ts??????????????????????????????????????????????????????UTypeScriptGeneratedClass????????????
    while (Class)
    {
        if (auto TypeScriptGeneratedClass = Cast<UTypeScriptGeneratedClass>(Class))
        {
            TypeScriptGeneratedClass->ObjectInitialize(ObjectInitializer);
            break;
        }
        Class = Class->GetSuperClass();
    }
}

void UTypeScriptGeneratedClass::ObjectInitialize(const FObjectInitializer& ObjectInitializer)
{
    auto Object = ObjectInitializer.GetObj();
    if (auto SuperTypeScriptGeneratedClass = Cast<UTypeScriptGeneratedClass>(GetSuperClass()))
    {
        SuperTypeScriptGeneratedClass->ObjectInitialize(ObjectInitializer);
    }
    else
    {
        GetSuperClass()->ClassConstructor(ObjectInitializer);
    }

#if !WITH_EDITOR
    if (Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
        return;
#endif

#ifdef THREAD_SAFE
    auto PinedDynamicInvoker = DynamicInvoker.Pin();
    if (PinedDynamicInvoker)
    {
        PinedDynamicInvoker->TsConstruct(this, Object);
    }
#else
    auto PinedDynamicInvoker = DynamicInvoker.Pin();
    if (PinedDynamicInvoker)
    {
        if (IsInGameThread())
        {
            ProcessPendingConstructJob();
            PinedDynamicInvoker->TsConstruct(this, Object);
        }
        else
        {
            FScopeLock ScopeLock(&PendingConstructJobMutex);

            TWeakObjectPtr<UTypeScriptGeneratedClass> Class = this;
            TWeakObjectPtr<UObject> Self = Object;
            TSharedPtr<int> Ref(new int(0));
            TWeakPtr<int> WeakRef(Ref);
            int Index = PendingConstructInfos.Num();
            PendingConstructInfos.AddDefaulted();
            PendingConstructJobInfo& Info = PendingConstructInfos[Index];
            Info.Ref = Ref;
            Info.Func = [Class, Self, Index]()
            {
                if (Class.IsValid())
                {
                    FScopeLock ScopeLock(&Class->PendingConstructJobMutex);
                    if (Self.IsValid() && Class->PendingConstructInfos.Num() > Index)
                    {
                        auto PinedDynamicInvoker = Class->DynamicInvoker.Pin();
                        if (PinedDynamicInvoker)
                        {
                            PinedDynamicInvoker->TsConstruct(Class.Get(), Self.Get());
                        }
                        else
                        {
                            UE_LOG(Puerts, Error, TEXT("call delay TsConstruct of %s(%p) fail!, DynamicInvoker invalid"),
                                *Self->GetName(), Self.Get());
                        }
                    }
                    else
                    {
                        UE_LOG(Puerts, Error, TEXT("call delay TsConstruct fail!, Self of %s invalid"), *Class->GetName());
                    }
                }
                else
                {
                    UE_LOG(Puerts, Error, TEXT("call delay TsConstruct fail!, Class invalid"));
                }
            };
            FFunctionGraphTask::CreateAndDispatchWhenReady(
                [Class, Self, Index, WeakRef]()
                {
                    if (Class.IsValid())
                    {
                        if (!WeakRef.IsValid())
                            return;
                        FScopeLock ScopeLock(&Class->PendingConstructJobMutex);
                        if (Class->PendingConstructInfos.Num() > Index && Class->PendingConstructInfos[Index].Func)
                        {
                            auto ExecFunction = Class->PendingConstructInfos[Index].Func;
                            Class->PendingConstructInfos[Index].Func = nullptr;
                            Class->PendingConstructInfos[Index].Ref.Reset();
                            ExecFunction();
                        }
                    }
                    else
                    {
                        UE_LOG(Puerts, Error, TEXT("call delay TsConstruct fail!, Class invalid"));
                    }
                },
                TStatId{}, nullptr, ENamedThreads::GameThread);
        }
    }
#endif
    else
    {
        UE_LOG(Puerts, Error, TEXT("call TsConstruct of %s(%p) fail!, DynamicInvoker invalid"), *Object->GetName(), Object);
    }
}

void UTypeScriptGeneratedClass::RedirectToTypeScript(UFunction* InFunction)
{
    if (!FunctionToRedirect.Contains(InFunction->GetFName()))
    {
        return;
    }
    InFunction->FunctionFlags |= FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public;
    InFunction->SetNativeFunc(&UTypeScriptGeneratedClass::execCallJS);
    AddNativeFunction(*InFunction->GetName(), &UTypeScriptGeneratedClass::execCallJS);
}

void UTypeScriptGeneratedClass::RedirectToTypeScriptFinish()
{
    for (TFieldIterator<UFunction> FuncIt(this, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
    {
        auto Function = *FuncIt;
        if (!FunctionToRedirect.Contains(Function->GetFName()))
        {
            continue;
        }
        Function->FunctionFlags |= FUNC_BlueprintCallable | FUNC_BlueprintEvent | FUNC_Public | FUNC_Native;
    }
}

void UTypeScriptGeneratedClass::CancelRedirection()
{
    for (TFieldIterator<UFunction> FuncIt(this, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
    {
        auto Function = *FuncIt;
        if (!FunctionToRedirect.Contains(Function->GetFName()))
        {
            continue;
        }
        Function->FunctionFlags &= ~FUNC_Native;
        Function->SetNativeFunc(ProcessInternal);
        NativeFunctionLookupTable.RemoveAll(
            [=](const FNativeFunctionLookup& NativeFunctionLookup) { return Function->GetFName() == NativeFunctionLookup.Name; });
    }
}

bool UTypeScriptGeneratedClass::NotSupportInject()
{
    return (GetName().StartsWith("SKEL_") || GetName().StartsWith("REINST_") || GetName().StartsWith("TRASHCLASS_") ||
            GetName().StartsWith("PLACEHOLDER-") || GetName().StartsWith("HOTRELOADED_"));
}

void UTypeScriptGeneratedClass::Bind()
{
    if (GetName().StartsWith("REINST_"))
    {
        for (TFieldIterator<UFunction> FuncIt(this, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
        {
            auto Function = *FuncIt;
            Function->FunctionFlags &= ~FUNC_Native;
        }
    }

    Super::Bind();

    if (NotSupportInject())
    {
        return;
    }

    if (HasConstructor)
    {
        //??????????????????CDO????????????CDO????????????AR?????????????????????UBlueprintGeneratedClass::SerializeDefaultObject????????????
        //??????TS?????????????????????????????????????????????????????????????????????????????????????????????CPF_SkipSerialization???????????????????????????
        //?????????????????????????????????PostConstructInit?????????????????????????????????CDO?????????????????????ps???UE???????????????????????????????????????
        //?????????????????????CDO????????????????????????????????????flag?????????CPF_Transient | CPF_InstancedReference????????????
        // TODO: ???????????????TypeScript??????????????????UBlueprintGeneratedClass????????????????????????????????????
        for (TFieldIterator<PropertyMacro> PropertyIt(this, EFieldIteratorFlags::ExcludeSuper); PropertyIt; ++PropertyIt)
        {
            PropertyMacro* Property = *PropertyIt;
            Property->SetPropertyFlags(CPF_SkipSerialization | CPF_Transient | CPF_InstancedReference);
        }

        //????????????CDO??????PostConstructInit??????????????????
        // ClassFlags |= CLASS_Native;
    }
#if WITH_EDITOR
    if (DynamicInvoker.IsValid())
#endif
    {
        ClassConstructor = &StaticConstructor;
    }
}
