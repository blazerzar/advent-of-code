use std::collections::HashSet;
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

    let jets = read_file(&input_path);
    println!("{}", first_puzzle(&jets));
    println!("{}", second_puzzle(&jets));
}

fn read_file(path: &str) -> Vec<isize> {
    fs::read_to_string(path)
        .unwrap()
        .trim()
        .as_bytes()
        .iter()
        .map(|jet| *jet as isize - 61)
        .collect()
}

fn test(test_input_path: &str) {
    let jets = read_file(test_input_path);
    assert_eq!(first_puzzle(&jets), 3068);
    assert_eq!(second_puzzle(&jets), 1514285714288);
}

type Chamber = HashSet<(isize, isize)>;

struct Rock {
    width: isize,
    height: isize,
    // (dx, dy) for every rock unit
    units: Vec<(isize, isize)>,
}

impl Rock {
    /// Check if rock collides with already fallen rocks, walls or floor
    fn collision(&self, chamber: &Chamber, x: isize, y: isize) -> bool {
        // Check collision with the floor
        if y <= 0 {
            return true;
        }

        // Check collision with the walls
        if x < 0 || x + self.width > 7 {
            return true;
        }

        // Check collision with the rocks
        for (dx, dy) in &self.units {
            if chamber.contains(&(x + dx, y + dy)) {
                return true;
            }
        }

        false
    }
}

struct Rocks {
    next_rock: usize,
    rocks: Vec<Rock>,
}

impl Rocks {
    fn init() -> Rocks {
        Rocks {
            next_rock: 0,
            rocks: vec![
                Rock {
                    width: 4,
                    height: 1,
                    units: vec![(0, 0), (1, 0), (2, 0), (3, 0)],
                },
                Rock {
                    width: 3,
                    height: 3,
                    units: vec![(1, 0), (0, 1), (1, 1), (2, 1), (1, 2)],
                },
                Rock {
                    width: 3,
                    height: 3,
                    units: vec![(0, 0), (1, 0), (2, 0), (2, 1), (2, 2)],
                },
                Rock {
                    width: 1,
                    height: 4,
                    units: vec![(0, 0), (0, 1), (0, 2), (0, 3)],
                },
                Rock {
                    width: 2,
                    height: 2,
                    units: vec![(0, 0), (1, 0), (0, 1), (1, 1)],
                },
            ],
        }
    }

    fn next(&mut self) -> &Rock {
        let rock = &self.rocks[self.next_rock];
        self.next_rock = (self.next_rock + 1) % self.rocks.len();
        rock
    }
}

fn find_cycle(fallen_rocks: &[(isize, isize, isize)]) -> Option<(usize, usize)> {
    let end = (fallen_rocks.len() as isize - 10) as usize;

    // Check every possible cycle start point
    for start in (0..end).step_by(5) {
        // Each half needs to be divisible by 5
        let length = fallen_rocks.len() - start;
        if length % 10 > 0 {
            continue;
        }

        let middle = start + length / 2;

        let first_half = fallen_rocks[start..middle].iter().collect::<Vec<_>>();
        let start_height = first_half[0].1;
        let first_half_height = first_half
            .iter()
            .map(|(_, y, height)| y + height - 1)
            .max()
            .unwrap();

        // Both parts y coordinate needs to be mapped, so the first rock is at 1
        let first_half = first_half
            .iter()
            .cloned()
            .map(|&(x, y, _)| (x, y - start_height + 1))
            .collect::<HashSet<_>>();
        let second_half = fallen_rocks[middle..]
            .iter()
            .cloned()
            .map(|(x, y, _)| (x, y - first_half_height))
            .collect::<HashSet<_>>();

        if first_half.is_subset(&second_half) {
            return Some((start, middle - start));
        }
    }

    None
}

fn simulate(jets: &[isize], rocks_num: usize) -> usize {
    let mut rocks = Rocks::init();
    let mut max_height = 0;
    let mut jet_index = 0;
    let mut chamber: Chamber = HashSet::new();

    // For finding repetition
    let mut fallen_rocks = Vec::new();
    let (mut start, mut length) = (0, 0);

    // Simulate until cycle is found
    for i in 0..rocks_num {
        if i > 5 && i % 5 == 0 {
            if let Some((s, l)) = find_cycle(&fallen_rocks) {
                start = s;
                length = l;
                break;
            }
        }

        let rock = rocks.next();
        let (mut x, mut y) = (2, max_height + 4);

        loop {
            // Move rock sideways
            if !rock.collision(&chamber, x + jets[jet_index], y) {
                x += jets[jet_index];
            }
            jet_index = (jet_index + 1) % jets.len();

            // Move rock down, if it collides, rock stops falling
            if rock.collision(&chamber, x, y - 1) {
                let new_rock = rock.units.iter().map(|(dx, dy)| (x + dx, y + dy));
                chamber.extend(new_rock);

                fallen_rocks.push((x, y, rock.height));

                max_height = max_height.max(y + rock.height - 1);
                break;
            } else {
                y -= 1;
            }
        }
    }

    // Cycle has not been found
    if start == 0 && length == 0 {
        return max_height as usize;
    }

    // Find height of start and every rock in the cycle
    let start_height = fallen_rocks[..start]
        .iter()
        .map(|(_, y, height)| y + height - 1)
        .max()
        .unwrap();
    let cycle_heights = fallen_rocks[start..start + length]
        .iter()
        .map(|(_, y, height)| y + height - 1 - start_height)
        .collect::<Vec<_>>();

    let cyclic_rocks = rocks_num - start;
    let cycles_num = cyclic_rocks / length;
    let remaining_rocks = cyclic_rocks % length;

    let cycles_height = cycles_num * cycle_heights[length - 1] as usize;
    let remaining_height = if remaining_rocks == 0 {
        0
    } else {
        cycle_heights[remaining_rocks - 1] as usize
    };

    start_height as usize + cycles_height + remaining_height
}

fn first_puzzle(jets: &[isize]) -> usize {
    simulate(jets, 2022)
}

fn second_puzzle(jets: &[isize]) -> usize {
    simulate(jets, 1000000000000)
}
