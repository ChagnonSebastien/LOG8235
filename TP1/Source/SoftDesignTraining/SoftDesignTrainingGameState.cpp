// Fill out your copyright notice in the Description page of Project Settings.


#include "SoftDesignTrainingGameState.h"

void ASoftDesignTrainingGameState::incrementDeaths() {
	this->numberOfDeaths += 1;
}

void ASoftDesignTrainingGameState::incrementCollected() {
	this->numberOfCollected += 1;
}


int ASoftDesignTrainingGameState::getDeaths() {
	return this->numberOfDeaths;
}

int ASoftDesignTrainingGameState::getCollected() {
	return this->numberOfCollected;
}
