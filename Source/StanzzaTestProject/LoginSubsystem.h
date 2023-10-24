// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "LoginSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FLoginData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString AccessToken;

	UPROPERTY(BlueprintReadOnly)
	FString RefreshToken;

	UPROPERTY(BlueprintReadOnly)
	FString ExpiredAt;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuthSuccess, FLoginData, LoginData);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAuthError, FString, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCatalogSuccess, FString, CatalogMessage);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCatalogError, FString, ErrorMessage);

UCLASS()
class STANZZATESTPROJECT_API ULoginSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void Auth(const FString& Login, const FString& Password, const FString& Fingerprint, const FOnAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "SuccessCallback, ErrorCallback"))
	void GetCatalog(const FOnGetCatalogSuccess& SuccessCallback, const FOnGetCatalogError& ErrorCallback);

	UPROPERTY(BlueprintReadOnly)
	FLoginData LoginData;

private:
	void Auth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnAuthSuccess SuccessCallback, FOnAuthError ErrorCallback);
	
	void GetCatalog_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnGetCatalogSuccess SuccessCallback, FOnGetCatalogError ErrorCallback);

};
