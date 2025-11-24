#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Types/StructTypes.h"
#include "GASPCharacterMovementComponent.generated.h"

class GASP_API FGASPCharacterNetworkMoveData final : public FCharacterNetworkMoveData
{
	using Super = FCharacterNetworkMoveData;

public:
	// Flags
	uint8 bRotationModeUpdate : 1;

	FGameplayTag RotationMode{FGameplayTag::EmptyTag};
	FGameplayTag AllowedGait{FGameplayTag::EmptyTag};
	FGameplayTag StanceMode{FGameplayTag::EmptyTag};

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& Move, ENetworkMoveType MoveType) override;

	virtual bool Serialize(UCharacterMovementComponent& Movement, FArchive& Archive, UPackageMap* Map,
	                       ENetworkMoveType MoveType) override;
};

class GASP_API FGASPCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:
	TStaticArray<FGASPCharacterNetworkMoveData, 3> MoveData;

	FGASPCharacterNetworkMoveDataContainer();
};

class GASP_API FGASPSavedMove final : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	// Flags
	uint8 bRotationModeUpdate : 1;

	FGameplayTag RotationMode{FGameplayTag::EmptyTag};
	FGameplayTag AllowedGait{FGameplayTag::EmptyTag};
	FGameplayTag StanceMode{FGameplayTag::EmptyTag};

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	                            float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, const FVector& NewAccel,
	                        FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC,
	                         const FVector& OldStartLocation) override;
};

class GASP_API FNetworkPredictionData_Client_Base : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_Base(const UCharacterMovementComponent& ClientMovement);

	using Super = FNetworkPredictionData_Client_Character;

	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 *
 */
UCLASS()
class GASP_API UGASPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	friend FGASPSavedMove;

	FGaitSettings GaitSettings;

protected:
	FGASPCharacterNetworkMoveDataContainer MoveDataContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InAirRotationYaw{200.f};

	FGameplayTag RotationMode{FGameplayTag::EmptyTag};
	FGameplayTag AllowedGait{FGameplayTag::EmptyTag};
	FGameplayTag StanceMode{FGameplayTag::EmptyTag};
	bool bRotationModeUpdate{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, FGaitSettings> MovementSettings;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual float GetMappedSpeed() const;

	virtual void RefreshGaitSettings();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpeedMultiplier{1.f};

	UGASPCharacterMovementComponent();

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysNavWalking(float DeltaTime, int32 Iterations) override;
	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	virtual void MoveSmooth(const FVector& InVelocity, const float DeltaSeconds,
	                        FStepDownResult* OutStepDownResult = 0) override;
	virtual void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags,
	                            const FVector& NewAccel) override;

	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool HasMovementInputVector() const;
	virtual void UpdateRotationMode();

	virtual bool IsInAir() const;
	
	void SetGait(const FGameplayTag& NewGait);

	void SetStance(const FGameplayTag& NewStance);

	void RefreshGroundedSettings();

	void SetRotationMode(const FGameplayTag& NewRotationMode);

	FORCEINLINE void SetMovementSettings(const FGameplayTag& Key, const FGaitSettings& NewSettings)
	{
		MovementSettings.Add({Key, NewSettings});

		RefreshGaitSettings();
	}

	FORCEINLINE const FGameplayTag& GetGait() const
	{
		return AllowedGait;
	}

	FORCEINLINE const FGameplayTag& GetRotationMode() const
	{
		return RotationMode;
	}

	FORCEINLINE FGaitSettings GetGaitSettings() const
	{
		return GaitSettings;
	}
};
