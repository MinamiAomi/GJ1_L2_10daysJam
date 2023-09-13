#pragma once

#include "Particle/Circle.h"
#include "Particle/Follow.h"
#include "Particle/Splash.h"
#include "Particle/Pop.h"
#include "Particle/YenLetter.h"
#include "Particle/VerticalLine.h"

class ParticleManager {
public:
	void Initialize();
	void Update();
	void Draw();

public:
	Circle* GetCircle() { return circle_; }
	Follow* GetFollow() { return follow_; }
	Splash* GetSplash() { return splash_; }
	Pop* GetPop() { return pop_; }
	YenLetter* GetYenLetter() { return yenLetter_; }
	VerticalLine* GetVerticalLine() { return verticleLine_; }
private:
	Circle* circle_;
	Follow* follow_;
	Splash* splash_;
	Pop* pop_;
	YenLetter* yenLetter_;
	VerticalLine* verticleLine_;
};