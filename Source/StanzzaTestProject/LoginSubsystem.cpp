// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginSubsystem.h"
#include "Http.h"

void ULoginSubsystem::Auth(const FString& Login, const FString& Password, const FString& Fingerprint, const FOnAuthSuccess& SuccessCallback, const FOnAuthError& ErrorCallback)
{
	LoginData = FLoginData();

	TSharedPtr<FJsonObject> RequestDataJson = MakeShareable(new FJsonObject());
	RequestDataJson->SetStringField(TEXT("login"), Login);
	RequestDataJson->SetStringField(TEXT("password"), Password);
	RequestDataJson->SetStringField(TEXT("fingerprint"), Fingerprint);

	FString PostContent;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PostContent);
	FJsonSerializer::Serialize(RequestDataJson.ToSharedRef(), Writer);

	const FString Url = TEXT("https://stanzza-api.aicrobotics.ru/api/auth/login");

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetContentAsString(PostContent);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginSubsystem::Auth_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();

}

void ULoginSubsystem::GetCatalog(const FOnGetCatalogSuccess& SuccessCallback, const FOnGetCatalogError& ErrorCallback)
{
	const FString Url = TEXT("https://stanzza-api.aicrobotics.ru/api/v1/catalog/tree");

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *LoginData.AccessToken));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ULoginSubsystem::GetCatalog_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void ULoginSubsystem::Auth_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnAuthSuccess SuccessCallback, FOnAuthError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString ResponseString = HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			static const FString AccessTokenFieldName = TEXT("access_token");
			static const FString RefreshTokenFieldName = TEXT("refresh_token");
			static const FString ExpiredAtFieldName = TEXT("expired_at");
			if (JsonObject->HasTypedField<EJson::String>(AccessTokenFieldName))
			{
				LoginData.AccessToken = JsonObject->GetStringField(AccessTokenFieldName);
				LoginData.RefreshToken = JsonObject->GetStringField(RefreshTokenFieldName);
				LoginData.ExpiredAt = JsonObject->GetStringField(ExpiredAtFieldName);

				SuccessCallback.ExecuteIfBound(LoginData);
			}
			else
			{
				ErrorCallback.ExecuteIfBound(TEXT("Response json doesn't have access_token field"));
			}

		}
		else
		{
			ErrorCallback.ExecuteIfBound(TEXT("Cannot deserialize json"));
		}
	}
	else
	{
		ErrorCallback.ExecuteIfBound(TEXT("Something went wrong"));
	}
}

void ULoginSubsystem::GetCatalog_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, const bool bSucceeded, FOnGetCatalogSuccess SuccessCallback, FOnGetCatalogError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString ResponseString = HttpResponse->GetContentAsString();
		SuccessCallback.ExecuteIfBound(ResponseString);

		/*TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			SuccessCallback.ExecuteIfBound(ResponseString);
		}
		else
		{
			ErrorCallback.ExecuteIfBound(FString::Printf(TEXT("Cannot deserialize json. ResponseString = %s"), *ResponseString));
		}*/
	}
	else
	{
		ErrorCallback.ExecuteIfBound(TEXT("Something went wrong"));
	}
}
