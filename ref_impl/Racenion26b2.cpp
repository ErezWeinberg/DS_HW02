#include "Racenion26b2.h"
#include <algorithm>

Racenion::Racenion() {}

Racenion::~Racenion() {}

StatusType Racenion::add_team(int teamId) {
    if (teamId <= 0) return StatusType::INVALID_INPUT;
    if (teams.find(teamId) != teams.end()) return StatusType::FAILURE;
    teams[teamId] = {teamId, 0, 0, {}};
    return StatusType::SUCCESS;
}

StatusType Racenion::remove_team(int teamId) {
    if (teamId <= 0) return StatusType::INVALID_INPUT;
    if (teams.find(teamId) == teams.end()) return StatusType::FAILURE;
    teams.erase(teamId);
    return StatusType::SUCCESS;
}

StatusType Racenion::add_contestant(int contestantId,
                                    int teamId,
                                    const Skill &skill,
                                    int motivation,
                                    int missionsHad)
{
    if (contestantId <= 0 || teamId <= 0 || !skill.isValid() || motivation < 0 || missionsHad < 0) {
        return StatusType::INVALID_INPUT;
    }
    if (all_contestants.find(contestantId) != all_contestants.end()) return StatusType::FAILURE;
    if (teams.find(teamId) == teams.end()) return StatusType::FAILURE;
    
    RefContestant c = {contestantId, skill, missionsHad};
    all_contestants[contestantId] = c;
    contestant_team[contestantId] = teamId;
    teams[teamId].members.push_back(contestantId);
    teams[teamId].totalMotivation += motivation;
    
    return StatusType::SUCCESS;
}

output_t<int> Racenion::duel(int teamId1, int teamId2) {
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) return output_t<int>(StatusType::INVALID_INPUT);
    if (teams.find(teamId1) == teams.end() || teams.find(teamId2) == teams.end()) return output_t<int>(StatusType::FAILURE);
    
    RefTeam& t1 = teams[teamId1];
    RefTeam& t2 = teams[teamId2];
    
    if (t1.members.empty() || t2.members.empty()) return output_t<int>(StatusType::FAILURE);
    
    int score1 = t1.teamExp + t1.totalMotivation;
    int score2 = t2.teamExp + t2.totalMotivation;
    
    int win_res = 0;
    bool t1_won = false, t2_won = false;
    
    if (score1 > score2) {
        win_res = 1; t1_won = true;
    } else if (score1 < score2) {
        win_res = 3; t2_won = true;
    } else {
        Skill s1 = Skill::identity();
        for (int id : t1.members) s1 = s1 * all_contestants[id].skill;
        Skill s2 = Skill::identity();
        for (int id : t2.members) s2 = s2 * all_contestants[id].skill;
        
        int eff1 = s1.getEffectiveSkill();
        int eff2 = s2.getEffectiveSkill();
        
        if (eff1 > eff2) { win_res = 2; t1_won = true; }
        else if (eff1 < eff2) { win_res = 4; t2_won = true; }
        else win_res = 0;
    }
    
    if (t1_won) t1.teamExp += 3;
    else if (t2_won) t2.teamExp += 3;
    else { t1.teamExp += 1; t2.teamExp += 1; }
    
    for (int id : t1.members) all_contestants[id].missionsHad += 1;
    for (int id : t2.members) all_contestants[id].missionsHad += 1;
    
    return output_t<int>(win_res);
}

output_t<int> Racenion::get_contestant_missions_number(int contestantId) {
    if (contestantId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    if (all_contestants.find(contestantId) == all_contestants.end()) return output_t<int>(StatusType::FAILURE);
    
    return output_t<int>(all_contestants[contestantId].missionsHad);
}

output_t<int> Racenion::get_team_experience(int teamId) {
    if (teamId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    if (teams.find(teamId) == teams.end()) return output_t<int>(StatusType::FAILURE);
    
    return output_t<int>(teams[teamId].teamExp);
}

output_t<int> Racenion::get_ith_collective_motivation_team(int i) {
    if (teams.empty() || i < 1 || i > (int)teams.size()) return output_t<int>(StatusType::FAILURE);
    
    std::vector<std::pair<int, int>> sorted_teams;
    for (auto& pair : teams) {
        sorted_teams.push_back({pair.second.totalMotivation, pair.first});
    }
    std::sort(sorted_teams.begin(), sorted_teams.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        if (a.first != b.first) return a.first < b.first;
        return a.second < b.second;
    });
    
    return output_t<int>(sorted_teams[i-1].second);
}

output_t<Skill> Racenion::get_partial_team_skill(int contestantId) {
    if (contestantId <= 0) return output_t<Skill>(StatusType::INVALID_INPUT);
    if (all_contestants.find(contestantId) == all_contestants.end()) return output_t<Skill>(StatusType::FAILURE);
    
    int t_id = contestant_team[contestantId];
    if (teams.find(t_id) == teams.end()) return output_t<Skill>(StatusType::FAILURE);
    
    Skill res = Skill::identity();
    for (int id : teams[t_id].members) {
        res = res * all_contestants[id].skill;
        if (id == contestantId) return output_t<Skill>(res);
    }
    return output_t<Skill>(StatusType::FAILURE);
}

StatusType Racenion::recruit(int recruitingTeamId, int recruitedTeamId) {
    if (recruitingTeamId <= 0 || recruitedTeamId <= 0 || recruitingTeamId == recruitedTeamId) return StatusType::INVALID_INPUT;
    if (teams.find(recruitingTeamId) == teams.end() || teams.find(recruitedTeamId) == teams.end()) return StatusType::FAILURE;
    
    RefTeam& A = teams[recruitingTeamId];
    RefTeam& B = teams[recruitedTeamId];
    
    if (A.members.empty()) return StatusType::FAILURE;
    
    if (!B.members.empty()) {
        Skill sA = Skill::identity();
        for (int id : A.members) sA = sA * all_contestants[id].skill;
        Skill sB = Skill::identity();
        for (int id : B.members) sB = sB * all_contestants[id].skill;
        
        int scoreA = A.teamExp + A.totalMotivation + sA.getEffectiveSkill();
        int scoreB = B.teamExp + B.totalMotivation + sB.getEffectiveSkill();
        if (!(scoreA > scoreB)) return StatusType::FAILURE;
    }
    
    A.teamExp += B.teamExp;
    A.totalMotivation += B.totalMotivation;
    for (int id : B.members) {
        A.members.push_back(id);
        contestant_team[id] = A.id;
    }
    
    teams.erase(B.id);
    return StatusType::SUCCESS;
}
