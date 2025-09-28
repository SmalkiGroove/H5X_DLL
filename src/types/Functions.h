#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Artifacts.h"


typedef int(__thiscall* CountEquippedArtifacts)(int*, ARTIFACT_ID);
extern CountEquippedArtifacts count_equipped_artifact;


#endif
