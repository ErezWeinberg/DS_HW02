//
// 234218 Data Structures 1.
// Semester: 2026 B (Spring).
// Wet Exercise #2 - Racenion.
//
// The following header file contains all methods we expect you to implement.
// You MAY add private methods and fields of your own.
// DO NOT erase or modify the signatures of the public methods.
// DO NOT modify the preprocessors in this file.
// DO NOT use the preprocessors in your other code files.
//

#ifndef RACENION26B2_H_
#define RACENION26B2_H_
#include "wet2util.h"


class Racenion {
private:
	//
	// Here you may add anything you need to implement your Racenion class
	//

public:
	// <DO-NOT-MODIFY> {

	Racenion();
	virtual ~Racenion();

	StatusType add_team(int teamId);

	StatusType remove_team(int teamId);

	StatusType add_contestant(int contestantId,
	                          int teamId,
	                          const Skill &skill,
	                          int motivation,
	                          int missionsHad);

	output_t<int> duel(int teamId1, int teamId2);

	output_t<int> get_contestant_missions_number(int contestantId);

	output_t<int> get_team_experience(int teamId);

	output_t<int> get_ith_collective_motivation_team(int i);

	output_t<Skill> get_partial_team_skill(int contestantId);

	StatusType recruit(int recruitingTeamId, int recruitedTeamId);

	// } </DO-NOT-MODIFY>
};

#endif // RACENION26B2_H_
