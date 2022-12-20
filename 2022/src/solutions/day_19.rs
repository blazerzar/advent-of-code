use regex::Regex;
use std::fs;
use std::path::PathBuf;

pub fn solve() {
    let day = module_path!().split("::").last().unwrap();
    let day: u8 = day.split('_').last().unwrap().parse().unwrap();
    println!("Solving day {day}.");

    let input_path: PathBuf = ["inputs", &format!("day_{:02}.txt", day)].iter().collect();
    let test_input_path: PathBuf = ["inputs", &format!("day_{:02}_test.txt", day)]
        .iter()
        .collect();
    let input_path = input_path.display().to_string();
    let test_input_path = test_input_path.display().to_string();

    test(&test_input_path);

    let blueprints = read_file(&input_path);
    println!("{}", first_puzzle(&blueprints));
    println!("{}", second_puzzle(&blueprints));
}

type Recipe = (i32, i32, i32, i32);

struct Blueprint {
    ore: Recipe,
    clay: Recipe,
    obsidian: Recipe,
    geode: Recipe,
}

fn read_file(path: &str) -> Vec<Blueprint> {
    let pattern = r"Blueprint \d+: .+?(\d+).+?(\d+).+?(\d+).+?(\d+).+?(\d+).+?(\d+).+?";
    let re = Regex::new(pattern).unwrap();
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| {
            let cap = re.captures(line).unwrap();
            Blueprint {
                ore: (cap[1].parse().unwrap(), 0, 0, 0),
                clay: (cap[2].parse().unwrap(), 0, 0, 0),
                obsidian: (cap[3].parse().unwrap(), cap[4].parse().unwrap(), 0, 0),
                geode: (cap[5].parse().unwrap(), 0, cap[6].parse().unwrap(), 0),
            }
        })
        .collect()
}

fn test(test_input_path: &str) {
    let blueprints = read_file(test_input_path);
    assert_eq!(first_puzzle(&blueprints), 33);
}

type Materials = (i32, i32, i32, i32);
type Robots = (i32, i32, i32, i32);

fn enough_materials(materials: Materials, recipe: Recipe) -> bool {
    materials.0 >= recipe.0
        && materials.1 >= recipe.1
        && materials.2 >= recipe.2
        && materials.3 >= recipe.3
}

fn update_materials(materials: Materials, change: (i32, i32, i32, i32), sign: i32) -> Materials {
    (
        materials.0 + sign * change.0,
        materials.1 + sign * change.1,
        materials.2 + sign * change.2,
        materials.3 + sign * change.3,
    )
}

fn dfs(blueprint: &Blueprint, minute: i32, materials: Materials, robots: Robots, m: i32) -> i32 {
    if minute > m {
        return materials.3;
    }

    let updated_materials = update_materials(materials, robots, 1);
    let max_ore_cost = *[
        blueprint.ore.0,
        blueprint.clay.0,
        blueprint.obsidian.0,
        blueprint.geode.0,
    ]
    .iter()
    .max()
    .unwrap();

    // We always make the geode robot
    if enough_materials(materials, blueprint.geode) && minute < m {
        let materials = update_materials(updated_materials, blueprint.geode, -1);
        let mut robots = robots;
        robots.3 += 1;
        return dfs(blueprint, minute + 1, materials, robots, m);
    }

    let mut max_geode = 0;

    // Try making obsidian robot, if we need it
    if enough_materials(materials, blueprint.obsidian)
        && minute < (m - 1)
        && robots.2 < blueprint.geode.2
    {
        let materials = update_materials(updated_materials, blueprint.obsidian, -1);
        let mut robots = robots;
        robots.2 += 1;

        max_geode = dfs(blueprint, minute + 1, materials, robots, m);

        // If we have enough ore, always make obisidan robot
        if materials.0 >= max_ore_cost * (24 - minute) {
            return max_geode;
        }
    }

    let mut used_ore = false;

    // Try making clay robot, if we need it
    if enough_materials(materials, blueprint.clay)
        && minute < (m - 2)
        && robots.1 < blueprint.obsidian.1
    {
        let materials = update_materials(updated_materials, blueprint.clay, -1);
        let mut robots = robots;
        robots.1 += 1;

        max_geode = max_geode.max(dfs(blueprint, minute + 1, materials, robots, m));
        used_ore = true;
    }

    // Try making ore robot, if we need it
    if enough_materials(materials, blueprint.ore) && minute < (m - 2) && robots.0 < max_ore_cost {
        let materials = update_materials(updated_materials, blueprint.ore, -1);
        let mut robots = robots;
        robots.0 += 1;

        max_geode = max_geode.max(dfs(blueprint, minute + 1, materials, robots, m));
        used_ore = true;
    }

    // If we have enough ore, we might as well use it
    if !used_ore || materials.0 <= max_ore_cost * (m - minute + 1) {
        max_geode = max_geode.max(dfs(blueprint, minute + 1, updated_materials, robots, m));
    }

    max_geode
}

fn simulate(blueprint: &Blueprint, minutes: i32) -> i32 {
    dfs(blueprint, 1, (0, 0, 0, 0), (1, 0, 0, 0), minutes)
}

fn first_puzzle(blueprints: &[Blueprint]) -> i32 {
    blueprints
        .iter()
        .enumerate()
        .map(|(id, blueprint)| (id as i32 + 1) * simulate(blueprint, 24))
        .sum()
}

fn second_puzzle(blueprints: &[Blueprint]) -> i32 {
    blueprints[..3]
        .iter()
        .map(|blueprint| simulate(blueprint, 32))
        .product()
}
