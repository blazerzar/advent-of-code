use std::collections::{HashMap, HashSet};
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

    let grid = read_file(&input_path);
    println!("{}", first_puzzle(&grid));
    println!("{}", second_puzzle(&grid));
}

fn read_file(path: &str) -> Vec<Vec<u8>> {
    fs::read_to_string(path)
        .unwrap()
        .split('\n')
        .map(|line| line.as_bytes().to_vec())
        .collect()
}

fn test(test_input_path: &str) {
    let grid = read_file(test_input_path);
    assert_eq!(first_puzzle(&grid), 110);
    assert_eq!(second_puzzle(&grid), 20);
}

type Position = (i32, i32);

fn get_positions(grid: &[Vec<u8>]) -> HashSet<Position> {
    let mut positions = HashSet::new();

    for (i, line) in grid.iter().enumerate() {
        for (j, cell) in line.iter().enumerate() {
            if *cell == b'#' {
                positions.insert((j as i32, i as i32));
            }
        }
    }

    positions
}

const PROPOSITIONS: [[(i32, i32); 3]; 4] = [
    [(0, -1), (1, -1), (-1, -1)], // N, NE, NW
    [(0, 1), (1, 1), (-1, 1)],    // S, SE, SW
    [(-1, 0), (-1, -1), (-1, 1)], // W, NW, SW
    [(1, 0), (1, -1), (1, 1)],    // E, NE, SE
];

const NEIGHBOURS: [(i32, i32); 8] = [
    (-1, -1),
    (0, -1),
    (1, -1),
    (-1, 0),
    (1, 0),
    (-1, 1),
    (0, 1),
    (1, 1),
];

fn simulate_round(elves: &mut HashSet<Position>, round: usize) -> bool {
    // --> First half: Proposing first valid direction.

    // Maps proposed positions to list of elves that proposed it
    let mut propositions: HashMap<Position, Vec<Position>> = HashMap::new();

    for (x, y) in elves.iter() {
        // Elve does not move it has no neighbours
        if NEIGHBOURS
            .iter()
            .map(|(dx, dy)| !elves.contains(&(x + dx, y + dy)))
            .all(|x| x)
        {
            continue;
        }

        // Find first valid direction proposition
        let proposed_direction = (0..4)
            .map(|direction| {
                let direction = (direction + (round % 4)) % 4;
                let cells_empty =
                    PROPOSITIONS[direction].map(|(dx, dy)| !elves.contains(&(x + dx, y + dy)));
                (cells_empty, direction)
            })
            .filter(|(cells_empty, _)| cells_empty.iter().all(|x| *x))
            .map(|(_, direction)| direction)
            .next();

        // Save proposed direction, if it has been found
        if let Some(direction) = proposed_direction {
            let (dx, dy) = PROPOSITIONS[direction][0];
            let (proposed_x, proposed_y) = (x + dx, y + dy);
            let proposition_elves = propositions.entry((proposed_x, proposed_y)).or_default();
            proposition_elves.push((*x, *y));
        }
    }

    if propositions.is_empty() {
        return false;
    }

    // --> Second half: Moving to proposed positions.
    for (proposed, proposition_elves) in propositions.iter() {
        // If only one elve proposed it, add new position and remove the old one
        if proposition_elves.len() == 1 {
            elves.insert(*proposed);
            elves.remove(&proposition_elves[0]);
        }
    }

    // Return true, since elves have moved
    true
}

fn first_puzzle(grid: &[Vec<u8>]) -> usize {
    let mut elves = get_positions(grid);

    for round in 0..10 {
        simulate_round(&mut elves, round);
    }

    // Find smallest bounding rectangle
    let xs = elves.iter().map(|(x, _)| x);
    let ys = elves.iter().map(|(_, y)| y);

    let a = (xs.clone().max().unwrap() - xs.min().unwrap() + 1) as usize;
    let b = (ys.clone().max().unwrap() - ys.min().unwrap() + 1) as usize;

    // Return number of empty ground tiles
    a * b - elves.len()
}

fn second_puzzle(grid: &[Vec<u8>]) -> usize {
    let mut elves = get_positions(grid);
    let mut round = 0;

    // Simulate until elves stop moving
    while simulate_round(&mut elves, round) {
        round += 1;
    }

    round + 1
}
