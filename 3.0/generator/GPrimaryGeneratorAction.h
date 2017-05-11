#ifndef GPRIMARYGENERATORACTION_H
#define GPRIMARYGENERATORACTION_H 1

// geant4
#include "G4VUserPrimaryGeneratorAction.hh"

// to be removed?
#include "G4ParticleGun.hh"

class GPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	GPrimaryGeneratorAction();
	~GPrimaryGeneratorAction();

public:
	virtual void GeneratePrimaries(G4Event*);

private:
	G4ParticleGun* fParticleGun;

};


#endif
