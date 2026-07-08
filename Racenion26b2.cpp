// You can edit anything you want in this file.
// However, you need to implement all public Racenion functions, which are provided below as a template.

#include "Racenion26b2.h"

Racenion::Racenion() {}

Racenion::~Racenion() {
    teamsById.forEach([](Team* team) {
        delete team;
    });
}

StatusType Racenion::add_team(int teamId) {
    if (teamId <= 0) return StatusType::INVALID_INPUT;
    
    try {
        if (teamsById.find(teamId) != nullptr) {
            return StatusType::FAILURE;
        }
        
        Team* new_team = new Team(teamId);
        teamsById.insert(teamId, new_team);
        teamsByMotivation.insert(MotivationKey{0, teamId}, new_team);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    
    return StatusType::SUCCESS;
}

StatusType Racenion::remove_team(int teamId) {
    if (teamId <= 0) return StatusType::INVALID_INPUT;
    
    try {
        Team* team = teamsById.find(teamId);
        if (!team) return StatusType::FAILURE;
        
        teamsByMotivation.remove(MotivationKey{team->totalMotivation, teamId});
        teamsById.remove(teamId);
        
        if (team->uf_root) {
            team->uf_root->is_active = false;
        }
        
        delete team;
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    
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
    
    try {
        if (contestants.find(contestantId) != nullptr) {
            return StatusType::FAILURE;
        }
        
        Team* team = teamsById.find(teamId);
        if (!team) return StatusType::FAILURE;
        
        UFNode* node = new UFNode(contestantId, skill, missionsHad);
        contestants.insert(contestantId, node);
        
        teamsByMotivation.remove(MotivationKey{team->totalMotivation, teamId});
        team->totalMotivation += motivation;
        
        Skill old_total = team->totalSkill;
        team->totalSkill = old_total * skill;
        
        teamsByMotivation.insert(MotivationKey{team->totalMotivation, teamId}, team);
        
        if (!team->uf_root) {
            team->uf_root = node;
        } else {
            team->uf_root = union_trees(team->uf_root, node, old_total);
        }
        
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    
    return StatusType::SUCCESS;
}

output_t<int> Racenion::duel(int teamId1, int teamId2) {
    if (teamId1 <= 0 || teamId2 <= 0 || teamId1 == teamId2) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }
    
    try {
        Team* t1 = teamsById.find(teamId1);
        Team* t2 = teamsById.find(teamId2);
        
        if (!t1 || !t2) {
            return output_t<int>(StatusType::FAILURE);
        }
        
        int score1 = t1->teamExp + t1->totalMotivation;
        int score2 = t2->teamExp + t2->totalMotivation;
        
        int win_result = 0;
        bool t1_won = false, t2_won = false;
        
        if (score1 > score2) {
            win_result = 1; t1_won = true;
        } else if (score1 < score2) {
            win_result = 3; t2_won = true;
        } else {
            int skill1 = t1->totalSkill.getEffectiveSkill();
            int skill2 = t2->totalSkill.getEffectiveSkill();
            if (skill1 > skill2) {
                win_result = 2; t1_won = true;
            } else if (skill1 < skill2) {
                win_result = 4; t2_won = true;
            } else {
                win_result = 0; 
            }
        }
        
        if (t1_won) {
            t1->teamExp += 3;
        } else if (t2_won) {
            t2->teamExp += 3;
        } else {
            t1->teamExp += 1;
            t2->teamExp += 1;
        }
        
        if (t1->uf_root) t1->uf_root->missions_offset += 1;
        if (t2->uf_root) t2->uf_root->missions_offset += 1;
        
        return output_t<int>(win_result);
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Racenion::get_contestant_missions_number(int contestantId) {
    if (contestantId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    
    try {
        UFNode* x = contestants.find(contestantId);
        if (!x) return output_t<int>(StatusType::FAILURE);
        
        UFNode* root = find_root(x);
        
        int missions = (x == root) ? root->missions_offset : root->missions_offset + x->missions_offset;
        return output_t<int>(missions);
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Racenion::get_team_experience(int teamId) {
    if (teamId <= 0) return output_t<int>(StatusType::INVALID_INPUT);
    
    try {
        Team* t = teamsById.find(teamId);
        if (!t) return output_t<int>(StatusType::FAILURE);
        
        return output_t<int>(t->teamExp);
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> Racenion::get_ith_collective_motivation_team(int i) {
    if (teamsByMotivation.size() == 0 || i < 1 || i > teamsByMotivation.size()) {
        return output_t<int>(StatusType::FAILURE);
    }
    
    try {
        Team* t = teamsByMotivation.select(i);
        return output_t<int>(t->teamId);
    } catch (std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<Skill> Racenion::get_partial_team_skill(int contestantId) {
    if (contestantId <= 0) return output_t<Skill>(StatusType::INVALID_INPUT);
    
    try {
        UFNode* x = contestants.find(contestantId);
        if (!x) return output_t<Skill>(StatusType::FAILURE);
        
        UFNode* root = find_root(x);
        if (!root->is_active) return output_t<Skill>(StatusType::FAILURE);
        
        Skill res = (x == root) ? root->skill_multiplier : root->skill_multiplier * x->skill_multiplier;
        return output_t<Skill>(res);
    } catch (std::bad_alloc&) {
        return output_t<Skill>(StatusType::ALLOCATION_ERROR);
    }
}

StatusType Racenion::recruit(int recruitingTeamId, int recruitedTeamId) {
    if (recruitingTeamId <= 0 || recruitedTeamId <= 0 || recruitingTeamId == recruitedTeamId) {
        return StatusType::INVALID_INPUT;
    }
    
    try {
        Team* A = teamsById.find(recruitingTeamId);
        Team* B = teamsById.find(recruitedTeamId);
        
        if (!A || !B) return StatusType::FAILURE;
        if (!A->uf_root) return StatusType::FAILURE;
        
        if (B->uf_root) {
            int scoreA = A->teamExp + A->totalMotivation + A->totalSkill.getEffectiveSkill();
            int scoreB = B->teamExp + B->totalMotivation + B->totalSkill.getEffectiveSkill();
            if (!(scoreA > scoreB)) return StatusType::FAILURE;
        }
        
        teamsByMotivation.remove(MotivationKey{B->totalMotivation, B->teamId});
        teamsById.remove(B->teamId);
        
        teamsByMotivation.remove(MotivationKey{A->totalMotivation, A->teamId});
        
        A->teamExp += B->teamExp;
        A->totalMotivation += B->totalMotivation;
        Skill old_total_A = A->totalSkill;
        A->totalSkill = old_total_A * B->totalSkill;
        
        teamsByMotivation.insert(MotivationKey{A->totalMotivation, A->teamId}, A);
        
        A->uf_root = union_trees(A->uf_root, B->uf_root, old_total_A);
        
        delete B;
        
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    
    return StatusType::SUCCESS;
}
