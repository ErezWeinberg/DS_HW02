#ifndef RACENION26B2_H_
#define RACENION26B2_H_

#include "../wet2util.h"
#include <unordered_map>
#include <vector>
#include <set>

struct RefContestant {
    int id;
    Skill skill;
    int missionsHad;
};

struct RefTeam {
    int id;
    int teamExp;
    int totalMotivation;
    std::vector<int> members;
};

class Racenion {
private:
    std::unordered_map<int, RefTeam> teams;
    std::unordered_map<int, RefContestant> all_contestants;
    std::unordered_map<int, int> contestant_team;

public:
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
};

#endif // RACENION26B2_H_
