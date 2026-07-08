import random
import sys

def main():
    if len(sys.argv) < 2:
        print("Usage: python fuzz_generator.py <num_operations>")
        return
    num_operations = int(sys.argv[1])
    
    active_teams = set()
    all_teams = set()
    all_contestants = set()
    
    commands = []
    
    def get_team():
        if active_teams and random.random() < 0.9:
            return random.choice(list(active_teams))
        else:
            return random.randint(1, 1000)
            
    def get_contestant():
        if all_contestants and random.random() < 0.9:
            return random.choice(list(all_contestants))
        else:
            return random.randint(1, 5000)

    for i in range(num_operations):
        op = random.choice([
            "addTeam", "removeTeam", "addContestant", "duel", 
            "getContestantMissionsNumber", "getTeamExperience", 
            "getIthCollectiveMotivationTeam", "getPartialTeamSkill", "recruit"
        ])
        
        # Bias the operations to ensure structure builds up
        if i < num_operations // 10:
            op = random.choice(["addTeam", "addContestant", "addContestant"])
        
        if op == "addTeam":
            tid = random.randint(1, 1000)
            commands.append(f"addTeam {tid}")
            active_teams.add(tid)
            all_teams.add(tid)
            
        elif op == "removeTeam":
            tid = get_team()
            commands.append(f"removeTeam {tid}")
            if tid in active_teams:
                active_teams.remove(tid)
                
        elif op == "addContestant":
            cid = random.randint(1, 5000)
            tid = get_team()
            sa, sb, sc, sd = random.randint(0, 10), random.randint(0, 10), random.randint(0, 10), random.randint(0, 10)
            # Ensure valid skill
            while (sa*sd - sb*sc) % 32749 == 0:
                sa, sb, sc, sd = random.randint(0, 10), random.randint(0, 10), random.randint(0, 10), random.randint(0, 10)
            mot = random.randint(0, 100)
            missions = random.randint(0, 10)
            commands.append(f"addContestant {cid} {tid} {sa} {sb} {sc} {sd} {mot} {missions}")
            all_contestants.add(cid)
            
        elif op == "duel":
            t1 = get_team()
            t2 = get_team()
            while t1 == t2 and len(active_teams) > 1:
                t2 = get_team()
            commands.append(f"duel {t1} {t2}")
            
        elif op == "getContestantMissionsNumber":
            cid = get_contestant()
            commands.append(f"getContestantMissionsNumber {cid}")
            
        elif op == "getTeamExperience":
            tid = get_team()
            commands.append(f"getTeamExperience {tid}")
            
        elif op == "getIthCollectiveMotivationTeam":
            if active_teams:
                idx = random.randint(1, max(1, len(active_teams) + 5))
            else:
                idx = 1
            commands.append(f"getIthCollectiveMotivationTeam {idx}")
            
        elif op == "getPartialTeamSkill":
            cid = get_contestant()
            commands.append(f"getPartialTeamSkill {cid}")
            
        elif op == "recruit":
            t1 = get_team()
            t2 = get_team()
            while t1 == t2 and len(active_teams) > 1:
                t2 = get_team()
            commands.append(f"recruit {t1} {t2}")
            if t2 in active_teams:
                active_teams.remove(t2)
                
    for c in commands:
        print(c)

if __name__ == '__main__':
    main()
